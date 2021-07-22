#include "stdafx.h"

#include "MapManager.h"

#include "H2MOD\Modules\Config\Config.h"
#include "XLive\xnet\IpManagement\XnIp.h"
#include "..\Networking\Networking.h"
#include "H2MOD/Modules/OnScreenDebug/OnscreenDebug.h"

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


#pragma region custom map checks

bool open_cache_header(const wchar_t *lpFileName, tags::cache_header *cache_header_ptr, HANDLE *map_handle)
{
	typedef char(__cdecl open_cache_header)(const wchar_t *lpFileName, tags::cache_header *lpBuffer, HANDLE *map_handle, DWORD NumberOfBytesRead);
	auto open_cache_header_impl = Memory::GetAddress<open_cache_header*>(0x642D0, 0x4C327);
	return open_cache_header_impl(lpFileName, cache_header_ptr, map_handle, 0);
}

void close_cache_header(HANDLE *map_handle)
{
	typedef void __cdecl close_cache_header(HANDLE *a1);
	auto close_cache_header_impl = Memory::GetAddress<close_cache_header*>(0x64C03, 0x4CC5A);
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
	if (!header.is_multiplayer() && !header.is_single_player())
	{
		LOG_TRACE_FUNCW(L"\"{}\" is not playable", file_name);
		return false;
	}

	close_cache_header(&map_cache_handle);
	// needed because the game loads the human readable map name and description from scenario after checks
	// without this the map is just called by it's file name

	// todo move the code for loading the descriptions to our code and get rid of this
	typedef int __cdecl validate_and_add_custom_map_interal(BYTE *a1);
	auto validate_and_add_custom_map_interal_impl = Memory::GetAddress<validate_and_add_custom_map_interal*>(0x4F690, 0x56890);
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
		wcsncpy_s(reinterpret_cast<wchar_t*>(a1 + 32), 0x20, fallback_name.c_str(), fallback_name.length());
	}
	// load the map even if some of the checks failed, will still mostly work
	return true;
}

bool __cdecl is_supported_build(const char *build)
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
signed int __cdecl get_map_load_status_for_all_peers_hook(signed int *out_smallest_load_percentage, unsigned int *out_host_map_status) {
	// this just gets the current network_session, but has some extra misc checks
	typedef bool(__cdecl* get_network_session_with_misc_checks)(network_session**);
	auto p_get_network_session_with_misc_checks = Memory::GetAddress<get_network_session_with_misc_checks>(0x1AD782, 0x1A66FF);

	network_session* session = nullptr;
	int result_map_status = 0;
	int result_map_percentage = 0;
	bool someone_downloading_map = false;

	if (p_get_network_session_with_misc_checks(&session)) {
		switch (session->local_session_state)
		{
		case network_session_state_none:
		case network_session_state_peer_joining:
		case network_session_state_peer_join_abort:
		case network_session_state_election:
		case network_session_state_unk_2:
			break;

		case network_session_state_peer_established:
		case network_session_state_peer_leaving:
		case network_session_state_host_established:
		case network_session_state_host_disband:
		case network_session_state_host_handoff:
		case network_session_state_host_reestablish:
			membership_info* membership = &session->membership;

			if (out_host_map_status)
				*out_host_map_status = membership->peer_info[session->session_host_peer_index].map_status;

			result_map_status = map_loaded;

			// i don't think this is really used anymore, it has been replaced by the load screen in H2v from Xbox
			result_map_percentage = 100; 
			
			for (int i = 0; i < session->membership.peer_count; i++) {
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

	if (out_smallest_load_percentage)
		*out_smallest_load_percentage = result_map_percentage;

	return result_map_status;
}

// this is actually thiscall, but the parameter is unused
bool __stdcall get_map_load_status_for_all_peers_hook_2(int a1, network_session *session, DWORD *peers_that_cant_load_map_flags)
{
	int result_bitflags = 0;
	bool all_peers_can_load_map = true;

	membership_info* membership = &session->membership;

	if (membership->peer_count > 0) {
		for (int i = 0; i < membership->peer_count; i++) {
			// check only session host and local peer indexes
			if (i == session->local_peer_index
				|| i == session->session_host_peer_index) {
				switch (membership->peer_info[i].map_status)
				{
				case unk_0:
				case map_unavailable:
				case map_someone_loading:
				case map_is_downloading:
					result_bitflags |= FLAG(i);
					all_peers_can_load_map = false;
					break;
				case map_available:
				case map_loaded:
					break;
				}
			}
		}
	}
	if (peers_that_cant_load_map_flags)
		*peers_that_cant_load_map_flags = result_bitflags;

	return all_peers_can_load_map;
}

__declspec(naked) void get_map_load_status_for_all_peers_hook_2_to_stdcall() {
	__asm {
		mov esi, [esp+4+4] // local stack + parameter
		push esi
		mov esi, [esp+8+0] // local stack + parameter
		push esi
		push ecx // push this on stack
		call get_map_load_status_for_all_peers_hook_2 // this will clear 12 bytes off the stack
		retn 8 // clear the remaining 8 that we pushed previously
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
	mapManager->addDownloadQuery(L"");
	auto query = mapManager->getLastDownloadQueryAdded();
	// request the filename
	CustomPackets::sendRequestMapFilename(query->id);
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
	if (!mapManager->m_mapDownloadQueue.empty())
		return mapManager->m_mapDownloadQueue.front()->GetDownloadPercentage();
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
void* unknown_xbox_live_data1 = nullptr;
void __declspec(naked) load_map_data_for_display() {
	__asm {
		mov eax, [esp + 0x10] // grab map_data pointer from stack
		mov ecx, [eax + 0x964] // mov bitmap pointer into ecx
		mov[ebx], ecx // mov bitmap pointer into map_data on stack
		mov eax, unknown_xbox_live_data1
		ret // return to original load_map_data_for display function
	}
}
/**
* Makes changes to game functionality
*/
void MapManager::applyHooks() {

	if (!Memory::isDedicatedServer()) {

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
		unknown_xbox_live_data1 = Memory::GetAddress<void*>(0x9712C8);
	}

	// Both server and client
	WriteJmpTo(Memory::GetAddress(0x1467, 0x12E2), is_supported_build);
	PatchCall(Memory::GetAddress(0x1E49A2, 0x1EDF0), validate_and_add_custom_map);
	PatchCall(Memory::GetAddress(0x4D3BA, 0x417FE), validate_and_add_custom_map);
	PatchCall(Memory::GetAddress(0x4CF26, 0x41D4E), validate_and_add_custom_map);
	PatchCall(Memory::GetAddress(0x8928, 0x1B6482), validate_and_add_custom_map);

	// allow host to start the game, even if there are peers that didn't load the map
	p_get_map_load_status_for_all_peers = (get_map_load_status_for_all_peers)DetourFunc(Memory::GetAddress<BYTE*>(0x1B1929, 0x197879), (BYTE*)get_map_load_status_for_all_peers_hook, 8);
	WriteJmpTo(Memory::GetAddress<BYTE*>(0x1D76C5, 0x1BCD32), get_map_load_status_for_all_peers_hook_2_to_stdcall);

	// disables game's map downloading implementation
	NopFill(Memory::GetAddress(0x1B5421, 0x1A917F), 5);
}

/**
* Actually calls the real map reload function in halo2.exe
*/
void MapManager::reloadAllMaps() {
	typedef char(__thiscall *custom_maps_load_info)(int thisx);
	typedef char(__thiscall *cache_custom_map_file_image_preview)(int thisx);

	DWORD customMapData = Memory::GetAddress(0x482D70, 0x4A70D8);
	auto p_custom_maps_load_info = Memory::GetAddress<custom_maps_load_info>(0x4D021, 0x419B5);
	auto p_cache_custom_map_file_image_preview = Memory::GetAddress<cache_custom_map_file_image_preview>(0x4CC30, 0x41501); // this caches the map preview image for the map selection menu

	LOG_TRACE_GAME("[h2mod-mapmanager] before cache_custom_map_file_image_preview()");
	p_cache_custom_map_file_image_preview((int)customMapData);
	LOG_TRACE_GAME("[h2mod-mapmanager] after cache_custom_map_file_image_preview()");

	LOG_TRACE_GAME("[h2mod-mapmanager] before custom_maps_load_info()");
	p_custom_maps_load_info((int)customMapData);
	LOG_TRACE_GAME("[h2mod-mapmanager] after custom_maps_load_info()");
}

bool MapManager::loadMapInfo(std::wstring& mapFileLocation) {
	BYTE data[2960];
	ZeroMemory(data, sizeof(data));

	typedef bool(__thiscall* reload_map)(void* a1, BYTE* a2);
	auto p_reload_map = Memory::GetAddress<reload_map>(0x4CD1E);

	wcscpy_s(reinterpret_cast<wchar_t*>(data + 2432), 256, mapFileLocation.c_str());

	bool mapLoaded = false;
	if (validate_and_add_custom_map(data)) {
		void* customMapData = Memory::GetAddress<void*>(0x482D70, 0x4A70D8);
		if (p_reload_map(customMapData, data)){
			mapLoaded = true;
		}
		else {
			// TODO: try removing a map from the 50 map cache, then try loading the map again
			// until 50 map limit is removed
		}
	}

	return mapLoaded;
}

void MapManager::getMapFilename(std::wstring& buffer) {
	wchar_t map_file_location[256];
	network_session* session = nullptr;

	// we want this to work in-game too
	if (/*p_get_lobby_state() == game_lobby_states::in_lobby && */ NetworkSession::getCurrentNetworkSession(&session)) {
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

void MapManager::MapDownloadUpdateTick()
{
	for (auto it = m_mapDownloadQueue.begin(); it != m_mapDownloadQueue.end(); it++)
	{
		MapDownloadQuery& query = **it;
		if (query.m_downloadFinished)
		{
			addDebugText("deleting map download query");
			m_mapDownloadQueue.erase(it);
		}
	}
}

bool MapDownloadQuery::ShouldStopDownload() {
	return m_forceStopDownload == true || !NetworkSession::localPeerIsEstablished();
}

void MapDownloadQuery::StopDownload() {
	m_forceStopDownload = true;
};

MapDownloadQuery::MapDownloadQuery(std::wstring& _mapToDownload, int _downloadId)
	: id(_downloadId)
{
	SetMapNameToDownload(_mapToDownload);
}

MapDownloadQuery::MapDownloadQuery(MapDownloadQuery& other) :
	m_clientMapFilename(other.m_clientMapFilename),
	m_clientMapFilenameWide(other.m_clientMapFilenameWide),
	m_downloadPercentage(other.m_downloadPercentage)
{
	bool _forceStopDownload = other.m_forceStopDownload;
	m_forceStopDownload = _forceStopDownload;
}

MapDownloadQuery::~MapDownloadQuery() {
	addDebugText("Calling deconstructor for: %s", __FUNCTION__);
}

size_t write_data(void *ptr, size_t size, size_t nmemb, FILE *stream) {
	return fwrite(ptr, size, nmemb, stream);
}

static int xferinfo(void *p, curl_off_t dltotal, curl_off_t dlnow, curl_off_t ultotal, curl_off_t ulnow) {
	MapDownloadQuery* mapDownloadQuery = (MapDownloadQuery*)p;
	mapDownloadQuery->SetDownloadPercentage(((double)dlnow / (double)dltotal) * 100);
	return mapDownloadQuery->ShouldStopDownload();
}

bool MapDownloadQuery::downloadFromRepo() {
	std::string url(cartographerMapRepoURL + "/");

	FILE *fp = nullptr;
	CURL *curl = nullptr;
	CURLcode res;

	std::wstring mapFilePathWide(Memory::GetAddress<wchar_t*>(0x482D70 + 0x2423C));
	std::string nonUnicodeMapFilePath(mapFilePathWide.begin(), mapFilePathWide.end());
	nonUnicodeMapFilePath += m_clientMapFilename;

	curl = curl_interface_init_no_ssl();
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
			if (http_code != 404 && mapManager->loadMapInfo(mapFilePathWide + m_clientMapFilenameWide)) {
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
			if (!downloadFromRepo() 
				&& !NetworkSession::localPeerIsSessionHost())
			{
				LOG_TRACE_GAME("[h2mod-mapmanager] {}() - {}() failed, leaving session!",
					STRINGIFY(handle_map_download_callback),
					STRINGIFY(downloadFromRepo));

				addDebugText("Failed to download custom map %s .", m_clientMapFilename.c_str());
				h2mod->leave_session(); // download has failed
			}
		}
		else {
			// no map filename (probably packet hasn't been received)
			addDebugText("Failed to download custom map, no filename to download.");
			LOG_TRACE_GAME("[h2mod-mapmanager] no map filename received from host!");
			h2mod->leave_session();
		}

		// set the game to map is loaded state
		*mapDownloadStatus = 0;
		m_downloadFinished = true;
	};

	if (NetworkSession::localPeerIsEstablished())
		std::thread(mapDownloadThread).detach();
	else
		m_downloadFinished = true;
}


void MapDownloadQuery::SetMapNameToDownload(std::wstring& _mapNameToDownloadWide)
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
