#pragma once 

#include "..\xnet.h"
#include "..\Sockets\XSocket.h"

#include "H2MOD\Modules\Shell\Shell.h"
#include "H2MOD\GUI\ImGui_Integration\Console\CommandHandler.h"

#define XNIP_MAX_PCK_STR_HDR_LEN 32

#define XNIP_MAX_NET_STATS_SAMPLES 30

extern const char requestStrHdr[XNIP_MAX_PCK_STR_HDR_LEN];
extern const char broadcastStrHdr[XNIP_MAX_PCK_STR_HDR_LEN];

#define XnIp_ConnectionIndexMask 0xFF000000

#define XnIp_ConnectionTimeOut (15 * 1000) // msec

// Network long LOOPBACK address
#define XnIp_LOOPBACK_ADDR_NL (htonl(INADDR_LOOPBACK)) // 127.0.0.1

#define XNIP_FLAG(_bit) (1<<(_bit))
#define XNIP_SET_BIT(_val, _bit) ((_val) |= XNIP_FLAG((_bit)))
#define XNIP_TEST_BIT(_val, _bit) (((_val) & XNIP_FLAG((_bit))) != 0)

enum eXnip_ConnectRequestType : int
{
	XnIp_ConnectionRequestInvalid = -1,

	XnIp_ConnectionPing,
	XnIp_ConnectionPong,
	XnIp_ConnectionUpdateNAT,
	XnIp_ConnectionEstablishSecure,
	XnIp_ConnectionDeclareConnected,
	XnIp_ConnectionCloseSecure
};

enum eXnIp_ConnectionRequestBitFlags
{
	XnIp_HasEndpointNATData = 0,

};

enum class H2v_sockets : int
{
	Sock1000 = 0,
	Sock1001
};

struct XNetPacketHeader
{
	DWORD intHdr;
	char HdrStr[XNIP_MAX_PCK_STR_HDR_LEN];
};

struct XBroadcastPacket
{
	XBroadcastPacket()
	{
		pckHeader.intHdr = 'BrOd';
		strncpy(pckHeader.HdrStr, broadcastStrHdr, XNIP_MAX_PCK_STR_HDR_LEN);
		ZeroMemory(&data, sizeof(data));
		data.name.sin_addr.s_addr = INADDR_BROADCAST;
	};

	XNetPacketHeader pckHeader;
	struct
	{
		sockaddr_in name;
	} data;
};

struct XNetRequestPacket
{
	XNetRequestPacket()
	{
		pckHeader.intHdr = 'XNeT';
		memset(pckHeader.HdrStr, 0, sizeof(pckHeader.HdrStr));
		strncpy(pckHeader.HdrStr, requestStrHdr, XNIP_MAX_PCK_STR_HDR_LEN);
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
			struct // XnIp_ConnectionUpdateNAT XnIp_ConnectEstablishSecure
			{
				DWORD flags;
				bool connectionInitiator;
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
	bool bInit;

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

	void PckDataSampleUpdate()
	{
		if (!bInit)
		{
			bInit = true;
			pckSent = 0;
			pckRecvd = 0;
			pckBytesSent = 0;
			pckBytesRecvd = 0;

			pckSentPerSecIdx = 0;
			pckRecvdPerSecIdx = 0;
			pckCurrentSendPerSecIdx = -1;
			pckCurrentRecvdPerSecIdx = -1;

			memset(pckSentPerSec, 0, sizeof(pckSentPerSec));
			memset(pckRecvdPerSec, 0, sizeof(pckRecvdPerSec));

			lastTimeUpdate = _Shell::QPCToTimeNowMsec();
		}
		else
		{
			const ULONGLONG sample_end_time = 1ull * 1000ull;

			if (_Shell::QPCToTimeNowMsec() - lastTimeUpdate >= sample_end_time)
			{
				pckSentPerSecIdx = (pckSentPerSecIdx + 1) % XNIP_MAX_NET_STATS_SAMPLES;
				pckRecvdPerSecIdx = (pckRecvdPerSecIdx + 1) % XNIP_MAX_NET_STATS_SAMPLES;

				pckSentPerSec[pckSentPerSecIdx] = 0;
				pckBytesSentPerSec[pckSentPerSecIdx] = 0;

				pckRecvdPerSec[pckRecvdPerSecIdx] = 0;
				pckBytesRecvdPerSec[pckRecvdPerSecIdx] = 0;

				pckCurrentSendPerSecIdx = (pckCurrentSendPerSecIdx + 1) % XNIP_MAX_NET_STATS_SAMPLES;
				pckCurrentRecvdPerSecIdx = (pckCurrentRecvdPerSecIdx + 1) % XNIP_MAX_NET_STATS_SAMPLES;

				lastTimeUpdate = _Shell::QPCToTimeNowMsec();
			}
		}
	}

	void PckSendStatsUpdate(unsigned int _pckXmit, unsigned int _pckXmitBytes)
	{
		PckDataSampleUpdate();

		pckSent += _pckXmit;
		pckBytesSent += _pckXmitBytes;

		pckSentPerSec[pckSentPerSecIdx] += _pckXmit;
		pckBytesSentPerSec[pckSentPerSecIdx] += _pckXmitBytes;
	}

	void PckRecvdStatsUpdate(unsigned int _pckRecvd, unsigned int _pckRecvdBytes)
	{
		PckDataSampleUpdate();

		pckRecvd += _pckRecvd;
		pckBytesRecvd += _pckRecvdBytes;

		pckRecvdPerSec[pckRecvdPerSecIdx] += _pckRecvd;
		pckBytesRecvdPerSec[pckRecvdPerSecIdx] += _pckRecvdBytes;

		lastPacketReceivedTime = _Shell::QPCToTimeNowMsec();
	}

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

	bool m_valid;
	int m_connectStatus;
	int m_connectionPacketsSentCount;
	
	ULONGLONG m_lastConnectionInteractionTime;

	BYTE m_nonce[8];
	BYTE m_endpointNonce[8];
	bool m_endpointNonceValid;

	// describes if this connection was created
	// in the event of a received packet
	// if true, the endpoint initiated the connection
	// if false, local machine attempted to connect
	bool m_connectionInitiator;

	enum eXnIp_Flags
	{
		XnIp_ConnectDeclareConnectedRequestSent,
	};
	DWORD flags;

#pragma region NAT handling

	// TODO: currently we use multiple system sockets
	// to send data to the corresponding virtual socket
	// when in practice we could just use one
	struct NatTranslation
	{
		enum class eNatDataState : unsigned int
		{
			natUnavailable,
			natAvailable,
		};

		eNatDataState state;
		sockaddr_in natAddress;
	};
	NatTranslation m_natTranslation[2];

	const sockaddr_in* NatGetAddr(H2v_sockets natIndex) const
	{
		int index = (int)natIndex;
		return &m_natTranslation[index].natAddress;
	}

	void NatUpdate(H2v_sockets natIndex, const sockaddr_in* addr)
	{
		int index = (int)natIndex;
		m_natTranslation[index].natAddress = *addr;
		m_natTranslation[index].state = NatTranslation::eNatDataState::natAvailable;
	}

	void NatDiscard()
	{
		for (int i = 0; i < ARRAYSIZE(m_natTranslation); i++)
		{
			memset(&m_natTranslation[i], 0, sizeof(*m_natTranslation));
			m_natTranslation[i].state = NatTranslation::eNatDataState::natUnavailable;
		}
	}

	bool NatIsUpdated(int natIndex) const
	{
		return m_natTranslation[natIndex].state == NatTranslation::eNatDataState::natAvailable;
	}

	bool NatIsUpdated() const
	{
		for (int i = 0; i < ARRAYSIZE(m_natTranslation); i++)
		{
			if (m_natTranslation[i].state != NatTranslation::eNatDataState::natAvailable)
				return false;
		}
		return true;
	}
#pragma endregion

public:
	XnIpPckTransportStats m_pckStats;

	void PckStatsReset()
	{
		m_pckStats.bInit = false;
	}

	bool PckGetStats(const XnIpPckTransportStats** outPckStats) const
	{
		if (m_pckStats.bInit)
		{
			*outPckStats = &m_pckStats;
			return true;
		}

		return false;
	}

	IN_ADDR GetOnlineIpAddr() const
	{
		return m_xnaddr.inaOnline;
	}

	IN_ADDR GetLanIpAddr() const
	{
		return m_xnaddr.ina;
	}

	bool IsValid(IN_ADDR identifier) const
	{
		bool valid = m_valid
			&& identifier.s_addr == GetConnectionId().s_addr;

		if (!valid)
		{
			LOG_CRITICAL_NETWORK("{} - m_valid: {} or {:X} != {:X}", __FUNCTION__, m_valid, identifier.s_addr, GetConnectionId().s_addr);
			return false;
		}

		return valid;
	}

	void UpdateInteractionTimeHappened()
	{
		m_lastConnectionInteractionTime = _Shell::QPCToTimeNowMsec();
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

	bool ConnectStatusConnected() const 
	{
		return GetConnectStatus() == XNET_CONNECT_STATUS_CONNECTED;
	}

	bool ConnectStatusLost() const
	{
		return GetConnectStatus() == XNET_CONNECT_STATUS_LOST;
	}

	static int GetConnectionIndex(IN_ADDR connectionId);

	void SaveNatInfo(XSocket* xsocket, const sockaddr_in* addr);
	void HandleConnectionPacket(XSocket* xsocket, const XNetRequestPacket* reqPacket, const sockaddr_in* recvAddr, LPDWORD lpBytesRecvdCount);
	void HandleDisconnectPacket(XSocket* xsocket, const XNetRequestPacket* disconnectReqPck, const sockaddr_in* recvAddr); // TODO:

	/* sends a request over the socket to the other socket end, with the same identifier */
	void SendXNetRequest(XSocket* xsocket, eXnip_ConnectRequestType reqType);

	/* sends a request to all open sockets */
	void SendXNetRequestAllSockets(eXnip_ConnectRequestType reqType);
};

class XnIpManager
{
public:

	XnIpManager()
	{
		memset(&m_startupParams, 0, sizeof(m_startupParams));
	}

	~XnIpManager()
	{
		// TODO maybe terminate all connections
	}

	// disable copy/move
	XnIpManager(const XnIpManager& other) = delete;
	XnIpManager(XnIpManager&& other) = delete;

	void Initialize(const XNetStartupParams* netStartupParams);

	// Connection data getters 
	XnIp* GetConnection(const IN_ADDR ina) const;
	int GetEstablishedConnectionIdentifierByRecvAddr(XSocket* xsocket, const sockaddr_in* addr, IN_ADDR* outConnectionIdentifier) const;

	// Miscellaneous
	void ClearLostConnections();

	// local network address
	static XnIp* GetLocalUserXn();
	static void UnregisterLocalConnectionInfo();
	static void SetupLocalConnectionInfo(unsigned long xnaddr, unsigned long lanaddr, unsigned short baseport, const char* abEnet, const char* abOnline);

	// Performance counters
	void UpdatePacketReceivedCounters(IN_ADDR ipIdentifier, unsigned int bytesRecvdCount);
	
	// Packet handlers
	int HandleRecvdPacket(XSocket* xsocket, sockaddr_in* lpFrom, WSABUF* lpBuffers, DWORD dwBufferCount, LPDWORD bytesRecvdCount);
	void HandleXNetRequestPacket(XSocket* xsocket, const XNetRequestPacket* reqPaket, const sockaddr_in* recvAddr, LPDWORD lpBytesRecvdCount);
	void HandleDisconnectPacket(XSocket* xsocket, const XNetRequestPacket* disconnectReqPck, const sockaddr_in* recvAddr);

	// XnIp handling function
	XnIp* XnIpLookup(const XNADDR* pxna, const XNKID* xnkid) const;
	int CreateOrGetXnIpIdentifierFromPacket(const XNADDR* pxna, const XNKID* xnkid, const XNetRequestPacket* reqPacket, IN_ADDR* outIpIdentifier);
	int RegisterNewXnIp(const XNADDR* pxna, const XNKID* pxnkid, IN_ADDR* outIpIdentifier);
	void UnregisterXnIpIdentifier(const IN_ADDR ina);
	
	// Key functions
	int RegisterKey(XNKID*, XNKEY*);
	void UnregisterKey(const XNKID* xnkid);
	XnKeyPair* KeyPairLookup(const XNKID* xnkid) const;
	
	// Logging 
	void LogConnectionsToConsole(ConsoleLog* output) const;
	void LogConnectionsErrorDetails(const sockaddr_in* address, int errorCode, const XNKID* receivedKey) const;

	// XNet startup parameters
	int GetMaxXnConnections()				const { return m_startupParams.cfgSecRegMax; }
	int GetReqQoSBufferSize()				const { return m_startupParams.cfgQosDataLimitDiv4 * 4; }
	int GetMaxXnKeyPairs()					const { return m_startupParams.cfgKeyRegMax; }
	int GetMinSockRecvBufferSizeInBytes()	const { return m_startupParams.cfgSockDefaultRecvBufsizeInK * SOCK_K_UNIT; }
	int GetMinSockSendBufferSizeInBytes()	const { return m_startupParams.cfgSockDefaultSendBufsizeInK * SOCK_K_UNIT; }

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