#pragma once 

#include "stdafx.h"

struct XnIp
{
	IN_ADDR connectionIdentifier;
	XNADDR xnaddr;
	XNKID xnkid;
	bool bValid;
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
	void CreateXnIpIdentifier(const XNADDR* pxna, const XNKID* xnkid, IN_ADDR* outIpIdentifier);
	void UnregisterSecureAddr(const IN_ADDR ina);

	void UpdateConnectionStatus();
	BOOL LocalUserLoggedIn();
	void UnregisterLocal();
	void ConfigureLocalUser(XNADDR* pxna, XUID xuid, char* username);
	int getConnectionIndex(IN_ADDR connectionIdentifier);
	int sendNatInfoUpdate(SOCKET s, short port);

	void SaveNatInfo(IN_ADDR ipIdentifier, sockaddr* addr);
	void SetKeys(XNKID*, XNKEY*);
	void EraseKeys();
	void GetKeys(XNKID* xnkid, XNKEY* xnkey);

	BOOL GetLocalXNAddr(XNADDR* pxna);

	std::array<XnIp, 32> XnIPs; // ConnectionIndex->CUser
	std::unordered_map<std::pair<ULONG, SHORT>, IN_ADDR> connection_identifiers_map; // Map Key(XNHost,XnPort)->Secure
	std::array<SHORT, 32> pmap_a;
	std::array<SHORT, 32> pmap_b;
	//std::array<SHORT, 32> pmap_c;
	//std::array<SHORT, 32> pmap_d;
	std::unordered_map<SOCKET, SHORT> sockmap;

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
