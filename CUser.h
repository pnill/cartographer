#ifndef CUSER_H
#define CUSER_H
#include "stdafx.h"
#include <unordered_map>
class CUser
{
public:
	XNADDR pxna;
	IN_ADDR pina;
	XNKID pxkid;
	time_t last_pong;
	BOOL bValid;
};

class CUserManagement
{
public:
	void GetSecureAddr(XNADDR* pxna, IN_ADDR* pina);
	void RegisterUser(XNADDR* pxna, ULONG pina);
	void GetXNAddr(XNADDR* pxna, const IN_ADDR pina);
	void Unregister(in_addr* pina);
	void Register(XNADDR* pxna, IN_ADDR* pina,int type);
	void RegisterLocalRequest();
	void UnregisterSecureAddr(const IN_ADDR ina);
	void RegisterSecureAddr(ULONG xnaddr, ULONG pina);
	ULONG GetLocalSecLong();
	ULONG GetXNLong(ULONG iplong);
	BOOL GetLocalXNAddr(XNADDR* pxna);
	std::unordered_map<ULONG, ULONG> xnmap;
	std::unordered_map<ULONG, ULONG> smap;
	std::unordered_map<ULONG, SHORT> pmap_a;
	std::unordered_map<ULONG, SHORT> pmap_b;
	std::unordered_map<ULONG, SHORT> pmap_c;
	std::unordered_map<ULONG, SHORT> pmap_d;
	std::unordered_map<SOCKET, SHORT> sockmap;

	int LastUser = 1;
};
#endif