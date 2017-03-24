#include "Globals.h"
#include <WinInet.h>
#include <h2mod.pb.h>
#include <fstream>
#include <Urlmon.h>
#include <Mswsock.h>
#include <WS2tcpip.h>

#pragma comment (lib, "Wininet.lib")
#pragma comment (lib, "urlmon.lib")
#pragma comment (lib, "mswsock.lib")
#define POST 1
#define GET 0

#include <miniz.c>
extern UINT g_port;

std::string DOWNLOADING_MAP("Downloading Map");
std::string WAITING_FOR_MAP_DOWNLOAD_URL("Waiting for map url from server");
std::string FOUND_MAP_DOWNLOAD_URL("Found map download url");
std::string DOWNLOADING_COMPLETE("Downloading complete");
std::string RELOADING_MAPS("Reloading maps in memory");
std::string UNZIPPING_MAP_DOWNLOAD("Unzipping map download");
std::string FAILED_TO_OPEN_ZIP_FILE("Failed to open the zip file");
std::string STILL_SEARCHING_FOR_MAP("Could not find maps from server, still searching");
std::string COULD_NOT_FIND_MAPS("Couldn't find the map");

std::wstring CUSTOM_MAP = L"Custom Map";
wchar_t empty2 = '\0';

/*
typedef struct _XLOCATOR_SEARCHRESULT {
ULONGLONG serverID;                     // the ID of dedicated server
DWORD dwServerType;                     // see XLOCATOR_SERVERTYPE_PUBLIC, etc
XNADDR serverAddress;                   // the address dedicated server
XNKID xnkid;
XNKEY xnkey;
DWORD dwMaxPublicSlots;
DWORD dwMaxPrivateSlots;
DWORD dwFilledPublicSlots;
DWORD dwFilledPrivateSlots;
DWORD cProperties;                      // number of custom properties.
PXUSER_PROPERTY pProperties;            // an array of custom properties.
} XLOCATOR_SEARCHRESULT, *PXLOCATOR_SEARCHRESULT;
*/
/*
WARNING! You MUST free memory allocated to returning type (string pointer of server reply)
*/
char** Request(int Method, LPCSTR Host, LPCSTR url, LPCSTR header, LPSTR data)
{
	DWORD messageTotalSize = 1;
	BYTE* message = (BYTE*)malloc(messageTotalSize);
	message[0] = '\0';

	try {
		//Retrieve default http user agent
		char httpUseragent[512];
		DWORD szhttpUserAgent = sizeof(httpUseragent);
		ObtainUserAgentString(0, httpUseragent, &szhttpUserAgent);

		char m[5];

		if (Method == GET)
			strcpy(m, "GET");
		else
			strcpy(m, "POST");


		HINTERNET internet = InternetOpenA(httpUseragent, INTERNET_OPEN_TYPE_PRECONFIG, NULL, NULL, 0);
		if (internet != NULL)
		{

			HINTERNET connect = InternetConnectA(internet, Host, INTERNET_DEFAULT_HTTP_PORT, NULL, NULL, INTERNET_SERVICE_HTTP, 0, 0);
			if (connect != NULL)
			{
				HINTERNET request = HttpOpenRequestA(connect, m, url, "HTTP/1.1", NULL, NULL,
					INTERNET_FLAG_HYPERLINK | INTERNET_FLAG_IGNORE_CERT_CN_INVALID |
					INTERNET_FLAG_IGNORE_CERT_DATE_INVALID |
					INTERNET_FLAG_IGNORE_REDIRECT_TO_HTTP |
					INTERNET_FLAG_IGNORE_REDIRECT_TO_HTTPS |
					INTERNET_FLAG_NO_AUTH |
					INTERNET_FLAG_NO_CACHE_WRITE |
					INTERNET_FLAG_NO_UI |
					INTERNET_FLAG_PRAGMA_NOCACHE |
					INTERNET_FLAG_RELOAD, NULL);

				if (request != NULL)
				{
					int datalen = 0;
					if (data != NULL) datalen = strlen(data);
					int headerlen = 0;
					if (header != NULL) headerlen = strlen(header);

					HttpSendRequestA(request, header, headerlen, data, datalen);

					DWORD dwBytesAvailable;
					while (InternetQueryDataAvailable(request, &dwBytesAvailable, 0, 0))
					{
						BYTE *pMessageBody = (BYTE *)malloc(dwBytesAvailable + 1);

						DWORD dwBytesRead;
						BOOL bResult = InternetReadFile(request, pMessageBody,
							dwBytesAvailable, &dwBytesRead);
						if (!bResult)
						{
							//fprintf(stderr, "InternetReadFile failed, error = %d (0x%x)\n",
							//	GetLastError(), GetLastError());
							break;
						}

						if (dwBytesRead == 0)
							break;	// End of File.

						pMessageBody[dwBytesRead] = '\0';

						message = (BYTE*)realloc(message, messageTotalSize + dwBytesRead);
						memcpy(message + messageTotalSize - 1, pMessageBody, dwBytesRead);
						message[messageTotalSize + dwBytesRead - 1] = '\0';
						messageTotalSize += dwBytesRead;


						free(pMessageBody);
					}

					InternetCloseHandle(request);
				}
			}
			InternetCloseHandle(connect);
		}
		InternetCloseHandle(internet);
	}
	catch (...) {}

	return (char**)&message;
}

MapManager::MapManager() {
	//init the custom map file name string
	this->customMapFileName = L"";
	this->checkedMaps.insert(CUSTOM_MAP);
}

void MapManager::startListening() {
	this->tcpServer = new MapManager::TcpServer();
	this->tcpServer->startListening();
}

void MapManager::stopListening() {
	this->tcpServer->stopListening();
}

void MapManager::TcpServer::stopListening() {
	this->shutdownServerSocket(); 
	listenerThreadRunning = false;
}

std::wstring MapManager::getEnglishMapName() {
	wchar_t* englishMapName = (wchar_t*)(h2mod->GetBase() + 0x97737C);

	DWORD dwBack;
	VirtualProtect(englishMapName, 4, PAGE_EXECUTE_READWRITE, &dwBack);
	//set access for copy of the string
	std::wstring ucurrentMapName(englishMapName);
	VirtualProtect(englishMapName, 4, dwBack, NULL);

	return ucurrentMapName;
}

bool MapManager::hasMap(std::wstring mapName) {
	DWORD dwBack;
	wchar_t* mapsDirectory = (wchar_t*)(h2mod->GetBase() + 0x482D70 + 0x2423C);
	VirtualProtect(mapsDirectory, 4, PAGE_EXECUTE_READ, &dwBack);

	std::wstring mapFileName(mapsDirectory);
	VirtualProtect(mapsDirectory, 4, dwBack, NULL);

	mapFileName += mapName + L".map";
	TRACE("Checking if the following map exists, %s", mapFileName.c_str());
	std::ifstream file(mapFileName.c_str());
	return (bool)file;
}


//this only try a few times and give up completely till a rejoin occurs
void MapManager::requestMapDownloadUrl(SOCKET comm_socket, SOCKADDR_IN SenderAddr) {
	if (requestMapUrl) {
		this->requestMapUrl = false;
		//only ask the server for the map url if we don't have one
		H2ModPacket pack;
		pack.set_type(H2ModPacket_Type_get_map_download_url);

		char* SendBuf = new char[pack.ByteSize()];
		memset(SendBuf, 0x00, pack.ByteSize());
		pack.SerializeToArray(SendBuf, pack.ByteSize());

		sendto(comm_socket, SendBuf, pack.ByteSize(), 0, (SOCKADDR*)&SenderAddr, sizeof(SenderAddr));

		TRACE_GAME_N("Requesting map download");
		delete[] SendBuf;
	}
}

void MapManager::reloadMaps() {
	typedef char(__thiscall *possible_map_reload)(int thisx);
	possible_map_reload map_reload_method = (possible_map_reload)(h2mod->GetBase() + 0x4D021);
	DWORD* mapsObject = (DWORD*)(h2mod->GetBase() + 0x482D70);

	DWORD dwBack;
	BOOL canprotect = VirtualProtect((WORD*)((int)mapsObject + 148016), sizeof(WORD), PAGE_EXECUTE_READWRITE, &dwBack);
	if (!canprotect && GetLastError()) {
		TRACE("canprotect=%s, error=%d", canprotect, GetLastError());
	}
	map_reload_method((int)mapsObject);
}

void MapManager::resetMapDownloadUrl() {
	this->mapDownloadUrl = "";
	this->requestMapUrl = false;
	this->checkedMaps.clear();
	this->threadRunning = false;
}

void MapManager::setCustomLobbyMessage(const char* newStatus, BOOL sleep) {
	this->customLobbyMessage = newStatus;
	if (sleep) {
		Sleep(1500);
	}
}

void MapManager::setCustomLobbyMessage(const char* newStatus) {
	setCustomLobbyMessage(newStatus, true);
}

const char* MapManager::getCustomLobbyMessage()
{
	return this->customLobbyMessage;
}

void MapManager::unzipArchive(std::wstring localPath, std::wstring mapsDir) {
	mz_bool status;
	mz_zip_archive zip_archive;
	memset(&zip_archive, 0, sizeof(zip_archive));

	status = mz_zip_reader_init_file(&zip_archive, std::string(localPath.begin(), localPath.end()).c_str(), 0);
	if (!status) {
		TRACE("Failed to open zip file, status=%d", status);
		this->setCustomLobbyMessage(FAILED_TO_OPEN_ZIP_FILE.c_str());
		return;
	}
	this->setCustomLobbyMessage(UNZIPPING_MAP_DOWNLOAD.c_str());

	mz_uint numFiles = mz_zip_reader_get_num_files(&zip_archive);
	for (int i = 0; i < (int)numFiles; i++) {

		mz_zip_archive_file_stat file_stat;
		if (!mz_zip_reader_file_stat(&zip_archive, i, &file_stat)) {
			TRACE("mz_zip_reader_file_stat() failed!");
			mz_zip_reader_end(&zip_archive);
			return;
		}

		if (!mz_zip_reader_is_file_a_directory(&zip_archive, i)) {
			std::string zipDest(mapsDir.begin(), mapsDir.end());
			std::string zipEntryFileName(file_stat.m_filename);

			std::vector<std::string> v = split(zipEntryFileName, '.');

			//TODO: this might not be the maps actual name in memory, might need to get that somehow

			//each map unzipped gets counted as a checked map
			this->checkedMaps.insert(std::wstring(v[0].begin(), v[0].end()));

			zipDest += zipEntryFileName;
			if (!mz_zip_reader_extract_to_file(&zip_archive, i, zipDest.c_str(), 0)) {
				TRACE("Could not extract zip file");
			}
		}
		else {
			TRACE("nO Support to extract directories, cause I'm lazy");
		}
	}
	mz_zip_reader_end(&zip_archive);
}

BOOL MapManager::downloadMap(std::wstring mapName) {
	std::string url = this->mapDownloadUrl;
	std::string type = this->mapDownloadType;
	std::wstring unicodeUrl(url.begin(), url.end());

	DWORD dwBack;
	wchar_t* mapsDirectory = (wchar_t*)(h2mod->GetBase() + 0x482D70 + 0x2423C);

	VirtualProtect(mapsDirectory, 4, PAGE_EXECUTE_READ, &dwBack);
	std::wstring mapsDir(mapsDirectory);
	std::wstring localPath(mapsDir);
	VirtualProtect(mapsDirectory, 4, dwBack, NULL);

	localPath += mapName + L"." + std::wstring(type.begin(), type.end());

	this->setCustomLobbyMessage(DOWNLOADING_MAP.c_str());
	//TODO: make async
	//TODO: probably just use libcurl
	HRESULT res = URLDownloadToFile(NULL, unicodeUrl.c_str(), localPath.c_str(), 0, NULL);

	if (res == S_OK) {
		this->setCustomLobbyMessage(DOWNLOADING_COMPLETE.c_str());
		TRACE("Map downloaded");
		//if we downloaded a zip file, unzip it
		if (type == "zip") {
			unzipArchive(localPath, mapsDir);
			//delete any zip files in map folder
			_wremove(localPath.c_str());
		}

		this->setCustomLobbyMessage(RELOADING_MAPS.c_str());
		this->reloadMaps();
		//TODO: add counter reset method
		this->requestMapUrl = false;
		return true;
	}
	else if (res == E_OUTOFMEMORY) {
		TRACE("Buffer length invalid, or insufficient memory");
	}
	else if (res == INET_E_DOWNLOAD_FAILURE) {
		TRACE("URL is invalid");
	}
	else {
		TRACE("Other error: %d\n", res);
	}

	return false;
}

void MapManager::setMapDownloadUrl(std::string url) {
	this->mapDownloadUrl = url;
}

void MapManager::setMapDownloadType(std::string type) {
	this->mapDownloadType = type;
}

void MapManager::searchForMap() {
	threadRunning = true;
	overrideUnicodeMessage = true;

	std::wstring currentMapName = this->getEnglishMapName();
	//regardless what happens during the search, we only check for a map once per game session
	checkedMaps.insert(currentMapName);

	if (hasMap(currentMapName)) {
		//if somehow we have the map and made it here, something is wrong, exit
		TRACE_GAME_N("Map already exists, should not be searching for it");
		goto cleanup;
	}

	if (isLobby) {
		//only run map searches if in lobby

		/*
		if (this->downloadFromExternal()) {
			return;
		}

		this->setCustomLobbyMessage(STILL_SEARCHING_FOR_MAP.c_str());
		if (this->downloadFromUrl()) {
			return;
		}

		this->setCustomLobbyMessage(STILL_SEARCHING_FOR_MAP.c_str());
		*/
		//TODO: control via xlive property
		if (this->downloadFromHost()) {
			goto cleanup;
		}

		this->setCustomLobbyMessage(COULD_NOT_FIND_MAPS.c_str());
	}

	cleanup:
	threadRunning = false;
	overrideUnicodeMessage = false;
}

bool MapManager::downloadFromUrl() {
	std::wstring ucurrentMapName = this->getEnglishMapName();
	//wait 2 minutes
	int attempts = 120;
	//this attempt logic exists soley because the hook that this triggers from could be called before
	//the server has had a chance to send us a download url for the map, so we check if the url is set
	//over every second for 5 seconds till we find it
	while (attempts > 0) {
		this->requestMapUrl = true;
		this->setCustomLobbyMessage(WAITING_FOR_MAP_DOWNLOAD_URL.c_str());
		if (!this->mapDownloadUrl.empty()) {
			requestMapUrl = false;
			this->setCustomLobbyMessage(FOUND_MAP_DOWNLOAD_URL.c_str());
			if (downloadMap(ucurrentMapName)) {
				//TODO: use constants
				this->setCustomLobbyMessage("Finished");
				this->setCustomLobbyMessage(NULL);
				return true;
			}
			//try other downloading options since our download attempt failed
			break;
		}
		Sleep(1000);
		attempts--;
	}
	return false;
}



bool MapManager::downloadFromExternal() {
	std::wstring ucurrentMapName = this->getEnglishMapName();
	std::string str(ucurrentMapName.begin(), ucurrentMapName.end());

	std::replace(str.begin(), str.end(), ' ', '_');

	char URL[1024];
	if (map_download_external_uri == NULL || map_download_external_uri[0] == '\0') {
		TRACE_GAME_N("No external map download uri string set");
		return false;
	}

	std::string bb = std::string(map_download_external_uri) + str;

	wsprintfA(URL, bb.c_str());

	//one example from twigzie api
	//Soda Hil|http://dl.halomaps.org/dl.cfm?fid=2154&f=zbe_soda.zip&S=2&h=2B8C199DDBAFCB0CAF1A74DC0CB3E265

	if (map_download_external_host == NULL || map_download_external_host[0] == '\0') {
		TRACE_GAME_N("No external map download host string set");
		return false;
	}

	this->setCustomLobbyMessage("Sending map dl request");
	char** message = Request(0, map_download_external_host, URL, NULL, NULL);
	char* body = *message;
	std::string meh = "http://" + std::string(map_download_external_host) + "/" + bb;
	TRACE_GAME_N("External map download url, url=%s", meh.c_str());

	bool returnn = false;

	if (body != NULL && body[0] != '\0') {
		try {
			std::string sbody(body);
			std::string error("error");
			if (!sbody.compare(0, error.size(), error)) {
				this->setCustomLobbyMessage("Error response from external");
			}
			else {
				//split on first character |
				std::string downloadUrl = sbody.substr(sbody.find("|") + 1);
				TRACE_GAME_N("PreParsedMapDownloadUrl = %s", sbody.c_str());

				if (downloadUrl.empty()) {
					this->setCustomLobbyMessage("No download url from external");
				}
				else {
					this->setCustomLobbyMessage("Got url from external, dl...");
					this->setMapDownloadUrl(downloadUrl);
					this->setMapDownloadType("zip");

					this->downloadMap(ucurrentMapName);

					returnn = true;
				}
			}
		}
		catch (...) {
		}

		delete body;
	}
	else {
		this->setCustomLobbyMessage("No valid response from external");
	}
	return returnn;
}

#define DEFAULT_BUFLEN 65536

std::string startingDownloadString = ("Downloaded 0%");

bool MapManager::downloadFromHost() {
	BOOL rr = false;
	SOCKET ConnectSocket = INVALID_SOCKET;
	try {
		if (precalculatedDownloadPercentageStrings.empty()) {
			for (int i = 0; i <= 100; i++) {
				std::string downloadMsg;
				downloadMsg = "Downloaded ";
				downloadMsg += std::to_string(i);
				downloadMsg += "%";
				precalculatedDownloadPercentageStrings[i] = downloadMsg;
			}
		}
		DWORD* mapsObject = (DWORD*)(h2mod->GetBase() + 0x482D70);

		//mapsObject += 148028;
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
			printf("WSAStartup failed with error: %d\n", iResult);
			return 1;
		}

		ZeroMemory(&hints, sizeof(hints));
		hints.ai_family = AF_UNSPEC;
		hints.ai_socktype = SOCK_STREAM;
		hints.ai_protocol = IPPROTO_TCP;
		std::string addr = inet_ntoa(join_game_xn.ina);
		std::string prt = std::to_string(ntohs(join_game_xn.wPortOnline) + 9);
		TRACE_GAME_N("Client map dl, addr=%s, port=%s", addr.c_str(), prt.c_str());
		// Resolve the server address and port
		iResult = getaddrinfo(addr.c_str(), prt.c_str(), &hints, &result);
		if (iResult != 0) {
			printf("getaddrinfo failed with error: %d\n", iResult);
			//WSACleanup();
			return 1;
		}

		// Attempt to connect to an address until one succeeds
		for (ptr = result; ptr != NULL; ptr = ptr->ai_next) {

			// Create a SOCKET for connecting to server
			ConnectSocket = socket(ptr->ai_family, ptr->ai_socktype,
				ptr->ai_protocol);
			if (ConnectSocket == INVALID_SOCKET) {
				TRACE_GAME_N("socket failed with error: %ld", WSAGetLastError());
				//WSACleanup();
				return 1;
			}

			// Connect to server.
			this->setCustomLobbyMessage("Connecting to host");
			iResult = connect(ConnectSocket, ptr->ai_addr, (int)ptr->ai_addrlen);
			if (iResult == SOCKET_ERROR) {
				closesocket(ConnectSocket);
				ConnectSocket = INVALID_SOCKET;
				this->setCustomLobbyMessage("Socket error connecting to host");
				continue;
			}
			break;
		}

		freeaddrinfo(result);

		if (ConnectSocket == INVALID_SOCKET) {
			TRACE_GAME_N("Unable to connect to server!");
			this->setCustomLobbyMessage("Unable to connect to server");
			//WSACleanup();
			return 1;
		}

		BOOL receivedFilename = false;
		BOOL receivedFileSize = false;

		std::string mapName("");
		int fileSize;

		char* map = NULL;

		int bytesRead = 0;

		// Receive until the peer closes the connection
		int prevDownloadPercentage = 0;
		const char* downloadLobbyMessage = startingDownloadString.c_str();

		do {

			char recvbuf[DEFAULT_BUFLEN];

			iResult = recv(ConnectSocket, recvbuf, recvbuflen, 0);
			if (iResult > 0) {
				this->setCustomLobbyMessage(downloadLobbyMessage, false);
				//TRACE_GAME_N("Bytes received: %d", iResult);
				if (receivedFilename && receivedFileSize) {
					for (int i = 0; i < iResult; i++) {
						map[bytesRead++] = recvbuf[i];
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
						std::size_t extOffset = mapName.find(".map");
						mapName = mapName.substr(0, extOffset);
						receivedFilename = true;
						this->setCustomLobbyMessage("Got filename from host");
						continue;
					}

					if (!receivedFileSize) {
						std::string recv(recvbuf);
						size_t index = recv.find("$");
						std::string fileSizee = recv.substr(0, index);
						bool has_only_digits = (fileSizee.find_first_not_of("0123456789") == std::string::npos);
						if (!has_only_digits) {
							TRACE_GAME_N("Encountered bad file size");
							this->setCustomLobbyMessage("Got bad file size, rejoin");
							goto end;
						}
						if (fileSizee.empty()) {
							TRACE_GAME_N("Encountered bad file size");
							this->setCustomLobbyMessage("No file size, rejoin");
						}
						fileSize = stoi(fileSizee);
						receivedFileSize = true;
						if (map == NULL) {
							map = new char[fileSize];
						}
						for (int i = index + 1; i < iResult; i++) {
							map[bytesRead++] = recvbuf[i];
						}
						this->setCustomLobbyMessage("Got filesize from host");
						continue;
					}
				}
			}
			else if (iResult == 0) {
				TRACE_GAME_N("Connection closed");
				this->setCustomLobbyMessage("Connection to host closed");
			}
			else {
				TRACE_GAME_N("recv failed with error: %d", WSAGetLastError());
				this->setCustomLobbyMessage("Failure receiving, try rejoining");
			}

		} while (iResult > 0);


		if (bytesRead >= fileSize) {
			mapName += ".map";
			std::string mapPath = customMapsDir;
			mapPath += mapName;

			FILE* file;
			try {
				file = fopen(mapPath.c_str(), "wb");
				fwrite(map, sizeof(char), fileSize, file);
				fclose(file);
				delete[] map;
				map = NULL;
				this->setCustomLobbyMessage("Wrote downloaded map to disk");
			}
			catch (std::exception const& e) {
				TRACE_GAME_N("std exception thrown when trying to write map file = %s", e.what());
			}
			catch (...) {
				TRACE_GAME_N("unknown exception occurred occurred when trying to write to map file");
			}
			if (map != NULL) {
				delete[] map;
			}

			this->setCustomLobbyMessage("Map download complete");
			this->setCustomLobbyMessage("Reloading maps");
			this->reloadMaps();
			rr = true;
			goto end;
		}
	}
	catch (std::exception const& e) {
		this->setCustomLobbyMessage("std exception thrown");
		TRACE_GAME_N("std exception thrown = %s", e.what());
	}
	catch (...) {
		this->setCustomLobbyMessage("Unknown error occurred");
		TRACE_GAME_N("unknown exception occurred occurred");
	}
end:
	// cleanup
	closesocket(ConnectSocket);
	//WSACleanup();
	return rr;
}

void MapManager::startMapDownload() {
	if (!threadRunning) {
		std::thread t1(&MapManager::searchForMap, this);
		t1.detach();
	}
}

bool MapManager::canDownload() {
	wchar_t* currentMapName = (wchar_t*)(h2mod->GetBase() + 0x97737C);

	DWORD dwBack;
	VirtualProtect(currentMapName, 4, PAGE_EXECUTE_READWRITE, &dwBack);


	bool isMapNameEmpty = currentMapName == NULL || currentMapName[0] == L'\0';
	if (isMapNameEmpty) {
		//if pointer to map name is null or there is no string data, then we don't try to download
		return false;
	}


	bool isExcludedMap = wcscmp(currentMapName, CUSTOM_MAP.c_str()) == 0;
	if (isExcludedMap) {
		//if the map is named "custom map" don't download
		return false;
	}

	for (auto it = checkedMaps.begin(); it != checkedMaps.end(); ++it) {
		if (wcscmp(currentMapName, it->c_str()) == 0) {
			//we already downloaded the map, no need to check for it again
			return false;
		}
	}

	VirtualProtect(currentMapName, 4, dwBack, NULL);

	//if we made it here, start downloading
	return true;
}

void handleConnection(SOCKET socket, std::wstring customMapName) {
	HANDLE file;
	int result;
	try {
		//TODO: use TCP_CORK (might not need with TransmitFile)

		file = CreateFile(customMapName.c_str(), GENERIC_READ, FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE, 0, OPEN_EXISTING, 0, 0);

		if (file == INVALID_HANDLE_VALUE) {
			TRACE_GAME("Unable to open file %s for read", customMapName.c_str());
			return;
		}

		int fileSize = GetFileSize(file, NULL);
		if (fileSize == INVALID_FILE_SIZE) {
			TRACE_GAME_N("invalid file size, %d", GetLastError());
			return;
		}

		std::string nonUnicodeCustomMapFilename(customMapName.begin(), customMapName.end());
		std::size_t offset = nonUnicodeCustomMapFilename.find_last_of("\\");
		std::size_t extOffset = nonUnicodeCustomMapFilename.find_last_not_of(".map");
		std::string mapName = nonUnicodeCustomMapFilename.substr(offset + 1, extOffset);

		std::string buf = mapName;
		std::string buf2 = std::to_string(fileSize);
		buf2 += "$";

		result = send(socket, mapName.c_str(), (int)strlen(mapName.c_str()), 0);
		if (result == SOCKET_ERROR) {
			TRACE_GAME_N("filename send failed with error: %d", WSAGetLastError());
			result = send(socket, mapName.c_str(), (int)strlen(mapName.c_str()), 0);
			if (result == SOCKET_ERROR) {
				TRACE_GAME_N("filename retry send failed with error: %d", WSAGetLastError());
				return;
			}
		}

		result = send(socket, buf2.c_str(), (int)strlen(buf2.c_str()), 0);
		if (result == SOCKET_ERROR) {
			TRACE_GAME_N("filesize send failed with error: %d", WSAGetLastError());
			result = send(socket, buf2.c_str(), (int)strlen(buf2.c_str()), 0);
			if (result == SOCKET_ERROR) {
				TRACE_GAME_N("filesize retry send failed with error: %d", WSAGetLastError());
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
					TRACE_GAME_N("Transmit file succeeded");
				}
				else {
					TRACE_GAME_N("Transmit file failed, errorCode=%d", WSAGetLastError());
				}
			}
			catch (...) {
				TRACE_GAME_N("c++ exception thrown");
			}
		}

		goto cleanup;
	}
	catch (std::exception const& e) {
		TRACE_GAME_N("std exception thrown = %s",e.what());
	}
	catch (...) {
		TRACE_GAME_N("unknown exception occurred occurred");
	}
cleanup:
	//close the file
	if (CloseHandle(file)) {
		TRACE_GAME_N("closed file handle");
	}
	else {
		TRACE_GAME_N("unable to close file");
	}
	// shutdown the connection since we're done
	result = shutdown(socket, SD_SEND);
	if (result == SOCKET_ERROR) {
		TRACE_GAME_N("shutdown failed with error: %d", WSAGetLastError());
		closesocket(socket);
		return;
	}

	if (closesocket(socket) == SOCKET_ERROR) {
		TRACE_GAME_N("close of socket failed with error: %d", WSAGetLastError());
	}
}

void MapManager::TcpServer::shutdownServerSocket() {
	if (!serverSocketShutdown) {
		closesocket(serverSocket);
		//shut down once
		serverSocketShutdown = true;
	}
}

void MapManager::TcpServer::startListening()
{
	struct addrinfo *result = NULL;
	struct addrinfo hints;
	int iResult;
	int port = g_port + 9;           /* port number to use */
	int client;                  /* file descriptor for socket */
	int fileDescriptorForFile;                    /* file descriptor for file to send */
	struct sockaddr_in addr;   /* socket parameters for bind */
	int    addrlen;            /* argument to accept */
	struct stat stat_buf;      /* argument to fstat */
	off_t offset = 0;          /* file offset */
	char filename[128];   /* filename to send */

	WSADATA wsaData;

	// Initialize Winsock
	iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (iResult != 0) {
		TRACE_GAME_N("WSAStartup failed with error: %d", iResult);
		return ;
	}

	ZeroMemory(&hints, sizeof(hints));
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP;
	hints.ai_flags = AI_PASSIVE;

	// Resolve the server address and port
	iResult = getaddrinfo(NULL, std::to_string(port).c_str(), &hints, &result);
	if (iResult != 0) {
		TRACE_GAME_N("getaddrinfo failed with error: %d, port:", iResult, std::to_string(port).c_str());
		//WSACleanup();
		return;
	}

	// Create a SOCKET for connecting to server
	serverSocket = socket(result->ai_family, result->ai_socktype, result->ai_protocol);
	if (serverSocket == INVALID_SOCKET) {
		TRACE_GAME_N("socket failed with error: %ld", WSAGetLastError());
		freeaddrinfo(result);
		//WSACleanup();
		return;
	}

	/* bind socket to the port */
	iResult = bind(serverSocket, result->ai_addr, (int)result->ai_addrlen);
	if (iResult == SOCKET_ERROR) {
		TRACE_GAME_N("unable to bind to socket: %s", strerror(errno));
		freeaddrinfo(result);
		closesocket(serverSocket);
		return;
	}

	freeaddrinfo(result);

	/* listen for clients on the socket */
	iResult = listen(serverSocket, SOMAXCONN);
	if (iResult == SOCKET_ERROR) {
		TRACE_GAME_N("Error trying to listen on port: %s", strerror(errno));
		closesocket(serverSocket);
		return;
	}
	TRACE_GAME_N("Listening on port: %d", port);
	//server socket is up
	serverSocketShutdown = false;

	while (listenerThreadRunning) {
		/* wait for a client to connect */
		client = accept(serverSocket, 0, 0);
		if (client == -1) {
			TRACE_GAME_N("accept failed: %s", strerror(errno));
			//try to accept again
			//TODO: if it fails too many times, exit
			continue;
		}

		//0x30 (difference from start of maps object to first custom map)
		//0xB90 (difference between each custom map name)
		//0x960 (difference between custom map name and its file path
		DWORD offset;
		if (h2mod->Server) {
			offset = 0x4A70D8;
		}
		else {
			offset = 0x482D70;
		}
		DWORD* mapsObject = (DWORD*)(h2mod->GetBase() + offset + 0x30);
		//move to first map
		DWORD currentMapNameOffset;
		if (h2mod->Server) {
			//H2Server.exe+5349B4
			//H2Server.exe+535C64
			currentMapNameOffset = 0x5349B4;
		}
		else {
			currentMapNameOffset = 0x97737C;
		}
		const wchar_t* currentMapName = (wchar_t*)(h2mod->GetBase() + currentMapNameOffset);
		for (int i = 1; i <= 51; i++) {
			wchar_t* mapName = (wchar_t*)((DWORD*)(h2mod->GetBase() + offset + 0x30 + (i * 0xB90)));

			if (wcscmp(currentMapName, mapName) == 0) {
				wchar_t* mapPath = (wchar_t*)((DWORD*)(h2mod->GetBase() + offset + 0x30 + ((i * 0xB90) + 0x960)));
				mapManager->customMapFileName = std::wstring(mapPath);
				break;
			}
		}
		std::wstring customMapName = mapManager->customMapFileName;

		if (customMapName.empty()) {
			TRACE_GAME_N("Could not determine custom map file name");
			continue;
		}

		std::thread t1(&handleConnection, client, customMapName);
		t1.detach();
	}
}
