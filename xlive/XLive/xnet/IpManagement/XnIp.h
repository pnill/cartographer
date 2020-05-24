#pragma once 

#include "..\xnet.h"
#include "..\Sockets\XSocket.h"

#define MAX_HDR_STR 32

const char requestStrHdr[MAX_HDR_STR] = "XNetBrOadPack";
const char broadcastStrHdr[MAX_HDR_STR] = "XNetReqPack";

#define XnIp_ConnectionPing 0x0
#define XnIp_ConnectionPong 0x2
#define XnIp_ConnectionClose 0x4
#define XnIp_ConnectionEstablishSecure 0x8

struct XnIp
{
	IN_ADDR connectionIdentifier;
	XNADDR xnaddr;
	XNKID xnkid;
	bool bValid;
	int xnetstatus;
	int connectionPacketsSentCount;
	int lastConnectionInteractionTime;

	// NAT info
	sockaddr_in NatAddrSocket1000; // TODO: allocate dynamically based on how many sockets are up
	sockaddr_in NatAddrSocket1001;

	bool isValid(IN_ADDR identifier) { return bValid && identifier.s_addr == connectionIdentifier.s_addr; }
};

struct XNetPacketHeader
{
	DWORD intHdr;
	char HdrStr[MAX_HDR_STR];
};

struct XBroadcastPacket
{
	XNetPacketHeader pckHeader;
	struct XBroadcast
	{
		sockaddr_in name;
	};
	XBroadcast data;
};

struct XNetRequestPacket 
{
	XNetRequestPacket()
	{
		pckHeader.intHdr = 'XNeT';
		memset(pckHeader.HdrStr, 0, sizeof(pckHeader.HdrStr));
		strncpy(pckHeader.HdrStr, requestStrHdr, MAX_HDR_STR);
	}

	XNetPacketHeader pckHeader;
	struct XNetReq
	{
		XNKID xnkid;
		DWORD reqType;
		union
		{
			XNADDR xnaddr;
		};
	};
	XNetReq data;
};

class CXnIp
{
public:
	int CreateXnIpIdentifier(const XNADDR* pxna, const XNKID* xnkid, IN_ADDR* outIpIdentifier, bool handleFromConnectionPacket);
	void UnregisterXnIpIdentifier(const IN_ADDR ina);

	void checkForLostConnections();
	void setTimeConnectionInteractionHappened(IN_ADDR ina, int time);
	int getConnectionIndex(IN_ADDR connectionIdentifier);
	void SetupLocalConnectionInfo(XNADDR* pxna);
	int sendConnectionRequest(XSocket* xsocket, IN_ADDR ipIdentifier);
	int handleRecvdPacket(XSocket* xsocket, sockaddr_in* lpFrom, WSABUF* lpBuffers, LPDWORD bytesRecvdCount);

	void UnregisterLocalConnectionInfo();
	void Initialize(const XNetStartupParams* netStartupParams);
	IN_ADDR GetConnectionIdentifierByRecvAddr(XSocket* xsocket, sockaddr_in* addr);
	void SaveConnectionNatInfo(XSocket* xsocket, IN_ADDR ipIdentifier, sockaddr_in* addr);
	void HandleConnectionPacket(XSocket* xsocket, XNetRequestPacket* connectReqPacket, sockaddr_in* addr);
	void RegisterKeys(XNKID*, XNKEY*);
	void UnregisterKeys();
	void getRegisteredKeys(XNKID* xnkid, XNKEY* xnkey);
	
	XnIp localUser;
	BOOL GetLocalXNAddr(XNADDR* pxna);

	XnIp* XnIPs = nullptr;
	std::vector<XSocket*> SocketPtrArray = {};

	// TODO: get rid of this
	XNADDR gameHostXn; // the Xn of the host from the join game hook

	XNetStartupParams startupParams;
	int GetMaxXnConnections() { return startupParams.cfgSecRegMax; }
	int GetReqQoSBufferSize() { return startupParams.cfgQosDataLimitDiv4 * 4; }

private:
	XNKID host_xnkid;
	XNKEY host_xnkey;
};

extern CXnIp ipManager;
