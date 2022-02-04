#pragma once

/*
* Class used to define a map download query
* NOTE: if id of the query is none, the download is initiated by a console command
*/
class MapDownloadQuery {
public:
	MapDownloadQuery::MapDownloadQuery(const std::wstring& _mapToDownload, int _downloadId);
	MapDownloadQuery::~MapDownloadQuery();

	void SetMapNameToDownload(const std::wstring& mapNameToDownload);
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
	void reloadAllMaps();

	static void applyHooks();

	void getMapFilename(std::wstring& buffer);

	std::shared_ptr<MapDownloadQuery> addDownloadQuery(const std::wstring& mapToDownload)
	{
		m_mapDownloadQueryList.push_back(std::make_shared<MapDownloadQuery>(mapToDownload, rand()));
		return getLastDownloadQueryAdded();
	}

	std::shared_ptr<MapDownloadQuery> getLastDownloadQueryAdded()
	{
		if (!m_mapDownloadQueryList.empty())
			return m_mapDownloadQueryList.back();

		return nullptr;
	}

	std::shared_ptr<MapDownloadQuery> getDownloadQueryById(int id)
	{
		for (auto& query : m_mapDownloadQueryList)
		{
			if (query->id == id)
				return query;
		}

		return nullptr;
	}

	void forceStopDownloadQueries()
	{
		while (m_mapDownloadQueryList.size() > 0)
		{
			auto& mapDownloadQuery = m_mapDownloadQueryList.front();
			mapDownloadQuery->StopDownload();
			m_mapDownloadQueryList.pop_front();
		}
	}

	void MapDownloadUpdateTick();

	std::list<std::shared_ptr<MapDownloadQuery>> m_mapDownloadQueryList;

private:

};

extern MapManager* mapManager;