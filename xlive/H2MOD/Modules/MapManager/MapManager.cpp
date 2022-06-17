#include "stdafx.h"

#include "MapManager.h"
#include "CustomMapDataCache\CustomMapDataCache.h"

#include "H2MOD\Modules\Shell\Config.h"
#include "H2MOD\Modules\Networking\Networking.h"
#include "H2MOD\Tags\TagInterface.h"
#include "H2MOD\Modules\OnScreenDebug\OnscreenDebug.h"

#include "Util\Hooks\Hook.h"
#include "XLive\xnet\IpManagement\XnIp.h"

MapManager* mapManager = new MapManager();

/* String constants below for client/server messages */
const char* DOWNLOADING_MAP = "Downloading Map";
const char* DOWNLOAD_MAP_PERCENTAGE_PREFIX = "Downloaded ";
const char* WAITING_FOR_MAP_DOWNLOAD_URL = "Waiting for map url from server";
const char* FOUND_MAP_DOWNLOAD_URL = "Found map download url";
const char* DOWNLOADING_COMPLETE = "Downloading complete";
const char* RELOADING_MAPS = "Reloading maps in memory";
const char* UNZIPPING_MAP_DOWNLOAD = "Unzipping map download";
const char* FAILED_TO_OPEN_ZIP_FILE = "Failed to open the zip file";
const char* STILL_SEARCHING_FOR_MAP = "Could not find maps from server, still searching";
const char* COULD_NOT_FIND_MAPS = "Couldn't find the map";
const char* UNABLE_TO_CONNECT_TO_SERVER = "Unable to connect to server";
const char* BAD_FILE_SIZE = "Got bad file size, rejoin";
const char* VALID_FILE_SIZE = "Got filesize from host";
const char* CONNECTION_TO_HOST_CLOSED = "Connection to host closed";
const char* VALID_FILE_NAME = "Got filename from host";
const char* MAP_WRITTEN_TO_DISK = "Wrote downloaded map to disk";
const char* CONNECTING_TO_HOST = "Connecting to host";
const char* ERROR_CONNECTING_TO_HOST = "Socket error connecting to host";
const char* CHECKING_IF_MAP_EXISTS = "Checking if the following map exists, %s";
const char* MAP_DOESNT_EXIST_IN_REPO = "Download failed, map doesn't exist in repo";

std::string validDigits("0123456789");
std::string mapExt(".map");
std::wstring mapExtUnicode(L".map");
std::string fileReadMode("wb");
std::string traceTemplate("%s");
std::string fileSizeDelim("$");

std::wstring CUSTOM_MAP = L"Custom Map";
wchar_t EMPTY_UNICODE_STR = '\0';

#pragma region allow host to start game, without all players loading the map

typedef signed int(__cdecl* network_life_cycle_session_get_global_map_precache_status)(signed int*, unsigned int*);
network_life_cycle_session_get_global_map_precache_status p_network_life_cycle_session_get_global_map_precache_status;

// allow hosts to start the game while other peers didn't load the map
// TODO: simplify and cleanup
signed int __cdecl network_life_cycle_session_get_global_map_precache_status_hook(signed int *out_smallest_load_percentage, unsigned int *out_host_map_status) {
	// this just gets the current network_session, but has some extra misc checks
	typedef bool(__cdecl* get_network_session_with_misc_checks)(s_network_session**);
	auto p_get_network_session_with_misc_checks = Memory::GetAddress<get_network_session_with_misc_checks>(0x1AD782, 0x1A66FF);

	s_network_session* session = nullptr;
	int result_map_status = 0;
	int result_precache_percentage = 0;
	bool someone_downloading_map = false;
	e_map_status local_peer_map_status = _network_session_map_status_none;
	e_map_status host_peer_map_status = _network_session_map_status_none;
	int peer_count_with_map_status_precached = 0;
	int peer_count_with_map_status_downloading = 0;
	int peer_count_with_map_status_unable_to_precache = 0;

	if (p_get_network_session_with_misc_checks(&session)) {
		switch (session->local_session_state)
		{
		case _network_session_state_none:
		case _network_session_state_peer_joining:
		case _network_session_state_peer_join_abort:
		case _network_session_state_election:
		case _network_session_state_dead:
			break;

		case _network_session_state_peer_established:
		case _network_session_state_peer_leaving:
		case _network_session_state_host_established:
		case _network_session_state_host_disband:
		case _network_session_state_host_handoff:
		case _network_session_state_host_reestablish:
			s_membership_information* membership = &session->membership[0];

			if (out_host_map_status)
				*out_host_map_status = membership->peer_data[session->session_host_peer_index].map_status;

			result_map_status = _network_session_map_status_loaded;
			result_precache_percentage = 100; // i don't think this is used anymore, it has been replaced by the loading screen in H2v from Xbox
			
			for (int i = 0; i < membership->peer_count; i++) {
				
				// NOTE UPDATE 7/29/2021: now this checks if there's any peer that can load the map, instead of if there's any peer that cannot load the map
				// *************

				// now we only check our peer and session host peer, instead of all the peers
				// but make sure the game won't start if we have just 1 player that doesn't have the map
				if (session->local_peer_index == i)
					local_peer_map_status = membership->peer_data[i].map_status;

				if (session->session_host_peer_index == i)
					host_peer_map_status = membership->peer_data[i].map_status;

				switch (membership->peer_data[i].map_status)
				{
				case _network_session_map_status_unable_to_precache:
					result_precache_percentage = 0;
					peer_count_with_map_status_unable_to_precache++;
					break;

				case _network_session_map_status_precaching:
					result_precache_percentage = min(membership->peer_data[i].map_progress_percentage, result_precache_percentage); // get the least map precaching percentage
					break;

				case _network_session_map_status_loaded:
				case _network_session_map_status_precached:
					peer_count_with_map_status_precached++;
					break;

				case _network_session_map_status_downloading:
					someone_downloading_map = true;
					peer_count_with_map_status_downloading++;
					break;

				default:
					break;
				}
			}

			// checks if local/host map status is fine
			// if not just tell the game we cannot load the map
			auto test_map_status = [&](e_map_status status) -> bool
			{
				switch (status)
				{
				case _network_session_map_status_loaded:
				case _network_session_map_status_precached:
					if (peer_count_with_map_status_precached > 0) {
						result_map_status = _network_session_map_status_precached;
						return true;
					}
					else if (peer_count_with_map_status_downloading > peer_count_with_map_status_unable_to_precache) {
						result_map_status = _network_session_map_status_downloading;
						return false;
					}
					else {
						result_map_status = _network_session_map_status_unable_to_precache;
						return false;
					}
				case _network_session_map_status_precaching:
					result_map_status = _network_session_map_status_precaching;
					return false;
				case _network_session_map_status_unable_to_precache:
					result_map_status = _network_session_map_status_unable_to_precache;
					result_precache_percentage = 0;
					return false;
				case _network_session_map_status_downloading:
					result_map_status = _network_session_map_status_downloading;
					result_precache_percentage = 0;
					return false;
				default:
					return false;
				}
			};
			
			// first test the host map load
			if (test_map_status(host_peer_map_status))
			{
				// then the local peer
				test_map_status(local_peer_map_status);
			}
		}
	}

	if (out_smallest_load_percentage)
		*out_smallest_load_percentage = result_precache_percentage;

	return result_map_status;
}

// this is actually thiscall, but the parameter is unused
bool __stdcall get_map_load_status_for_all_peers_hook_2(int a1, s_network_session *session, DWORD *out_peers_that_cant_load_map_flags)
{
	s_membership_information* membership = &session->membership[0];

	int result_bitflags = 0;
	if (membership->peer_count > 0)
		result_bitflags = (FLAG(membership->peer_count - 1) - 1) | FLAG(membership->peer_count - 1); // set all flags

	int peers_that_can_load_map = 0;
	bool local_or_host_peer_cannot_load_map = false;

	if (membership->peer_count > 0) {
		for (int i = 0; i < membership->peer_count; i++) {
			// NOTE UPDATE 7/29/2021: now this checks if there's any peer that can load the map, instead of if there's any peer that cannot load the map
			// *************

			// check only session host and local peer indexes
			//if ((i == session->local_peer_index || i == session->session_host_peer_index) 
				//|| !more_than_one_peer_other_than_dedicated_server(session)) 
			{
				switch (membership->peer_data[i].map_status)
				{
				case _network_session_map_status_none:
				case _network_session_map_status_unable_to_precache:
				case _network_session_map_status_precaching:
				case _network_session_map_status_downloading:
					if (i == session->local_peer_index || i == session->session_host_peer_index)
						local_or_host_peer_cannot_load_map = true;
					break;
				case _network_session_map_status_precached:
				case _network_session_map_status_loaded:
					result_bitflags &= ~FLAG(i);
					peers_that_can_load_map++;
					break;

				default:
					break;
				}
			}
		}
	}

	if (out_peers_that_cant_load_map_flags)
		*out_peers_that_cant_load_map_flags = result_bitflags;

	// subtract from peers_that_can_load_map 1 to remove the host/local
	return !local_or_host_peer_cannot_load_map && peers_that_can_load_map - (session->parameters[0].dedicated_server ? 1 : 0) > 0;
}

__declspec(naked) void get_map_load_status_for_all_peers_hook_2_to_stdcall() {
	__asm {
		pop eax
		push ecx
		push eax
		jmp get_map_load_status_for_all_peers_hook_2
	}
}
#pragma endregion

/**
* Download map callback
*/
char __cdecl handle_map_download_callback() {
	// add force download stop? for now no
	// mapManager->forceStopDownloadQueries()

	// add download query
	mapManager->AddDownloadQuery(L"");
	auto query = mapManager->GetLastDownloadQueryAdded();
	// request the filename
	NetworkMessage::SendRequestMapFilename(query->id);
	// then start download thread
	query->StartMapDownload();

	return 1;
}

typedef char(__cdecl* leave_game_callback_def)();
leave_game_callback_def p_original_leave_game_callback;

char leavegame_callback()
{
	return p_original_leave_game_callback();
}

/**
* Menu constructor hook
*/
void __cdecl display_map_downloading_menu(int a1, signed int a2, int a3, __int16 a4, DWORD map_download_callback, DWORD leave_game_callback, int a7, int a8, int a9, int a10)
{
	typedef void(__cdecl map_downloading_menu_constructor)(int a1, signed int a2, int a3, __int16 a4, int a5, int a6, int a7, int a8, int a9, int a10);
	auto p_map_downloading_menu_constructor = Memory::GetAddress<map_downloading_menu_constructor*>(0x20E2E0);

	p_original_leave_game_callback = (leave_game_callback_def)leave_game_callback;
	p_map_downloading_menu_constructor(a1, a2, a3, a4, (DWORD)handle_map_download_callback, (DWORD)leavegame_callback, a7, a8, a9, a10);
}

int __cdecl get_total_map_downloading_percentage()
{
	if (!mapManager->m_mapDownloadQueryList.empty())
		return mapManager->m_mapDownloadQueryList.front()->GetDownloadPercentage();
	else
		return 0;
}

wchar_t* receiving_map_string[] = {
	L"You are receiving the map from %s. \r\nPlease wait...%i%%"
};

wchar_t* get_receiving_map_string()
{ 
	int(__cdecl* get_default_game_language)() = (int(__cdecl*)())((char*)Memory::GetAddress(0x381fd));
	wchar_t** receiving_map_message = Memory::GetAddress<wchar_t**>(0x46575C);

	if (get_default_game_language() == 0) // check if english
		return receiving_map_string[0];

	return receiving_map_message[get_default_game_language()];
}

wchar_t* repo_string[] = { L"repository" };
void get_map_download_source(int a1, wchar_t* out_string)
{	
	if (out_string != NULL)
		wcsncpy_s(out_string, 512, repo_string[0], _TRUNCATE);
}
/*
	Seems to be a function responsible for loading data about maps when displaying them.
	This is hooked to fix/re-add removed custom map images.
*/
void* user_interface_data = nullptr;
void __declspec(naked) load_map_data_for_display() {
	__asm {
		mov eax, [esp + 0x10] // grab map_data pointer from stack
		mov ecx, [eax + 0x964] // mov bitmap pointer into ecx
		mov[ebx], ecx // mov bitmap pointer into map_data on stack
		mov eax, user_interface_data
		ret // return to original load_map_data_for display function
	}
}
/**
* Makes changes to game functionality
*/
void MapManager::ApplyHooks() {

	if (!Memory::IsDedicatedServer()) {

		BYTE jmp[] = { 0xEB };

		WriteBytes(Memory::GetAddress(0x215A9E), jmp, sizeof(jmp)); /* Allow map download in network */
		WriteBytes(Memory::GetAddress(0x215AC9), jmp, sizeof(jmp)); /* Disable "Match has begun" bullshit */
		PatchCall(Memory::GetAddress(0x244A4A), display_map_downloading_menu); /* Redirect the menu constructor to our code to replace the game's map downloading code callback */

		// code below is for percentage display
		PatchCall(Memory::GetAddress(0x244B77), get_total_map_downloading_percentage); /* Redirects map downloading percentage to our custom downloader */
		PatchCall(Memory::GetAddress(0x22EE41), get_total_map_downloading_percentage); /* Redirects map downloading percentage to our custom downloader */
		PatchCall(Memory::GetAddress(0x244B8F), get_map_download_source);
		PatchCall(Memory::GetAddress(0x244B9D), get_receiving_map_string);

		//Hooked to fix custom map images.
		Codecave(Memory::GetAddress(0x593F0), load_map_data_for_display, 0);
		user_interface_data = Memory::GetAddress<void*>(0x9712C8);
	}
	
	// allow host to start the game, even if there are peers that didn't load the map
	p_network_life_cycle_session_get_global_map_precache_status = (network_life_cycle_session_get_global_map_precache_status)DetourFunc(Memory::GetAddress<BYTE*>(0x1B1929, 0x197879), (BYTE*)network_life_cycle_session_get_global_map_precache_status_hook, 8);
	WriteJmpTo(Memory::GetAddress<BYTE*>(0x1D76C5, 0x1BCD32), get_map_load_status_for_all_peers_hook_2_to_stdcall);

	// disables game's map downloading implementation
	NopFill(Memory::GetAddress(0x1B5421, 0x1A917F), 5);

	// custom map cache patches/hooks
	s_custom_map_data::ApplyCustomMapExtensionLimitPatches();
}

/**
* Actually calls the real map reload function in halo2.exe
*/
void MapManager::ReloadAllMaps() {
	getCustomMapData()->load_custom_map_data_cache();
	getCustomMapData()->start_custom_map_sync();
}

void MapManager::GetMapFilename(std::wstring& buffer) {
	wchar_t map_file_location[256];
	s_network_session* session = nullptr;

	// we want this to work in-game too
	if (/*p_get_lobby_state() == game_lobby_states::in_lobby && */ NetworkSession::GetCurrentNetworkSession(&session)) {
		ZeroMemory(map_file_location, sizeof(map_file_location));
		NetworkSession::GetMapFileLocation(map_file_location, sizeof(map_file_location));

		std::wstring unicodeMapFileLocation(map_file_location);
		std::size_t mapNameOffset = unicodeMapFileLocation.find_last_of(L"\\");
		std::size_t mapNameOffsetEnd = unicodeMapFileLocation.find_last_not_of(L'.');
		std::wstring filename = unicodeMapFileLocation.substr(mapNameOffset + 1, mapNameOffsetEnd);
		if (!filename.empty()) {
			//if the filename exists and the current map english name is equal to the iterated map name
			buffer = filename;
		}
	}
}

void MapManager::MapDownloadUpdateTick()
{
	if (m_mapDownloadQueryList.empty())
		return;

	for (auto it = m_mapDownloadQueryList.begin(); it != m_mapDownloadQueryList.end(); )
	{
		MapDownloadQuery& query = **it;
		if (query.m_downloadFinished)
		{
			it = m_mapDownloadQueryList.erase(it);
		}
		else
		{
			it++;
		}
	}
}

bool MapDownloadQuery::ShouldStopDownload() {
	return m_forceStopDownload == true || !NetworkSession::LocalPeerIsEstablished();
}

void MapDownloadQuery::StopDownload() {
	m_forceStopDownload = true;
};

MapDownloadQuery::MapDownloadQuery(const std::wstring& _mapToDownload, int _downloadId)
{
	id = _downloadId;
	SetMapNameToDownload(_mapToDownload);
}

MapDownloadQuery::~MapDownloadQuery() {
}

size_t write_data(void *ptr, size_t size, size_t nmemb, FILE *stream) {
	return fwrite(ptr, size, nmemb, stream);
}

static int xferinfo(void *p, curl_off_t dltotal, curl_off_t dlnow, curl_off_t ultotal, curl_off_t ulnow) {
	MapDownloadQuery* mapDownloadQuery = (MapDownloadQuery*)p;
	mapDownloadQuery->SetDownloadPercentage(((double)dlnow / (double)dltotal) * 100);
	return mapDownloadQuery->ShouldStopDownload();
}

bool MapDownloadQuery::DownloadFromRepo() {
	std::string url(cartographerMapRepoURL + "/");

	FILE *fp = nullptr;
	CURL *curl = nullptr;
	CURLcode res;

	std::wstring mapFilePathWide(getCustomMapFolderPath());
	std::string nonUnicodeMapFilePath(mapFilePathWide.begin(), mapFilePathWide.end());
	nonUnicodeMapFilePath += m_clientMapFilename;

	curl = curl_interface_init_no_verify();
	if (curl) {
		fp = fopen(nonUnicodeMapFilePath.c_str(), "wb");
		if (fp == nullptr) {
			LOG_TRACE_GAME("{} - unable to open map file at: {}", __FUNCTION__, nonUnicodeMapFilePath.c_str());
			curl_easy_cleanup(curl);
			return false;
		}

		char *url_encoded_map_filename = curl_easy_escape(curl, m_clientMapFilename.c_str(), m_clientMapFilename.length());
		url += url_encoded_map_filename;
		curl_free(url_encoded_map_filename);

		//fail if 404 or any other type of http error
		curl_easy_setopt(curl, CURLOPT_FAILONERROR, true);
		curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
		curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_data);
		curl_easy_setopt(curl, CURLOPT_XFERINFOFUNCTION, xferinfo);
		curl_easy_setopt(curl, CURLOPT_XFERINFODATA, this);
		curl_easy_setopt(curl, CURLOPT_NOPROGRESS, 0L);
		curl_easy_setopt(curl, CURLOPT_WRITEDATA, fp);
		res = curl_easy_perform(curl);
		long http_code = 0;
		curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &http_code);
		/* always cleanup */
		curl_easy_cleanup(curl);
		fclose(fp);

		if (res == CURLE_OK)
		{
			if (http_code != 404 && getCustomMapData()->add_custom_map_entry_by_map_file_path(mapFilePathWide + m_clientMapFilenameWide)) {
				//if we succesfully downloaded the map, return true
				return true;
			}
			return false;
		}
		else
		{
			LOG_ERROR_GAME("{} - {} failed with error: {}", __FUNCTION__, STRINGIFY(curl_easy_perform()), res);
			if (res == CURLE_ABORTED_BY_CALLBACK)
				addDebugText("Map downloading aborted because of user input!");
			
			remove(nonUnicodeMapFilePath.c_str());

			return false;
		}
	}

	return false;
}

void MapDownloadQuery::StartMapDownload()
{
	CHRONO_DEFINE_TIME_AND_CLOCK();

	auto mapDownloadThread = [this]() {
		DWORD* mapDownloadStatus = Memory::GetAddress<DWORD*>(0x422570);

		auto timeAtStart = _clock::now();

		while (!m_readyToDownload && _clock::now() - timeAtStart < 5s)
			Sleep(50);

		// set the game to downloading map state
		*mapDownloadStatus = -1;

		if (m_readyToDownload) {
			LOG_TRACE_GAME("[h2mod-mapmanager] map file to download: {}", m_clientMapFilename);

			//TODO: set map filesize
			//TODO: if downloading from repo files, try p2p
			if (!DownloadFromRepo() 
				&& !NetworkSession::LocalPeerIsSessionHost())
			{
				LOG_TRACE_GAME("[h2mod-mapmanager] {}() - {}() failed, leaving session!",
					STRINGIFY(handle_map_download_callback),
					STRINGIFY(DownloadFromRepo));

				addDebugText("Failed to download custom map %s .", m_clientMapFilename.c_str());
				NetworkSession::LeaveSession(); // download has failed
			}
		}
		else {
			// no map filename (probably packet hasn't been received)
			addDebugText("Failed to download custom map, no filename to download.");
			LOG_TRACE_GAME("[h2mod-mapmanager] no map filename received from host!");
			NetworkSession::LeaveSession();
		}

		// set the game to map is loaded state
		*mapDownloadStatus = 0;
		m_downloadFinished = true;
	};

	if (NetworkSession::LocalPeerIsEstablished())
		std::thread(mapDownloadThread).detach();
	else
		m_downloadFinished = true;
}


void MapDownloadQuery::SetMapNameToDownload(const std::wstring& _mapNameToDownloadWide)
{
	if (!_mapNameToDownloadWide.empty())
	{
		m_clientMapFilenameWide = _mapNameToDownloadWide;
		m_clientMapFilename = std::string(_mapNameToDownloadWide.begin(), _mapNameToDownloadWide.end());
		m_readyToDownload = true;
	}
}

void MapDownloadQuery::SetMapNameToDownload(const wchar_t* _mapNameToDownload)
{
	SetMapNameToDownload(std::wstring(_mapNameToDownload));
}

int MapDownloadQuery::GetDownloadPercentage()
{
	return m_downloadPercentage;
}

void MapDownloadQuery::SetDownloadPercentage(int _downloadPercentage)
{
	m_downloadPercentage = _downloadPercentage;
}
