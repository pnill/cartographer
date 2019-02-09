#pragma once 

#include "stdafx.h"
#include <unordered_map>
#include <mutex>

class CUser
{
public:
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

class CUserManagement
{
public:
	void CreateUser(const XNADDR *pxna);
	void UnregisterSecureAddr(const IN_ADDR ina);

	void UpdateConnectionStatus();
	BOOL LocalUserLoggedIn();
	void UnregisterLocal();
	void ConfigureLocalUser(XNADDR* pxna, ULONGLONG xuid, char* username);

	BOOL GetLocalXNAddr(XNADDR* pxna);
	std::unordered_map<ULONG, CUser*> iplong_to_user; // saddr(security key)->CUser
	XNADDR game_host_xn;
};

extern wchar_t ServerLobbyName[32];
void SetUserUsername(char* username);
extern const DWORD annoyance_factor;
extern CUserManagement userManager;
