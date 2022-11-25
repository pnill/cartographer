#include "stdafx.h"

#include "StatsHandler.h"

#include "Blam/Engine/Game/memory/bitstream.h"
#include "Blam/Engine/Game/networking/messages/network_message_type_collection.h"

#include "H2MOD.h"
#include "H2MOD/Modules/EventHandler/EventHandler.hpp"
#include "H2MOD/Modules/Shell/Config.h"
#include "H2MOD/Modules/Shell/Startup/Startup.h"
#include "H2MOD/Utils/Utils.h"
#include "Util/hash.h"

bool MatchInvalidated = false;
StatsHandler::StatsAPIRegisteredStatus Status;

std::atomic<bool> rankStateUpdating;
std::queue<std::shared_ptr<rapidjson::Document>> rankDocUpdates;

using namespace std::chrono;

static steady_clock::time_point lastTimeRanksSynchronized;

void StatsHandler::Initialize()
{
	// TODO FIXME disabled for now
	return;

	if (Memory::IsDedicatedServer()) {
		Status.Registered = false;
		Status.RanksEnabled = false;
		Status.StatsEnabled = false;
		lastTimeRanksSynchronized = steady_clock::now();

		// server events
		EventHandler::register_callback(server_command_event, EventType::server_command, EventExecutionType::execute_before);
		EventHandler::register_callback(network_player_event, EventType::network_player, EventExecutionType::execute_after);
	}

	EventHandler::register_callback(game_life_cycle_update_event, EventType::gamelifecycle_change, EventExecutionType::execute_after);
}

void StatsHandler::sendStats()
{
	if (!Memory::IsDedicatedServer()
		|| !getRegisteredStatus().StatsEnabled)
		return;

	const char* token = getAPIToken();
	if (token == nullptr) {
		return;
	}

	int verifyPlaylistResponse = verifyPlaylist(token);
	if (verifyPlaylistResponse != 201) {
		switch (verifyPlaylistResponse)
		{
		case -1:
		case 500:
			LOG_ERROR_GAME("[H2MOD] playlist verification encountered a server error code: {}", verifyPlaylistResponse);
			break;
		default:
			LOG_ERROR_GAME("[H2MOD] playlist verification encountered unkown error code: {}", verifyPlaylistResponse);
			break;
		}

		return;
	}

	if (uploadPlaylist(token) != 200) {
		LOG_ERROR_GAME("[H2MOD] playlist uploading encountered an error");
	}

	const char* filepath = buildPostGameCarnageReportJson();
	if (!strcmp(filepath, ""))
	{
		LOG_ERROR_GAME("[H2MOD] stats Json failed to build");
		return;
	}

	if (uploadStats(filepath, token) == 200)
	{
		LOG_TRACE_GAME("[H2MOD] stats uploaded successfully");
	}
	else
	{
		LOG_ERROR_GAME("[H2MOD] stats uploading encountered an error");
	}

	// delete the file
	remove(filepath);
}

void StatsHandler::verifySendPlaylist()
{
	if (!Memory::IsDedicatedServer()
		|| !getRegisteredStatus().StatsEnabled)
		return;

	const char* token = getAPIToken();
	int verifyPlaylistResponse = verifyPlaylist(token);
	switch (verifyPlaylistResponse)
	{
	case 201:
		if (uploadPlaylist(token) != 200) {
			LOG_ERROR_GAME("[H2MOD] Playlist Uploading encountered an error");
		}
		break;
	case 500:
	case -1:
	default:
		LOG_ERROR_GAME("[H2MOD] Playlist Verification encountered a server error");
		break;
	}
}

void StatsHandler::game_life_cycle_update_event(e_game_life_cycle state)
{
	auto updateStatsLifeCycle = [](e_game_life_cycle state)
	{
		static bool pregameFirstInitialization = false;

		switch (state)
		{
		case _life_cycle_pre_game:
			if (!pregameFirstInitialization)
			{
				verifyRegistrationStatus();
				verifySendPlaylist();
				pregameFirstInitialization = true;
				return;
			}
			else
			{
				invalidateMatch(false);
				return;
			}
		case _life_cycle_in_game:
			invalidateMatch(false);
			return;
		case _life_cycle_post_game:
			sendStats();
			return;
		default:
			break;
		}
	};

	if (Memory::IsDedicatedServer())
	{
		std::thread(updateStatsLifeCycle, state).detach();
	}
	else
	{
		if (state == _life_cycle_none)
		{
			players::carto_set_local_rank(255);
			return;
		}
	}
}

void StatsHandler::network_player_event(int peerIndex, EventHandler::NetworkPlayerEventType type)
{
	switch (type)
	{
	case EventHandler::NetworkPlayerEventType::add:
		playerJoinEvent(peerIndex);
		break;
	case EventHandler::NetworkPlayerEventType::remove:
		playerLeftEvent(peerIndex);
		break;
	}
}

void StatsHandler::server_command_event(ServerConsole::e_server_console_commands command)
{
	if (Memory::IsDedicatedServer()
		|| !Status.StatsEnabled)
		return;

	if (command == ServerConsole::skip && network_life_cycle::get_game_life_cycle() == _life_cycle_in_game)
	{
		invalidateMatch(true);
	}
}

/*
 * Player leave store the data  if non team game sort rank them at the bottom of the list
 * if team game rank them with their team, will require changes to the API. Will probably be easy
 * to impliment just modify the way placement is calculate to allow placements under 16
 * All placements under 16 should be considered maximum loss in non team games.
 * Team Games match with thier team
 * FFA place at losest remove maximum XP.
 */
const wchar_t* StatsHandler::getPlaylistFile()
{
	return Memory::GetAddress<wchar_t*>(0, 0x3B3704);
}
std::string StatsHandler::getChecksum()
{
	std::string output;
	return hashes::calc_file_md5(getPlaylistFile(), output) ? output : "";
}

struct curl_response_text {
	char* ptr;
	size_t len;
};

static void init_curl_response(struct curl_response_text* s) {
	s->len = 0;
	s->ptr = (char*)malloc(16);
	if (s->ptr == NULL) {
		fprintf(stderr, "malloc() failed\n");
		exit(EXIT_FAILURE);
	}
	s->ptr[0] = '\0';
}

static size_t writefunc(void* ptr, size_t size, size_t nmemb, struct curl_response_text* s)
{
	size_t new_len = s->len + size * nmemb;
	s->ptr = (char*)realloc(s->ptr, new_len + 1);
	if (s->ptr == NULL) {
		fprintf(stderr, "realloc() failed\n");
		exit(EXIT_FAILURE);
	}
	memcpy(s->ptr + s->len, ptr, size * nmemb);
	s->ptr[new_len] = '\0';
	s->len = new_len;

	return size * nmemb;
}

void StatsHandler::verifyRegistrationStatus()
{
	char* authKey = checkServerRegistration();
	if (authKey != NULL && strlen(authKey) == 32)
	{
		// check result returned a new AuthKey, attempt to register the server with it
		if (serverRegistration(authKey)) {
			// success save AuthKey to config
			strncpy(H2Config_stats_authkey, authKey, 32);
			SaveH2Config();
			Status.Registered = true;
			Status.StatsEnabled = true;
			LOG_TRACE_GAME("{} was successful.", __FUNCTION__);
		}
	}
	else
	{
		LOG_ERROR_GAME("{} webserver error on register attempt, this will be attempted again next server launch", __FUNCTION__);
	}
}

char* StatsHandler::checkServerRegistration()
{
	CURL* curl;
	int curl_err;
	CURLcode curlErr;
	char* result;

	curl = curl_interface_init_no_verify();
	if (!curl)
	{
		LOG_ERROR_GAME("{} failed to init curl", __FUNCTION__);
		return NULL;
	}

	std::string http_request_body = "https://www.halo2pc.com/test-pages/CartoStat/API/get.php?Type=ServerRegistrationCheck&Server_XUID=";
	unsigned long long dedicated_server_id = NetworkSession::GetCurrentNetworkSession()->membership[0].dedicated_server_xuid;
	http_request_body += std::to_string(dedicated_server_id);
	struct curl_response_text s;
	init_curl_response(&s);

	//Set the URL for the GET
	curl_easy_setopt(curl, CURLOPT_URL, http_request_body.c_str());
	curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, writefunc);
	curl_easy_setopt(curl, CURLOPT_WRITEDATA, &s);

	curlErr = curl_easy_perform(curl);
	if (curlErr == CURLE_OK)
	{
		curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &curl_err);

		switch (curl_err)
		{
		case 200:
		{
			rapidjson::Document doc;
			doc.Parse(s.ptr);
			Status.Registered = true;
			Status.RanksEnabled = doc["RanksEnabled"].GetBool();
			Status.StatsEnabled = doc["StatsEnabled"].GetBool();
			// server is registered
			result = NULL;
		}
		break;

		case 201:
			// server is unregisterd and returned a AuthKey
			result = s.ptr;
			break;
		case 500:
		default:
			result = NULL;
			break;
		}
	}
	else
	{
		LOG_ERROR_GAME("{} failed to execute curl", __FUNCTION__);
		result = NULL;
	}

	curl_easy_cleanup(curl);
	return result;
}

bool StatsHandler::serverRegistration(const char* authKey)
{
	CURL* curl;
	int response_code;
	CURLcode curl_err;
	curl_mime* form = NULL;
	curl_mimepart* field = NULL;
	struct curl_slist* headerlist = NULL;
	bool result;

	curl = curl_interface_init_no_verify();
	if (!curl)
	{
		LOG_ERROR_GAME("{} failed to init curl", __FUNCTION__);
		return false;
	}

	unsigned long long dedicated_server_id = NetworkSession::GetCurrentNetworkSession()->membership[0].dedicated_server_xuid;

	form = curl_mime_init(curl);
	field = curl_mime_addpart(form);
	curl_mime_name(field, "Type");
	curl_mime_data(field, "ServerRegistration", CURL_ZERO_TERMINATED);
	field = curl_mime_addpart(form);
	curl_mime_name(field, "Server_Name");
	curl_mime_data(field, H2Config_dedi_server_name, CURL_ZERO_TERMINATED);
	field = curl_mime_addpart(form);
	curl_mime_name(field, "Server_XUID");
	curl_mime_data(field, std::to_string(dedicated_server_id).c_str(), CURL_ZERO_TERMINATED);
	field = curl_mime_addpart(form);
	curl_mime_name(field, "AuthKey");
	curl_mime_data(field, authKey, CURL_ZERO_TERMINATED);
	headerlist = curl_slist_append(headerlist, "Expect:");

	curl_easy_setopt(curl, CURLOPT_URL, "https://www.halo2pc.com/test-pages/CartoStat/API/post.php");
	curl_easy_setopt(curl, CURLOPT_MIMEPOST, form);
	curl_err = curl_easy_perform(curl);
	if (curl_err == CURLE_OK)
	{
		curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &response_code);
		switch (response_code)
		{
		case 200:
			result = true;
			break;
		case 500:
		default:
			result = false;
			break;
		}
	}
	else
	{
		LOG_ERROR_GAME("{} failed to execute curl", __FUNCTION__);
		result = false;
	}

	curl_easy_cleanup(curl);
	return result;
}

const char* StatsHandler::getAPIToken()
{
	if (MatchInvalidated) {
		LOG_INFO_GAME("StatsHandler - Match was invalidated and will not be sent to the API");
		return NULL;
	}

	CURL* curl;
	int response_code;
	CURLcode curl_err;
	curl_mime* form = NULL;
	curl_mimepart* field = NULL;
	struct curl_slist* headerlist = NULL;
	char* result;

	curl = curl_interface_init_no_verify();
	if (!curl)
	{
		LOG_ERROR_GAME("{} failed to init curl", __FUNCTION__);
		return NULL;
	}
	form = curl_mime_init(curl);
	field = curl_mime_addpart(form);
	curl_mime_name(field, "Type");
	curl_mime_data(field, "ServerLogin", CURL_ZERO_TERMINATED);
	field = curl_mime_addpart(form);
	curl_mime_name(field, "AuthKey");
	curl_mime_data(field, H2Config_stats_authkey, CURL_ZERO_TERMINATED);
	headerlist = curl_slist_append(headerlist, "Expect:");

	struct curl_response_text s;
	init_curl_response(&s);
	curl_easy_setopt(curl, CURLOPT_URL, "https://www.halo2pc.com/test-pages/CartoStat/API/post.php");
	curl_easy_setopt(curl, CURLOPT_MIMEPOST, form);
	curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, writefunc);
	curl_easy_setopt(curl, CURLOPT_WRITEDATA, &s);
	curl_err = curl_easy_perform(curl);
	if (curl_err == CURLE_OK)
	{
		curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &response_code);

		switch (response_code)
		{
		case 200:
			result = s.ptr;
			break;
		case 500:
			LOG_INFO_GAME("{} failed get token for stats API", __FUNCTION__);
		default:
			result = NULL;
			break;
		}
	}
	else
	{
		LOG_ERROR_GAME("{} failed to execute curl", __FUNCTION__);
		result = NULL;
	}

	curl_easy_cleanup(curl);
	return result;
}


int StatsHandler::uploadPlaylist(const char* token)
{
	LOG_INFO_GAME("Uploading Playlist");
	const wchar_t* playlist_file = getPlaylistFile();
	std::string playlistFilename(std::wstring(playlist_file).begin(), std::wstring(playlist_file).end());

	std::string checksum = getChecksum();
	if (checksum.empty())
	{
		LOG_ERROR_GAME("{} - failed to hash playlist file", __FUNCTION__);
		return -1;
	}

	LOG_INFO_GAME("{} - playlist checksum: ", __FUNCTION__);
	LOG_INFO_GAME(checksum);

	CURL* curl;
	int response_code;
	CURLcode curl_err;
	curl_mime* form = NULL;
	curl_mimepart* field = NULL;
	struct curl_slist* headerlist = NULL;

	curl = curl_interface_init_no_verify();
	if (!curl)
	{
		LOG_ERROR_GAME("{} failed to init curl", __FUNCTION__);
		return -1;
	}

	form = curl_mime_init(curl);
	field = curl_mime_addpart(form);
	curl_mime_name(field, "file");
	curl_mime_filedata(field, playlistFilename.c_str());
	field = curl_mime_addpart(form);
	curl_mime_name(field, "Type");
	curl_mime_data(field, "PlaylistUpload", CURL_ZERO_TERMINATED);
	field = curl_mime_addpart(form);
	curl_mime_name(field, "Playlist_Checksum");
	curl_mime_data(field, checksum.c_str(), CURL_ZERO_TERMINATED);
	field = curl_mime_addpart(form);
	curl_mime_name(field, "AuthToken");
	curl_mime_data(field, token, CURL_ZERO_TERMINATED);
	headerlist = curl_slist_append(headerlist, "Expect:");

	curl_easy_setopt(curl, CURLOPT_URL, "https://www.halo2pc.com/test-pages/CartoStat/API/post.php");
	curl_easy_setopt(curl, CURLOPT_MIMEPOST, form);

	curl_err = curl_easy_perform(curl);
	if (curl_err == CURLE_OK)
	{
		curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &response_code);
		LOG_TRACE_GAME("{} response code: {}", __FUNCTION__, response_code);
	}
	else
	{
		LOG_ERROR_GAME("{} curl_easy_perform failed, error code: {}", __FUNCTION__, curl_easy_strerror(curl_err));
		response_code = -1;
	}

	return response_code;
}

int StatsHandler::verifyPlaylist(const char* token)
{
	CURL* curl;
	int response_code;
	CURLcode curl_err;
	char* response;

	LOG_TRACE_GAME("Verifying Playlist..");
	std::string http_request_body = "https://www.halo2pc.com/test-pages/CartoStat/API/get.php?Type=PlaylistCheck&Playlist_Checksum=";
	LOG_TRACE_GAME("{} loaded playlist is: ", __FUNCTION__);
	LOG_TRACE_GAME(L"{}", getPlaylistFile());

	std::string checksum = getChecksum();
	if (checksum == "")
	{
		LOG_ERROR_GAME("{} failed to Hash Playlist file", __FUNCTION__);
		return -1;
	}

	LOG_TRACE_GAME("playlist hash: {}", checksum);

	http_request_body.append(checksum);
	LOG_TRACE_GAME(http_request_body);
	http_request_body.append("&AuthToken=");
	http_request_body.append(token != nullptr ? token : "");

	curl = curl_interface_init_no_verify();
	if (!curl)
	{
		LOG_ERROR_GAME("{} curl_easy_init failed", __FUNCTION__);
		return -1;
	}

	//Set the URL for the GET
	curl_easy_setopt(curl, CURLOPT_URL, http_request_body.c_str());

	curl_err = curl_easy_perform(curl);
	if (curl_err == CURLE_OK)
	{
		curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &response_code);
		LOG_TRACE_GAME("{} response code: {}", __FUNCTION__, response_code);
	}
	else
	{
		LOG_ERROR_GAME("{} curl_easy_perform failed, error code: {}", __FUNCTION__, curl_easy_strerror(curl_err));
		response_code = -1;
	}

	curl_easy_cleanup(curl);
	return response_code;
}

int StatsHandler::uploadStats(const char* filepath, const char* token)
{
	CURL* curl;
	int response_code;
	CURLcode curl_err;
	curl_mime* form = NULL;
	curl_mimepart* field = NULL;
	struct curl_slist* headerlist = NULL;

	curl = curl_interface_init_no_verify();
	if (!curl)
	{
		LOG_ERROR_GAME("{} curl_easy_init failed", __FUNCTION__);
		return -1;
	}

	form = curl_mime_init(curl);
	field = curl_mime_addpart(form);
	curl_mime_name(field, "file");
	curl_mime_filedata(field, filepath);
	field = curl_mime_addpart(form);
	curl_mime_name(field, "Type");
	curl_mime_data(field, "GameStats", CURL_ZERO_TERMINATED);
	field = curl_mime_addpart(form);
	curl_mime_name(field, "AuthToken");
	curl_mime_data(field, token, CURL_ZERO_TERMINATED);
	headerlist = curl_slist_append(headerlist, "Expect:");

	curl_easy_setopt(curl, CURLOPT_URL, "https://www.halo2pc.com/test-pages/CartoStat/API/post.php");
	curl_easy_setopt(curl, CURLOPT_MIMEPOST, form);
	curl_err = curl_easy_perform(curl);
	if (curl_err == CURLE_OK)
	{
		curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &response_code);
	}
	else
	{
		LOG_ERROR_GAME("{} curl_easy_perform failed, error code: {}", __FUNCTION__, curl_easy_strerror(curl_err));
		response_code = -1;
	}

	curl_easy_cleanup(curl);
	return response_code;
}

const char* StatsHandler::buildPostGameCarnageReportJson()
{
	WDocument document;
	WValue value;
	document.SetObject();

	//Inital Properties
	rapidjson::Document::AllocatorType& allocator = document.GetAllocator();
	auto checksum = getChecksum();
	auto wchecksum = std::wstring(checksum.begin(), checksum.end());
	value.SetString(wchecksum.c_str(), allocator);
	document.AddMember(L"PlaylistChecksum", value, allocator);
	auto Scenario = Memory::GetAddress<wchar_t*>(0, 0x4DC504);

	std::wstring ScenarioPath = std::wstring(Scenario);
	std::wstring Scen(ScenarioPath.substr(ScenarioPath.rfind('\\') + 1));

	value.SetString(Scen.c_str(), allocator);
	document.AddMember(L"Scenario", value, allocator);

	//Build the Variant Object
	WValue Variant(rapidjson::kObjectType);
	auto VariantName = Memory::GetAddress<wchar_t*>(0, 0x4DC3D4);
	value.SetString(VariantName, allocator);
	Variant.AddMember(L"Name", value, allocator);
	BYTE VariantType = *Memory::GetAddress<BYTE*>(0, 0x4dc414);
	Variant.AddMember(L"Type", VariantType, allocator);

	WValue VariantSettings(rapidjson::kObjectType);
	BYTE TeamPlay = *Memory::GetAddress<BYTE*>(0, 0x992880);
	VariantSettings.AddMember(L"Team Play", TeamPlay, allocator);
	Variant.AddMember(L"Settings", VariantSettings, allocator);
	document.AddMember(L"Variant", Variant, allocator);

	auto ServerName = Memory::GetAddress<wchar_t*>(0, 0x52FC88);

	//Really should make structs for these but I don't want to take the time to learn how.
	const int baseOffset = 0x4dc722;
	const int rtPCROffset = 0x4DD1EE;
	const int PCROffset = 0x49F6B0;

	//Players
	int playerCount = 0;
	WValue Players(rapidjson::kArrayType);
	for (int i = 0; i < ENGINE_MAX_PLAYERS; i++)
	{
		int calcBaseOffset = baseOffset + (i * 0x94);
		auto playerId = *Memory::GetAddress<unsigned long long*>(0, calcBaseOffset);
		if (playerId == 0) //Skip if it doesnt exists
			continue;

		int calcRTPCROffset = rtPCROffset + (i * 0x36A);
		int calcPCROffset = 0;
		int EndgameIndex = 0;
		WValue Player(rapidjson::kObjectType);

		auto Gamertag = Memory::GetAddress<wchar_t*>(0, calcBaseOffset + 0xA);
		for (int j = 0; j < ENGINE_MAX_PLAYERS; j++)
		{
			auto tGamertag = Memory::GetAddress<wchar_t*>(0, PCROffset + (j * 0x110));
			if (!_wcsnicmp(Gamertag, tGamertag, XUSER_MAX_NAME_LENGTH))
			{
				calcPCROffset = PCROffset + (j * 0x110);
				EndgameIndex = i;
				break;
			}
		}

		playerCount++;

#pragma region Memory_Reading
		auto PrimaryColor = *Memory::GetAddress<BYTE*>(0, calcBaseOffset + 0x4A);
		auto SecondaryColor = *Memory::GetAddress<BYTE*>(0, calcBaseOffset + 0x4B);
		auto PrimaryEmblem = *Memory::GetAddress<BYTE*>(0, calcBaseOffset + 0x4C);
		auto SecondaryEmblem = *Memory::GetAddress<BYTE*>(0, calcBaseOffset + 0x4D);
		auto PlayerModel = *Memory::GetAddress<BYTE*>(0, calcBaseOffset + 0x4E);
		auto EmblemForeground = *Memory::GetAddress<BYTE*>(0, calcBaseOffset + 0x4F);
		auto EmblemBackground = *Memory::GetAddress<BYTE*>(0, calcBaseOffset + 0x50);
		auto EmblemToggle = *Memory::GetAddress<BYTE*>(0, calcBaseOffset + 0x51);
		auto ClanDescripton = Memory::GetAddress<wchar_t*>(0, calcBaseOffset + 0x5A);
		auto ClanTag = Memory::GetAddress<wchar_t*>(0, calcBaseOffset + 0x7A);
		auto Team = *Memory::GetAddress<BYTE*>(0, calcBaseOffset + 0x86);
		auto Handicap = *Memory::GetAddress<BYTE*>(0, calcBaseOffset + 0x87);
		auto Rank = *Memory::GetAddress<BYTE*>(0, calcBaseOffset + 0x88);
		auto Nameplate = *Memory::GetAddress<BYTE*>(0, calcBaseOffset + 0x8B);

		wchar_t Place[16];
		std::wcsncpy(Place, Memory::GetAddress<wchar_t*>(0, calcPCROffset + 0xE0), 16);

		wchar_t Score[16];
		std::wcsncpy(Score, Memory::GetAddress<wchar_t*>(0, calcPCROffset + 0x40), 16);

		auto Kills = *Memory::GetAddress<unsigned short*>(0, calcRTPCROffset);
		auto Assists = *Memory::GetAddress<unsigned short*>(0, calcRTPCROffset + 0x2);
		auto Deaths = *Memory::GetAddress<unsigned short*>(0, calcRTPCROffset + 0x4);
		auto Betrayals = *Memory::GetAddress<unsigned short*>(0, calcRTPCROffset + 0x6);
		auto Suicides = *Memory::GetAddress<unsigned short*>(0, calcRTPCROffset + 0x8);
		auto BestSpree = *Memory::GetAddress<unsigned short*>(0, calcRTPCROffset + 0xA);
		auto TimeAlive = *Memory::GetAddress<unsigned short*>(0, calcRTPCROffset + 0xC);
		auto ShotsFired = *Memory::GetAddress<unsigned short*>(0, calcPCROffset + 0x84);
		auto ShotsHit = *Memory::GetAddress<unsigned short*>(0, calcPCROffset + 0x88);
		auto HeadShots = *Memory::GetAddress<unsigned short*>(0, calcPCROffset + 0x8C);

		auto FlagScores = *Memory::GetAddress<unsigned short*>(0, calcRTPCROffset + 0xE);
		auto FlagSteals = *Memory::GetAddress<unsigned short*>(0, calcRTPCROffset + 0x10);
		auto FlagSaves = *Memory::GetAddress<unsigned short*>(0, calcRTPCROffset + 0x12);
		auto FlagUnk = *Memory::GetAddress<unsigned short*>(0, calcRTPCROffset + 0x14);

		auto BombScores = *Memory::GetAddress<unsigned short*>(0, calcRTPCROffset + 0x18);
		auto BombKills = *Memory::GetAddress<unsigned short*>(0, calcRTPCROffset + 0x1A);
		auto BombGrabs = *Memory::GetAddress<unsigned short*>(0, calcRTPCROffset + 0x1C);

		auto BallScore = *Memory::GetAddress<unsigned short*>(0, calcRTPCROffset + 0x20);
		auto BallKills = *Memory::GetAddress<unsigned short*>(0, calcRTPCROffset + 0x22);
		auto BallCarrierKills = *Memory::GetAddress<unsigned short*>(0, calcRTPCROffset + 0x24);

		auto KingKillsAsKing = *Memory::GetAddress<unsigned short*>(0, calcRTPCROffset + 0x26);
		auto KingKilledKings = *Memory::GetAddress<unsigned short*>(0, calcRTPCROffset + 0x28);

		auto JuggKilledJuggs = *Memory::GetAddress<unsigned short*>(0, calcRTPCROffset + 0x3C);
		auto JuggKillsAsJugg = *Memory::GetAddress<unsigned short*>(0, calcRTPCROffset + 0x3E);
		auto JuggTime = *Memory::GetAddress<unsigned short*>(0, calcRTPCROffset + 0x40);

		auto TerrTaken = *Memory::GetAddress<unsigned short*>(0, calcRTPCROffset + 0x48);
		auto TerrLost = *Memory::GetAddress<unsigned short*>(0, calcRTPCROffset + 0x46);
#pragma endregion 

#pragma region Document_Writing
		value.SetInt(EndgameIndex);
		Player.AddMember(L"EndGameIndex", value, allocator);
		value.SetString(std::to_wstring(playerId).c_str(), allocator);
		Player.AddMember(L"XUID", value, allocator);
		value.SetString(Gamertag, allocator);
		Player.AddMember(L"Gamertag", value, allocator);
		Player.AddMember(L"PrimaryColor", PrimaryColor, allocator);
		Player.AddMember(L"SecondaryColor", SecondaryColor, allocator);
		Player.AddMember(L"PrimaryEmblem", PrimaryEmblem, allocator);
		Player.AddMember(L"SecondaryEmblem", SecondaryEmblem, allocator);
		Player.AddMember(L"PlayerModel", PlayerModel, allocator);
		Player.AddMember(L"EmblemForeground", EmblemForeground, allocator);
		Player.AddMember(L"EmblemBackground", EmblemBackground, allocator);
		Player.AddMember(L"EmblemToggle", EmblemToggle, allocator);
		value.SetString(ClanDescripton, allocator);
		Player.AddMember(L"ClanDescription", value, allocator);
		value.SetString(ClanTag, allocator);
		Player.AddMember(L"ClanTag", value, allocator);
		Player.AddMember(L"Team", Team, allocator);
		Player.AddMember(L"Handicap", Handicap, allocator);
		Player.AddMember(L"Rank", Rank, allocator);
		Player.AddMember(L"Nameplate", Nameplate, allocator);
		value.SetString(Place, allocator);
		Player.AddMember(L"Place", value, allocator);
		value.SetString(Score, allocator);
		Player.AddMember(L"Score", value, allocator);
		value.SetInt(Kills);
		Player.AddMember(L"Kills", Kills, allocator);
		value.SetInt(Assists);
		Player.AddMember(L"Assists", value, allocator);
		value.SetInt(Deaths);
		Player.AddMember(L"Deaths", value, allocator);
		value.SetInt(Betrayals);
		Player.AddMember(L"Betrayals", value, allocator);
		value.SetInt(Suicides);
		Player.AddMember(L"Suicides", value, allocator);
		value.SetInt(BestSpree);
		Player.AddMember(L"BestSpree", value, allocator);
		value.SetInt(TimeAlive);
		Player.AddMember(L"TimeAlive", value, allocator);
		value.SetInt(ShotsFired);
		Player.AddMember(L"ShotsFired", value, allocator);
		value.SetInt(ShotsHit);
		Player.AddMember(L"ShotsHit", value, allocator);
		value.SetInt(HeadShots);
		Player.AddMember(L"HeadShots", value, allocator);
		value.SetInt(FlagScores);
		Player.AddMember(L"FlagScores", value, allocator);
		value.SetInt(FlagSteals);
		Player.AddMember(L"FlagSteals", value, allocator);
		value.SetInt(FlagSaves);
		Player.AddMember(L"FlagSaves", value, allocator);
		value.SetInt(FlagUnk);
		Player.AddMember(L"FlagUnk", value, allocator);
		value.SetInt(BombScores);
		Player.AddMember(L"BombScores", value, allocator);
		value.SetInt(BombKills);
		Player.AddMember(L"BombKills", value, allocator);
		value.SetInt(BombGrabs);
		Player.AddMember(L"BombGrabs", value, allocator);
		value.SetInt(BallScore);
		Player.AddMember(L"BallScore", value, allocator);
		value.SetInt(BallKills);
		Player.AddMember(L"BallKills", value, allocator);
		value.SetInt(BallCarrierKills);
		Player.AddMember(L"BallCarrierKills", value, allocator);
		value.SetInt(KingKillsAsKing);
		Player.AddMember(L"KingKillsAsKing", value, allocator);
		value.SetInt(KingKilledKings);
		Player.AddMember(L"KingKilledKings", value, allocator);
		value.SetInt(JuggKilledJuggs);
		Player.AddMember(L"JuggKilledJuggs", value, allocator);
		value.SetInt(JuggKillsAsJugg);
		Player.AddMember(L"JuggKillsAsJugg", value, allocator);
		value.SetInt(JuggTime);
		Player.AddMember(L"JuggTime", value, allocator);
		value.SetInt(TerrTaken);
		Player.AddMember(L"TerrTaken", value, allocator);
		value.SetInt(TerrLost);
		Player.AddMember(L"TerrLost", value, allocator);

		WValue Medals(rapidjson::kArrayType);
		for (int j = 0; j < 24; j++)
		{
			value.SetInt(*Memory::GetAddress<unsigned short*>(0, calcRTPCROffset + 0x4A + (j * 2)));
			Medals.PushBack(value, allocator);
		}
		Player.AddMember(L"MedalData", Medals, allocator);

		WValue DamageReport(rapidjson::kObjectType);
		for (int j = 0; j < 36; j++)
		{
			WValue DamageType(rapidjson::kArrayType);
			int uselessCheck = 0;
			for (int k = 0; k < 7; k++)
			{
				/*
				 * 0 = Kills
				 * 1 = Deaths
				 * 2 = Betrayals
				 * 3 = Suicides
				 * 4 = Shots Fired
				 * 5 = Shots Hit
				 * 6 = Headshot kills
				 * 8 = Nothing stored here, move on.
				 */
				auto val = *Memory::GetAddress<unsigned short*>(0, calcRTPCROffset + 0x8E + (j * 0x10) + (k * 2));
				value.SetInt(val);
				DamageType.PushBack(value, allocator);
				if (val > 0)
					uselessCheck++;
			}
			if (uselessCheck > 0) {
				WValue n(std::to_wstring(j).c_str(), allocator); //This is fucking stupid.
				DamageReport.AddMember(n, DamageType, allocator);
			}
		}
		Player.AddMember(L"DamageData", DamageReport, allocator);
#pragma endregion

		Players.PushBack(Player, allocator);
	}

	for (int i = 0; i < playerCount; i++) {
		WValue Versus(rapidjson::kArrayType);
		int iOffset = PCROffset + Players[i][L"EndGameIndex"].GetInt() * 0x110;
		for (int j = 0; j < playerCount; j++)
		{
			WValue Matchup(rapidjson::kArrayType);
			int jOffset = PCROffset + Players[j][L"EndGameIndex"].GetInt() * 0x110;
			//Player I: Killed J
			value.SetInt(*Memory::GetAddress<unsigned int*>(0, iOffset + 0x90 + (j * 0x4)));
			Matchup.PushBack(value, allocator);
			//Player I: Killed by J
			value.SetInt(*Memory::GetAddress<unsigned int*>(0, jOffset + 0x90 + (i * 0x4)));
			Matchup.PushBack(value, allocator);
			Versus.PushBack(Matchup, allocator);
		}
		Players[i].AddMember(L"VersusData", Versus, allocator);
	}

	document.AddMember(L"Players", Players, allocator);

	//Export the file
	rapidjson::StringBuffer strbuf;
	rapidjson::Writer<rapidjson::StringBuffer, rapidjson::UTF16<>> writer(strbuf);
	document.Accept(writer);
	std::string json(strbuf.GetString(), strbuf.GetSize());

	wchar_t fileOutPath[1024];
	time_t timer;
	struct tm y2k = { 0 };
	double seconds;

	y2k.tm_hour = 0;   y2k.tm_min = 0; y2k.tm_sec = 0;
	y2k.tm_year = 100; y2k.tm_mon = 0; y2k.tm_mday = 1;

	time(&timer);
	seconds = difftime(timer, mktime(&y2k));
	wchar_t unix[100];
	unsigned long long dedcated_server_id = NetworkSession::GetCurrentNetworkSession()->membership[0].dedicated_server_xuid;

	swprintf(unix, 100, L"%.f", seconds);
	swprintf(fileOutPath, 1024, L"%wsz%s-%s.json", H2ProcessFilePath, std::to_wstring(dedcated_server_id).c_str(), unix);
	std::ofstream of(fileOutPath);
	of << json;
	if (!of.good())
	{
		LOG_ERROR_GAME("{} Failed to write to file", __FUNCTION__);
	}

	size_t oFileBufSize = (wcslen(fileOutPath) + 1) * sizeof(char);
	char* oFile = (char*)malloc(oFileBufSize);
	wcstombs2(fileOutPath, oFile, oFileBufSize);
	return oFile;
}

void StatsHandler::invalidateMatch(bool state)
{
	MatchInvalidated = state;
}

void StatsHandler::playerRanksUpdateTick()
{
	if (!Memory::IsDedicatedServer()
		|| !getRegisteredStatus().StatsEnabled)
		return;

	auto sendRankUpdate = []()
	{
		rankStateUpdating = true;

		// don't send if rank update stack is empty
		if (rankDocUpdates.empty())
			return;

		while (true)
		{
			// get the oldest document update in the stack and update up to the back or lastest update
			auto doc = rankDocUpdates.front(); 
			sendRankChangeFromDocument(doc); // send data from the first document

			if (rankDocUpdates.size() > 1)
			{
				// keep the latest rank update in the buffer
				rankDocUpdates.pop();
			}
			else
			{
				break;
			}
		}

		rankStateUpdating = false;
	};

	// update each 2 seconds
	if (steady_clock::now() - lastTimeRanksSynchronized < 2s)
		return;

	if (rankStateUpdating)
		return; // return if queue is in use

	// this will update all players that don't have the matching rank from the host
	sendRankUpdate();

	// update last time, if more than 2 seconds passed, but only if we actually sent something
	lastTimeRanksSynchronized = steady_clock::now();
}

void StatsHandler::playerLeftEvent(int peerIndex)
{
}

void StatsHandler::playerJoinEvent(int peerIndex)
{
	// for now, update all ranks at once, shouldn't cause any issue
	// but only in the post game carnage report/pregame
	// ranks will update at the end of the game

	if (network_life_cycle::get_game_life_cycle() != _life_cycle_pre_game
		&& network_life_cycle::get_game_life_cycle() != _life_cycle_post_game)
		return;

	auto activePlayerIdList = std::make_shared<std::vector<unsigned long long>>(NetworkSession::GetActivePlayerIdList());
	if (!activePlayerIdList->empty())
		std::thread(getPlayerRanksByStringList, activePlayerIdList).detach();
}

void StatsHandler::sendRankChangeFromDocument(std::shared_ptr<rapidjson::Document> doc)
{
	if (!NetworkSession::LocalPeerIsSessionHost()
		|| !getRegisteredStatus().RanksEnabled)
		return;

	for (int i = 0; i < doc->MemberCount(); i++)
	{
		size_t sz = 0;
		BYTE rank = std::stoi(doc->operator[](i)["Rank"].GetString(), 0);
		long long playerIdentifier = std::stoll(doc->operator[](i)["XUID"].GetString(), &sz, 0);

		for (int j = 0; j < ENGINE_MAX_PLAYERS; j++)
		{
			if (NetworkSession::PlayerIsActive(j))
			{
				auto playerInfo = NetworkSession::GetPlayerInformation(j); // for now we only support local player 0

				if (playerIdentifier == NetworkSession::GetPlayerId(j)
					&& playerInfo->properties.player_displayed_skill != rank)
				{
					NetworkMessage::SendRankChange(NetworkSession::GetPeerIndex(j), rank);
					LOG_TRACE_GAME("{} - sent rank update to player index: {}, player identifier: {}", __FUNCTION__, j, playerIdentifier);
				}
			}
		}
	}
}

StatsHandler::StatsAPIRegisteredStatus StatsHandler::getRegisteredStatus()
{
	return Status;
}

void StatsHandler::getPlayerRanksByStringList(std::shared_ptr<std::vector<unsigned long long>> playerList)
{
	auto addDocToQueue = [](std::shared_ptr<rapidjson::Document> doc) -> void
	{
		while (rankStateUpdating)
			Sleep(100);

		rankStateUpdating = true;
		rankDocUpdates.push(doc);
		rankStateUpdating = false;
	};

	if (playerList->empty())
		return;

	std::shared_ptr<rapidjson::Document> doc = std::make_shared<rapidjson::Document>();
	unsigned long long dedicated_server_id = NetworkSession::GetCurrentNetworkSession()->membership[0].dedicated_server_xuid;

	std::string http_request_body = "https://www.halo2pc.com/test-pages/CartoStat/API/get.php?Type=PlaylistRanks&Playlist_Checksum=";
	http_request_body.append(getChecksum());
	http_request_body.append("&Server_XUID=");
	http_request_body.append(std::to_string(dedicated_server_id));
	http_request_body.append("&Player_XUIDS=");
	for (const auto& playerId : *playerList) {
		http_request_body.append(std::to_string(playerId));
		http_request_body += ',';
	}
	// trim out the last ','
	http_request_body.pop_back();

	CURLcode curl_err;
	int response_code;
	CURL* curl;

	curl = curl_interface_init_no_verify();
	if (!curl)
	{
		LOG_ERROR_GAME("{} curl_easy_init failed", __FUNCTION__);
		return;
	}

	struct curl_response_text s;
	init_curl_response(&s);

	//Set the URL for the GET
	curl_easy_setopt(curl, CURLOPT_URL, http_request_body.c_str());
	curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, writefunc);
	curl_easy_setopt(curl, CURLOPT_WRITEDATA, &s);

	curl_err = curl_easy_perform(curl);
	if (curl_err == CURLE_OK)
	{
		curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &response_code);
		LOG_TRACE_GAME("{} - player stats: ");
		LOG_TRACE_GAME(s.ptr);
		doc->Parse(s.ptr);
		// write to queue buffer
		addDocToQueue(doc);
		free(s.ptr);
	}
	else
	{
		LOG_ERROR_GAME("{} curl_easy_perform failed, error code: {}", __FUNCTION__, curl_easy_strerror(curl_err));
	}

	curl_easy_cleanup(curl);
	return;
}