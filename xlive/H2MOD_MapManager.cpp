#include "Globals.h"
#include <h2mod.pb.h>
#include <fstream>
#include <Mswsock.h>
#include <WS2tcpip.h>

#pragma comment (lib, "mswsock.lib")

#define DEFAULT_BUFLEN 65536

extern UINT g_port;
extern UINT g_debug;

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

std::string startingDownloadString = ("Downloaded 0%");
std::string validDigits("0123456789");
std::string mapExt(".map");
std::wstring mapExtUnicode(L".map");
std::string fileReadMode("wb");
std::string traceTemplate("%s");
std::string fileSizeDelim("$");

std::wstring CUSTOM_MAP = L"Custom Map";
wchar_t EMPTY_UNICODE_STR = '\0';
std::wstring EMPTY_WSTRING(L"");

/**
* Constructs the map manager for client/servers
*/
MapManager::MapManager() {
	//init the custom map file name string
	this->customMapFileName = EMPTY_WSTRING;
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
std::wstring MapManager::getEnglishMapName() {
	wchar_t* englishMapName = (wchar_t*)(h2mod->GetBase() + 0x97737C);

	DWORD dwBack;
	//set r/w access on string so we don't have any issues when we do the implicit copy below
	VirtualProtect(englishMapName, 4, PAGE_EXECUTE_READWRITE, &dwBack);
	std::wstring ucurrentMapName(englishMapName);
	VirtualProtect(englishMapName, 4, dwBack, NULL);

	return ucurrentMapName;
}

/**
* Checks if the client has the custom map already
* NOTE - only works on peers (not dedis)
*/
bool MapManager::hasCustomMap(std::wstring mapName) {
	DWORD dwBack;
	wchar_t* mapsDirectory = (wchar_t*)(h2mod->GetBase() + 0x482D70 + 0x2423C);
	VirtualProtect(mapsDirectory, 4, PAGE_EXECUTE_READ, &dwBack);

	std::wstring mapFileName(mapsDirectory);
	VirtualProtect(mapsDirectory, 4, dwBack, NULL);

	mapFileName += mapName + mapExtUnicode;
	std::ifstream file(mapFileName.c_str());
	return (bool)file;
}

/**
* Actually calls the real map reload function in halo2.exe
*/
void MapManager::reloadMaps() {
	typedef char(__thiscall *possible_map_reload)(int thisx);
	possible_map_reload map_reload_method = (possible_map_reload)(h2mod->GetBase() + 0x4D021);
	DWORD* mapsObject = (DWORD*)(h2mod->GetBase() + 0x482D70);

	DWORD dwBack;
	BOOL canprotect = VirtualProtect((WORD*)((int)mapsObject + 148016), sizeof(WORD), PAGE_EXECUTE_READWRITE, &dwBack);
	if (!canprotect && GetLastError()) {
		if (g_debug)
			TRACE_GAME_N("reloadMaps - canprotect=%d, error=%d", canprotect, GetLastError());
	}
	map_reload_method((int)mapsObject);
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

/**
* Searches for a map to download based on the english map name and the actual filename
* NOTE - only tries to download from host if we haven't downloaded it before in current game session AND
* if the client is in the lobby
*/
void MapManager::searchForMap() {
	threadRunning = true;
	overrideUnicodeMessage = true;

	std::wstring currentMapName = this->getEnglishMapName();

	if (hasCustomMap(currentMapName)) {
		//if somehow we have the map and made it here, something is wrong, exit
		if (g_debug)
			TRACE_GAME_N("Map already exists, should not be searching for it");
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
	overrideUnicodeMessage = false;
}

/**
* Cleanups client and server download states
*/
void MapManager::cleanup() {
	this->stopListeningForClients();
	this->resetClient();
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
			if (g_debug)
				TRACE_GAME_N("WSAStartup failed with error: %d\n", iResult);
			return 1;
		}

		ZeroMemory(&hints, sizeof(hints));
		hints.ai_family = AF_UNSPEC;
		hints.ai_socktype = SOCK_STREAM;
		hints.ai_protocol = IPPROTO_TCP;
		std::string addr = inet_ntoa(join_game_xn.ina);
		std::string prt = std::to_string(ntohs(join_game_xn.wPortOnline) + 9);
		if (g_debug)
			TRACE_GAME_N("Client map dl, addr=%s, port=%s", addr.c_str(), prt.c_str());
		// Resolve the server address and port
		iResult = getaddrinfo(addr.c_str(), prt.c_str(), &hints, &result);
		if (iResult != 0) {
			if (g_debug)
				TRACE_GAME_N("getaddrinfo failed with error: %d\n", iResult);
			//WSACleanup();
			return 1;
		}

		// Attempt to connect to an address until one succeeds
		for (ptr = result; ptr != NULL; ptr = ptr->ai_next) {

			// Create a SOCKET for connecting to server
			connectSocket = socket(ptr->ai_family, ptr->ai_socktype,
				ptr->ai_protocol);
			if (connectSocket == INVALID_SOCKET) {
				if (g_debug)
					TRACE_GAME_N("socket failed with error: %d", WSAGetLastError());
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
			if (g_debug)
				TRACE_GAME_N("%s", UNABLE_TO_CONNECT_TO_SERVER.c_str());
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
							if (g_debug)
								TRACE_GAME_N("%s", BAD_FILE_SIZE.c_str());
							this->setCustomLobbyMessage(BAD_FILE_SIZE.c_str());
							goto end;
						}
						if (fileSizee.empty()) {
							if (g_debug)
								TRACE_GAME_N("%s", BAD_FILE_SIZE.c_str());
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
				if (g_debug)
					TRACE_GAME_N("%s", CONNECTION_TO_HOST_CLOSED.c_str());
				this->setCustomLobbyMessage(CONNECTION_TO_HOST_CLOSED.c_str());
			}
			else {
				if (g_debug)
					TRACE_GAME_N("recv failed with error: %d", WSAGetLastError());
				this->setCustomLobbyMessage("Failure receiving, try rejoining");
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
		if (g_debug) {
			this->setCustomLobbyMessage("std exception thrown");
			TRACE_GAME_N("std exception thrown = %s", e.what());
		}
	}
	catch (...) {
		if (g_debug) {
			this->setCustomLobbyMessage("Unknown error occurred");
			TRACE_GAME_N("unknown exception occurred occurred");
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
			if (g_debug)
				TRACE_GAME("Unable to open file %s for read", customMapName.c_str());
			return;
		}

		int fileSize = GetFileSize(file, NULL);
		if (fileSize == INVALID_FILE_SIZE) {
			if (g_debug)
				TRACE_GAME_N("invalid file size, %d", GetLastError());
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
			if (g_debug)
				TRACE_GAME_N("filename send failed with error: %d", WSAGetLastError());
			result = send(socket, mapName.c_str(), (int)strlen(mapName.c_str()), 0);
			if (result == SOCKET_ERROR) {
				if (g_debug)
					TRACE_GAME_N("filename retry send failed with error: %d", WSAGetLastError());
				return;
			}
		}

		result = send(socket, buf2.c_str(), (int)strlen(buf2.c_str()), 0);
		if (result == SOCKET_ERROR) {
			if (g_debug)
				TRACE_GAME_N("filesize send failed with error: %d", WSAGetLastError());
			result = send(socket, buf2.c_str(), (int)strlen(buf2.c_str()), 0);
			if (result == SOCKET_ERROR) {
				if (g_debug)
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
					if (g_debug)
						TRACE_GAME_N("Transmit file succeeded");
				}
				else {
					if (g_debug)
						TRACE_GAME_N("Transmit file failed, errorCode=%d", WSAGetLastError());
				}
			}
			catch (...) {
				if (g_debug)
					TRACE_GAME_N("c++ exception thrown");
			}
		}

		goto cleanup;
	}
	catch (std::exception const& e) {
		if (g_debug)
			TRACE_GAME_N("std exception thrown = %s", e.what());
	}
	catch (...) {
		if (g_debug)
			TRACE_GAME_N("unknown exception occurred occurred");
	}
cleanup:
	//close the file
	if (CloseHandle(file)) {
		if (g_debug)
			TRACE_GAME_N("closed file handle");
	}
	else {
		if (g_debug)
			TRACE_GAME_N("unable to close file");
	}
	// shutdown the connection since we're done
	result = shutdown(socket, SD_SEND);
	if (result == SOCKET_ERROR) {
		if (g_debug)
			TRACE_GAME_N("shutdown failed with error: %d", WSAGetLastError());
		closesocket(socket);
		return;
	}

	if (closesocket(socket) == SOCKET_ERROR) {
		if (g_debug)
			TRACE_GAME_N("close of socket failed with error: %d", WSAGetLastError());
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
		if (g_debug)
			TRACE_GAME_N("WSAStartup failed with error: %d", iResult);
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
		if (g_debug)
			TRACE_GAME_N("getaddrinfo failed with error: %d, port:", iResult, std::to_string(port).c_str());
		//WSACleanup();
		return;
	}

	// Create a SOCKET for connecting to server
	serverSocket = socket(result->ai_family, result->ai_socktype, result->ai_protocol);
	if (serverSocket == INVALID_SOCKET) {
		if (g_debug)
			TRACE_GAME_N("socket failed with error: %ld", WSAGetLastError());
		freeaddrinfo(result);
		serverSocket = NULL;
		//WSACleanup();
		return;
	}

	/* bind socket to the port */
	iResult = bind(serverSocket, result->ai_addr, (int)result->ai_addrlen);
	if (iResult == SOCKET_ERROR) {
		if (g_debug)
			TRACE_GAME_N("unable to bind to socket: %s", strerror(errno));
		freeaddrinfo(result);
		closesocket(serverSocket);
		serverSocket = NULL;
		return;
	}

	freeaddrinfo(result);

	/* listen for clients on the socket */
	iResult = listen(serverSocket, SOMAXCONN);
	if (iResult == SOCKET_ERROR) {
		if (g_debug)
			TRACE_GAME_N("Error trying to listen on port: %s", strerror(errno));
		closesocket(serverSocket);
		serverSocket = NULL;
		return;
	}

	if (g_debug)
		TRACE_GAME_N("Listening on port: %d", port);

	while (listenerThreadRunning) {
		/* wait for a client to connect */
		client = accept(serverSocket, 0, 0);
		if (client == -1) {
			if (g_debug)
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
			//H2Server.exe+535C64 (another offset to use if the above fails for whatever reason)
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
				if (g_debug)
					TRACE_GAME("CurrentMapPath = %s", mapPath);
				mapManager->customMapFileName = std::wstring(mapPath);
				break;
			}
		}
		std::wstring customMapName = mapManager->customMapFileName;

		if (customMapName.empty()) {
			if (g_debug)
				TRACE_GAME_N("Could not determine custom map file name");
			continue;
		}

		std::thread t1(&handleClientConnection, client, customMapName);
		t1.detach();
	}
}
