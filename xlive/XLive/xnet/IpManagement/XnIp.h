#pragma once 

#include "stdafx.h"
#include "..\xnet.h"
#include "..\Sockets\XSocket.h"

struct XnIp
{
	IN_ADDR connectionIdentifier;
	XNADDR xnaddr;
	XNKID xnkid;
	bool bValid;
	int xnetstatus;
	int connectionPacketsSentCount;
	int lastConnectionInteractionTime;

	// TODO:
	//int socketCount;

	// NAT info
	sockaddr_in NatAddrSocket1000; // TODO: allocate dynamically based on how many sockets are up
	sockaddr_in NatAddrSocket1001;

	bool isValid(IN_ADDR identifier) { return bValid && identifier.s_addr == connectionIdentifier.s_addr; }
};

template <class T>
inline void hash_combine(std::size_t & seed, const T & v)
{
	std::hash<T> hasher;
	seed ^= hasher(v) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
}

template<typename S, typename T> struct std::hash < std::pair<S, T> >
{
	inline size_t operator()(const pair<S, T> & v) const
	{
		size_t seed = 0;
		::hash_combine(seed, v.first);
		::hash_combine(seed, v.second);
		return seed;
	}
};

struct XNetConnectionReqPacket 
{
	DWORD ConnectPacketIdentifier;
	XNADDR xnaddr;
	XNKID xnkid;
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
	int handleRecvdPacket(XSocket* xsocket, sockaddr_in* lpFrom, WSABUF* lpBuffers, int bytesRecvdCount);

	void UnregisterLocalConnectionInfo();
	void GetStartupParamsAndUpdate(const XNetStartupParams* netStartupParams);
	IN_ADDR GetConnectionIdentifierByNat(sockaddr_in* addr);
	void SaveConnectionNatInfo(XSocket* xsocket, IN_ADDR ipIdentifier, sockaddr_in* addr);
	void HandleConnectionPacket(XSocket* xsocket, XNetConnectionReqPacket* connectReqPacket, sockaddr_in* addr);
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

	DWORD connectPacketIdentifier = 0x8E0A40F8; // DO NOT TOUCH THIS

private:
	XNKID host_xnkid;
	XNKEY host_xnkey;
};

extern CXnIp ipManager;
void SetUserUsername(char* username);
