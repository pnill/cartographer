#pragma once

#define XUSER_PROPERTY_GAMETYPE_NAME		0x40008228
#define XUSER_PROPERTY_GAMETYPE_NAME_2		0x4000822B
#define XUSER_PROPERTY_SERVER_NAME			0x40008230
#define XUSER_PROPERTY_SERVER_DESC			0x40008225
#define XUSER_PROPERTY_MAP_ID				0x10008207
#define XUSER_PROPERTY_MAP_ID_2				0x1000820A
#define XUSER_PROPERTY_MAP_NAME				0x40008226
#define XUSER_PROPERTY_MAP_NAME_2			0x40008229
#define XUSER_PROPERTY_GAMETYPE_ID			0x10008209
#define XUSER_PROPERTY_GAME_STATUS			0x10008211
#define XUSER_PROPERTY_VERSION_1			0x1000820E
#define XUSER_PROPERTY_VERSION_2			0x1000820F
#define XUSER_PROPERTY_UNKNOWN_INT64		0x2000821B
#define XUSER_PROPERTY_MAP_HASH_1			0x40008227
#define XUSER_PROPERTY_MAP_HASH_2			0x4000822A
#define XUSER_PROPERTY_USER_INT				0x10008202
#define XUSER_PROPERTY_UNKNOWN_INT32_1		0x10008208
#define XUSER_PROPERTY_UNKNOWN_INT32_2		0x1000820B
#define XUSER_PROPERTY_UNKNOWN_INT32_3		0x1000820C
#define XUSER_PROPERTY_UNKNOWN_INT32_4		0x1000820D
#define XUSER_PROPERTY_PARTY_PRIVACY		0x10008210
#define XUSER_PROPERTY_UNKNOWN_INT32_6		0x10008212
#define XUSER_PROPERTY_UNKNOWN_INT32_7		0x10008213
#define XUSER_PROPERTY_USERNAME_2			0x4000822C
#define XUSER_PROPERTY_XUID                 0x2000822F

#define X_PROPERTY_UNICODE_BUFFER_SIZE				(sizeof(WCHAR) * 64 + 2) // 2 bytes for NULL character
#define XLOCATOR_DEDICATEDSERVER_PROPERTY_START     0x200

// These properties are used for search only.
// The search result header should already contains the information, and the query should not request these properties again.
#define X_PROPERTY_DEDICATEDSERVER_IDENTITY             XPROPERTYID(1, XUSER_DATA_TYPE_INT64,  XLOCATOR_DEDICATEDSERVER_PROPERTY_START + XTS_SEARCH_FIELD_ServerIdentity)   // server id. supports '=' operator onl$
#define X_PROPERTY_DEDICATEDSERVER_TYPE                 XPROPERTYID(1, XUSER_DATA_TYPE_INT32,  XLOCATOR_DEDICATEDSERVER_PROPERTY_START + XTS_SEARCH_FIELD_ServerType)
#define X_PROPERTY_DEDICATEDSERVER_MAX_PUBLIC_SLOTS     XPROPERTYID(1, XUSER_DATA_TYPE_INT32,  XLOCATOR_DEDICATEDSERVER_PROPERTY_START + XTS_SEARCH_FIELD_MaxPublicSlots)
#define X_PROPERTY_DEDICATEDSERVER_MAX_PRIVATE_SLOTS    XPROPERTYID(1, XUSER_DATA_TYPE_INT32,  XLOCATOR_DEDICATEDSERVER_PROPERTY_START + XTS_SEARCH_FIELD_MaxPrivateSlots)
#define X_PROPERTY_DEDICATEDSERVER_AVAILABLE_PUBLIC_SLOTS   XPROPERTYID(1, XUSER_DATA_TYPE_INT32,  XLOCATOR_DEDICATEDSERVER_PROPERTY_START + XTS_SEARCH_FIELD_AvailablePublicSlots)
#define X_PROPERTY_DEDICATEDSERVER_AVAILABLE_PRIVATE_SLOTS  XPROPERTYID(1, XUSER_DATA_TYPE_INT32,  XLOCATOR_DEDICATEDSERVER_PROPERTY_START + XTS_SEARCH_FIELD_AvailablePrivateSlots)
#define X_PROPERTY_DEDICATEDSERVER_FILLED_PUBLIC_SLOTS      XPROPERTYID(1, XUSER_DATA_TYPE_INT32,   XLOCATOR_DEDICATEDSERVER_PROPERTY_START + XTS_SEARCH_FIELD_FilledPublicSlots)
#define X_PROPERTY_DEDICATEDSERVER_FILLED_PRIVATE_SLOTS     XPROPERTYID(1, XUSER_DATA_TYPE_INT32,   XLOCATOR_DEDICATEDSERVER_PROPERTY_START + XTS_SEARCH_FIELD_FilledPrivateSlots)


// the following properties only support XTS_FILTER_COMPARE_OPERATOR_Equals operator
#define X_PROPERTY_DEDICATEDSERVER_OWNER_XUID           XPROPERTYID(1, XUSER_DATA_TYPE_INT64,   XLOCATOR_DEDICATEDSERVER_PROPERTY_START + XTS_SEARCH_FIELD_OwnerXuid)
#define X_PROPERTY_DEDICATEDSERVER_OWNER_GAMERTAG       XPROPERTYID(1, XUSER_DATA_TYPE_UNICODE,   XLOCATOR_DEDICATEDSERVER_PROPERTY_START + XTS_SEARCH_FIELD_OwnerGamerTag)
#define X_PROPERTY_DEDICATEDSERVER_REGIONID             XPROPERTYID(1, XUSER_DATA_TYPE_INT32,   XLOCATOR_DEDICATEDSERVER_PROPERTY_START + XTS_SEARCH_FIELD_RegionID)
#define X_PROPERTY_DEDICATEDSERVER_LANGUAGEID           XPROPERTYID(1, XUSER_DATA_TYPE_INT32,   XLOCATOR_DEDICATEDSERVER_PROPERTY_START + XTS_SEARCH_FIELD_LanguageID)

// Predefined dedicated server types
#define XLOCATOR_SERVERTYPE_PUBLIC          0   // dedicated server is for all players.
#define XLOCATOR_SERVERTYPE_GOLD_ONLY       1   // dedicated server is for Gold players only.
#define XLOCATOR_SERVERTYPE_PEER_HOSTED     2   // dedicated server is a peer-hosted game server.
#define XLOCATOR_SERVERTYPE_PEER_HOSTED_GOLD_ONLY   3   // dedicated server is a peer-hosted game server (gold only).
#define XLOCATOR_SERVICESTATUS_PROPERTY_START     0x100

#define XLOCATOR_SERVER_PAGE_REPORT_ITEM_COUNT_MIN 24

#define XLOCATOR_SERVER_PAGE_MIN_ITEMS		50
#define XLOCATOR_SERVER_PAGE_MAX_ITEMS		200

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

typedef struct _HALO2VISTA_TITLE_SERVICE_PROPERTIES
{
	int total_count_property_id = 0x0;
	int total_count = 0;
	int total_public_property_id = 0x0;
	int total_public = 0;
	int total_peer_property_id = 0x0;
	int total_peer = 0;
	int total_peer_gold_property_id = 0x0;
	int total_peer_gold = 0;
	int total_public_gold_property_id = 0x0;
	int total_public_gold = 0;
} HALO2VISTA_TITLE_SERVICE_PROPERTIES;

class CServerList
{
public:
	CServerList::CServerList(const CServerList& other) = delete;
	CServerList::CServerList(CServerList&& other) = delete;

	static bool CountResultsUpdated;


	static DWORD Enumerate(HANDLE hHandle, DWORD cbBuffer, CHAR* pvBuffer, PXOVERLAPPED pOverlapped);

	// basic functions 
	static void GetServerCounts(PXOVERLAPPED);
	static void RemoveServer(PXOVERLAPPED pOverlapped);
	static void AddServer(DWORD dwUserIndex, DWORD dwServerType, XNKID xnkid, XNKEY xnkey, DWORD dwMaxPublicSlots, DWORD dwMaxPrivateSlots, DWORD dwFilledPublicSlots, DWORD dwFilledPrivateSlots, DWORD cProperties, PXUSER_PROPERTY pProperties, PXOVERLAPPED pOverlapped);

#pragma region ServerListQuery
	HANDLE Handle = INVALID_HANDLE_VALUE;

	CServerList::CServerList(DWORD _cItemsPerPage, DWORD _cSearchPropertiesIDs, DWORD* _pSearchProperties)
	{
		m_itemsPerPageCount = _cItemsPerPage;
		m_searchPropertiesIdCount = _cSearchPropertiesIDs;
		m_pSearchPropertyIds = new DWORD[_cSearchPropertiesIDs];
		memcpy(m_pSearchPropertyIds, _pSearchProperties, _cSearchPropertiesIDs * sizeof(*_pSearchProperties));
	}

	CServerList::~CServerList()
	{
		delete[] m_pSearchPropertyIds;

		// this must be locked when deconstructing
		m_itemQueryMutex.unlock();
	}

	int GetItemLeftCount();
	int GetValidItemsFoundCount();

	void CancelOperation() { m_cancelOperation = true; }

	void EnumerateFromHttp();
	bool SearchResultParseAndWrite(std::shared_ptr<std::string> serverResultData, XLOCATOR_SEARCHRESULT* pOutSearchResult, XUSER_PROPERTY** propertiesBuffer, WCHAR** stringBuffer);

	void SetNewPageBuffer(DWORD cbBuffer, CHAR* pvBuffer)
	{
		m_resultBuffer = pvBuffer;
		m_resultBufferSize = cbBuffer;
	}

	enum
	{
		OperationPending,
		OperationIncomplete,
		OperationFailed,
		OperationFinished,
	};

	PXOVERLAPPED m_pOverlapped = nullptr;
	std::atomic<int> m_itemsLeftInDoc = 0;
	std::atomic<int> m_operationState = OperationPending;

	int m_itemsPerPageCount;
	std::atomic<int> m_pageItemsFoundCount;
	
	DWORD m_searchPropertiesIdCount = 0;
	DWORD* m_pSearchPropertyIds = nullptr;

	CHAR* m_resultBuffer = nullptr;
	DWORD m_resultBufferSize = 0;

	std::string m_serverListToDownload;

	std::atomic<bool> m_cancelOperation = false;
	std::atomic<bool> m_operationOnPause = false;

	// mainly used for resource discard
	std::mutex m_itemQueryMutex;

	std::thread m_searchThread;

// #pragma region ServerListQuery
#pragma endregion 

	static std::mutex addServerMutex;
	static std::mutex removeServerMutex;
	static std::mutex getServerCountsMutex;
};

extern std::unordered_map<HANDLE, CServerList*> serverListRequests;
