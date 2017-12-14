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
	void reloadMaps();
	void startListeningForClients();
	void startMapDownload();
	void searchForMap();
	void cleanup();
	const char* getCustomLobbyMessage();

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

	bool downloadFromHost();
	void stopListeningForClients();
	void resetClient();
	void setCustomLobbyMessage(const char* newStatus);
	bool hasCustomMap(std::wstring mapName);
	std::wstring getEnglishMapName();

	std::wstring customMapFileName;
	TcpServer* tcpServer = NULL;
	bool requestMapUrl = false;
	std::wstring currentMap;
	const char* customLobbyMessage = NULL;
	//we precalculate the strings when the MapManager class is loaded to avoid any expensive object creation during game/lobby time
	std::unordered_map<int, std::string> precalculatedDownloadPercentageStrings;
	volatile BOOL threadRunning = false;
};
