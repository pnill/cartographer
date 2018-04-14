#pragma once

#include <set>

/**
* Class used for downloading maps (for peers) and hosting map downloads (for peer host and dedi host)
* NOTE - there are g_debug conditions in front of every trace call, this is because I want to avoid any string creation (since its expensive)
* during game time, unless g_debug is on.
*/
class MapManager {
public:
	MapManager();
	//client and server functions below
	void reloadMaps();
	void cleanup();
	void sendMapInfoPacket();
	const char* getCustomLobbyMessage();
	std::wstring getMapName();
	bool hasCustomMap(std::string mapName);
	bool hasCustomMap(std::wstring mapName);

	//client functions/data below
	std::string getMapFilenameToDownload();
	void setMapFileNameToDownload(std::string mapFilenameToDownload);
	void startListeningForClients();
	void startMapDownload();
	void searchForMap();
	bool downloadFromRepo(std::string mapFilename);
	std::string clientMapFilename;
	void setCustomLobbyMessage(const char* newStatus);
	//we precalculate the strings when the MapManager class is loaded to avoid any expensive object creation during game/lobby time
	std::unordered_map<int, std::string> precalculatedDownloadPercentageStrings;

	//server functions below
	std::string getMapFilename();
	std::string getCachedMapFilename();
	void reloadMapFilenames();

private:
	class TcpServer {
	public:
		void startListening();
		void stop();
	private:
		volatile BOOL listenerThreadRunning = true;
		void shutdownServerSocket();
		SOCKET serverSocket = NULL;
	};

	//client functions below
	bool downloadFromHost();
	void resetClient();

	//server functions below
	void stopListeningForClients();

	TcpServer* tcpServer = NULL;
	bool requestMapUrl = false;
	std::wstring currentMap;
	const char* customLobbyMessage = NULL;
	volatile BOOL threadRunning = false;
	std::set<std::string> downloadedMaps;
	std::string mapFilenameToDownload;
	std::string cachedMapFilename;
	std::unordered_map<std::wstring, std::string> mapNameToFileName;
};
