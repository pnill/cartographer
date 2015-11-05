#include "CUser.h"
#include "stdafx.h"
#include "packet.pb.h"

extern ULONG broadcast_server;
extern SOCKET boundsock;

CUser Users[16];

enum
{
	_get_xnaddr,
	_get_secureaddr
};

void CUserManagement::UnregisterSecureAddr(const IN_ADDR ina)
{
	for (int i = 0; i < 16; i++)
	{
		if (Users[i].pina.s_addr == ina.s_addr)
		{
			smap[Users[i].pxna.ina.s_addr] = 0;
			Users[i].bValid = false;
			Users[i].pina.s_addr = 0;
			memset(&Users[i].pxna, 0x00, sizeof(XNADDR));
			xnmap[ina.s_addr] = 0;
			LastUser = LastUser - 1;
		}
	}
}

ULONG CUserManagement::GetLocalSecLong()
{
	return Users[0].pina.s_addr;
}
ULONG CUserManagement::GetXNLong(ULONG iplong)
{
	for (int i = 0; i < 16; i++)
	{
		if (Users[i].pina.s_addr == iplong && Users[i].bValid == true)
		{
			return Users[i].pxna.ina.s_addr;
		}
	}
}

void CUserManagement::RegisterSecureAddr(ULONG xnaddr, ULONG pina)
{
	BOOL UserExist = FALSE;

}

void CUserManagement::GetSecureAddr(XNADDR* pxna, IN_ADDR* pina)
{
	BOOL UserExist = FALSE;
	for (int i = 0; i < 16; i++)
	{
		if (!memcmp(&Users[i].pxna.abEnet, &pxna->abEnet, 6) && Users[i].bValid)
		{
			pina->s_addr = Users[i].pina.s_addr;
			
			if (pina->s_addr != 0x00000000)
			{
				TRACE("GetSecureAddr: User Exists");
				UserExist = TRUE;
			}
			TRACE("GetSecureAddr: pina: %08X", pina->s_addr);
			TRACE("GetSecureAddr: pxna: %08X", pxna->ina.s_addr);
			break;
		}
	}
	if (UserExist == FALSE)
	{
		TRACE("GetSecureAddr: User does not exist calling Register() _get_secureaddr");
		this->Register(pxna, pina, _get_secureaddr);
	}
}

void CUserManagement::GetXNAddr(XNADDR* pxna, const IN_ADDR pina)
{

	BOOL UserExist = FALSE;
	for (int i = 0; i < 16; i++)
	{
		if (Users[i].pina.s_addr == pina.s_addr && Users[i].bValid)
		{
			memcpy(pxna, &Users[i].pxna, sizeof(XNADDR));
			TRACE("GetXNAddr: %08X", pxna->ina.s_addr);
			TRACE("GetXNAddr: %08X", pina.s_addr);

			if (pxna->ina.s_addr != 0x00000000)
			{
				UserExist = TRUE;
				TRACE("GetXNAddr: User Exists");
			}
			break;
		}
	}
	
	if (UserExist == FALSE)
	{
		TRACE("GetXNAddr: User does not exist calling Register() _get_xnaddr");
		this->Register(pxna, (IN_ADDR*)&pina, _get_xnaddr);
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

	delete[] SendBuf;

	char RecvBuf[2048];
	int RecvResult;
	

	DWORD dwTime = 5;

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

			Users[0].pina.s_addr = secured_addr;
			Users[0].pxna.ina.s_addr = _xnaddr;


			memcpy(&Users[0].pxna.abEnet, RecvPak.lconfirm().abenet().c_str(), 6);
			memcpy(&Users[0].pxna.abOnline, RecvPak.lconfirm().abonline().c_str(), 20);
			Users[0].bValid = true;

			ldata->set_port(port);
			ldata->set_secure_addr(secured_addr);
			ldata->set_xnaddr(_xnaddr);
			ldata->set_abenet(RecvPak.lconfirm().abenet().c_str(),6);
			ldata->set_abonline(RecvPak.lconfirm().abonline().c_str(), 20);
			
			SendBuf = new char[ConfirmPak.ByteSize()];
			ConfirmPak.SerializeToArray(SendBuf, ConfirmPak.ByteSize());

			sendto(boundsock, SendBuf, ConfirmPak.ByteSize(), 0, (SOCKADDR*)&RecvAddr, sizeof(RecvAddr));

			

		}

	}

	if (RecvResult == SOCKET_ERROR)
	{

		TRACE("RegisterLocalRequest - ERROR: %i", WSAGetLastError());
	}

}

void CUserManagement::RegisterUser(XNADDR* pxna, ULONG pina)
{

	BOOL UserExist = FALSE;
	int CurrentUser = LastUser;
	for(int i = 1; i < 15;i++)
	{
		if (!memcmp(&Users[i].pxna.abEnet, &pxna->abEnet, 6))
		{
			TRACE("RegisterUser(): User existed");
			UserExist = TRUE;
			CurrentUser = i;
		}

	}
	
	if (!UserExist)
	{
		TRACE("RegisterUser(): User did not exist");
		memcpy(&Users[CurrentUser].pxna, pxna, sizeof(XNADDR));
		Users[CurrentUser].pina.s_addr = pina;
		Users[CurrentUser].bValid = TRUE;
		LastUser++;
	}
}

void CUserManagement::Register(XNADDR *pxna, IN_ADDR *pina, int type)
{
	sockaddr_in RecvAddr;

	unsigned short port = 27019;

	RecvAddr.sin_family = AF_INET;
	RecvAddr.sin_port = htons(port);
	RecvAddr.sin_addr.s_addr = broadcast_server;

	char RecvBuf[2048];
	int RecvResult;

	DWORD dwTime = 5;


	sockaddr_in SenderAddr;
	int SenderAddrSize = sizeof(SenderAddr);

	if (setsockopt(boundsock, SOL_SOCKET, SO_RCVTIMEO, (const char*)&dwTime, sizeof(dwTime)) < 0)
	{
		TRACE("Socket Error on register request");
	}

	if (type == _get_xnaddr) // Get XNADDR from Secure Addr
	{
		Packet Pack;
		Pack.set_type(Packet_Type_xnaddr_request);
		xnaddr_request* xnreq = Pack.mutable_xrequest();
		xnreq->set_secure(pina->s_addr);

		char* SendBuf = new char[Pack.ByteSize()];
		Pack.SerializeToArray(SendBuf, Pack.ByteSize());

		sendto(boundsock, SendBuf, Pack.ByteSize(), 0, (SOCKADDR*)&RecvAddr, sizeof(RecvAddr));

		delete[] SendBuf;

		while (1)
		{
			TRACE("Trying recvfrom - Register() _get_xnaddr");

			RecvResult = recvfrom(boundsock, RecvBuf, 2048, 0, (SOCKADDR*)&SenderAddr, &SenderAddrSize);

			if (RecvResult > 0)
			{

				Packet RecvPak;
				RecvPak.ParseFromArray(RecvBuf, RecvResult);
				if (RecvPak.has_xreply())
				{
					int CurrentUser = LastUser;
					bool UserExist = FALSE;
					TRACE("Register() _get_xnaddr - XNADDR Reply Size: %i", RecvPak.ByteSize());
					u_long xnaddress = RecvPak.xreply().xnaddr();
					int port = RecvPak.xreply().port();

					TRACE("Register() _get_xnaddr - xnaddress: %08X", xnaddress);
					TRACE("Register() _get_xnaddr - xnport: %08X", port);
					TRACE_GAME_NETWORK("Register() _get_xnaddr - xnaddress: %08X", xnaddress);
					
					memset(pxna, 0x00, sizeof(XNADDR));

					pxna->ina.s_addr = xnaddress;
					memcpy(pxna->abOnline, RecvPak.xreply().abonline().c_str(), 20);
					memcpy(pxna->abEnet,RecvPak.xreply().abenet().c_str(),6);
					for (int i = 0; i < sizeof(Users); i++)
					{
						if (Users[i].pina.s_addr == pina->s_addr && Users[i].bValid)
						{
							CurrentUser = i;
							UserExist = TRUE;
							TRACE("Register() _get_xnaddr - User exists, populating data");
							break;
						}
					}

					memcpy(&Users[CurrentUser].pxna, pxna, sizeof(XNADDR));
					memcpy(&Users[CurrentUser].pina, pina, sizeof(IN_ADDR));

					Users[CurrentUser].bValid = TRUE;
					
					if (UserExist == FALSE)
					{
						TRACE("Register() _get_xnaddr - User did not exist");
						LastUser++;
					}

					break;
				}
			}

		}
	}

	if (type == _get_secureaddr)//Get Secure Addr from XNADDR
	{
		Packet Pack;
		Pack.set_type(Packet_Type_secure_request);
		secure_request* secreq = Pack.mutable_srequest();
		secreq->set_xnaddr(pxna->ina.s_addr);

		char* SendBuf = new char[Pack.ByteSize()];
		Pack.SerializeToArray(SendBuf, Pack.ByteSize());

		sendto(boundsock, SendBuf, Pack.ByteSize(), 0, (SOCKADDR*)&RecvAddr, sizeof(RecvAddr));
		delete[] SendBuf;

		while (1)
		{
			TRACE("Trying recvfrom - Register() _get_secureaddr");

			RecvResult = recvfrom(boundsock, RecvBuf, 2048, 0, (SOCKADDR*)&SenderAddr, &SenderAddrSize);

			if (RecvResult > 0)
			{

				Packet RecvPak;
				RecvPak.ParseFromArray(RecvBuf, RecvResult);
				if (RecvPak.has_sreply())
				{
					TRACE("Register() _get_secureaddr - Secure Reply Size: %i", RecvPak.ByteSize());
					TRACE("Register() _get_secureaddr - Secure Address: %08X", RecvPak.sreply().secure());
					TRACE_GAME_NETWORK("Register() _get_secureaddr - Secure Address: %08X", RecvPak.sreply().secure());

					pina->s_addr = RecvPak.sreply().secure();
					int CurrentUser = LastUser;
					BOOL UserExists = FALSE;

					for (int i = 0; i < 16; i++)
					{
						if (!memcmp(&Users[i].pxna.abEnet, &pxna->abEnet, 6) && Users[i].bValid)
						{
							CurrentUser = i;
							UserExists = TRUE;
							TRACE("Register() _get_secureaddr - User already exists");
							break;
						}
					}
					
					Users[CurrentUser].pina.s_addr = pina->s_addr;
					Users[CurrentUser].bValid = TRUE;

					if (UserExists == FALSE)
					{
						TRACE("Register() _get_secureaddr - User did not exist");
						LastUser++;
					}

					break;
				}
			}

		}
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