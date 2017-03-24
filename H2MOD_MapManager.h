#ifndef MAP_DOWNLOAD_H
#define MAP_DOWNLOAD_H

#include <set>

class MapManager {
public:
	MapManager();
	void reloadMaps();
	void startListening(); 
	void stopListening();
	void startMapDownload();
	void searchForMap();
	void resetMapDownloadUrl();
	void setCustomLobbyMessage(const char* newStatus); 
	void setCustomLobbyMessage(const char* newStatus, BOOL sleep);
	const char* getCustomLobbyMessage();
	std::wstring getEnglishMapName();
	void setMapDownloadUrl(std::string url);
	void setMapDownloadType(std::string type);
	void requestMapDownloadUrl(SOCKET comm_socket, SOCKADDR_IN SenderAddr);
	bool canDownload();

	bool downloadFromUrl();
	bool downloadFromExternal();
	bool downloadFromHost();

	std::wstring customMapFileName;
	int customMapIndex;

private:
	class TcpServer {
	public:
		void startListening();
		void stopListening();
	private:
		volatile BOOL listenerThreadRunning = true;
		volatile BOOL serverSocketShutdown = true; //true is default until the server actually starts listening
		void shutdownServerSocket();
		SOCKET serverSocket;
	};

	TcpServer* tcpServer;
	bool requestMapUrl = false;
	bool hasMap(std::wstring mapName);
	void unzipArchive(std::wstring localPath, std::wstring mapsDir);
	BOOL downloadMap(std::wstring mapName);

	std::wstring currentMap;
	std::string mapDownloadUrl;
	std::string mapDownloadType;

	const char* customLobbyMessage = NULL;
	std::unordered_map<int, std::string> precalculatedDownloadPercentageStrings;

	std::set<std::wstring> checkedMaps;

	volatile BOOL threadRunning = false;
};

#endif