#pragma once

#include <set>

/**
* Class used for downloading maps (for peers) and hosting map downloads (for peer host and dedi host)
* NOTE - there are g_debug conditions in front of every trace call, this is because I want to avoid any string creation (since its expensive)
* during game time, unless g_debug is on.
*/
class MapManager {
public:
	//client and server functions below
	void reloadAllMaps();
	bool loadMapInfo(std::wstring& mapFileLocation);
	std::wstring getMapName();
	bool hasCustomMap(std::string mapName);
	bool hasCustomMap(std::wstring mapName);

	//client functions/data below
	void applyGamePatches();
	std::string getMapFilenameToDownload();
	void clearMapFileNameToDownload();
	void setMapFileNameToDownload(std::string mapFilenameToDownload);
	void setMapFileNameToDownload(std::wstring mapFilenameToDownload);
	bool downloadFromRepo(std::string mapFilename);
	std::wstring clientMapFilename;
	//we precalculate the strings when the MapManager class is loaded to avoid any expensive object creation during game/lobby time
	std::unordered_map<int, std::string> precalculatedDownloadPercentageStrings;
	void leaveSessionIfAFK();

	//server functions below
	void getMapFilename(std::wstring& buffer);

private:

	bool requestMapUrl = false;
	std::string mapFilenameToDownload;
};

extern MapManager* mapManager;