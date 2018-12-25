#include "Globals.h"
#include <H2MOD\protobuf\h2mod.pb.h>
#include <fstream>
#include <Mswsock.h>
#include <WS2tcpip.h>
#include "H2MOD\Modules\Config\Config.h"
#include <curl/curl.h>

#pragma comment (lib, "mswsock.lib")

#define DEFAULT_BUFLEN 65536

/* String constants below for client/server messages */
std::string DOWNLOADING_MAP("Downloading Map");
std::string DOWNLOAD_MAP_PERCENTAGE_PREFIX("Downloaded ");
std::string DOWNLOAD_MAP_PERCENTAGE_CHAR("%");
std::string WAITING_FOR_MAP_DOWNLOAD_URL("Waiting for map url from server");
std::string FOUND_MAP_DOWNLOAD_URL("Found map download url");
std::string DOWNLOADING_COMPLETE("Downloading complete");
std::string RELOADING_MAPS("Reloading maps in memory");
std::string UNZIPPING_MAP_DOWNLOAD("Unzipping map download");
std::string FAILED_TO_OPEN_ZIP_FILE("Failed to open the zip file");
std::string STILL_SEARCHING_FOR_MAP("Could not find maps from server, still searching");
std::string COULD_NOT_FIND_MAPS("Couldn't find the map");
std::string UNABLE_TO_CONNECT_TO_SERVER("Unable to connect to server");
std::string BAD_FILE_SIZE("Got bad file size, rejoin");
std::string VALID_FILE_SIZE("Got filesize from host");
std::string CONNECTION_TO_HOST_CLOSED("Connection to host closed");
std::string VALID_FILE_NAME("Got filename from host");
std::string MAP_WRITTEN_TO_DISK("Wrote downloaded map to disk");
std::string CONNECTING_TO_HOST("Connecting to host");
std::string ERROR_CONNECTING_TO_HOST("Socket error connecting to host");
std::string CHECKING_IF_MAP_EXISTS("Checking if the following map exists, %s");
std::string MAP_DOESNT_EXIST_IN_REPO("Download failed, map doesn't exist in repo");

std::string startingDownloadString = ("Downloaded 0%");
std::string validDigits("0123456789");
std::string mapExt(".map");
std::wstring mapExtUnicode(L".map");
std::string fileReadMode("wb");
std::string traceTemplate("%s");
std::string fileSizeDelim("$");

std::wstring CUSTOM_MAP = L"Custom Map";
wchar_t EMPTY_UNICODE_STR = '\0';
std::string EMPTY_STR("");

int downloadPercentage = 0;

/**
* Constructs the map manager for client/servers
*/
MapManager::MapManager() {}

/**
* Download map callback
*/
char __cdecl handle_map_download_callback()
{
	downloadPercentage = 0;

	auto mapDownload = []()
	{
		DWORD* mapDownloadStatus = reinterpret_cast<DWORD*>(h2mod->GetBase() + 0x422570);

		// set the game to downloading map state
		*mapDownloadStatus = -1;

		if (!mapManager->getMapFilenameToDownload().empty())
		{
			TRACE_GAME_N("[h2mod-network] map file name from membership packet %s", mapManager->getMapFilenameToDownload().c_str());
			if (!mapManager->hasCustomMap(mapManager->getMapFilenameToDownload())) {
				//TODO: set map filesize
				//TODO: if downloading from repo files, try p2p
				if (!mapManager->downloadFromRepo(mapManager->getMapFilenameToDownload()))
					h2mod->exit_game(); // download has failed
			}
			else {
				TRACE_GAME_N("[h2mod-network] already has map %s", mapManager->getMapFilenameToDownload().c_str());
			}
			mapManager->setMapFileNameToDownload("");
		}
		else 
		{
			// no map filename (probably packet hasn't been received)
			h2mod->exit_game();
		}

		// set the game to map is loaded state
		*mapDownloadStatus = 0;
	};

	std::thread(mapDownload).detach();

	return 1;
}

/**
* Menu constructor hook
*/
void __cdecl display_map_downloading_menu(int a1, signed int a2, int a3, __int16 a4, int map_download_callback, int leave_game_callback, int a7, int a8, int a9, int a10)
{
	typedef void(__cdecl* map_downloading_menu_constructor)(int a1, signed int a2, int a3, __int16 a4, int a5, int a6, int a7, int a8, int a9, int a10);
	auto p_map_downloading_menu_constructor = (map_downloading_menu_constructor)(h2mod->GetBase() + 0x20E2E0);

	// TODO: place a timer so if the player doesn't choose any option from the menu they get kicked out in order to stop afks or trolls

	p_map_downloading_menu_constructor(a1, a2, a3, a4, reinterpret_cast<int>(handle_map_download_callback), leave_game_callback, a7, a8, a9, a10);
}

int __cdecl get_total_map_downloading_percentage()
{
	return downloadPercentage;
}

wchar_t receiving_map_wstr[] = L"You are receiving the map from %s. \r\nPlease wait...%i%%";
wchar_t* get_receiving_map_string()
{ 
	int(__cdecl* get_default_game_language)() = (int(__cdecl*)())((char*)H2BaseAddr + 0x381fd);
	wchar_t** str_array = (wchar_t**)(h2mod->GetBase() + 0x46575C);

	if (get_default_game_language() == 0) // check if english
		return receiving_map_wstr;

	return str_array[get_default_game_language()];
}

wchar_t repo_wstr[] = L"repository";
void get_map_download_source_str(int a1, wchar_t* buffer)
{	
	if (buffer == NULL)
		return;

	wcsncpy_s(buffer, 512, repo_wstr, -1);
}

/**
* Makes changes to game functionality
*/
void MapManager::applyGamePatches() {

	if (!h2mod->Server) {

		BYTE jmp[1] = { 0xEB };

		WriteBytes(h2mod->GetBase() + 0x215A9E, jmp, 1); /* Allow map download in network */
		WriteBytes(h2mod->GetBase() + 0x215AC9, jmp, 1); /* Disable "Match has begun" bullshit */
		PatchCall(h2mod->GetBase() + 0x244A4A, display_map_downloading_menu); /* Redirect the menu constructor to our code to replace the game's map downloading code callback */

		// code below is for percentage display
		PatchCall(h2mod->GetBase() + 0x244B77, get_total_map_downloading_percentage); /* Redirects map downloading percentage to our custom downloader */
		PatchCall(h2mod->GetBase() + 0x22EE41, get_total_map_downloading_percentage); /* Redirects map downloading percentage to our custom downloader */
		PatchCall(h2mod->GetBase() + 0x244B8F, get_map_download_source_str);
		PatchCall(h2mod->GetBase() + 0x244B9D, get_receiving_map_string);
	}

	// disables game's map downloading implementation
	NopFill<5>(h2mod->GetBase() + (h2mod->Server ? 0x1A917F : 0x1B5421));
}

std::string MapManager::getMapFilenameToDownload()
{
	return this->mapFilenameToDownload;
}

void MapManager::setMapFileNameToDownload(std::string mapFilenameToDownload) {
	this->mapFilenameToDownload = mapFilenameToDownload;
}

/**
* Start listening for tcp clients
*/
void MapManager::startListeningForClients() {
	this->tcpServer = new MapManager::TcpServer();
	this->tcpServer->startListening();
}

/**
* Stop listening for tcp clients if a tcp server exists
*/
void MapManager::stopListeningForClients() {
	if (this->tcpServer != NULL) {
		this->tcpServer->stop();
	}
}

/**
* Stops the tcp server
*/
void MapManager::TcpServer::stop() {
	this->shutdownServerSocket();
	listenerThreadRunning = false;
}

/**
* Gets a copy of the english map name as a wstring
* NOTE - this only works on peers (not dedis)
*/
std::wstring MapManager::getMapName() {
	DWORD currentMapNameOffset;
	if (h2mod->Server) {
		//H2Server.exe+5349B4
		//H2Server.exe+535C64 (another offset to use if the above fails for whatever reason)
		currentMapNameOffset = 0x5349B4;
	}
	else {
		currentMapNameOffset = 0x97737C;
	}
	const wchar_t* currentMapName = (wchar_t*)(h2mod->GetBase() + currentMapNameOffset);

	DWORD dwBack;
	//set r/w access on string so we don't have any issues when we do the implicit copy below
	VirtualProtect((LPVOID)currentMapName, 4, PAGE_READWRITE, &dwBack);
	std::wstring ucurrentMapName(currentMapName);
	VirtualProtect((LPVOID)currentMapName, 4, dwBack, &dwBack);

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
	wchar_t* mapsDirectory = (wchar_t*)(h2mod->GetBase() + 0x482D70 + 0x2423C);

	VirtualProtect(mapsDirectory, 4, PAGE_READWRITE, &dwBack);
	std::wstring mapFileName(mapsDirectory);
	VirtualProtect(mapsDirectory, 4, dwBack, &dwBack);

	mapFileName += mapName;
	std::ifstream file(mapFileName.c_str());
	return file.good();
}

void swap(DWORD*& a, DWORD*& b)
{
	DWORD* c = a;
	a = b;
	b = c;
}

/**
* Actually calls the real map reload function in halo2.exe
*/
void MapManager::reloadMaps() {
	typedef char(__thiscall *map_reload_function_type)(int thisx);
	map_reload_function_type reloadMaps = (map_reload_function_type)(h2mod->GetBase() + (h2mod->Server ? 0x419B5 : 0x4D021));
	map_reload_function_type reloadMapsSet = (map_reload_function_type)(h2mod->GetBase() + (h2mod->Server ? 0x41501 : 0x4CC30));
	DWORD* mapsObject = (DWORD*)(h2mod->GetBase() + (h2mod->Server ? 0x4A70D8 : 0x482D70));
	DWORD dwBack;
	TRACE_GAME("[h2mod-mapmanager] before virtual protect");
	BOOL canprotect = VirtualProtect((WORD*)((int)mapsObject + 148016), sizeof(WORD), PAGE_EXECUTE_READWRITE, &dwBack);
	if (!canprotect && GetLastError()) {
		if (H2Config_debug_log)
			TRACE_GAME_N("[h2mod-mapmanager] reloadMaps - canprotect=%d, error=%d", canprotect, GetLastError());
	}
	TRACE_GAME("[h2mod-mapmanager] after virtual protect");

	TRACE_GAME("[h2mod-mapmanager] before reload map sets");
	EnterCriticalSection(*(LPCRITICAL_SECTION *)(int)mapsObject);
	reloadMapsSet((int)mapsObject);
	LeaveCriticalSection(*(LPCRITICAL_SECTION *)(int)mapsObject);
	TRACE_GAME("[h2mod-mapmanager] after reload maps");

	TRACE_GAME("[h2mod-mapmanager] before reload map");
	EnterCriticalSection(*(LPCRITICAL_SECTION *)(int)mapsObject);
	reloadMaps((int)mapsObject);
	LeaveCriticalSection(*(LPCRITICAL_SECTION *)(int)mapsObject);
	TRACE_GAME("[h2mod-mapmanager] after reload maps");
}

/**
* Resets the map downloading state for clients
*/
void MapManager::resetClient() {
	this->threadRunning = false;
}

/**
* Sets the custom lobby message
*/
void MapManager::setCustomLobbyMessage(const char* newStatus) {
	this->customLobbyMessage = newStatus;
}

/**
* Gets pointer to the custom lobby message
*/
const char* MapManager::getCustomLobbyMessage() {
	return this->customLobbyMessage;
}


std::string MapManager::getMapFilename() {

	int lobby_ptr = 0;
	wchar_t map_file_location[256];

	// we want this to work in-game too
	if (/*p_get_lobby_state() == game_lobby_states::in_lobby && */ get_lobby_globals_ptr(&lobby_ptr))
	{
		memset(map_file_location, NULL, sizeof(map_file_location));
		get_current_lobby_map_file_location(lobby_ptr, map_file_location, sizeof(map_file_location));

		std::wstring unicodeMapFileLocation(map_file_location);
		std::string nonUnicodeMapFileLocation(unicodeMapFileLocation.begin(), unicodeMapFileLocation.end());
		std::size_t mapNameOffset = nonUnicodeMapFileLocation.find_last_of("\\");
		std::size_t mapNameOffsetEnd = nonUnicodeMapFileLocation.find_last_not_of('.');
		std::string nonUnicodeMapFileName = nonUnicodeMapFileLocation.substr(mapNameOffset + 1, mapNameOffsetEnd);
		if (!nonUnicodeMapFileName.empty()) {
			//if the filename exists and the current map english name is equal to the iterated map name
			return nonUnicodeMapFileName;
		}
	}
	
	return "";
}

/**
* Searches for a map to download based on the english map name and the actual filename
* NOTE - only tries to download from host if we haven't downloaded it before in current game session AND
* if the client is in the lobby
*/
void MapManager::searchForMap() {
	threadRunning = true;

	std::wstring currentMapName = this->getMapName();

	if (hasCustomMap(currentMapName)) {
		//if somehow we have the map and made it here, something is wrong, exit
		if (H2Config_debug_log)
			TRACE_GAME_N("[h2mod-mapmanager] Map already exists, should not be searching for it");
		goto cleanup;
	}

	if (isLobby) {
		//only run map searches if client is in lobby
		if (this->downloadFromHost()) {
			goto cleanup;
		}

		this->setCustomLobbyMessage(COULD_NOT_FIND_MAPS.c_str());
	}

cleanup:
	threadRunning = false;
}

/**
* Cleanups client and server download states
*/
void MapManager::cleanup() {
	this->stopListeningForClients();
	this->resetClient();
	//clear any attempted map downloads on game end
	this->downloadedMaps.clear();
}

void MapManager::sendMapInfoPacket()
{
#ifdef _DEBUG
	int tmpFlagOrig = _CrtSetDbgFlag(_CRTDBG_REPORT_FLAG);
	int tmpFlag = tmpFlagOrig;
	tmpFlag &= 0xFFFFFFFF ^ (_CRTDBG_CHECK_ALWAYS_DF | _CRTDBG_LEAK_CHECK_DF);
	_CrtSetDbgFlag(tmpFlag);
#endif

	H2ModPacket teampak;
	teampak.set_type(H2ModPacket_Type_map_info_request);

	h2mod_map_info* map_info = teampak.mutable_map_info();
	//TODO: check if its empty before sending
	std::string mapFilenameStr = this->getMapFilename();
	if (mapFilenameStr.empty()) {
		TRACE_GAME_N("[h2mod-mapmanager] custom map filename missing");
		return;
	}
	const char* mapFilename = mapFilenameStr.c_str();
	TRACE_GAME_N("[h2mod-mapmanager] custom map name being sent %s", mapFilename);
	map_info->set_mapfilename(mapFilenameStr);
	//TODO: send over size so p2p can work easier
	map_info->set_mapsize(0);

	network->send_h2mod_packet(teampak);

#ifdef _DEBUG
	_CrtSetDbgFlag(tmpFlagOrig);
#endif
}

size_t write_data(void *ptr, size_t size, size_t nmemb, FILE *stream) {
	return fwrite(ptr, size, nmemb, stream);
}

static int xferinfo(void *p, curl_off_t dltotal, curl_off_t dlnow, curl_off_t ultotal, curl_off_t ulnow) {
	if (mapManager->precalculatedDownloadPercentageStrings.empty()) {
		for (int i = 0; i <= 100; i++) {
			std::string downloadMsg;
			downloadMsg = DOWNLOAD_MAP_PERCENTAGE_PREFIX;
			downloadMsg += std::to_string(i);
			downloadMsg += DOWNLOAD_MAP_PERCENTAGE_CHAR;
			mapManager->precalculatedDownloadPercentageStrings[i] = downloadMsg;
		}
	}
	downloadPercentage = ((double)dlnow / (double)dltotal) * 100;
	mapManager->setCustomLobbyMessage(mapManager->precalculatedDownloadPercentageStrings[downloadPercentage].c_str());
	return 0;
}

bool MapManager::downloadFromRepo(std::string mapFilename) {
	if (downloadedMaps.find(mapFilename) != downloadedMaps.end()) {
		TRACE_GAME_N("[h2mod-mapmanager] Already tried downloading map %s", mapFilename.c_str());
		return true;
	}
	downloadedMaps.insert(mapFilename);
	std::string url("http://www.h2maps.net/Cartographer/CustomMaps/");
	url += mapFilename;

	CURL *curl;
	FILE *fp;
	CURLcode res;

	DWORD dwBack;
	wchar_t* mapsDirectory = (wchar_t*)(h2mod->GetBase() + 0x482D70 + 0x2423C);
	VirtualProtect(mapsDirectory, 4, PAGE_EXECUTE_READ, &dwBack);

	std::wstring mapFileName(mapsDirectory);
	VirtualProtect(mapsDirectory, 4, dwBack, &dwBack);
	std::string nonUnicodeMapFilePath(mapFileName.begin(), mapFileName.end());
	nonUnicodeMapFilePath += mapFilename;

	curl = curl_easy_init();
	if (curl) {
		fp = fopen(nonUnicodeMapFilePath.c_str(), "wb");
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
		if (http_code != 404) {
			//if we succesfully downloaded the map, reload the maps and return out
			setCustomLobbyMessage(NULL);
			this->reloadMaps();
			return true;
		}
		else {
			//if we failed, remove the file we created and exit
			setCustomLobbyMessage(MAP_DOESNT_EXIST_IN_REPO.c_str());
			remove(nonUnicodeMapFilePath.c_str());
		}
	}
	return false;
}

/**
* Downloads .map file from host
* NOTE - I left the WSACleanup calls commented out to serve as documentation, they are not used cause we don't want to unload the winsock stuff
* at runtime, since there are other parts of the game using this library, and I can't really know if WSAStartup is called before every
* socket call.
*/
bool MapManager::downloadFromHost() {
	bool downloadResult = false;
	SOCKET connectSocket = INVALID_SOCKET;
	try {
		if (precalculatedDownloadPercentageStrings.empty()) {
			for (int i = 0; i <= 100; i++) {
				std::string downloadMsg;
				downloadMsg = DOWNLOAD_MAP_PERCENTAGE_PREFIX;
				downloadMsg += std::to_string(i);
				downloadMsg += DOWNLOAD_MAP_PERCENTAGE_CHAR;
				precalculatedDownloadPercentageStrings[i] = downloadMsg;
			}
		}
		DWORD* mapsObject = (DWORD*)(h2mod->GetBase() + 0x482D70);
		wchar_t* customMapsDirectory = (wchar_t*)((int)mapsObject + 148028);
		std::wstring customMapsDirU(customMapsDirectory);
		std::string customMapsDir(customMapsDirU.begin(), customMapsDirU.end());

		WSADATA wsaData;
		struct addrinfo *result = NULL,
			*ptr = NULL,
			hints;
		int iResult;
		int recvbuflen = DEFAULT_BUFLEN;

		// Initialize Winsock
		iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
		if (iResult != 0) {
			if (H2Config_debug_log)
				TRACE_GAME_N("[h2mod-mapmanager] WSAStartup failed with error: %d\n", iResult);
			return 1;
		}

		ZeroMemory(&hints, sizeof(hints));
		hints.ai_family = AF_UNSPEC;
		hints.ai_socktype = SOCK_STREAM;
		hints.ai_protocol = IPPROTO_TCP;
		std::string addr = inet_ntoa(join_game_xn.ina);
		std::string prt = std::to_string(ntohs(join_game_xn.wPortOnline) + 9);
		if (H2Config_debug_log)
			TRACE_GAME_N("[h2mod-mapmanager] Client map dl, addr=%s, port=%s", addr.c_str(), prt.c_str());
		// Resolve the server address and port
		iResult = getaddrinfo(addr.c_str(), prt.c_str(), &hints, &result);
		if (iResult != 0) {
			if (H2Config_debug_log)
				TRACE_GAME_N("[h2mod-mapmanager] getaddrinfo failed with error: %d\n", iResult);
			//WSACleanup();
			return 1;
		}

		// Attempt to connect to an address until one succeeds
		for (ptr = result; ptr != NULL; ptr = ptr->ai_next) {

			// Create a SOCKET for connecting to server
			connectSocket = socket(ptr->ai_family, ptr->ai_socktype,
				ptr->ai_protocol);
			if (connectSocket == INVALID_SOCKET) {
				if (H2Config_debug_log)
					TRACE_GAME_N("[h2mod-mapmanager] socket failed with error: %d", WSAGetLastError());
				//WSACleanup();
				return 1;
			}

			// Connect to server
			this->setCustomLobbyMessage(CONNECTING_TO_HOST.c_str());
			iResult = connect(connectSocket, ptr->ai_addr, (int)ptr->ai_addrlen);
			if (iResult == SOCKET_ERROR) {
				closesocket(connectSocket);
				connectSocket = INVALID_SOCKET;
				this->setCustomLobbyMessage(ERROR_CONNECTING_TO_HOST.c_str());
				continue;
			}
			break;
		}

		freeaddrinfo(result);

		if (connectSocket == INVALID_SOCKET) {
			if (H2Config_debug_log)
				TRACE_GAME_N("[h2mod-mapmanager] %s", UNABLE_TO_CONNECT_TO_SERVER.c_str());
			this->setCustomLobbyMessage(UNABLE_TO_CONNECT_TO_SERVER.c_str());
			//WSACleanup();
			return 1;
		}

		BOOL receivedFilename = false;
		BOOL receivedFileSize = false;
		std::string mapName("");
		int fileSize;
		int bytesRead = 0;
		FILE* file;

		// Receive until the peer closes the connection
		int prevDownloadPercentage = 0;
		const char* downloadLobbyMessage = startingDownloadString.c_str();

		do {
			char recvbuf[DEFAULT_BUFLEN];

			iResult = recv(connectSocket, recvbuf, recvbuflen, 0);
			if (iResult > 0) {
				this->setCustomLobbyMessage(downloadLobbyMessage);
				//TRACE_GAME_N("Bytes received: %d", iResult);
				if (receivedFilename && receivedFileSize) {
					for (int i = 0; i < iResult; i++) {
						fseek(file, bytesRead++, SEEK_SET);
						fputc(recvbuf[i], file);
						int downloadPercentage = ((double)bytesRead / (double)fileSize) * 100;
						if (prevDownloadPercentage != downloadPercentage) {
							downloadLobbyMessage = precalculatedDownloadPercentageStrings[downloadPercentage].c_str();
							prevDownloadPercentage = downloadPercentage;
						}
					}
				}
				else {
					//the first/second batch of bytes received should be the filename and the file size of the map
					if (!receivedFilename) {
						mapName = std::string(recvbuf);
						std::size_t extOffset = mapName.find(mapExt.c_str());
						mapName = mapName.substr(0, extOffset);
						receivedFilename = true;
						this->setCustomLobbyMessage(VALID_FILE_NAME.c_str());


						mapName += mapExt.c_str();
						std::string mapPath = customMapsDir;
						mapPath += mapName;
						file = fopen(mapPath.c_str(), fileReadMode.c_str());
						continue;
					}

					if (!receivedFileSize) {
						std::string recv(recvbuf);
						//after file size is sent, we mark the end of the bytes for size with the character $, the rest of the characters would be actual
						//map data
						size_t index = recv.find(fileSizeDelim);
						std::string fileSizee = recv.substr(0, index);
						bool has_only_digits = (fileSizee.find_first_not_of(validDigits.c_str()) == std::string::npos);
						if (!has_only_digits) {
							if (H2Config_debug_log)
								TRACE_GAME_N("[h2mod-mapmanager] %s", BAD_FILE_SIZE.c_str());
							this->setCustomLobbyMessage(BAD_FILE_SIZE.c_str());
							goto end;
						}
						if (fileSizee.empty()) {
							if (H2Config_debug_log)
								TRACE_GAME_N("[h2mod-mapmanager] %s", BAD_FILE_SIZE.c_str());
							this->setCustomLobbyMessage(BAD_FILE_SIZE.c_str());
						}
						fileSize = stoi(fileSizee);
						receivedFileSize = true;
						for (int i = index + 1; i < iResult; i++) {
							fseek(file, bytesRead++, SEEK_SET);
							fputc(recvbuf[i], file);
						}
						this->setCustomLobbyMessage(VALID_FILE_SIZE.c_str());
						continue;
					}
				}
			}
			else if (iResult == 0) {
				if (H2Config_debug_log)
					TRACE_GAME_N("[h2mod-mapmanager] %s", CONNECTION_TO_HOST_CLOSED.c_str());
				this->setCustomLobbyMessage(CONNECTION_TO_HOST_CLOSED.c_str());
			}
			else {
				if (H2Config_debug_log)
					TRACE_GAME_N("[h2mod-mapmanager] recv failed with error: %d", WSAGetLastError());
				this->setCustomLobbyMessage("[h2mod-mapmanager] Failure receiving, try rejoining");
			}
		} while (iResult > 0);

		fclose(file);

		this->setCustomLobbyMessage(DOWNLOADING_COMPLETE.c_str());
		this->setCustomLobbyMessage(RELOADING_MAPS.c_str());
		this->reloadMaps();
		downloadResult = true;
		goto end;
	}
	catch (std::exception const& e) {
		if (H2Config_debug_log) {
			this->setCustomLobbyMessage("[h2mod-mapmanager] std exception thrown");
			TRACE_GAME_N("[h2mod-mapmanager] std exception thrown = %s", e.what());
		}
	}
	catch (...) {
		if (H2Config_debug_log) {
			this->setCustomLobbyMessage("[h2mod-mapmanager] Unknown error occurred");
			TRACE_GAME_N("[h2mod-mapmanager] unknown exception occurred occurred");
		}
	}
end:
	// cleanup
	closesocket(connectSocket);
	//WSACleanup();
	return downloadResult;
}

void MapManager::startMapDownload() {
	if (!threadRunning) {
		std::thread t1(&MapManager::searchForMap, this);
		t1.detach();
	}
}

void handleClientConnection(SOCKET socket, std::wstring customMapName) {
	HANDLE file;
	int result;
	try {
		file = CreateFile(customMapName.c_str(), GENERIC_READ, FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE, 0, OPEN_EXISTING, 0, 0);

		if (file == INVALID_HANDLE_VALUE) {
			if (H2Config_debug_log)
				TRACE_GAME("[h2mod-mapmanager] Unable to open file %s for read", customMapName.c_str());
			return;
		}

		int fileSize = GetFileSize(file, NULL);
		if (fileSize == INVALID_FILE_SIZE) {
			if (H2Config_debug_log)
				TRACE_GAME_N("[h2mod-mapmanager] invalid file size, %d", GetLastError());
			return;
		}

		std::string nonUnicodeCustomMapFilename(customMapName.begin(), customMapName.end());
		std::size_t offset = nonUnicodeCustomMapFilename.find_last_of("\\");
		std::size_t extOffset = nonUnicodeCustomMapFilename.find_last_not_of(mapExt);
		std::string mapName = nonUnicodeCustomMapFilename.substr(offset + 1, extOffset);

		std::string buf = mapName;
		std::string buf2 = std::to_string(fileSize);
		buf2 += fileSizeDelim;

		result = send(socket, mapName.c_str(), (int)strlen(mapName.c_str()), 0);
		if (result == SOCKET_ERROR) {
			if (H2Config_debug_log)
				TRACE_GAME_N("[h2mod-mapmanager] filename send failed with error: %d", WSAGetLastError());
			result = send(socket, mapName.c_str(), (int)strlen(mapName.c_str()), 0);
			if (result == SOCKET_ERROR) {
				if (H2Config_debug_log)
					TRACE_GAME_N("[h2mod-mapmanager] filename retry send failed with error: %d", WSAGetLastError());
				return;
			}
		}

		result = send(socket, buf2.c_str(), (int)strlen(buf2.c_str()), 0);
		if (result == SOCKET_ERROR) {
			if (H2Config_debug_log)
				TRACE_GAME_N("[h2mod-mapmanager] filesize send failed with error: %d", WSAGetLastError());
			result = send(socket, buf2.c_str(), (int)strlen(buf2.c_str()), 0);
			if (result == SOCKET_ERROR) {
				if (H2Config_debug_log)
					TRACE_GAME_N("[h2mod-mapmanager] filesize retry send failed with error: %d", WSAGetLastError());
				return;
			}
			else {
				goto sendFile;
			}
		}
		else {
		sendFile:
			try {
				if (TransmitFile(socket, file, fileSize, DEFAULT_BUFLEN, NULL, NULL, TF_DISCONNECT)) {
					if (H2Config_debug_log)
						TRACE_GAME_N("[h2mod-mapmanager] Transmit file succeeded");
				}
				else {
					if (H2Config_debug_log)
						TRACE_GAME_N("[h2mod-mapmanager] Transmit file failed, errorCode=%d", WSAGetLastError());
				}
			}
			catch (...) {
				if (H2Config_debug_log)
					TRACE_GAME_N("[h2mod-mapmanager] c++ exception thrown");
			}
		}

		goto cleanup;
	}
	catch (std::exception const& e) {
		if (H2Config_debug_log)
			TRACE_GAME_N("[h2mod-mapmanager] std exception thrown = %s", e.what());
	}
	catch (...) {
		if (H2Config_debug_log)
			TRACE_GAME_N("[h2mod-mapmanager] unknown exception occurred occurred");
	}
cleanup:
	//close the file
	if (CloseHandle(file)) {
		if (H2Config_debug_log)
			TRACE_GAME_N("[h2mod-mapmanager] closed file handle");
	}
	else {
		if (H2Config_debug_log)
			TRACE_GAME_N("[h2mod-mapmanager] unable to close file");
	}
	// shutdown the connection since we're done
	result = shutdown(socket, SD_SEND);
	if (result == SOCKET_ERROR) {
		if (H2Config_debug_log)
			TRACE_GAME_N("[h2mod-mapmanager] shutdown failed with error: %d", WSAGetLastError());
		closesocket(socket);
		return;
	}

	if (closesocket(socket) == SOCKET_ERROR) {
		if (H2Config_debug_log)
			TRACE_GAME_N("[h2mod-mapmanager] close of socket failed with error: %d", WSAGetLastError());
	}
}

/**
* Shutdowns the tcp server socket
*/
void MapManager::TcpServer::shutdownServerSocket() {
	if (serverSocket != NULL) {
		closesocket(serverSocket);
		//shut down once
		serverSocket = NULL;
	}
}

/**
* Start listening for tcp clients
*/
void MapManager::TcpServer::startListening() {
	struct addrinfo *result = NULL;
	struct addrinfo hints;
	int iResult;
	int port = H2Config_base_port + 9;           /* port number to use */
	int client;                  /* file descriptor for socket */
	off_t offset = 0;          /* file offset */

	WSADATA wsaData;

	// Initialize Winsock
	iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (iResult != 0) {
		if (H2Config_debug_log)
			TRACE_GAME_N("[h2mod-mapmanager] WSAStartup failed with error: %d", iResult);
		return;
	}

	ZeroMemory(&hints, sizeof(hints));
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP;
	hints.ai_flags = AI_PASSIVE;

	// Resolve the server address and port
	iResult = getaddrinfo(NULL, std::to_string(port).c_str(), &hints, &result);
	if (iResult != 0) {
		if (H2Config_debug_log)
			TRACE_GAME_N("[h2mod-mapmanager] getaddrinfo failed with error: %d, port:", iResult, std::to_string(port).c_str());
		//WSACleanup();
		return;
	}

	// Create a SOCKET for connecting to server
	serverSocket = socket(result->ai_family, result->ai_socktype, result->ai_protocol);
	if (serverSocket == INVALID_SOCKET) {
		if (H2Config_debug_log)
			TRACE_GAME_N("[h2mod-mapmanager] socket failed with error: %ld", WSAGetLastError());
		freeaddrinfo(result);
		serverSocket = NULL;
		//WSACleanup();
		return;
	}

	/* bind socket to the port */
	iResult = bind(serverSocket, result->ai_addr, (int)result->ai_addrlen);
	if (iResult == SOCKET_ERROR) {
		if (H2Config_debug_log)
			TRACE_GAME_N("[h2mod-mapmanager] unable to bind to socket: %s", strerror(errno));
		freeaddrinfo(result);
		closesocket(serverSocket);
		serverSocket = NULL;
		return;
	}

	freeaddrinfo(result);

	/* listen for clients on the socket */
	iResult = listen(serverSocket, SOMAXCONN);
	if (iResult == SOCKET_ERROR) {
		if (H2Config_debug_log)
			TRACE_GAME_N("[h2mod-mapmanager] Error trying to listen on port: %s", strerror(errno));
		closesocket(serverSocket);
		serverSocket = NULL;
		return;
	}

	if (H2Config_debug_log)
		TRACE_GAME_N("[h2mod-mapmanager] Listening on port: %d", port);

	while (listenerThreadRunning) {
		/* wait for a client to connect */
		client = accept(serverSocket, 0, 0);
		if (client == -1) {
			if (H2Config_debug_log)
				TRACE_GAME_N("[h2mod-mapmanager] accept failed: %s", strerror(errno));
			//try to accept again
			//TODO: if it fails too many times, exit
			continue;
		}

		//std::thread t1(&handleClientConnection, client, customMapName);
		//t1.detach();
	}
}