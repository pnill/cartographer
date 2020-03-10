#include "stdafx.h"

#include "XnIp.h"
#include "..\..\Cryptography\Rc4.h"
#include "H2MOD\Modules\Config\Config.h"
#include "H2MOD\Modules\Achievements\Achievements.h"
#include "H2MOD\Modules\Networking\NetworkSession\NetworkSession.h"

CXnIp ipManager;
XECRYPT_RC4_STATE Rc4StateRand;

extern int WINAPI XSocketSendTo(SOCKET s, const char *buf, int len, int flags, sockaddr *to, int tolen);

/*
NOTE:
	Check inside Tweaks.cpp for removeXNetSecurity
*/

bool sockAddrInEqual(sockaddr_in* a1, sockaddr_in* a2)
{
	return (a1->sin_addr.s_addr == a2->sin_addr.s_addr && a1->sin_port == a2->sin_port);
}

bool sockAddrInIsNull(sockaddr_in* a1)
{
	return a1->sin_addr.s_addr == 0 && a1->sin_port == 0;
}

void CXnIp::GetStartupParamsAndUpdate(const XNetStartupParams* netStartupParams)
{
	memcpy(&startupParams, netStartupParams, sizeof(XNetStartupParams));

	if (startupParams.cfgSecRegMax == 0)
		startupParams.cfgSecRegMax = 32; // default 32 XNet connections

	if (XnIPs)
		delete[] XnIPs;

	XnIPs = new	XnIp[startupParams.cfgSecRegMax];
	SecureZeroMemory(XnIPs, startupParams.cfgSecRegMax * sizeof(XnIp));

	if (startupParams.cfgQosDataLimitDiv4 == 0)
		startupParams.cfgQosDataLimitDiv4 = 64; // 256 bytes as default
}

/*
	Use this to get the connection index from an XLive API call
*/
int CXnIp::getConnectionIndex(IN_ADDR connectionIdentifier)
{
	return connectionIdentifier.s_addr >> 24;
}

void CXnIp::setTimeConnectionInteractionHappened(IN_ADDR ina, int time)
{
	XnIp* xnIp = &XnIPs[getConnectionIndex(ina)];
	if (xnIp->isValid(ina))
		xnIp->lastConnectionInteractionTime = time;
}

int CXnIp::handleRecvdPacket(XSocket* xsocket, sockaddr_in* lpFrom, WSABUF* lpBuffers, int bytesRecvdCount)
{
	XNetConnectionReqPacket* connectionPck = reinterpret_cast<XNetConnectionReqPacket*>(lpBuffers->buf);

	if (lpFrom->sin_addr.s_addr == H2Config_master_ip)
	{
		lpFrom->sin_addr.s_addr = INADDR_BROADCAST;
		return ERROR_SUCCESS;
	}

	else if (bytesRecvdCount == sizeof(XNetConnectionReqPacket)
		&& connectionPck->ConnectPacketIdentifier == connectPacketIdentifier)
	{
		// TODO: add more XNet request types (like disconnect, network pulse etc...)
		/*switch (xnetpacket->reqType) 
		{


		default:
			break;
		}*/

		LOG_TRACE_NETWORK("handleRecvdPacket() - Received secure packet with ip address {:x}, port: {}", htonl(lpFrom->sin_addr.s_addr), htons(lpFrom->sin_port));
		HandleConnectionPacket(xsocket, connectionPck, lpFrom); // save NAT info and send back a connection packet

		WSASetLastError(WSAEWOULDBLOCK);
		return SOCKET_ERROR;
	}
	else
	{
		IN_ADDR ipIdentifier = GetConnectionIdentifierByNat(lpFrom);

		/* Let the game know the packet received came from an unkown source */
		if (ipIdentifier.s_addr == 0)
		{
			WSASetLastError(WSAEWOULDBLOCK);
			return SOCKET_ERROR;
		}

		lpFrom->sin_addr = ipIdentifier;
		setTimeConnectionInteractionHappened(ipIdentifier, timeGetTime());

		return ERROR_SUCCESS;
	}
}

void CXnIp::checkForLostConnections(IN_ADDR connectionIdentifier)
{
	XnIp* xnIp = &XnIPs[getConnectionIndex(connectionIdentifier)];
	if (xnIp->isValid(connectionIdentifier)
		&& timeGetTime() - xnIp->lastConnectionInteractionTime >= 15 * 1000)
	{
		UnregisterXnIpIdentifier(xnIp->connectionIdentifier);
	}
}

int CXnIp::sendConnectionRequest(XSocket* xsocket, IN_ADDR connectionIdentifier /* TODO: int reqType */)
{
	sockaddr_in sendToAddr;
	SecureZeroMemory(&sendToAddr, sizeof(sockaddr_in));
	
	XnIp* xnIp = &XnIPs[getConnectionIndex(connectionIdentifier)];

	if (xnIp->isValid(connectionIdentifier))
	{
		sendToAddr.sin_family = AF_INET;
		sendToAddr.sin_addr = connectionIdentifier;

		XNetConnectionReqPacket connectionPacket;
		SecureZeroMemory(&connectionPacket, sizeof(XNetConnectionReqPacket));

		XNetGetTitleXnAddr(&connectionPacket.xnaddr);
		getRegisteredKeys(&connectionPacket.xnkid, nullptr);
		connectionPacket.ConnectPacketIdentifier = /* reqType */ connectPacketIdentifier;

		xnIp->connectionPacketsSentCount++;

		int ret = XSocketSendTo((SOCKET)xsocket, (char*)&connectionPacket, sizeof(XNetConnectionReqPacket), 0, (sockaddr*)&sendToAddr, sizeof(sendToAddr));
		LOG_INFO_NETWORK("sendConnectionRequest() secure packet sent socket handle: {}, connection index: {}, connection identifier: {:x}", xsocket->winSockHandle, getConnectionIndex(connectionIdentifier), sendToAddr.sin_addr.s_addr);
		return ret;
	}
	else
	{
		LOG_ERROR_NETWORK("sendConnectionRequest() - connection index: {}, identifier: {:x} is invalid!", getConnectionIndex(connectionIdentifier), connectionIdentifier.s_addr);
		return -1;
	}
}

IN_ADDR CXnIp::GetConnectionIdentifierByNat(sockaddr_in* fromAddr)
{
	IN_ADDR addrInval;
	addrInval.s_addr = 0;

	for (int i = 0; i < GetMaxXnConnections(); i++)
	{
		XnIp* xnIp = &XnIPs[i];
		if (xnIp->bValid)
		{
			// TODO: get rid of H2v only sockets
			if (sockAddrInEqual(fromAddr, &xnIp->NatAddrSocket1000)
				|| sockAddrInEqual(fromAddr, &xnIp->NatAddrSocket1001))
			{
				if (XNetGetConnectStatus(xnIp->connectionIdentifier) == XNET_CONNECT_STATUS_CONNECTED)
					return xnIp->connectionIdentifier;
				else
					return addrInval; // user is not connected
			}
		}
	}
	
	return addrInval;
}

void CXnIp::SaveConnectionNatInfo(XSocket* xsocket, IN_ADDR connectionIdentifier, sockaddr_in* addr)
{
	LOG_INFO_NETWORK("SaveNatInfo() - socket: {}, connection index: {}, identifier: {:x}", xsocket->winSockHandle, getConnectionIndex(connectionIdentifier), connectionIdentifier.s_addr);

	XnIp* xnIp = &this->XnIPs[getConnectionIndex(connectionIdentifier)];
	if (xnIp->isValid(connectionIdentifier))
	{
		// TODO: handle dynamically
		/* Store NAT data
		   First we look at our socket's intended port.
		   port 1000 is mapped to the receiving address/port xnIp->NatAddrSocket1000 via the connection identifier.
		   port 1001 is mapped to the receiving address/port xnIp->NatAddrSocket1001 via the connection identifier.
		*/

		switch (xsocket->getNetworkSocketPort())
		{
		case 1000:
			//LOG_TRACE_NETWORK("SaveConnectionNatInfo() xnIp->NatAddrSocket1000 mapping port 1000 - port: {}, connection identifier: {:x}", htons(addr->sin_port), xnIp->connectionIdentifier.s_addr);
			xnIp->NatAddrSocket1000 = *addr;
			break;

		case 1001:
			//LOG_TRACE_NETWORK("SaveConnectionNatInfo() xnIp->NatAddrSocket1001 mapping port 1001 - port: {}, connection identifier: {:x}", htons(addr->sin_port), xnIp->connectionIdentifier.s_addr);
			xnIp->NatAddrSocket1001 = *addr;
			break;

		default:
			//LOG_ERROR_NETWORK("SaveConnectionNatInfo() xsocket->getNetworkSocketPort() didn't match any ports!");
			break;

		}
	}
	else
	{
		LOG_ERROR_NETWORK("SaveNatInfo() - connection index: {} identifier: {:x} is invalid!", getConnectionIndex(connectionIdentifier), connectionIdentifier.s_addr);
	}
}

void CXnIp::HandleConnectionPacket(XSocket* xsocket, XNetConnectionReqPacket* connectReqPacket, sockaddr_in* recvAddr)
{
	IN_ADDR connectionIdentifier;

	int ret = CreateXnIpIdentifier(&connectReqPacket->xnaddr, &connectReqPacket->xnkid, &connectionIdentifier, true);
	if (ret == ERROR_SUCCESS)
	{
		/* TODO: support multiple sockets, not just H2v sockets */
		SaveConnectionNatInfo(xsocket, connectionIdentifier, recvAddr);

		XnIp* xnIp = &XnIPs[getConnectionIndex(connectionIdentifier)];

		if (xnIp->isValid(connectionIdentifier)
			&& xnIp->xnetstatus < XNET_CONNECT_STATUS_CONNECTED)
		{
			sendConnectionRequest(xsocket, xnIp->connectionIdentifier);
			setTimeConnectionInteractionHappened(xnIp->connectionIdentifier, timeGetTime());

			// TODO: handle dynamically
			if (!sockAddrInIsNull(&xnIp->NatAddrSocket1000) 
				&& !sockAddrInIsNull(&xnIp->NatAddrSocket1001))
				xnIp->xnetstatus = XNET_CONNECT_STATUS_CONNECTED; // if we have the NAT data for each port, set the status to CONNECTED
			else
				xnIp->xnetstatus = XNET_CONNECT_STATUS_PENDING;

		}
	}
	else
	{
		LOG_TRACE_NETWORK("HandleConnectionPacket() - secure connection cannot be established!");
	}
}

int CXnIp::CreateXnIpIdentifier(const XNADDR* pxna, const XNKID* xnkid, IN_ADDR* outIpIdentifier, bool handleFromConnectionPacket)
{
	/*
		Update: 1/31/2020
		- Creates an identifier to be used by WinSock calls from a XNADDR address
		- It gets created when we received a XNet request connection packet, or when the game calls XNetXnAddrToInAddr
		- If XNetInAddrToXnAddr passes an invalid identifier, it will return WSAEINVAL
	*/

	int firstUnusedConnectionIndex = 0;
	bool firstUnusedConnectionIndexFound = false;

	XNADDR localXn;
	if (!GetLocalXNAddr(&localXn))
	{
		LOG_TRACE_NETWORK("CreateXnIpIdentifier() - XNADDR information is not populated!");
		return WSAEINVAL;
	}

	XNKID XnKid;
	getRegisteredKeys(&XnKid, nullptr);

	if (memcmp(xnkid, &XnKid, sizeof(XNKID)) != 0)
	{
		LOG_INFO_NETWORK("CreateXnIpIdentifier() - the specified XNKID key is incorrect!");
		return WSAEINVAL;
	}

	// do not allow the connection if the received XNADDR is the same with the local one
	if (memcmp(&localXn.abEnet, pxna->abEnet, sizeof(((XNADDR*)0)->abEnet)) == 0
		&& memcmp(&localXn.abOnline, pxna->abOnline, sizeof(((XNADDR*)0)->abOnline)) == 0)
	{
		LOG_INFO_NETWORK("CreateXnIpIdentifier() - the specified XNADDR is the same with the local one, aborting connection.");
		return WSAEINVAL;
	}

	// check if the user is already in the system
	for (int i = 0; i < GetMaxXnConnections(); i++)
	{
		if (XnIPs[i].bValid && memcmp(&XnIPs[i].xnaddr, pxna, sizeof(XNADDR)) == 0)
		{
			if (outIpIdentifier) {
				*outIpIdentifier = XnIPs[i].connectionIdentifier;
				LOG_INFO_NETWORK("CreateXnIpIdentifier() - already present connection index: {}, identifier: {:x}", i, XnIPs[i].connectionIdentifier.s_addr);
			}

			setTimeConnectionInteractionHappened(XnIPs[i].connectionIdentifier, timeGetTime());

			return ERROR_SUCCESS;
		}

		/*
			We check for connections that have not received any packets or XNetGetConnectStatus hasn't been called for some connections in the last 15 seconds, then proceed to create a new identifier
			to prevent people from failing to connect
		*/
		checkForLostConnections(XnIPs[i].connectionIdentifier);

		if (XnIPs[i].bValid == false && firstUnusedConnectionIndexFound == false)
		{
			firstUnusedConnectionIndex = i; // save the first unused data index, in case the XnIp is not in the system, so we don't have to loop again to find a free spot
			firstUnusedConnectionIndexFound = true;
		}
	
		// after looping through all connections without returning, attempt to create a new connection
		if (i == GetMaxXnConnections() - 1)
		{	
			if (firstUnusedConnectionIndexFound)
			{
				std::mt19937 mt_rand(rd());
				std::uniform_int_distribution<int> dist(1, 255);

				XnIp* newXnIp = &XnIPs[firstUnusedConnectionIndex];
				SecureZeroMemory(newXnIp, sizeof(XnIp));

				newXnIp->xnkid = *xnkid;
				newXnIp->xnaddr = *pxna;

				int randIdentifier = dist(mt_rand);
				randIdentifier <<= 8;
				LOG_INFO_NETWORK("CreateXnIpIdentifier() - new connection index {}, identifier {:x}", firstUnusedConnectionIndex, htonl(firstUnusedConnectionIndex | randIdentifier));

				if (outIpIdentifier)
					outIpIdentifier->s_addr = htonl(firstUnusedConnectionIndex | randIdentifier);	

				newXnIp->xnetstatus = XNET_CONNECT_STATUS_IDLE;
				newXnIp->connectionIdentifier.s_addr = htonl(firstUnusedConnectionIndex | randIdentifier);
				setTimeConnectionInteractionHappened(newXnIp->connectionIdentifier, timeGetTime());

				newXnIp->bValid = true;

				return ERROR_SUCCESS;
			}
			else 
			{
				LOG_TRACE_NETWORK("CreateXnIpIdentifier() - no more available connection spots!");
				return WSAENOMORE;
			}
		}
	}

	return WSAENOMORE;
}

void CXnIp::UnregisterXnIpIdentifier(const IN_ADDR ina)
{
	// TODO: let the other connection end know that the connection has to be closed
	XnIp* xnIp = &this->XnIPs[getConnectionIndex(ina)];
	if (xnIp->isValid(ina))
	{
		LOG_INFO_NETWORK("UnregisterXnIpIdentifier() - Unregistered connection index: {}, identifier: {:x}", getConnectionIndex(ina), xnIp->connectionIdentifier.s_addr);
		SecureZeroMemory(xnIp, sizeof(XnIp));
	}
}

void CXnIp::UnregisterLocalConnectionInfo()
{
	SecureZeroMemory(&localUser, sizeof(XnIp));
}

void CXnIp::RegisterKeys(XNKID* xnkid, XNKEY* xnkey)
{
	if (xnkid)
		host_xnkid = *xnkid;

	if (xnkey)
		host_xnkey = *xnkey;
}

void CXnIp::UnregisterKeys()
{
	SecureZeroMemory(&this->host_xnkid, sizeof(XNKID));
	SecureZeroMemory(&this->host_xnkey, sizeof(XNKEY));
}

void CXnIp::getRegisteredKeys(XNKID* xnkid, XNKEY* xnkey)
{
	if (xnkid)
		*xnkid = host_xnkid;

	if (xnkey)
		*xnkey = host_xnkey;
}

void CXnIp::SetupLocalConnectionInfo(XNADDR* pxna) 
{
	SecureZeroMemory(&localUser, sizeof(XnIp));
	localUser.xnaddr = *pxna;
	localUser.bValid = true;
}

BOOL CXnIp::GetLocalXNAddr(XNADDR* pxna)
{
	SecureZeroMemory(pxna, sizeof(XNADDR));
	if (localUser.bValid)
	{
		*pxna = localUser.xnaddr;
		return TRUE;
	}

	return FALSE;
}

// #51: XNetStartup
int WINAPI XNetStartup(const XNetStartupParams *pxnsp)
{
	ipManager.GetStartupParamsAndUpdate(pxnsp);
	return 0;
}

// #53: XNetRandom
INT WINAPI XNetRandom(BYTE * pb, UINT cb)
{
	static bool Rc4CryptInitialized = false;

	LARGE_INTEGER key;

	if (Rc4CryptInitialized == false)
	{
		QueryPerformanceCounter(&key);
		XeCryptRc4Key(&Rc4StateRand, (BYTE*)&key, sizeof(key));
		Rc4CryptInitialized = true;
	}

	XeCryptRc4Ecb(&Rc4StateRand, pb, cb);
	return ERROR_SUCCESS;
}

// #54: XNetCreateKey
INT WINAPI XNetCreateKey(XNKID * pxnkid, XNKEY * pxnkey)
{
	LOG_INFO_NETWORK("XNetCreateKey()");
	if (pxnkid && pxnkey)
	{
		XNetRandom((BYTE*)pxnkid, sizeof(XNKID));
		XNetRandom((BYTE*)pxnkey, sizeof(XNKEY));
		pxnkid->ab[0] &= ~XNET_XNKID_MASK;
		pxnkid->ab[0] |= XNET_XNKID_SYSTEM_LINK_XPLAT;
	}

	return 0;
}

// #57: XNetXnAddrToInAddr
INT WINAPI XNetXnAddrToInAddr(const XNADDR *pxna, const XNKID *pxnkid, IN_ADDR *pina)
{
	LOG_INFO_NETWORK("XNetXnAddrToInAddr() - local-address: {:x}, online-address: {:x}", pxna->ina.s_addr, pxna->inaOnline.s_addr); // TODO

	if (pxna == nullptr 
		|| pxnkid == nullptr
		|| pina == nullptr)
		return WSAEINVAL;

	int ret = ipManager.CreateXnIpIdentifier(pxna, pxnkid, pina, false);
	 
	return ret;
}

// #60: XNetInAddrToXnAddr
INT WINAPI XNetInAddrToXnAddr(const IN_ADDR ina, XNADDR* pxna, XNKID* pxnkid)
{
	LOG_INFO_NETWORK("XNetInAddrToXnAddr() - connection index: {}, identifier {:x}", ipManager.getConnectionIndex(ina), ina.s_addr);
	
	if (pxna == nullptr
		|| pxnkid == nullptr)
		return WSAEINVAL;

	SecureZeroMemory(pxna, sizeof(XNADDR));
	SecureZeroMemory(pxnkid, sizeof(XNKID));

	XnIp* xnIp = &ipManager.XnIPs[ipManager.getConnectionIndex(ina)];
	
	if (xnIp->isValid(ina))
	{
		*pxna = xnIp->xnaddr;
		*pxnkid = xnIp->xnkid;

		return ERROR_SUCCESS;
	}

	LOG_ERROR_NETWORK("XNetInAddrToXnAddr() - connection index: {}, identifier: {:x} are invalid!", ipManager.getConnectionIndex(ina), ina.s_addr);

	return WSAEINVAL;
}

// #63: XNetUnregisterInAddr
int WINAPI XNetUnregisterInAddr(const IN_ADDR ina)
{
	LOG_INFO_NETWORK("XNetUnregisterInAddr() - connection index {}, identifier: {:x}", ipManager.getConnectionIndex(ina), ina.s_addr);
	ipManager.UnregisterXnIpIdentifier(ina);
	return ERROR_SUCCESS;
}

// #65: XNetConnect
int WINAPI XNetConnect(const IN_ADDR ina)
{
	LOG_INFO_NETWORK("XNetConnect() - connection index {}, identifier: {:x}", ipManager.getConnectionIndex(ina), ina.s_addr);

	XnIp* xnIp = &ipManager.XnIPs[ipManager.getConnectionIndex(ina)];
	if (xnIp->isValid(ina))
	{
		if (xnIp->xnetstatus == XNET_CONNECT_STATUS_IDLE)
		{
			// TODO: handle dinamically, so it can be used by other games too
			extern XSocket* game_network_data_gateway_socket_1000; // used for game data
			extern XSocket* game_network_message_gateway_socket_1001; // used for messaging like connection requests
			ipManager.sendConnectionRequest(game_network_data_gateway_socket_1000, xnIp->connectionIdentifier);
			ipManager.sendConnectionRequest(game_network_message_gateway_socket_1001, xnIp->connectionIdentifier);
			
			xnIp->xnetstatus = XNET_CONNECT_STATUS_PENDING;
		}

		return ERROR_SUCCESS;
	}

	return WSAEINVAL;
}

// #66: XNetGetConnectStatus
int WINAPI XNetGetConnectStatus(const IN_ADDR ina)
{
	//LOG_INFO_NETWORK("XNetConnect(): connection index {}, identifier: {:x}", ipManager.getConnectionIndex(ina), ina.s_addr);
	XnIp* xnIp = &ipManager.XnIPs[ipManager.getConnectionIndex(ina)];
	if (xnIp->isValid(ina))
	{
		/* 
			Mainly for H2v because it has P2P connection even on dedicated servers, if the connect status is checked by the game, it means the connection identifier is still used
			This prevents connection info being cleared even if no data has been received from the connection (probably the ports were not forwarded/ no data is sent at all between the peers) 
		*/
		ipManager.setTimeConnectionInteractionHappened(ina, timeGetTime()); 
		return xnIp->xnetstatus;
	}
	return WSAEINVAL;
}

// #73: XNetGetTitleXnAddr
DWORD WINAPI XNetGetTitleXnAddr(XNADDR * pAddr)
{
	if (pAddr)
	{
		if (ipManager.GetLocalXNAddr(pAddr))
			return XNET_GET_XNADDR_ETHERNET;
		else
			return XNET_GET_XNADDR_PENDING;
	}

	return XNET_GET_XNADDR_PENDING;
}


// #55: XNetRegisterKey //need #51
int WINAPI XNetRegisterKey(XNKID *pxnkid, XNKEY *pxnkey)
{
	LOG_INFO_NETWORK("XNetRegisterKey()");
	ipManager.RegisterKeys(pxnkid, pxnkey);
	return ERROR_SUCCESS;
}


// #56: XNetUnregisterKey // need #51
int WINAPI XNetUnregisterKey(const XNKID* pxnkid)
{
	LOG_INFO_NETWORK("XNetUnregisterKey()");
	ipManager.UnregisterKeys();
	return ERROR_SUCCESS;
}