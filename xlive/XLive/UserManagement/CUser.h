#pragma once 

#include "stdafx.h"
#include <unordered_map>
#include <mutex>

class CUser
{
public:
	XNADDR xnaddr;
	XNKID xnkid;
	IN_ADDR secure;
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

class CUserManagement
{
public:
	ULONG GetXNFromSecure(ULONG secure);
	ULONG GetSecureFromXN(XNADDR *pxna);

	void CreateUser(const XNADDR *pxna, BOOL user);
	void RegisterLocalRequest(char* token, int a2);
	void UnregisterSecureAddr(const IN_ADDR ina);

	void UpdateConnectionStatus();
	BOOL LocalUserLoggedIn();
	void UnregisterLocal();
	void ConfigureLocalUser(XNADDR* pxna, ULONGLONG xuid, char* username);
	int sendSecurePacket(SOCKET s, short to_port);

	BOOL GetLocalXNAddr(XNADDR* pxna);

	std::unordered_map<ULONG, CUser*> cusers; // Map Key(SecureADDR)->CUser
	std::unordered_map<std::pair<ULONG, SHORT>, ULONG> secure_map; // Map Key(XNHost,XnPort)->Secure
	std::unordered_map<ULONG, ULONG> xnmap; // Map Key(Secure)->XNHost;
	std::unordered_map<std::string, ULONG> xntosecure; //Map Key(Xn->Abenet)->Secure Addr
	std::unordered_map<ULONG, SHORT> pmap_a;
	std::unordered_map<ULONG, SHORT> pmap_b;
	std::unordered_map<ULONG, SHORT> pmap_c;
	std::unordered_map<ULONG, SHORT> pmap_d;
	std::unordered_map<SOCKET, SHORT> sockmap;

	CUser local_user;
	XNADDR game_host_xn;
	char* secure_packet;
};

extern wchar_t ServerLobbyName[32];
void SetUserUsername(char* username);
extern CUserManagement userManager;
extern const DWORD annoyance_factor;
