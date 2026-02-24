#pragma once 

#include "../xnet.h"
#include "../net_utils.h"


#define XNIP_MAX_PCK_STR_HDR_LEN 32
#define XNIP_MAX_NET_STATS_SAMPLES 30

#define XNIP_QOS_PORT_OFFSET 10

enum 
{
	EXNIP_PACKET_SIGNATURE_XNET_REQUEST		= 'XNeT',
	EXNIP_PACKET_SIGNATURE_XNET_BROADCAST	= 'BrOd',
};

#define XNIP_REQUEST_HEADER_STR		"XNetReqPack"
#define XNIP_BROADCAST_HEADER_STR	"XNetBrOadPack"

#define XnIp_ConnectionIndexMask 0xFF000000

#define XnIp_ConnectionTimeOut (15 * 1000) // msec

#define XNIP_FLAG(_bit) (1<<(_bit))
#define XNIP_SET_BIT(_flags, _bit, _value) ((_value) ? ((_flags) |= XNIP_FLAG((_bit))) : ((_flags) &= ~(XNIP_FLAG(_bit))))
#define XNIP_TEST_BIT(_flags, _bit) (((_flags) & XNIP_FLAG((_bit))) != 0)

TEST_N_DEF(XL0);

enum eXnip_ConnectRequestType : int
{
	EXNIP_CONNECTION_REQUEST_INVALID = -1,

	EXNIP_CONNECTION_PING,
	EXNIP_CONNECTION_PONG,
	EXNIP_CONNECTION_PORT_MAPPING_UPDATE,
	EXNIP_CONNECTION_FINISH_ESTABLISH_SECURE_CHANNEL,
	EXNIP_CONNECTION_ACKNOWLEDGE_CONNECTED_SECURE_CHANNEL,
	EXNIP_CONNECTION_CLOSE_SECURE
};

enum eXnIp_ConnectionRequestBitFlags
{
	XnIp_HasPortMappingsUpdated = 0,
};

struct XNetPacketHeader
{
	DWORD signature;
	char signatureString[XNIP_MAX_PCK_STR_HDR_LEN];
};

struct XBroadcastPacket
{
	XBroadcastPacket();

	XNetPacketHeader pckHeader;
	struct
	{
		DWORD titleId;
		sockaddr_in name;
	} data;
};

struct XNetRequestPacket
{
	XNetRequestPacket()
	{
		pckHeader.signature = EXNIP_PACKET_SIGNATURE_XNET_REQUEST;
		memset(pckHeader.signatureString, 0, sizeof(pckHeader.signatureString));
		strncpy_s(pckHeader.signatureString, XNIP_REQUEST_HEADER_STR, XNIP_MAX_PCK_STR_HDR_LEN);
		ZeroMemory(&data, sizeof(data));
	}

	XNetPacketHeader pckHeader;
	struct
	{
		XNADDR xnaddr;
		XNKID xnkid;
		BYTE nonceKey[8];
		eXnip_ConnectRequestType reqType;
		union
		{
			struct // EXNIP_CONNECTION_PORT_MAPPING_UPDATE EXNIP_CONNECTION_FINISH_ESTABLISH_SECURE_CHANNEL
			{
				DWORD flags;
				bool connectionInitiator;
				WORD senderVirtualPort;
			};
		};
	} data;
};

struct XnKeyPair
{
	bool m_valid;
	XNKID m_xnkid;
	XNKEY m_xnkey;
};

struct XnIpPckTransportStats
{
	bool initialized;

	unsigned int pckSent;
	unsigned int pckRecvd;
	unsigned int pckBytesSent;
	unsigned int pckBytesRecvd;

	unsigned int pckSentPerSec[XNIP_MAX_NET_STATS_SAMPLES];
	unsigned int pckBytesSentPerSec[XNIP_MAX_NET_STATS_SAMPLES];

	unsigned int pckRecvdPerSec[XNIP_MAX_NET_STATS_SAMPLES];
	unsigned int pckBytesRecvdPerSec[XNIP_MAX_NET_STATS_SAMPLES];

	int			 pckCurrentSendPerSecIdx;
	int			 pckCurrentRecvdPerSecIdx;

	ULONGLONG	 lastPacketReceivedTime;

	void PckDataSampleUpdate();

	void PckSendStatsUpdate(unsigned int _pckXmit, unsigned int _pckXmitBytes);

	void PckRecvdStatsUpdate(unsigned int _pckRecvd, unsigned int _pckRecvdBytes);

private:
	ULONGLONG lastTimeUpdate;
	int pckSentPerSecIdx;
	int pckRecvdPerSecIdx;
};

struct XnIp
{
	IN_ADDR m_connectionId;
	XNADDR m_xnaddr;
	// key we connected with
	XnKeyPair* m_keyPair;

	TEST_N_DEF(XL4);

	bool m_valid;
	int m_connectStatus;
	int m_connectionPacketsSentCount;
	
	ULONGLONG m_lastConnectionInteractionTime;

	BYTE m_nonce[8];
	BYTE m_endpointNonce[8];
	bool m_endpointNonceValid;

	bool m_requestContext;

	NetLinkedList m_netAddrMappings;

	// describes if this connection was created
	// in the event of a received packet
	// if true, the endpoint initiated the connection
	// if false, local machine attempted to connect
	bool m_connectionInitiator;

	enum eXnIp_Flags
	{
		XnIp_ConnectDeclareConnectedRequestSent,
		XnIp_ReconnectionAttempt,
	};
	int m_flags;

public:
	XnIpPckTransportStats m_pckStats;

	void PckStatsReset()
	{
		m_pckStats.initialized = false;
	}

	bool PckGetStats(const XnIpPckTransportStats** outPckStats) const
	{
		bool result = false;
		if (m_pckStats.initialized)
		{
			*outPckStats = &m_pckStats;
			result = true;
		}

		return result;
	}

	IN_ADDR GetOnlineIpAddr() const
	{
#ifdef XL5
		TEST_N_DEF(XL5);
#else
		return m_xnaddr.inaOnline;
#endif
	}

	IN_ADDR GetLanIpAddr() const
	{
		return m_xnaddr.ina;
	}

	IN_ADDR GetConnectionId() const
	{
		return m_connectionId;
	}

	bool InitiatedConnectRequest() const 
	{
		return m_connectionInitiator;
	}

	int GetConnectStatus() const
	{
		return m_connectStatus;
	}

	void SetConnectStatus(int connectStatus)
	{
		m_connectStatus = connectStatus;
	}

	bool ConnectStatusIdle() const
	{
		return GetConnectStatus() == XNET_CONNECT_STATUS_IDLE;
	}

	bool ConnectStatusPending() const
	{
		return GetConnectStatus() == XNET_CONNECT_STATUS_PENDING;
	}

	bool ConnectStatusConnected() const 
	{
		return GetConnectStatus() == XNET_CONNECT_STATUS_CONNECTED;
	}

	bool ConnectStatusLost() const
	{
		return GetConnectStatus() == XNET_CONNECT_STATUS_LOST;
	}

	static int GetConnectionIndex(IN_ADDR connectionId);

	bool IsValid(IN_ADDR identifier) const;

	void UpdateInteractionTimeHappened();

	bool ConnectionTimedOut() const;

	void SavePortMapping(struct XVirtualSocket* xsocket, WORD virtualPort, const struct sockaddr_in* addr) const;
	void HandleConnectionPacket(struct XVirtualSocket* xsocket, const XNetRequestPacket* reqPacket, const struct sockaddr_in* recvAddr, LPDWORD lpBytesRecvdCount);
	void HandleDisconnectPacket(struct XVirtualSocket* xsocket, const XNetRequestPacket* disconnectReqPck, const struct sockaddr_in* recvAddr) const; // TODO:
	void UpdateNonceKeyFromPacket(const XNetRequestPacket* reqPacket);

	/* sends a request over the socket to the other socket end, with the same identifier */
	void SendXNetRequest(struct XVirtualSocket* xsocket, eXnip_ConnectRequestType reqType);

	/* sends a request to all open sockets */
	void SendXNetRequestAllSockets(eXnip_ConnectRequestType reqType);

	void InsertPortMapping(struct PortMapping* mapping);

	const struct sockaddr_in* GetPortMapping(WORD virtualPort) const;

	void UpdatePortMapping(WORD virtualPort, const struct sockaddr_in* addr) const;

	bool PortMappingAvailable(WORD virtualPort) const;

	bool PortMappingsAvailable() const;

	void DiscardPortMappings()
	{
		m_netAddrMappings.dispose();
	}
};

class XnIpManager
{
public:

	XnIpManager()
	{
		memset(&m_startupParams, 0, sizeof(m_startupParams));
	}

	// TODO maybe terminate all connections
	~XnIpManager() = default;

	// disable copy/move
	XnIpManager(const XnIpManager& other) = delete;
	XnIpManager(XnIpManager&& other) = delete;

	void Initialize(const XNetStartupParams* netStartupParams);
	void Dispose();

	// Connection data getters 
	XnIp* GetConnection(const IN_ADDR ina) const;
	int GetEstablishedConnectionIdentifierByRecvAddr(struct XVirtualSocket* xsocket, const struct sockaddr_in* addr, IN_ADDR* outConnectionIdentifier) const;

	// Miscellaneous
	void ClearLostConnections();

	// local network address
	static XnIp* GetLocalUserXn();
	static unsigned short GetQoSPort();

	static void UnregisterLocalConnectionInfo();
	static void SetupLocalConnectionInfo(unsigned long xnaddr, unsigned long lanaddr, unsigned short baseport, const char* machineUID, const char* abOnline);

	// Performance counters
	void UpdatePacketReceivedCounters(IN_ADDR ipIdentifier, unsigned int bytesRecvdCount) const;

	// Packet handlers
	int HandleRecvdPacket(struct XVirtualSocket* xsocket, sockaddr_in* lpFrom, WSABUF* lpBuffers, DWORD dwBufferCount, LPDWORD bytesRecvdCount);
	void HandleXNetRequestPacket(struct XVirtualSocket* xsocket, const XNetRequestPacket* reqPaket, const struct sockaddr_in* recvAddr, LPDWORD lpBytesRecvdCount);
	void HandleDisconnectPacket(struct XVirtualSocket* xsocket, const XNetRequestPacket* disconnectReqPck, const struct sockaddr_in* recvAddr) const;

	// XnIp handling function
	XnIp* XnIpLookup(const XNADDR* pxna, const XNKID* xnkid) const;
	int CreateOrGetXnIpIdentifierFromPacket(const XNADDR* pxna, const XNKID* xnkid, const XNetRequestPacket* reqPacket, IN_ADDR* outIpIdentifier);
	int RegisterNewXnIp(const XNADDR* pxna, const XNKID* pxnkid, IN_ADDR* outIpIdentifier);
	void UnregisterXnIpIdentifier(const IN_ADDR ina) const;

	// Key functions
	int RegisterKey(XNKID*, XNKEY*);
	void UnregisterKey(const XNKID* xnkid);
	XnKeyPair* KeyPairLookup(const XNKID* xnkid) const;
	
	// Logging 
#ifdef TERMINAL_ENABLED
	void LogConnectionsToConsole(int(__cdecl* outputCb)(int, const char*, ...)) const;
#endif
	void LogConnectionsErrorDetails(const sockaddr_in* address, int errorCode, const XNKID* receivedKey) const;

	// XNet startup parameters
	int GetMaxXnConnections()				const { return m_startupParams.cfgSecRegMax; }
	int GetReqQoSBufferSize()				const { return m_startupParams.cfgQosDataLimitDiv4 * 4; }
	int GetMaxXnKeyPairs()					const { return m_startupParams.cfgKeyRegMax; }
	int GetMinSockRecvBufferSizeInBytes()	const;
	int GetMinSockSendBufferSizeInBytes()	const;

	int GetRegisteredKeyCount() const
	{
		int keysCount = 0;
		for (int i = 0; i < GetMaxXnKeyPairs(); i++)
		{
			if (m_XnKeyPairs[i].m_valid)
			{
				keysCount++;
			}
		}

		return keysCount;
	}

	// Data
	XnIp* m_XnIPs = nullptr;
	XnKeyPair* m_XnKeyPairs = nullptr;

private:
	static XnIp m_ipLocal;
	XNetStartupParams m_startupParams;
};

extern XnIpManager gXnIpMgr;

int WINAPI XNetRegisterKey(XNKID *pxnkid, XNKEY *pxnkey);