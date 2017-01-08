#ifndef MAP_DOWNLOAD_H
#define MAP_DOWNLOAD_H

#include <set>

class MapManager {
public:
	MapManager();
	void reloadMaps();
	void startMapDownload();
	void searchForMap();
	void resetMapDownloadUrl();
	void setCustomLobbyMessage(const wchar_t* newStatus);
	const wchar_t* getCustomLobbyMessage();
	void setMapDownloadUrl(std::string url);
	void setMapDownloadType(std::string type);
	void requestMapDownloadUrl(SOCKET comm_socket, SOCKADDR_IN SenderAddr);
	bool canDownload();

	bool downloadFromUrl();
	bool downloadFromExternal();
	bool downloadFromHost();

private:
	bool requestMapUrl = false;
	std::wstring getCurrentMapName();
	bool hasMap(std::wstring mapName);
	void unzipArchive(std::wstring localPath, std::wstring mapsDir);
	BOOL downloadMap(std::wstring mapName);

	std::wstring currentMap;
	std::string mapDownloadUrl;
	std::string mapDownloadType;

	const wchar_t* customLobbyMessage = NULL;

	std::set<std::wstring> checkedMaps;
};

#endif