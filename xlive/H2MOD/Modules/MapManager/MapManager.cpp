#include "stdafx.h"

#include "MapManager.h"

#include "H2MOD\Modules\Config\Config.h"
#include "XLive\xnet\IpManagement\XnIp.h"
#include "..\Networking\Networking.h"
#include "H2MOD\Modules\Networking\NetworkSession\NetworkSession.h"

#include <Mswsock.h>
#include <WS2tcpip.h>
#include <curl/curl.h>

#pragma comment (lib, "mswsock.lib")

MapManager* mapManager = new MapManager();

#define DEFAULT_BUFLEN 65536

/* String constants below for client/server messages */
char DOWNLOADING_MAP[] = "Downloading Map";
char DOWNLOAD_MAP_PERCENTAGE_PREFIX[] = "Downloaded ";
char WAITING_FOR_MAP_DOWNLOAD_URL[] = "Waiting for map url from server";
char FOUND_MAP_DOWNLOAD_URL[] = "Found map download url";
char DOWNLOADING_COMPLETE[] = "Downloading complete";
char RELOADING_MAPS[] = "Reloading maps in memory";
char UNZIPPING_MAP_DOWNLOAD[] = "Unzipping map download";
char FAILED_TO_OPEN_ZIP_FILE[] = "Failed to open the zip file";
char STILL_SEARCHING_FOR_MAP[] = "Could not find maps from server, still searching";
char COULD_NOT_FIND_MAPS[] = "Couldn't find the map";
char UNABLE_TO_CONNECT_TO_SERVER[] = "Unable to connect to server";
char BAD_FILE_SIZE[] = "Got bad file size, rejoin";
char VALID_FILE_SIZE[] = "Got filesize from host";
char CONNECTION_TO_HOST_CLOSED[] = "Connection to host closed";
char VALID_FILE_NAME[] = "Got filename from host";
char MAP_WRITTEN_TO_DISK[] = "Wrote downloaded map to disk";
char CONNECTING_TO_HOST[] = "Connecting to host";
char ERROR_CONNECTING_TO_HOST[] = "Socket error connecting to host";
char CHECKING_IF_MAP_EXISTS[] = "Checking if the following map exists, %s";
char MAP_DOESNT_EXIST_IN_REPO[] = "Download failed, map doesn't exist in repo";

std::string validDigits("0123456789");
std::string mapExt(".map");
std::wstring mapExtUnicode(L".map");
std::string fileReadMode("wb");
std::string traceTemplate("%s");
std::string fileSizeDelim("$");

std::wstring CUSTOM_MAP = L"Custom Map";
wchar_t EMPTY_UNICODE_STR = '\0';

int downloadPercentage = 0;
bool mapDownloadCountdown = false;
auto promptOpenTime = std::chrono::high_resolution_clock::now();

#pragma region custom map checks

bool open_cache_header(const wchar_t *lpFileName, tags::cache_header *cache_header_ptr, HANDLE *map_handle)
{
	typedef char(__cdecl open_cache_header)(const wchar_t *lpFileName, tags::cache_header *lpBuffer, HANDLE *map_handle, DWORD NumberOfBytesRead);
	auto open_cache_header_impl = h2mod->GetAddress<open_cache_header*>(0x642D0, 0x4C327);
	return open_cache_header_impl(lpFileName, cache_header_ptr, map_handle, 0);
}

void close_cache_header(HANDLE *map_handle)
{
	typedef void __cdecl close_cache_header(HANDLE *a1);
	auto close_cache_header_impl = h2mod->GetAddress<close_cache_header*>(0x64C03, 0x4CC5A);
	close_cache_header_impl(map_handle);
}

static std::wstring_convert<std::codecvt_utf8<wchar_t>> wstring_to_string;

int __cdecl validate_and_add_custom_map(BYTE *a1)
{
	tags::cache_header header;
	HANDLE map_cache_handle;
	wchar_t *file_name = (wchar_t*)(a1 + 2432);
	if (!open_cache_header(file_name, &header, &map_cache_handle))
		return false;
	if (header.magic != 'head' || header.foot != 'foot' || header.file_size <= 0 || header.engine_gen != 8)
	{
		LOG_TRACE_FUNCW(L"\"{}\" has invalid header", file_name);
		return false;
	}
	if (header.type > 5 || header.type < 0)
	{
		LOG_TRACE_FUNCW(L"\"{}\" has bad scenario type", file_name);
		return false;
	}
	if (strnlen_s(header.name, 0x20u) >= 0x20 || strnlen_s(header.version, 0x20) >= 0x20)
	{
		LOG_TRACE_FUNCW(L"\"{}\" has invalid version or name string", file_name);
		return false;
	}
	if (header.type != scnr_type::Multiplayer && header.type != scnr_type::SinglePlayer)
	{
		LOG_TRACE_FUNCW(L"\"{}\" is not playable", file_name);
		return false;
	}

	close_cache_header(&map_cache_handle);
	// needed because the game loads the human readable map name and description from scenario after checks
	// without this the map is just called by it's file name

	// todo move the code for loading the descriptions to our code and get rid of this
	typedef int __cdecl validate_and_add_custom_map_interal(BYTE *a1);
	auto validate_and_add_custom_map_interal_impl = h2mod->GetAddress<validate_and_add_custom_map_interal*>(0x4F690, 0x56890);
	if (!validate_and_add_custom_map_interal_impl(a1))
	{
		LOG_TRACE_FUNCW(L"warning \"{}\" has bad checksums or is blacklisted, map may not work correctly", file_name);
		std::wstring fallback_name;
		if (strnlen_s(header.name, sizeof(header.name)) > 0) {
			fallback_name = wstring_to_string.from_bytes(header.name, &header.name[sizeof(header.name) - 1]);
		}
		else {
			std::wstring full_file_name = file_name;
			auto start = full_file_name.find_last_of('\\');
			fallback_name = full_file_name.substr(start != std::wstring::npos ? start : 0, full_file_name.find_last_not_of('.'));
		}
		wcsncpy_s(reinterpret_cast<wchar_t*>(a1 + 32), 0x20, fallback_name.c_str(), fallback_name.size());
	}
	// load the map even if some of the checks failed, will still mostly work
	return true;
}

bool __cdecl is_supported_build(char *build)
{
	const static std::unordered_set<std::string> offically_supported_builds{ "11122.07.08.24.1808.main", "11081.07.04.30.0934.main" };
	if (offically_supported_builds.count(build) == 0)
	{
		LOG_TRACE_FUNC("Build '{}' is not offically supported consider repacking and updating map with supported tools", build);
	}
	return true;
}
#pragma endregion

#pragma region allow host to start game, without all players loading the map

typedef signed int(__cdecl* get_map_load_status_for_all_peers)(signed int*, unsigned int*);
get_map_load_status_for_all_peers p_get_map_load_status_for_all_peers;

//allow hosts to start the game while other peers didn't load the map
signed int __cdecl get_map_load_status_for_all_peers_hook(signed int *smallest_load_percentage, unsigned int *host_map_status)
{
	// this just gets the current network_session, but has some extra misc checks
	typedef bool(__cdecl* get_network_session_with_misc_checks)(network_session**);
	auto p_get_network_session_with_misc_checks = h2mod->GetAddress<get_network_session_with_misc_checks>(0x1AD782, 0x1A66FF);

	network_session* session = nullptr;
	int result_map_status = 0;
	int result_map_percentage = 0;
	bool someone_downloading_map = false;

	if (p_get_network_session_with_misc_checks(&session))
	{
		switch (session->local_session_state)
		{
		case network_session_state_none:
		case network_session_state_peer_joining:
		case network_session_state_peer_join_abort:
		case network_session_state_election:
		case network_session_state_unk_2:
			break;

		case network_session_state_peer_established:
		case network_session_state_leaving:
		case network_session_state_session_host:
		case network_session_state_host_disband:
		case network_session_state_host_handoff:
		case network_session_state_host_reestablish:
			membership_info* membership = &session->membership;

			if (host_map_status)
				*host_map_status = membership->peer_info[session->session_host_peer_index].map_status;

			result_map_status = map_loaded;

			// i don't think this is really used anymore, it has been replaced by the load screen in H2v from Xbox
			result_map_percentage = 100; 
			
			for (int i = 0; i < session->membership.peer_count; i++)
			{
				// now we only check our peer and session host peer, instead of all the peers
				if (i == session->session_host_peer_index
					|| i == session->local_peer_index)
				{
					if (membership->peer_info[i].map_status == map_unavailable)
					{
						result_map_status = map_unavailable;
						result_map_percentage = 0;
						break;
					}

					switch (membership->peer_info[i].map_status)
					{
					case map_is_downloading:
						someone_downloading_map = true;
						break;

					case map_available:
						if (result_map_status == map_loaded)
							result_map_status = map_available;
						break;

					case map_someone_loading:
						result_map_status = map_someone_loading;
						result_map_percentage = min(membership->peer_info[i].map_progress_percentage, result_map_percentage); // get the least map load percentage
						break;
					}
				}
			}

			if (someone_downloading_map && result_map_status != map_unavailable)
				result_map_status = map_is_downloading;
		}
	}

	if (smallest_load_percentage)
		*smallest_load_percentage = result_map_percentage;

	return result_map_status;
}

bool __stdcall get_map_load_status_for_all_peers_hook_2(network_session *session, DWORD *player_flags_that_didnt_load_map)
{
	int result_bitflags = 0;
	membership_info* membership = &session->membership;
	bool everyone_loaded_the_map = true;

	if (membership->peer_count > 0)
	{
		for (int i = 0; i < membership->peer_count; i++)
		{
			// check only session host and local peer indexes
			if (i == session->local_peer_index
				|| i == session->session_host_peer_index)
			{
				switch (membership->peer_info[i].map_status)
				{
				case unk_0:
				case map_unavailable:
				case map_someone_loading:
				case map_is_downloading:
					result_bitflags |= FLAG(i);
					everyone_loaded_the_map = false;
					break;
				case map_available:
				case map_loaded:
					break;
				}
			}
		}
	}
	if (player_flags_that_didnt_load_map)
		*player_flags_that_didnt_load_map = result_bitflags;

	return everyone_loaded_the_map;
}
#pragma endregion

void MapManager::leaveSessionIfAFK() {
	if (mapDownloadCountdown) {
		auto duraton = std::chrono::duration_cast<std::chrono::seconds>(std::chrono::high_resolution_clock::now() - promptOpenTime);
		if (duraton >= std::chrono::seconds(20)) {
			h2mod->leave_session();
			mapDownloadCountdown = false;
		}
	}
	if (get_game_life_cycle() == life_cycle_in_game 
		|| get_game_life_cycle() == life_cycle_start_game
		|| get_game_life_cycle() == life_cycle_none)
	{
		mapDownloadCountdown = false;
	}
}

/**
* Download map callback
*/
char __cdecl handle_map_download_callback() {
	downloadPercentage = 0;
	mapDownloadCountdown = false;

	auto mapDownloadThread = []()
	{
		DWORD* mapDownloadStatus = h2mod->GetAddress<DWORD*>(0x422570);

		// set the game to downloading map state
		*mapDownloadStatus = -1;

		// sleep 500 msec to let the game receive the packet
		Sleep(500);

		if (!mapManager->getMapFilenameToDownload().empty())
		{
			LOG_TRACE_NETWORK("[h2mod-mapmanager] map file name from packet: {}", mapManager->getMapFilenameToDownload());
			if (!mapManager->hasCustomMap(mapManager->getMapFilenameToDownload())) {
				//TODO: set map filesize
				//TODO: if downloading from repo files, try p2p
				if (!mapManager->downloadFromRepo(mapManager->getMapFilenameToDownload()))
				{
					LOG_TRACE_NETWORK("[h2mod-mapmanager] handle_map_download_callback() - MapManager::downloadFromRepo failed, leaving session!");
					h2mod->leave_session(); // download has failed
				}
			}
			else {
				LOG_TRACE_NETWORK("[h2mod-mapmanager] already has map {}", mapManager->getMapFilenameToDownload());
			}
			mapManager->clearMapFileNameToDownload();
		}
		else 
		{
			// no map filename (probably packet hasn't been received)
			LOG_TRACE_NETWORK("[h2mod-mapmanager] no map filename received from host!");
			h2mod->leave_session();
		}

		// set the game to map is loaded state
		*mapDownloadStatus = 0;
	};

	if (NetworkSession::getCurrentNetworkSession()->local_session_state != network_session_state_none)
		std::thread(mapDownloadThread).detach();

	return 1;
}

typedef char(__cdecl* leave_game_callback_def)();
leave_game_callback_def p_original_leave_game_callback;

char leavegame_callback()
{
	mapDownloadCountdown = false;
	return p_original_leave_game_callback();
}

/**
* Menu constructor hook
*/
void __cdecl display_map_downloading_menu(int a1, signed int a2, int a3, __int16 a4, DWORD map_download_callback, DWORD leave_game_callback, int a7, int a8, int a9, int a10)
{
	typedef void(__cdecl map_downloading_menu_constructor)(int a1, signed int a2, int a3, __int16 a4, int a5, int a6, int a7, int a8, int a9, int a10);
	auto p_map_downloading_menu_constructor = h2mod->GetAddress<map_downloading_menu_constructor*>(0x20E2E0);

	mapDownloadCountdown = true;
	promptOpenTime = std::chrono::high_resolution_clock::now();
	CustomPackets::sendRequestMapFilename();
	p_original_leave_game_callback = (leave_game_callback_def)leave_game_callback;
	p_map_downloading_menu_constructor(a1, a2, a3, a4, (DWORD)handle_map_download_callback, (DWORD)leavegame_callback, a7, a8, a9, a10);
}

int __cdecl get_total_map_downloading_percentage()
{
	return downloadPercentage;
}

wchar_t* receiving_map_wstr[] = {
	L"You are receiving the map from %s. \r\nPlease wait...%i%%"
};

wchar_t* get_receiving_map_string()
{ 
	int(__cdecl* get_default_game_language)() = (int(__cdecl*)())((char*)h2mod->GetAddress(0x381fd));
	wchar_t** str_array = h2mod->GetAddress<wchar_t**>(0x46575C);

	if (get_default_game_language() == 0) // check if english
		return receiving_map_wstr[0];

	return str_array[get_default_game_language()];
}

wchar_t repo_wstr[] = L"repository";
void get_map_download_source_str(int a1, wchar_t* buffer)
{	
	if (buffer != NULL)
		wcsncpy_s(buffer, 512, repo_wstr, -1);
}

/**
* Makes changes to game functionality
*/
void MapManager::applyHooks() {

	if (!h2mod->Server) {

		BYTE jmp[1] = { 0xEB };

		WriteBytes(h2mod->GetAddress(0x215A9E), jmp, 1); /* Allow map download in network */
		WriteBytes(h2mod->GetAddress(0x215AC9), jmp, 1); /* Disable "Match has begun" bullshit */
		PatchCall(h2mod->GetAddress(0x244A4A), display_map_downloading_menu); /* Redirect the menu constructor to our code to replace the game's map downloading code callback */

		// code below is for percentage display
		PatchCall(h2mod->GetAddress(0x244B77), get_total_map_downloading_percentage); /* Redirects map downloading percentage to our custom downloader */
		PatchCall(h2mod->GetAddress(0x22EE41), get_total_map_downloading_percentage); /* Redirects map downloading percentage to our custom downloader */
		PatchCall(h2mod->GetAddress(0x244B8F), get_map_download_source_str);
		PatchCall(h2mod->GetAddress(0x244B9D), get_receiving_map_string);
	}

	// Both server and client
	WriteJmpTo(h2mod->GetAddress(0x1467, 0x12E2), is_supported_build);
	PatchCall(h2mod->GetAddress(0x1E49A2, 0x1EDF0), validate_and_add_custom_map);
	PatchCall(h2mod->GetAddress(0x4D3BA, 0x417FE), validate_and_add_custom_map);
	PatchCall(h2mod->GetAddress(0x4CF26, 0x41D4E), validate_and_add_custom_map);
	PatchCall(h2mod->GetAddress(0x8928, 0x1B6482), validate_and_add_custom_map);

	// allow host to start the game, even if there are peers that didn't load the map
	p_get_map_load_status_for_all_peers = (get_map_load_status_for_all_peers)DetourFunc(h2mod->GetAddress<BYTE*>(0x1B1929, 0x197879), (BYTE*)get_map_load_status_for_all_peers_hook, 8);
	WriteJmpTo(h2mod->GetAddress<BYTE*>(0x1D76C5, 0x1BCD32), get_map_load_status_for_all_peers_hook_2);

	// disables game's map downloading implementation
	NopFill(h2mod->GetAddress(0x1B5421, 0x1A917F), 5);
}

std::string MapManager::getMapFilenameToDownload()
{
	return this->mapFilenameToDownload;
}

void MapManager::setMapFileNameToDownload(std::wstring mapFilenameToDownload) {
	if (mapFilenameToDownload.size() > 0) {
		std::wstring fileNameWide(mapFilenameToDownload);
		std::string filename_str(fileNameWide.begin(), fileNameWide.end());
		setMapFileNameToDownload(filename_str);
	}
}

void MapManager::setMapFileNameToDownload(std::string mapFilenameToDownload) {
	if (mapFilenameToDownload.size() > 0) {
		this->mapFilenameToDownload = mapFilenameToDownload;
	}
}

void MapManager::clearMapFileNameToDownload() {
	this->mapFilenameToDownload.clear();
}

/**
* Gets a copy of the english map name as a wstring
* NOTE - this only works on peers (not dedis)
*/
std::wstring MapManager::getMapName() {
	//H2Server.exe+5349B4
	//H2Server.exe+535C64 (another offset to use if the above fails for whatever reason)
	const wchar_t* currentMapName = h2mod->GetAddress<wchar_t*>(0x97737C, 0x5349B4);

	std::wstring ucurrentMapName(currentMapName);

	return ucurrentMapName;
}

bool MapManager::hasCustomMap(std::string mapName) {
	if (mapName.empty()) {
		return true;
	}
	std::wstring unicodeMapName(mapName.length(), L' ');
	std::copy(mapName.begin(), mapName.end(), unicodeMapName.begin());
	return this->hasCustomMap(unicodeMapName);
}

/**
* Checks if the client has the custom map already
* NOTE - only works on peers (not dedis)
*/
bool MapManager::hasCustomMap(std::wstring mapName) {
	wchar_t* mapsDirectory = h2mod->GetAddress<wchar_t*>(0x482D70 + 0x2423C);
	std::wstring mapFileName(mapsDirectory);

	mapFileName += mapName;
	std::ifstream file(mapFileName.c_str());
	return file.good();
}

/**
* Actually calls the real map reload function in halo2.exe
*/
void MapManager::reloadAllMaps() {
	typedef char(__thiscall *custom_maps_load_info)(int thisx);
	typedef char(__thiscall *cache_custom_map_file_image_preview)(int thisx);

	DWORD customMapData = h2mod->GetAddress(0x482D70, 0x4A70D8);
	auto p_custom_maps_load_info = h2mod->GetAddress<custom_maps_load_info>(0x4D021, 0x419B5);
	auto p_cache_custom_map_file_image_preview = h2mod->GetAddress<cache_custom_map_file_image_preview>(0x4CC30, 0x41501); // this caches the map preview image for the map selection menu

	LOG_TRACE_GAME("[h2mod-mapmanager] before cache_custom_map_file_image_preview()");
	p_cache_custom_map_file_image_preview((int)customMapData);
	LOG_TRACE_GAME("[h2mod-mapmanager] after cache_custom_map_file_image_preview()");

	LOG_TRACE_GAME("[h2mod-mapmanager] before custom_maps_load_info()");
	p_custom_maps_load_info((int)customMapData);
	LOG_TRACE_GAME("[h2mod-mapmanager] after custom_maps_load_info()");
}

bool MapManager::loadMapInfo(std::wstring& mapFileLocation)
{
	BYTE data[2960];
	ZeroMemory(data, sizeof(data));

	typedef bool(__thiscall* reload_map)(void* a1, BYTE* a2);
	auto p_reload_map = h2mod->GetAddress<reload_map>(0x4CD1E);

	wcscpy_s(reinterpret_cast<wchar_t*>(data + 2432), 256, mapFileLocation.c_str());

	bool mapLoaded = false;
	if (validate_and_add_custom_map(data))
	{
		void* customMapData = h2mod->GetAddress<void*>(0x482D70, 0x4A70D8);
		if (p_reload_map(customMapData, data))
			mapLoaded = true;
	}

	return mapLoaded;
}

void MapManager::getMapFilename(std::wstring& buffer)
{
	wchar_t map_file_location[256];
	network_session* session = nullptr;

	// we want this to work in-game too
	if (/*p_get_lobby_state() == game_lobby_states::in_lobby && */ NetworkSession::getCurrentNetworkSession(&session))
	{
		SecureZeroMemory(map_file_location, sizeof(map_file_location));
		NetworkSession::getMapFileLocation(map_file_location, sizeof(map_file_location));

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

size_t write_data(void *ptr, size_t size, size_t nmemb, FILE *stream) {
	return fwrite(ptr, size, nmemb, stream);
}

static int xferinfo(void *p, curl_off_t dltotal, curl_off_t dlnow, curl_off_t ultotal, curl_off_t ulnow) {
	downloadPercentage = ((double)dlnow / (double)dltotal) * 100;
	return 0;
}

bool MapManager::downloadFromRepo(std::string mapFilename) {
	std::string url(cartographerMapRepoURL + "/");

	FILE *fp = nullptr;
	CURL *curl = nullptr;
	CURLcode res;

	std::wstring mapFilePathWide(h2mod->GetAddress<wchar_t*>(0x482D70 + 0x2423C));
	std::string nonUnicodeMapFilePath(mapFilePathWide.begin(), mapFilePathWide.end());
	nonUnicodeMapFilePath += mapFilename;

	curl = curl_easy_init();
	if (curl) {
		fp = fopen(nonUnicodeMapFilePath.c_str(), "wb");
		if (fp == nullptr) {
			curl_easy_cleanup(curl);
			return false;
		}

		char *url_encoded_map_filename = curl_easy_escape(curl, mapFilename.c_str(), mapFilename.length());
		url += url_encoded_map_filename;
		curl_free(url_encoded_map_filename);

		//fail if 404 or any other type of http error
		curl_easy_setopt(curl, CURLOPT_FAILONERROR, true);
		curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
		curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_data);
		curl_easy_setopt(curl, CURLOPT_XFERINFOFUNCTION, xferinfo);
		curl_easy_setopt(curl, CURLOPT_NOPROGRESS, 0L);
		curl_easy_setopt(curl, CURLOPT_WRITEDATA, fp);
		res = curl_easy_perform(curl);
		long http_code = 0;
		curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &http_code);
		/* always cleanup */
		curl_easy_cleanup(curl);

		fclose(fp);
		if (http_code != 404 && this->loadMapInfo(std::wstring(nonUnicodeMapFilePath.begin(), nonUnicodeMapFilePath.end()))) {
			//if we succesfully downloaded the map, return true
			return true;
		}
		else {
			//if we failed, remove the file we created and exit
			remove(nonUnicodeMapFilePath.c_str());
		}
	}

	return false;
}