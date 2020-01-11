#pragma once 

#include "stdafx.h"
#include "..\Sockets\XSocket.h"

struct XnIp
{
	IN_ADDR connectionIdentifier;
	XNADDR xnaddr;
	XNKID xnkid;
	bool bValid;
	int xnetstatus;
	int connectionPacketsSentCount;

	// NAT info
	sockaddr_in NatAddrSocket1000; // TODO: allocate dynamically based on how many sockets are up
	sockaddr_in NatAddrSocket1001;
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

struct SecurePacket 
{
	DWORD annoyance_factor;
	XNADDR xnaddr;
	XNKID xnkid;
};

class CXnIp
{
public:
	int CreateXnIpIdentifier(const XNADDR* pxna, const XNKID* xnkid, IN_ADDR* outIpIdentifier);
	void UnregisterSecureAddr(const IN_ADDR ina);

	void UpdateConnectionStatus();
	BOOL LocalUserLoggedIn();
	void UnregisterLocal();
	void ConfigureLocalUser(XNADDR* pxna, XUID xuid, char* username);
	int getConnectionIndex(IN_ADDR connectionIdentifier);
	int sendConnectionRequest(XSocket* xsocket, IN_ADDR ipIdentifier);

	IN_ADDR GetConnectionIdentifierByNat(sockaddr* addr);
	void SaveConnectionNatInfo(XSocket* xsocket, IN_ADDR ipIdentifier, sockaddr* addr);
	void HandleConnectionPacket(XSocket* xsocket, const XNADDR* pxna, const XNKID* xnkid, sockaddr* addr);
	void SetKeys(XNKID*, XNKEY*);
	void EraseKeys();
	void GetKeys(XNKID* xnkid, XNKEY* xnkey);

	BOOL GetLocalXNAddr(XNADDR* pxna);

	std::array<XnIp, 32> XnIPs = {}; // ConnectionIndex->CUser
	std::vector<XSocket*> SocketPtrArray = {};

	XnIp local_user;
	XNADDR game_host_xn;
	int total_connections_saved;
	SecurePacket securePacket;

private:
	XNKID host_xnkid;
	XNKEY host_xnkey;
};

extern wchar_t ServerLobbyName[32];
void SetUserUsername(char* username);
extern CXnIp ipManager;
extern const DWORD annoyance_factor;
