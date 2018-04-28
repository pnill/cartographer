#include "CUser.h"
#include "stdafx.h"
#include "packet.pb.h"
#include <time.h>
#include <sstream>
#include "H2Config.h"
#include "H2OnscreenDebugLog.h"
#include "H2Startup.h"

extern SOCKET boundsock;
extern CHAR g_szUserName[4][16 + 1];
extern XUID xFakeXuid[4];


CUser Users[16];

enum
{
	_get_xnaddr,
	_get_secureaddr
};

ULONG CUserManagement::GetSecureFromXN(XNADDR* pxna)
{
	TRACE("CUserManagement::GetSecureFromXN()");
	ULONG secure;
	sockaddr_in RecvAddr;

	RecvAddr.sin_family = AF_INET;
	RecvAddr.sin_port = htons(H2Config_master_port_login);
	RecvAddr.sin_addr.s_addr = H2Config_master_ip;

	char RecvBuf[2048];
	int RecvResult;

	DWORD dwTime = 20;


	sockaddr_in SenderAddr;
	int SenderAddrSize = sizeof(SenderAddr);

	if (setsockopt(boundsock, SOL_SOCKET, SO_RCVTIMEO, (const char*)&dwTime, sizeof(dwTime)) < 0)
	{
		TRACE("Socket Error on register request");
	}
		
	Packet pak;
	pak.set_type(Packet_Type_secure_request);
	secure_request *secure_req = pak.mutable_srequest();

	std::string ab(reinterpret_cast<const char*>(pxna->abEnet), 6);
	secure_req->set_abenet(ab.c_str(),6);

	char* SendBuf = new char[pak.ByteSize()];
	pak.SerializeToArray(SendBuf, pak.ByteSize());

	sendto(boundsock, SendBuf, pak.ByteSize(), 0, (SOCKADDR*)&RecvAddr, sizeof(RecvAddr));

	time_t start = time(0);

	while (1)
	{
		TRACE("Trying recvfrom - GetSecureFromXN()");

		double seconds_since_start = difftime(time(0), start);

		if (seconds_since_start > 2)
		{
			sendto(boundsock, SendBuf, pak.ByteSize(), 0, (SOCKADDR*)&RecvAddr, sizeof(RecvAddr));
			start = time(0);
		}

		RecvResult = recvfrom(boundsock, RecvBuf, 2048, 0, (SOCKADDR*)&SenderAddr, &SenderAddrSize);

		if (RecvResult > 0)
		{
			Packet RecvPak;
			RecvPak.ParseFromArray(RecvBuf, RecvResult);

			if (RecvPak.has_sreply())
			{
				TRACE("GetSecureFromXN() - Got Secure Reply");
				secure = RecvPak.sreply().secure();
			
				
				CUser *nUser = new CUser;

				memset(&nUser->pxna, 0x00, sizeof(XNADDR));
				nUser->pxna.wPortOnline = htons((short)RecvPak.sreply().port());
				
				nUser->pxna.ina.s_addr = RecvPak.sreply().xnaddr();
				nUser->pxna.inaOnline.s_addr = secure;
				memcpy(nUser->pxna.abEnet, RecvPak.sreply().abenet().c_str(), 6);
				memcpy(nUser->pxna.abOnline, RecvPak.sreply().abonline().c_str(), 20);

				TRACE("GetSecureFromXN() - nUser->pxna.wPortOnline: %i", ntohs(nUser->pxna.wPortOnline));
				TRACE("GetSecureFromXN() - secure: %08X", secure);
				TRACE("GetSecureFromXN() - pxna->xnaddr: %08X", pxna->ina.s_addr);

				nUser->pina.s_addr = secure;
				nUser->bValid = true;

				this->cusers[secure] = nUser;
				this->xnmap[secure] = pxna->ina.s_addr;
				this->xntosecure[ab] = secure;
	
				RecvPak.Clear();
				break;
			
			}

			RecvPak.Clear();
		}
	}


	delete[] SendBuf;
	secure_req->Clear();
	pak.Clear();

	TRACE("CUserManagement::GetSecureFromXN() - END - secure: %08X",secure);
	return secure;
}

void CUserManagement::CreateUser(XNADDR* pxna, BOOL user)
{
	/*
		This only happens for servers because we have all their data from the get go :)...

		- Update 1/20/2017 - 
		This should now also be called when receiving the 'SecurityPacket' because we have the data on the first attempt to either establish a connection OR on the attempt to join a game,
		That should eliminate the need to talk to the Master server in order to get the XNADDR information from the secure address.
	*/
	TRACE("CUserManagement::CreateUser()");

	ULONG secure = pxna->inaOnline.s_addr;
	CUser *nUser = new CUser;
	memset(&nUser->pxna, 0x00, sizeof(XNADDR));

	memcpy(&nUser->pxna, pxna, sizeof(XNADDR));
	nUser->pina.s_addr = secure;
	std::string ab(reinterpret_cast<const char*>(pxna->abEnet), 6);

	nUser->bValid = true;

	/*
		In theory to handle multiple instance servers in the future what we can do is populate the port field of CreateUser,
		Then map the shit before we actually attempt a connection to the server here...

		That way we intercept it and don't even have to modify the game's engine.

		While we only have enough room for one port in the XNADDR struct we can just do, port+1.

		So,
		"connect" socket = 2001 if the XNADDR struct is 2000 which is the "join" socket.

		Then once TeamSpeak is in just do +6 because the game traditionally tries to map 1000-1006 so we'll just go 2007 = TS, etc.

		This should allow us to handle servers listening on any port without much effort or engine modification.
	*/
	if (user == FALSE)
	{
		/* This happens when joining a server, it's a fix to dynamic ports... */

		short nPort_base = pxna->wPortOnline;
		short nPort_join = htons(ntohs(pxna->wPortOnline) + 1);

		TRACE("CreateUser - nPort_base: %i", ntohs(nPort_base));
		TRACE("CreateUser - nPort_join: %i", ntohs(nPort_join));

		std::pair <ULONG, SHORT> hostpair = std::make_pair(pxna->ina.s_addr, nPort_join);
		std::pair <ULONG, SHORT> hostpair_1000 = std::make_pair(pxna->ina.s_addr, nPort_base);

		//std::pair <ULONG, SHORT> hostpair = std::make_pair(pxna->ina.s_addr, htons(1001)); // FIX ME ^, this is gonna get really fucked when servers listen on other ports...
		//std::pair <ULONG, SHORT> hostpair_1000 = std::make_pair(pxna->ina.s_addr, htons(1000)); // FIX ME ^


		if (H2Config_ip_wan == pxna->ina.s_addr)
		{
			std::pair <ULONG, SHORT> hostpair_or = std::make_pair(H2Config_ip_lan, nPort_join);
			std::pair <ULONG, SHORT> hostpair_1000_or = std::make_pair(H2Config_ip_lan, nPort_base);

			this->smap[hostpair_or] = secure;
			this->smap[hostpair_1000_or] = secure;

		}

		this->smap[hostpair] = secure;
		this->smap[hostpair_1000] = secure;
		this->cusers[secure] = nUser;
		this->xnmap[secure] = pxna->ina.s_addr;
		this->xntosecure[ab] = secure;

		this->pmap_a[secure] = nPort_base;
		this->pmap_b[secure] = nPort_join;
		//this->pmap_a[secure] = htons(1000); // FIX ME, ^ SEE ABOVE 1000 STATIC DEF.
		//this->pmap_b[secure] = htons(1001); // FIX ME, ^ SEE ABOVE 1001 STATIC DEF.
	}
	else 
	{
		/* 
			If the user variable is true, we update cusers "array" to map the secure address to the newly created object...
			Then map the secure address to the WAN IP (XN->ina) using xnmap, where secure address is the key to the array.
			Then map the abenet to the secure address via xntosecure[ab], where AB is the key to the array. 
		*/
		this->cusers[secure] = nUser;
		this->xnmap[secure] = pxna->ina.s_addr;
		this->xntosecure[ab] = secure;

	}


}

ULONG CUserManagement::GetXNFromSecure(ULONG secure)
{	
	TRACE("CUserManagement::GetXNFromSecure(%08X)",secure);
	u_long xnaddress;
	sockaddr_in RecvAddr;

	RecvAddr.sin_family = AF_INET;
	RecvAddr.sin_port = htons(H2Config_master_port_login);
	RecvAddr.sin_addr.s_addr = H2Config_master_ip;

	char RecvBuf[2048];
	int RecvResult;

	DWORD dwTime = 20;


	sockaddr_in SenderAddr;
	int SenderAddrSize = sizeof(SenderAddr);

	if (setsockopt(boundsock, SOL_SOCKET, SO_RCVTIMEO, (const char*)&dwTime, sizeof(dwTime)) < 0)
	{
		TRACE("Socket Error on register request");
	}

	Packet xnrequest;
	xnrequest.set_type(Packet_Type_xnaddr_request);

	xnaddr_request *rdata = xnrequest.mutable_xrequest();
	rdata->set_secure(secure);

	char *secr = new char[xnrequest.ByteSize()];
	xnrequest.SerializeToArray( secr, xnrequest.ByteSize() );
	
	sendto(boundsock, secr, xnrequest.ByteSize(), 0, (SOCKADDR*)&RecvAddr, sizeof(RecvAddr));

	time_t start = time(0);

	while (1)
	{
		TRACE("Trying recvfrom - GetXNFromSecure()");

		double seconds_since_start = difftime(time(0), start);

		if (seconds_since_start > 2)
		{
			sendto(boundsock, secr, xnrequest.ByteSize(), 0, (SOCKADDR*)&RecvAddr, sizeof(RecvAddr));
			start = time(0);
		}
		
		RecvResult = recvfrom(boundsock, RecvBuf, 2048, 0, (SOCKADDR*)&SenderAddr, &SenderAddrSize);

		if (RecvResult > 0)
		{
			Packet RecvPak;
			RecvPak.ParseFromArray(RecvBuf, RecvResult);
			if (RecvPak.has_xreply())
			{
				TRACE("GetXNFromSecure() - Reading Xreply data");
				xnaddress = RecvPak.xreply().xnaddr();
				short recvport = (short)RecvPak.xreply().port();
				std::string abEnet = RecvPak.xreply().abenet();
				std::string abOnline = RecvPak.xreply().abonline();

				CUser *nUser = new CUser;
				memset(&nUser->pxna, 0x00, sizeof(XNADDR));
				
				nUser->pxna.wPortOnline = htons(recvport);
				nUser->pxna.ina.s_addr = xnaddress;
				nUser->pxna.inaOnline.s_addr = secure;

				memcpy(nUser->pxna.abEnet, abEnet.c_str(), 6);
				memcpy(nUser->pxna.abOnline, abOnline.c_str(), 20);
				nUser->pina.s_addr = secure;
				nUser->bValid = true;

				TRACE("GetXNFromSecure() - nUser->pxna.wPortOnline: %i", ntohs(nUser->pxna.wPortOnline));
				TRACE("GetXNFromSecure() - secure: %08X", secure);
				TRACE("GetXNFromSecure() - xnaddr: %08X", xnaddress);

				this->cusers[secure] = nUser;
				this->xnmap[secure] = xnaddress;
				this->xntosecure[abEnet] = secure;

				RecvPak.Clear();
				break;
			}
			RecvPak.Clear();
		}

	}


	rdata->Clear();
	xnrequest.Clear();

	delete[] secr;

	return xnaddress;
}

void CUserManagement::UnregisterSecureAddr(const IN_ADDR ina)
{
	CUser* deluser = cusers[ina.s_addr];
	this->cusers.erase(ina.s_addr);


	if (deluser != 0)
	{
		std::string ab(reinterpret_cast<const char*>(deluser->pxna.abEnet), 6);
		delete[] deluser;
			
		if (xntosecure[ab] != 0)
			xntosecure.erase(ab);
		else
			TRACE("XNetUnregisterInAddr() xntosecure couldn't find that abenet");


		if (xnmap[ina.s_addr])
			xnmap.erase(ina.s_addr);
		else
			TRACE("XNetUnregisterInAddr() xnmap couldn't find that secure address");

		std::pair <ULONG, SHORT> hostpair = std::make_pair(ina.s_addr, pmap_a[ina.s_addr]);
		std::pair <ULONG, SHORT> hostpair2 = std::make_pair(ina.s_addr, pmap_b[ina.s_addr]);

		this->sentmap.erase(hostpair);
		this->sentmap.erase(hostpair2);

		if (pmap_a[ina.s_addr])
			pmap_a.erase(ina.s_addr);
		else
			TRACE("XNetUnregisterInAddr() pmap_a couldn't find that secure address");

		if (pmap_b[ina.s_addr])
			pmap_b.erase(ina.s_addr);
		else
			TRACE("XnetUnregisterInAddr() pmap_b couldn't find that secure address");
		
		typedef std::unordered_map<std::pair<ULONG, SHORT>, ULONG>::iterator it_type;

		for (it_type iterator = smap.begin(); iterator != smap.end(); ++iterator)
		{
			if (iterator->second == ina.s_addr)
			{
				TRACE("XNetUnregisterInAddr: Found secure addr in the smap deleting it.");
				smap.erase(iterator->first);
				break;
			}
		}

	}

}


void CUserManagement::UpdateConnectionStatus() {
	extern int MasterState;
	extern char* ServerStatus;
	if (this->LocalUserLoggedIn()) {
		MasterState = 10;
		if (!H2IsDediServer)
			snprintf(ServerStatus, 250, "Status: Online");
	}
	else
	{
		MasterState = 2;
		if (!H2IsDediServer)
			snprintf(ServerStatus, 250, "Status: Offline");
	}
}

BOOL CUserManagement::LocalUserLoggedIn() {
	return Users[0].bValid;
}

void CUserManagement::UnregisterLocal()
{
	if (!Users[0].bValid)
		return;

	delete[] this->SecurityPacket;
	Users[0].bValid = false;
	this->UpdateConnectionStatus();
}

void SetUserUsername(char* username) {

	ZeroMemory(g_szUserName[0], 17);
	snprintf(g_szUserName[0], 16, username);
	if (!H2IsDediServer) {
		snprintf((char*)((BYTE*)H2BaseAddr + 0x971316), 17, username);
		swprintf((wchar_t*)((BYTE*)H2BaseAddr + 0x96DA94), 17, L"%hs", username);
		swprintf((wchar_t*)((BYTE*)H2BaseAddr + 0x51A638), 17, L"%hs", username);
	}
}

const DWORD annoyance_factor = 0x11223341;

void CUserManagement::ConfigureUser(XNADDR* pxna, ULONGLONG xuid, char* username) {
	if (Users[0].bValid) {
		delete[] this->SecurityPacket;
		Users[0].bValid = false;
	}

	memcpy(&Users[0].pxna, pxna, sizeof(XNADDR));

	this->SecurityPacket = new char[8 + sizeof(XNADDR)];
	(*(DWORD*)&this->SecurityPacket[0]) = annoyance_factor;
	(*(DWORD*)&this->SecurityPacket[4]) = pxna->inaOnline.s_addr;

	SecurityPacket = this->SecurityPacket;

	Users[0].pina.s_addr = pxna->inaOnline.s_addr;

	xFakeXuid[0] = xuid;

	memcpy(&SecurityPacket[8], &Users[0].pxna, sizeof(XNADDR));


	LocalXN = &Users[0].pxna;
	LocalSec = pxna->inaOnline.s_addr;

	SetUserUsername(username);

	Users[0].bValid = true;

	this->UpdateConnectionStatus();
}

BOOL CUserManagement::GetLocalXNAddr(XNADDR* pxna)
{
	if (Users[0].bValid)
	{
		memcpy(pxna, &Users[0].pxna, sizeof(XNADDR));
		TRACE("GetLocalXNAddr: Returned");
		TRACE("XNADDR: %08X", pxna->ina.s_addr);
		return TRUE;
	}
	TRACE("GetLocalXNADDR: User data not populated yet.");
	//TRACE("GetLocalXNADDR: User data not populated yet calling LocalRegister()");
	//this->RegisterLocalRequest(H2Config_login_token);
	//Sleep(3000);

	return FALSE;
}




void CUserManagement::RegisterLocalRequest(char* token, int a2)
{
	if (Users[0].bValid)
		return;

	addDebugText("next0");

	u_long xnaddress;
	sockaddr_in RecvAddr;

	RecvAddr.sin_family = AF_INET;
	RecvAddr.sin_port = htons(H2Config_master_port_login);
	RecvAddr.sin_addr.s_addr = H2Config_master_ip;

	char RecvBuf[2048];
	int RecvResult;

	sockaddr_in SenderAddr;
	int SenderAddrSize = sizeof(SenderAddr);

	//DWORD dwTime = 20;

	struct timeval tv;
	tv.tv_sec = 2;
	tv.tv_usec = 0;
	if (setsockopt(boundsock, SOL_SOCKET, SO_RCVTIMEO, (const char*)&tv, sizeof(tv)) < 0) {
	//if (setsockopt(boundsock, SOL_SOCKET, SO_RCVTIMEO, (const char*)&dwTime, sizeof(dwTime)) < 0) {
		TRACE("Socket Error on register request");
	}

	Packet lrequest;
	lrequest.set_type(Packet_Type_login_request);

	login_request *ldata = lrequest.mutable_lrequest();
	ldata->set_login_token(token);
	ldata->set_port(H2Config_base_port);

	char *lreq = new char[lrequest.ByteSize()];

	lrequest.SerializeToArray(lreq, lrequest.ByteSize());

	if (!a2)
		sendto(boundsock, lreq, lrequest.ByteSize(), 0, (SOCKADDR*)&RecvAddr, sizeof(RecvAddr));
	else {
		RecvResult = recvfrom(boundsock, RecvBuf, 2048, 0, (SOCKADDR*)&SenderAddr, &SenderAddrSize);

		if (RecvResult > 0)
		{
			addDebugText("network reply");

			Packet RecvPak;
			RecvPak.ParseFromArray(RecvBuf, RecvResult);

			if (RecvPak.has_lreply())
			{
				TRACE("GetLocalRegistration() - Reading lreply data");

				char* login_token;
				ULONGLONG xuid = RecvPak.lreply().xuid();
				unsigned long saddr = RecvPak.lreply().secure_addr();
				unsigned long xnaddr = RecvPak.lreply().xnaddr();
				char abEnet[13] = { "" };
				char abOnline[41] = { "" };
				snprintf(abEnet, 13, RecvPak.lreply().abenet().c_str());
				snprintf(abOnline, 41, RecvPak.lreply().abonline().c_str());

				XNADDR pxna;

				pxna.inaOnline.s_addr = saddr;
				pxna.ina.s_addr = xnaddr;
				pxna.wPortOnline = htons((short)H2Config_base_port);
				memcpy(&pxna.abEnet, abEnet, 6);
				memcpy(&pxna.abOnline, abOnline, 20);

				extern CUserManagement User;
				User.ConfigureUser(&pxna, xuid, g_szUserName[0]);


				//*(__int64*)((BYTE*)H2BaseAddr + 0x96C860) = RecvPak.lreply().xuid();

				int(__cdecl* sub_209236)(int) = (int(__cdecl*)(int))((char*)H2BaseAddr + 0x209236);
				sub_209236(0);


				Users[0].bValid = true;

				this->UpdateConnectionStatus();
			}

			RecvPak.Clear();
		}
	}

	ldata->Clear();
	lrequest.Clear();

	delete[] lreq;

}
