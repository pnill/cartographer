#include "stdafx.h"

#include "MapManager.h"

#include "Globals.h"
#include <Mswsock.h>
#include <WS2tcpip.h>
#include "H2MOD\Modules\Config\Config.h"
#include <curl/curl.h>
#include "XLive\xnet\IpManagement\XnIp.h"
#include "..\Networking\Networking.h"
#include "H2MOD\Modules\Networking\NetworkSession\NetworkSession.h"

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
std::string EMPTY_STR("");

bool mapDownloadCountdown = false;
auto promptOpenTime = std::chrono::high_resolution_clock::now();
int downloadPercentage = 0;

void MapManager::leaveSessionIfAFK() {
	if (mapDownloadCountdown) {
		auto duraton = std::chrono::duration_cast<std::chrono::seconds>(std::chrono::high_resolution_clock::now() - promptOpenTime);
		if (duraton >= std::chrono::seconds(20)) {
			h2mod->exit_game();
			mapDownloadCountdown = false;
		}
	}
}

/**
* Download map callback
*/
char __cdecl handle_map_download_callback() {
	downloadPercentage = 0;
	mapDownloadCountdown = false;

	auto mapDownload = []()
	{
		DWORD* mapDownloadStatus = h2mod->GetAddress<DWORD*>(0x422570);

		// set the game to downloading map state
		*mapDownloadStatus = -1;

		if (!mapManager->getMapFilenameToDownload().empty())
		{
			LOG_TRACE_NETWORK("[h2mod-network] map file name from membership packet {}", mapManager->getMapFilenameToDownload());
			if (!mapManager->hasCustomMap(mapManager->getMapFilenameToDownload())) {
				//TODO: set map filesize
				//TODO: if downloading from repo files, try p2p
				if (!mapManager->downloadFromRepo(mapManager->getMapFilenameToDownload()))
					h2mod->exit_game(); // download has failed
			}
			else {
				LOG_TRACE_NETWORK("[h2mod-network] already has map {}", mapManager->getMapFilenameToDownload());
			}
			mapManager->clearMapFileNameToDownload();
		}
		else 
		{
			// no map filename (probably packet hasn't been received)
			h2mod->exit_game();
		}

		// set the game to map is loaded state
		*mapDownloadStatus = 0;
	};

	if (NetworkSession::getCurrentNetworkSession()->local_session_state != 0)
		std::thread(mapDownload).detach();

	return 1;
}

void* leavegame_callback_ptr;
char leavegame_callback()
{
	auto p_leave_game = (char(*)())(leavegame_callback_ptr);
	mapDownloadCountdown = false;
	return p_leave_game();
}

/**
* Menu constructor hook
*/
void __cdecl display_map_downloading_menu(int a1, signed int a2, int a3, __int16 a4, int map_download_callback, int leave_game_callback, int a7, int a8, int a9, int a10)
{
	typedef void(__cdecl map_downloading_menu_constructor)(int a1, signed int a2, int a3, __int16 a4, int a5, int a6, int a7, int a8, int a9, int a10);
	auto p_map_downloading_menu_constructor = h2mod->GetAddress<map_downloading_menu_constructor*>(0x20E2E0);

	mapDownloadCountdown = true;
	promptOpenTime = std::chrono::high_resolution_clock::now();
	CustomPackets::sendRequestMapFilename();
	leavegame_callback_ptr = (void*)leave_game_callback;
	p_map_downloading_menu_constructor(a1, a2, a3, a4, reinterpret_cast<int>(handle_map_download_callback), leave_game_callback, a7, a8, a9, a10);
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
void MapManager::applyGamePatches() {

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
	DWORD dwBack;
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
	char data[2960];
	ZeroMemory(data, sizeof(data));

	typedef bool(__thiscall* reload_map)(void* a1, char* a2);
	auto p_reload_map = h2mod->GetAddress<reload_map>(0x4CD1E);

	typedef bool(__cdecl* get_map_info)(char* a1);
	auto p_get_map_info = h2mod->GetAddress<get_map_info>(0x4F690);

	wcscpy_s(reinterpret_cast<wchar_t*>(data + 2432), 256, mapFileLocation.c_str());

	bool result = false;
	if (p_get_map_info(data))
	{
		void* customMapData = h2mod->GetAddress<void*>(0x482D70, 0x4A70D8);
		if (p_reload_map(customMapData, data))
			result = true;
	}

	return result;
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
	std::string url("http://www.h2maps.net/Cartographer/CustomMaps/");
	url += mapFilename;

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