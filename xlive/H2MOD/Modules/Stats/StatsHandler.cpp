#include "StatsHandler.h"
#include "stdafx.h"
#include "Globals.h"
#include "Util/hash.h"
#include "H2MOD\Modules\Utils\Utils.h"
#include "libcurl/curl/curl.h"
#include "H2MOD/Modules/Console/ConsoleCommands.h"
#include "rapidjson/document.h"
#include "rapidjson/writer.h"
#include "rapidjson/stringbuffer.h"
#include <iostream>
#include "XLive/XNet/upnp.h"
#include "H2MOD/Modules/Startup/Startup.h"
#include "XLive/xbox/xbox.h"
#include <ws2ipdef.h>
#include <WS2tcpip.h>
#ifdef _DEBUG
#pragma comment(lib, "libcurl_a_debug.lib")
#else
#pragma comment(lib, "libcurl_a.lib")
#endif

static const bool verbose = true;
StatsHandler::StatsHandler()
{
	
}
/*
 * Player leave store the data  if non team game sort rank them at the bottom of the list
 * if team game rank them with their team, will require changes to the API. Will probably be easy
 * to impliment just modify the way placement is calculate to allow placements under 16
 * All placements under 16 should be considered maximum loss in non team games.
 * Team Games match with thier team
 * FFA place at losest remove maximum XP.
 */
wchar_t* StatsHandler::getPlaylistFile()
{
	auto output = h2mod->GetAddress<wchar_t*>(0, 0x3B3704);
	return output;
}
std::string StatsHandler::getChecksum()
{
	std::string output;
	if (!hashes::calc_file_md5(getPlaylistFile(), output))
		output = "";
	return output;
}
int StatsHandler::uploadPlaylist()
{
	if (verbose)
		LOG_INFO_GAME("Uploading Playlist");
	wchar_t* playlist_file = getPlaylistFile();
	char* pFile = (char*)malloc(sizeof(char) * wcslen(playlist_file) + 1);
	wcstombs2(pFile, playlist_file, wcslen(playlist_file) + 1);
	if(verbose)
		LOG_INFO_GAME(pFile);
	std::string checksum = getChecksum();
	if (checksum == "")
	{
		LOG_ERROR_GAME(L"[H2MOD] playlistVerified failed to Hash Playlist file");
		return -1;
	}
	if (verbose)
		LOG_INFO_GAME(checksum);


	CURL *curl;
	CURLcode result;
	curl_mime *form = NULL;
	curl_mimepart *field = NULL;
	struct curl_slist *headerlist = NULL;
	static const char buf[] = "Expect:";
	curl_global_init(CURL_GLOBAL_ALL);
	curl = curl_easy_init();
	if(!curl)
	{
		LOG_ERROR_GAME(L"[H2MOD] uploadPlaylist failed to init curl");
		return -1;
	}
	form = curl_mime_init(curl);
	field = curl_mime_addpart(form);
	curl_mime_name(field, "file");
	curl_mime_filedata(field, pFile);
	field = curl_mime_addpart(form);
	curl_mime_name(field, "Type");
	curl_mime_data(field, "PlaylistUpload", CURL_ZERO_TERMINATED);
	field = curl_mime_addpart(form);
	curl_mime_name(field, "Playlist_Checksum");
	curl_mime_data(field, checksum.c_str(), CURL_ZERO_TERMINATED);
	headerlist = curl_slist_append(headerlist, buf);

	curl_easy_setopt(curl, CURLOPT_URL, "https://www.halo2pc.com/test-pages/CartoStat/API/post.php");
	curl_easy_setopt(curl, CURLOPT_MIMEPOST, form);
	result = curl_easy_perform(curl);
	if(result != CURLE_OK)
	{
		LOG_ERROR_GAME(L"[H2MOD]::playlistVerified failed to execute curl");
		//LOG_ERROR_GAME(curl_easy_strerror(result));
		return -1;
	}
	else
	{
		int response_code;
		curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &response_code);
		if (verbose)
			LOG_INFO_GAME(std::to_string(response_code));
		return response_code;
	}
	return -1;
}



int StatsHandler::verifyPlaylist()
{
	if (verbose)
		LOG_INFO_GAME(L"Verifying Playlist..");
	std::string http_request_body = "https://www.halo2pc.com/test-pages/CartoStat/API/get.php?Type=PlaylistCheck&Playlist_Checksum=";
	wchar_t* playlist_file = getPlaylistFile();
	if (verbose)
		LOG_INFO_GAME(L"[H2MOD] playlistVerified loaded playlist is {0}", playlist_file);

	std::string checksum = getChecksum();
	if (checksum == "")
	{
		LOG_ERROR_GAME(L"[H2MOD] playlistVerified failed to Hash Playlist file");
		return -1;
	}
	else
	{
		if (verbose)
			LOG_INFO_GAME(checksum);	
		http_request_body.append(checksum);
		if (verbose)
			LOG_INFO_GAME(http_request_body);
		char* Response;
		CURL *curl;
		CURLcode curlResult;
		CURLcode global_init = curl_global_init(CURL_GLOBAL_ALL);
		if(global_init != CURLE_OK)
		{
			LOG_INFO_GAME(L"[H2MOD]::playlistVerified failed to init curl");
			return -1;
		}
		curl = curl_easy_init();
		if(curl)
		{
			//Set the URL for the GET
			curl_easy_setopt(curl, CURLOPT_URL, http_request_body.c_str());
			curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 0L);
			curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L);
			curlResult = curl_easy_perform(curl);
			if(curlResult != CURLE_OK)
			{
				LOG_ERROR_GAME(L"[H2MOD]::playlistVerified failed to execute curl");
				//LOG_ERROR_GAME(curl_easy_strerror(curlResult));
				return -1;
			} else
			{
				int response_code;
				curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &response_code);
				if (verbose)
					LOG_INFO_GAME(std::to_string(response_code));
				return response_code;
			}
			curl_easy_cleanup(curl);
		}
	}
	return -1;
}


int StatsHandler::uploadStats(char* filepath)
{
	CURL *curl;
	CURLcode result;
	curl_mime *form = NULL;
	curl_mimepart *field = NULL;
	struct curl_slist *headerlist = NULL;
	static const char buf[] = "Expect:";
	curl_global_init(CURL_GLOBAL_ALL);
	curl = curl_easy_init();
	if (!curl)
	{
		LOG_ERROR_GAME(L"[H2MOD] uploadStats failed to init curl");
		return -1;
	}
	form = curl_mime_init(curl);
	field = curl_mime_addpart(form);
	curl_mime_name(field, "file");
	curl_mime_filedata(field, filepath);
	field = curl_mime_addpart(form);
	curl_mime_name(field, "Type");
	curl_mime_data(field, "GameStats", CURL_ZERO_TERMINATED);
	headerlist = curl_slist_append(headerlist, buf);

	curl_easy_setopt(curl, CURLOPT_URL, "https://www.halo2pc.com/test-pages/CartoStat/API/post.php");
	curl_easy_setopt(curl, CURLOPT_MIMEPOST, form);
	result = curl_easy_perform(curl);
	if (result != CURLE_OK)
	{
		LOG_ERROR_GAME(L"[H2MOD]::uploadStats failed to execute curl");
		//LOG_ERROR_GAME(curl_easy_strerror(result));
		return -1;
	}
	else
	{
		int response_code;
		curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &response_code);
		LOG_INFO_GAME(std::to_string(response_code));
		return response_code;
	}
	return -1;
}


//Really should make structs for these but I don't want to take the time to learn how.
static const int baseOffset  = 0x4dc722;
static const int rtPCROffset = 0x4DD1EE;
static const int PCROffset   = 0x49F6B0;

char* StatsHandler::buildJSON()
{
	typedef rapidjson::GenericDocument<rapidjson::UTF16<>> WDocument;
	typedef rapidjson::GenericValue<rapidjson::UTF16<>> WValue;
	WDocument document;
	WValue value;
	document.SetObject();


	//Inital Properties
	rapidjson::Document::AllocatorType& allocator = document.GetAllocator();
	auto checksum = getChecksum();
	auto wchecksum = std::wstring(checksum.begin(), checksum.end());
	value.SetString(wchecksum.c_str(), allocator);
	document.AddMember(L"PlaylistChecksum", value, allocator);
	auto Scenario = h2mod->GetAddress<wchar_t*>(0, 0x4DC504);

	std::wstring ScenarioPath = std::wstring(Scenario);
	std::wstring Scen(ScenarioPath.substr(ScenarioPath.rfind('\\') + 1));

	value.SetString(Scen.c_str(), allocator);
	document.AddMember(L"Scenario", value, allocator);

	//Build the Variant Object
	WValue Variant(rapidjson::kObjectType);
	auto VariantName = h2mod->GetAddress<wchar_t*>(0, 0x4DC3D4);
	value.SetString(VariantName, allocator);
	Variant.AddMember(L"Name", value, allocator);
	BYTE VariantType = *h2mod->GetAddress<BYTE*>(0, 0x4dc414);
	Variant.AddMember(L"Type", VariantType, allocator);
	
	WValue VariantSettings(rapidjson::kObjectType);
	BYTE TeamPlay = *h2mod->GetAddress<BYTE*>(0, 0x992880);
	value.SetString(IntToWString<BYTE>(TeamPlay, std::dec).c_str(), allocator);
	VariantSettings.AddMember(L"Team Play", value, allocator);
	Variant.AddMember(L"Settings", VariantSettings, allocator);
	document.AddMember(L"Variant", Variant, allocator);
	
	//Build the Server Object
	WValue Server(rapidjson::kObjectType);
	auto ServerXUID = *h2mod->GetAddress<::XUID*>(0, 0x52FC50);
	auto sXUID = IntToWString<::XUID>(ServerXUID, std::dec).c_str();
	value.SetString(sXUID, allocator);
	Server.AddMember(L"XUID", value, allocator);
	//h2mod->get_local_player_name(0)
	auto ServerName = h2mod->GetAddress<wchar_t*>(0, 0x52FC88);
	value.SetString(ServerName, allocator);
	Server.AddMember(L"Name", value, allocator);
	auto IP = NetworkSession::getLocalNetworkAddress();
	auto IPString = std::string(inet_ntoa(IP));
	auto wIPString = std::wstring(IPString.begin(), IPString.end());
	value.SetString(wIPString.c_str(), allocator);
	Server.AddMember(L"IP", value, allocator);
	document.AddMember(L"Server", Server, allocator);

	//Players
	int playerCount = 0;
	WValue Players(rapidjson::kArrayType);
	for(auto i = 0; i < 16; i++)
	{
		int calcBaseOffset  = baseOffset + (i * 0x94);
		auto XUID = *h2mod->GetAddress<::XUID*>(0, calcBaseOffset);
		if (XUID == 0) //Skip if it doesnt exists
			continue;


		int calcRTPCROffset = rtPCROffset + (i * 0x36A);
		int calcPCROffset = 0;
		int EndgameIndex = 0;
		WValue Player(rapidjson::kObjectType);
		
		
		auto Gamertag = h2mod->GetAddress<wchar_t*>(0, calcBaseOffset + 0xA);
		for(auto j = 0; j < 16; j++)
		{
			auto tGamertag = h2mod->GetAddress<wchar_t*>(0, PCROffset + (j * 0x110));
			//I dont know why but I couldn't get any other method of comparing to work,
			//Someone else can fix it because I know this probably is a terrible way
			//To compare the wchar_t*.
			LOG_INFO_GAME("Checking for match for..");
			LOG_INFO_GAME(Gamertag);
			LOG_INFO_GAME(tGamertag);
			if(std::wstring(Gamertag) == std::wstring(tGamertag))
			{
				
				calcPCROffset = PCROffset + (j * 0x110);
				LOG_INFO_GAME(IntToString(calcPCROffset, std::hex));
				EndgameIndex = i;
				break;
			}
		}
		
		playerCount++;
		auto PrimaryColor = *h2mod->GetAddress<BYTE*>(0, calcBaseOffset + 0x4A);
		auto SecondaryColor = *h2mod->GetAddress<BYTE*>(0, calcBaseOffset + 0x4B);
		auto PrimaryEmblem = *h2mod->GetAddress<BYTE*>(0, calcBaseOffset + 0x4C);
		auto SecondaryEmblem = *h2mod->GetAddress<BYTE*>(0, calcBaseOffset + 0x4D);
		auto PlayerModel = *h2mod->GetAddress<BYTE*>(0, calcBaseOffset + 0x4E);
		auto EmblemForeground = *h2mod->GetAddress<BYTE*>(0, calcBaseOffset + 0x4F);
		auto EmblemBackground = *h2mod->GetAddress<BYTE*>(0, calcBaseOffset + 0x50);
		auto EmblemToggle = *h2mod->GetAddress<BYTE*>(0, calcBaseOffset + 0x51);
		auto ClanDescripton = h2mod->GetAddress<wchar_t*>(0, calcBaseOffset + 0x5A);
		auto ClanTag = h2mod->GetAddress<wchar_t*>(0, calcBaseOffset + 0x7A);
		auto Team = *h2mod->GetAddress<BYTE*>(0, calcBaseOffset + 0x86);
		auto Handicap = *h2mod->GetAddress<BYTE*>(0, calcBaseOffset + 0x87);
		auto Rank = *h2mod->GetAddress<BYTE*>(0, calcBaseOffset + 0x88);
		auto Nameplate = *h2mod->GetAddress<BYTE*>(0, calcBaseOffset + 0x8B);
		
		wchar_t Place[16];
		std::wcsncpy(Place, h2mod->GetAddress<wchar_t*>(0, calcPCROffset + 0xE0), 16);
		
		wchar_t Score[16];
		std::wcsncpy(Score, h2mod->GetAddress<wchar_t*>(0, calcPCROffset + 0x40), 16);

		auto Kills = *h2mod->GetAddress<unsigned short*>(0, calcRTPCROffset);
		auto Assists = *h2mod->GetAddress<unsigned short*>(0, calcRTPCROffset + 0x2);
		auto Deaths = *h2mod->GetAddress<unsigned short*>(0, calcRTPCROffset + 0x4);
		auto Betrayals = *h2mod->GetAddress<unsigned short*>(0, calcRTPCROffset + 0x6);
		auto Suicides = *h2mod->GetAddress<unsigned short*>(0, calcRTPCROffset + 0x8);
		auto BestSpree = *h2mod->GetAddress<unsigned short*>(0, calcRTPCROffset + 0xA);
		auto TimeAlive = *h2mod->GetAddress<unsigned short*>(0, calcRTPCROffset + 0xC);
		auto ShotsFired = *h2mod->GetAddress<unsigned short*>(0, calcPCROffset + 0x84);
		auto ShotsHit = *h2mod->GetAddress<unsigned short*>(0, calcPCROffset + 0x88);
		auto HeadShots = *h2mod->GetAddress<unsigned short*>(0, calcPCROffset + 0x8C);

		auto FlagScores = *h2mod->GetAddress<unsigned short*>(0, calcRTPCROffset + 0xE);
		auto FlagSteals = *h2mod->GetAddress<unsigned short*>(0, calcRTPCROffset + 0x10);
		auto FlagSaves = *h2mod->GetAddress<unsigned short*>(0, calcRTPCROffset + 0x12);
		auto FlagUnk = *h2mod->GetAddress<unsigned short*>(0, calcRTPCROffset + 0x14);

		auto BombScores = *h2mod->GetAddress<unsigned short*>(0, calcRTPCROffset + 0x18);
		auto BombKills = *h2mod->GetAddress<unsigned short*>(0, calcRTPCROffset + 0x1A);
		auto BombGrabs = *h2mod->GetAddress<unsigned short*>(0, calcRTPCROffset + 0x1C);

		auto BallScore = *h2mod->GetAddress<unsigned short*>(0, calcRTPCROffset + 0x20);
		auto BallKills = *h2mod->GetAddress<unsigned short*>(0, calcRTPCROffset + 0x22);
		auto BallCarrierKills = *h2mod->GetAddress<unsigned short*>(0, calcRTPCROffset + 0x24);

		auto KingKillsAsKing = *h2mod->GetAddress<unsigned short*>(0, calcRTPCROffset + 0x26);
		auto KingKilledKings = *h2mod->GetAddress<unsigned short*>(0, calcRTPCROffset + 0x28);
		
		auto JuggKilledJuggs = *h2mod->GetAddress<unsigned short*>(0, calcRTPCROffset + 0x3C);
		auto JuggKillsAsJugg = *h2mod->GetAddress<unsigned short*>(0, calcRTPCROffset + 0x3E);
		auto JuggTime = *h2mod->GetAddress<unsigned short*>(0, calcRTPCROffset + 0x40);

		auto TerrTaken = *h2mod->GetAddress<unsigned short*>(0, calcRTPCROffset + 0x48);
		auto TerrLost = *h2mod->GetAddress<unsigned short*>(0, calcRTPCROffset + 0x46);

		value.SetInt(EndgameIndex);
		Player.AddMember(L"EndGameIndex", value, allocator);
		value.SetString(IntToWString<::XUID>(XUID, std::dec).c_str(), allocator);
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
		for(auto j = 0; j < 24; j++)
		{
			value.SetInt(*h2mod->GetAddress<unsigned short*>(0, calcRTPCROffset + 0x4A + (j * 2)));
			Medals.PushBack(value, allocator);
		}
		Player.AddMember(L"MedalData", Medals, allocator);

		WValue Weapons(rapidjson::kArrayType);
		for(auto j = 0; j < 36; j++)
		{
			WValue Weapon(rapidjson::kArrayType);
			for(auto k = 0; k < 6; k++)
			{
				value.SetInt(*h2mod->GetAddress<unsigned short*>(0, calcRTPCROffset + 0xDE + (j * 0x10) + (k * 2)));
				Weapon.PushBack(value, allocator);
			}
			Weapons.PushBack(Weapon, allocator);
		}
		Player.AddMember(L"WeaponData", Weapons, allocator);

		/*WValue Versus(rapidjson::kArrayType);
		for(auto j = 0; j < 16; j++)
		{
			WValue Matchup(rapidjson::kArrayType);
			value.SetInt(*h2mod->GetAddress<unsigned int*>(0, calcPCROffset + 0x90 + (i * 0x4)));
			Matchup.PushBack(value, allocator);
			Matchup.PushBack(0, allocator);
			Versus.PushBack(Matchup, allocator);
		}
		Player.AddMember(L"VersusData", Versus, allocator);*/

		Players.PushBack(Player, allocator);
	}


	for (auto i = 0; i < playerCount; i++) {
		WValue Versus(rapidjson::kArrayType);
		int iOffset = PCROffset + Players[i][L"EndGameIndex"].GetInt() * 0x110;
		for (auto j = 0; j < playerCount; j++)
		{
			WValue Matchup(rapidjson::kArrayType);
			int jOffset = PCROffset + Players[j][L"EndGameIndex"].GetInt() * 0x110;
			//Player I: Killed J
			value.SetInt(*h2mod->GetAddress<unsigned int*>(0, iOffset + 0x90 + (j * 0x4)));
			Matchup.PushBack(value, allocator);
			//Player I: Killed by J
			value.SetInt(*h2mod->GetAddress<unsigned int*>(0, jOffset + 0x90 + (i * 0x4)));
			Matchup.PushBack(value, allocator);
			Versus.PushBack(Matchup, allocator);
		}
		Players[i].AddMember(L"VersusData", Versus, allocator);
	}

	document.AddMember(L"Players", Players, allocator);

	//Shrink Versus Data
	

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

	swprintf(unix, 100, L"%.f", seconds);
	
	swprintf(fileOutPath, 1024, L"%wsstats\\%s-%s.json", H2ProcessFilePath, ServerName, unix);
	std::ofstream of(fileOutPath);
	of << json;
	if (!of.good())
	{
		LOG_ERROR_GAME("[H2MOD] Stats BuildJSON Failed to write to file");
	}
	char* oFile = (char*)malloc(sizeof(char) * wcslen(fileOutPath) + 1);
	wcstombs2(oFile, fileOutPath, wcslen(fileOutPath) + 1);
	return oFile;

	//rapidjson::Value array(rapidjson::kArrayType);
	//rapidjson::Document::AllocatorType& allocator = document.GetAllocator();
}


struct stringMe {
	char *ptr;
	size_t len;
};

static void init_string(struct stringMe *s) {
	s->len = 0;
	s->ptr = (char*)malloc(s->len + 1);
	if (s->ptr == NULL) {
		fprintf(stderr, "malloc() failed\n");
		exit(EXIT_FAILURE);
	}
	s->ptr[0] = '\0';
}

static size_t writefunc(void *ptr, size_t size, size_t nmemb, struct stringMe *s)
{
	size_t new_len = s->len + size * nmemb;
	s->ptr = (char*)realloc(s->ptr, new_len + 1);
	if (s->ptr == NULL) {
		fprintf(stderr, "realloc() failed\n");
		exit(EXIT_FAILURE);
	}
	memcpy(s->ptr + s->len, ptr, size*nmemb);
	s->ptr[new_len] = '\0';
	s->len = new_len;

	return size * nmemb;
}


std::vector<XUID> alreadySent;
struct compare
{
	XUID key;
	compare(XUID const &i) : key(i) { }

	bool operator()(XUID const &i)
	{
		return (i == key);
	}
};
void StatsHandler::playerLeftEvent(int peerIndex)
{
	//LeftPlayers.push_back(XUID)
	auto it = std::find(alreadySent.begin(), alreadySent.end(), NetworkSession::getPeerXUID(peerIndex));
	if(it != alreadySent.end())
		alreadySent.erase(alreadySent.begin() + std::distance(alreadySent.begin(), it));
}
void StatsHandler::playerJoinEvent(int peerIndex)
{
	/*No need to do anything fancy like filtering or 
	 *another function to send a specific rank
	 *The main function already filters out players
	 *who have been sent their rank before so just fire
	 *the main function.*/
	sendRankChange();
}
rapidjson::Document StatsHandler::getPlayerRanks(bool forceAll)
{
	std::string XUIDs;
	rapidjson::Document document;
	BYTE playerCount = 0;
	if (forceAll)
		alreadySent.clear();
	for (auto i = 0; i < NetworkSession::getPeerCount(); i++)
	{
		if(NetworkSession::getLocalPeerIndex() != i)
		{
			auto XUID = NetworkSession::getPeerXUID(i);
			if (std::none_of(alreadySent.begin(), alreadySent.end(), compare(XUID))) 
			{
				playerCount++;
				XUIDs.append(IntToString(XUID, std::dec));
				XUIDs.append(",");
				alreadySent.push_back(XUID);
			}
		}
		
	}
	if (playerCount == 0) return document;
	XUIDs = XUIDs.substr(0, XUIDs.length() - 1);
	std::string http_request_body = "https://www.halo2pc.com/test-pages/CartoStat/API/get.php?Type=PlaylistRanks&Playlist_Checksum=";
	http_request_body.append(getChecksum());
	http_request_body.append("&Server_XUID=");
	auto ServerXUID = *h2mod->GetAddress<::XUID*>(0, 0x52FC50);
	auto sXUID = IntToString<::XUID>(ServerXUID, std::dec);
	http_request_body.append(sXUID);
	http_request_body.append("&Player_XUIDS=");
	http_request_body.append(XUIDs);
	
	CURL *curl;
	CURLcode curlResult;
	CURLcode global_init = curl_global_init(CURL_GLOBAL_ALL);
	if (global_init != CURLE_OK)
	{
		char CurlError[500];
		snprintf(CurlError, 100, "curl_global_init(CURL_GLOBAL_ALL) failed: %s", curl_easy_strerror(global_init));
		LOG_INFO_GAME(L"[H2MOD]::getPlayerRanks failed to init curl");
		LOG_INFO_GAME(CurlError);
		alreadySent.clear();
		return document;
	}
	curl = curl_easy_init();
	if (curl)
	{

		struct stringMe s;
		init_string(&s);


		//Set the URL for the GET
		curl_easy_setopt(curl, CURLOPT_URL, http_request_body.c_str());
		curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 0L);
		curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L);
		curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, writefunc);
		curl_easy_setopt(curl, CURLOPT_WRITEDATA, &s);

		curlResult = curl_easy_perform(curl);
		if (curlResult != CURLE_OK)
		{
			LOG_ERROR_GAME(L"[H2MOD]::getPlayerRanks failed to execute curl");
			//LOG_ERROR_GAME(curl_easy_strerror(curlResult));
			return document;
		}
		else
		{

			int response_code;
			curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &response_code);
			if (verbose)
				LOG_INFO_GAME(s.ptr);
			document.Parse(s.ptr);
			curl_easy_cleanup(curl);
			free(s.ptr);
			return document;
		}
		
	}
	return document;
}
