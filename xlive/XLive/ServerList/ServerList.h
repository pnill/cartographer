#pragma once

#include "curl/curl.h"

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

class ServerList
{
public:
	static bool CountResultsUpdated;
	static int total_count;
	static int total_public;
	static int total_peer;
	static int total_peer_gold;
	static int total_public_gold;

	HANDLE Handle = INVALID_HANDLE_VALUE;

	static DWORD GetServers(HANDLE, DWORD, CHAR*, PXOVERLAPPED);

	static void GetServerCounts(PXOVERLAPPED);
	static void RemoveServer(PXOVERLAPPED pOverlapped);
	static void AddServer(DWORD dwUserIndex, DWORD dwServerType, XNKID xnkid, XNKEY xnkey, DWORD dwMaxPublicSlots, DWORD dwMaxPrivateSlots, DWORD dwFilledPublicSlots, DWORD dwFilledPrivateSlots, DWORD cProperties, PXUSER_PROPERTY pProperties, PXOVERLAPPED pOverlapped);

#pragma region ServerListQuery

	ServerList::ServerList(DWORD _cSearchPropertiesIDs, DWORD* _pSearchProperties)
	{
		cSearchPropertiesIDs = _cSearchPropertiesIDs;
		pSearchPropertyIDs = new DWORD[_cSearchPropertiesIDs];
		memcpy(pSearchPropertyIDs, _pSearchProperties, _cSearchPropertiesIDs * sizeof(*_pSearchProperties));
	}

	int GetServersLeft();
	int GetTotalServers();

	void CancelOperation() { cancelOperation = true; }

	void GetServersFromHttp(DWORD cbBuffer, CHAR* pvBuffer);
	void QueryServerData(CURL* curl, ULONGLONG xuid, XLOCATOR_SEARCHRESULT* nResult, XUSER_PROPERTY** propertiesBuffer, WCHAR** stringBuffer);

	PXOVERLAPPED ovelapped;

	enum OperationState : int
	{
		OperationPending,
		OperationIncomplete,
		OperationFinished
	};

	OperationState operationState = OperationPending;
	int ServersLeftInDocumentCount = -1;
	int total_servers = 0;

	DWORD cSearchPropertiesIDs = 0;
	DWORD* pSearchPropertyIDs = nullptr;

	//TODO:
	std::atomic<bool> cancelOperation = false;

	std::thread serv_thread;

#pragma endregion

	static std::mutex AddServerMutex;
	static std::mutex RemoveServerMutex;
	static std::mutex GetServerCountsMutex;
};

extern std::unordered_map<HANDLE, ServerList*> serverListRequests;
