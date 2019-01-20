#pragma once 

#include "stdafx.h"
#include <unordered_map>
#include <mutex>

class CUser
{
public:
	XNADDR pxna;
	IN_ADDR pina;
	XNKID pxkid;
	time_t last_pong;
	BOOL bValid;
};

template <class T>
inline void hash_combine(std::size_t & seed, const T & v)
{
	std::hash<T> hasher;
	seed ^= hasher(v) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
}

namespace std
{
	template<typename S, typename T> struct hash < pair<S, T> >
	{
		inline size_t operator()(const pair<S, T> & v) const
		{
			size_t seed = 0;
			::hash_combine(seed, v.first);
			::hash_combine(seed, v.second);
			return seed;
		}
	};
}

void SetUserUsername(char* username);
extern const DWORD annoyance_factor;

class CUserManagement
{
public:
	ULONG GetXNFromSecure(ULONG secure);
	ULONG GetSecureFromXN(XNADDR *pxna);

	void CreateUser(XNADDR *pxna, BOOL user);
	void RegisterLocalRequest(char* token, int a2);
	void UnregisterSecureAddr(const IN_ADDR ina);

	void UpdateConnectionStatus();
	BOOL LocalUserLoggedIn();
	void UnregisterLocal();
	void ConfigureUser(XNADDR* pxna, ULONGLONG xuid, char* username);

	BOOL GetLocalXNAddr(XNADDR* pxna);



	//Maps
	std::mutex xntosecure_mutex;
	std::mutex cusers_mutex;
	std::mutex smap_mutex;
	std::mutex sentmap_mutex;
	std::mutex stox_mutex;
	std::mutex pmap_a_mutex;
	std::mutex pmap_b_mutex;
	std::mutex sockmap_mutex;
	std::mutex xnmap_mutex;

	std::unordered_map<ULONG, CUser*> cusers; // Map Key(SecureADDR)->CUser
	std::unordered_map<std::pair<ULONG, SHORT>, ULONG> smap; // Map Key(XNHost,XnPort)->Secure
	std::unordered_map<std::pair<ULONG, SHORT>, ULONG> sentmap; // Map of servers/clients+ports which have already been sent to.
	std::unordered_map<ULONG, ULONG> stox;
	std::unordered_map<ULONG, ULONG> xnmap; // Map Key(Secure)->XNHost;
	std::unordered_map<std::string, ULONG> xntosecure; //Map Key(Xn->Abenet)->Secure Addr
	std::unordered_map<ULONG, SHORT> pmap_a;
	std::unordered_map<ULONG, SHORT> pmap_b;
	std::unordered_map<ULONG, SHORT> pmap_c;
	std::unordered_map<ULONG, SHORT> pmap_d;
	std::unordered_map<SOCKET, SHORT> sockmap;
	
	



	char* SecurityPacket;

	XNADDR *LocalXN;
	ULONG LocalSec;

	int LastUser = 1;
};
