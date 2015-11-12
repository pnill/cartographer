#include "CUser.h"
#include "stdafx.h"
#include "packet.pb.h"
#include <time.h>
#include <sstream>

extern ULONG broadcast_server;
extern SOCKET boundsock;

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

	unsigned short port = 27019;

	RecvAddr.sin_family = AF_INET;
	RecvAddr.sin_port = htons(port);
	RecvAddr.sin_addr.s_addr = broadcast_server;

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
			
				pxna->inaOnline.s_addr = 0x00000000;
				CUser *nUser = new CUser;
				nUser->pxna.ina.s_addr = pxna->ina.s_addr;
				nUser->pxna.wPortOnline = pxna->wPortOnline;
				nUser->pxna.inaOnline = pxna->inaOnline;
				memcpy(nUser->pxna.abEnet, pxna->abEnet, 6);
				memcpy(nUser->pxna.abOnline, pxna->abOnline, 20);

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

ULONG CUserManagement::GetXNFromSecure(ULONG secure)
{	
	TRACE("CUserManagement::GetXNFromSecure(%08X)",secure);
	u_long xnaddress;
	sockaddr_in RecvAddr;

	unsigned short port = 27019;

	RecvAddr.sin_family = AF_INET;
	RecvAddr.sin_port = htons(port);
	RecvAddr.sin_addr.s_addr = broadcast_server;

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
				short port = RecvPak.xreply().port();
				std::string abEnet = RecvPak.xreply().abenet();
				std::string abOnline = RecvPak.xreply().abonline();

				CUser *nUser = new CUser;
				nUser->pxna.ina.s_addr = xnaddress;
				nUser->pxna.wPortOnline = 0x0000;
				nUser->pxna.inaOnline.s_addr = 0x00000000;
				memcpy(nUser->pxna.abEnet, abEnet.c_str(), 6);
				memcpy(nUser->pxna.abOnline, abOnline.c_str(), 20);
				nUser->pina.s_addr = secure;
				nUser->bValid = true;

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
	for (int i = 1; i < 15; i++)
	{
		if (Users[i].pina.s_addr == ina.s_addr)
		{
		//	std::string ab(reinterpret_cast<const char*>(&Users[i].pxna.abEnet), 6);
		//	smap[ab] = 0;
			Users[i].bValid = false;
			Users[i].pina.s_addr = 0;
			memset(&Users[i].pxna, 0x00, sizeof(XNADDR));
			xnmap[ina.s_addr] = 0;
			LastUser = LastUser - 1;
		}
	}
}




void CUserManagement::RegisterLocalRequest()
{
	SOCKET SendSocket = INVALID_SOCKET;
	sockaddr_in RecvAddr;

	unsigned short port = 27019;

	RecvAddr.sin_family = AF_INET;
	RecvAddr.sin_port = htons(port);
	RecvAddr.sin_addr.s_addr = broadcast_server;
	
	sockaddr_in SenderAddr;
	int SenderAddrSize = sizeof(SenderAddr);

	int Result;
	Packet pak;
	pak.set_type(Packet_Type_local_request);
	
	char* SendBuf = new char[pak.ByteSize()];
	pak.SerializeToArray(SendBuf, pak.ByteSize());

	Result = sendto(boundsock, SendBuf, pak.ByteSize(), 0, (SOCKADDR*)&RecvAddr, sizeof(RecvAddr));
	time_t start = time(0);
	delete[] SendBuf;

	char RecvBuf[2048];
	int RecvResult;
	

	DWORD dwTime = 15;

	if (setsockopt(boundsock, SOL_SOCKET, SO_RCVTIMEO, (const char*)&dwTime, sizeof(dwTime)) < 0)
	{
		TRACE("RegisterLocalRequest() - Socket Error on register request");
	}

	TRACE("RegisterLocalRequest() - Trying recvfrom");

	RecvResult = recvfrom(boundsock, RecvBuf, 2048, 0, (SOCKADDR*)&SenderAddr, &SenderAddrSize);
	
	if (RecvResult > 0)
	{
		Packet RecvPak;
		RecvPak.ParseFromArray(RecvBuf, RecvResult);
		if (RecvPak.has_lconfirm())
		{
		

			TRACE("RegisterLocal() - Secure ADDR: %08X", RecvPak.lconfirm().secure_addr());
			TRACE("RegisterLocal() - XNADDR : %08X", RecvPak.lconfirm().xnaddr());

			TRACE_GAME_NETWORK("RegisterLocal() - Secure ADDR: %08X", RecvPak.lconfirm().secure_addr());
			TRACE_GAME_NETWORK("RegisterLocal() - XNADDR: %08X", RecvPak.lconfirm().xnaddr());

			TRACE("RegisterLocal() - Port: %08X", RecvPak.lconfirm().port());

			Packet ConfirmPak;
			ConfirmPak.set_type(Packet_Type_local_confirm);
			local_confirm *ldata = ConfirmPak.mutable_lconfirm();
		
			int port = RecvPak.lconfirm().port();
			ULONG secured_addr = RecvPak.lconfirm().secure_addr();
			ULONG _xnaddr = RecvPak.lconfirm().xnaddr();
			
		
			memset(&Users[0].pxna, 0x00, sizeof(XNADDR));
			this->SecurityPacket = new char[8];

			(*(DWORD*)&this->SecurityPacket[0]) = 0x11223344;
			(*(DWORD*)&this->SecurityPacket[4]) = secured_addr;

			Users[0].pina.s_addr = secured_addr;
			Users[0].pxna.ina.s_addr = _xnaddr;
			Users[0].pxna.inaOnline.s_addr = 0x00000000;
			Users[0].pxna.wPortOnline = 0x0000;

			memcpy(&Users[0].pxna.abEnet, RecvPak.lconfirm().abenet().c_str(), 6);
			memcpy(&Users[0].pxna.abOnline, RecvPak.lconfirm().abonline().c_str(), 20);

			LocalXN = &Users[0].pxna;
			LocalSec = secured_addr;

			Users[0].bValid = true;

			ldata->set_port(port);
			ldata->set_secure_addr(secured_addr);
			ldata->set_xnaddr(_xnaddr);
			ldata->set_abenet(RecvPak.lconfirm().abenet().c_str(),6);
			ldata->set_abonline(RecvPak.lconfirm().abonline().c_str(), 20);
			
			SendBuf = new char[ConfirmPak.ByteSize()];
			ConfirmPak.SerializeToArray(SendBuf, ConfirmPak.ByteSize());

			sendto(boundsock, SendBuf, ConfirmPak.ByteSize(), 0, (SOCKADDR*)&RecvAddr, sizeof(RecvAddr));

			ldata->Clear();
			ConfirmPak.Clear();

			delete[] SendBuf;
			
		}

	}

	if (RecvResult == SOCKET_ERROR)
	{

		TRACE("RegisterLocalRequest - ERROR: %i", WSAGetLastError());
	}

}


BOOL CUserManagement::GetLocalXNAddr(XNADDR* pxna)
{
	if (Users[0].bValid)
	{
		memcpy(pxna, &Users[0].pxna, sizeof(XNADDR));
		TRACE("GetLocalXNAddr: Returned");
		return TRUE;
	}

	TRACE("GetLocalXNADDR: User data not populated yet calling LocalRegister()");
	this->RegisterLocalRequest();

	return FALSE;
}