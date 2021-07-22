#pragma once

/*
* Class used to define a map download query
* NOTE: if id of the query is none, the download is initiated by a console command
*/
class MapDownloadQuery {
public:
	// pretty much a copy constructor
	MapDownloadQuery::MapDownloadQuery(MapDownloadQuery& other);
	MapDownloadQuery::MapDownloadQuery(std::wstring& _mapToDownload, int _downloadId);

	MapDownloadQuery::~MapDownloadQuery();

	void SetMapNameToDownload(std::wstring& mapNameToDownload);
	void SetMapNameToDownload(const wchar_t* mapNameToDownload);

	int GetDownloadPercentage();
	void SetDownloadPercentage(int _downloadPercentage);

	void StartMapDownload();
	bool downloadFromRepo();

	bool ShouldStopDownload();
	void StopDownload();

	std::string m_clientMapFilename;
	std::wstring m_clientMapFilenameWide;
	std::atomic<bool> m_downloadFinished = false;
	std::atomic<bool> m_readyToDownload = false;

	int id;

private:

	std::atomic<bool> m_forceStopDownload = false;
	int m_downloadPercentage = 0;
};

/**
* Class used for downloading maps (for peers) and hosting map downloads (for peer host and dedi host)
*/
class MapManager {
public:
	//client and server functions below
	void reloadAllMaps();
	bool loadMapInfo(std::wstring& mapFileLocation);

	//client functions/data below
	void applyHooks();

	//server functions below
	void getMapFilename(std::wstring& buffer);

	std::shared_ptr<MapDownloadQuery> addDownloadQuery(std::wstring mapToDownload)
	{
		m_mapDownloadQueue.push_back(std::shared_ptr<MapDownloadQuery>(new MapDownloadQuery(mapToDownload, rand())));
		return getLastDownloadQueryAdded();
	}

	std::shared_ptr<MapDownloadQuery> getLastDownloadQueryAdded()
	{
		if (!m_mapDownloadQueue.empty())
			return m_mapDownloadQueue.back();

		return nullptr;
	}

	std::shared_ptr<MapDownloadQuery> getDownloadQueryById(int id)
	{
		for (auto& query : m_mapDownloadQueue)
		{
			if (query->id == id)
				return query;
		}

		return nullptr;
	}

	void forceStopDownloadQueries()
	{
		while (m_mapDownloadQueue.size() > 0)
		{
			auto& mapDownloadQuery = m_mapDownloadQueue.front();
			mapDownloadQuery->StopDownload();
			m_mapDownloadQueue.pop_front();
		}
	}

	void MapDownloadUpdateTick();

	std::list<std::shared_ptr<MapDownloadQuery>> m_mapDownloadQueue;

private:

};

extern MapManager* mapManager;