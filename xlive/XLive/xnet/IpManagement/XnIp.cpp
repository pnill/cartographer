#include "stdafx.h"

#include "XnIp.h"
#include "..\..\Cryptography\Rc4.h"
#include "H2MOD\Modules\Config\Config.h"

#include "H2MOD\Modules\Utils\Utils.h"

CXnIp ipManager;
XECRYPT_RC4_STATE Rc4StateRand;

/*
NOTE:
	Check inside Tweaks.cpp for removeXNetSecurity
*/

void CXnIp::Initialize(const XNetStartupParams* netStartupParams)
{
	memcpy(&startupParams, netStartupParams, sizeof(XNetStartupParams));

	if (startupParams.cfgSecRegMax == 0)
		startupParams.cfgSecRegMax = 32; // default 32 XNet connections

	if (XnIPs != nullptr)
		delete[] XnIPs;

	XnIPs = new	XnIp[startupParams.cfgSecRegMax];
	SecureZeroMemory(XnIPs, startupParams.cfgSecRegMax * sizeof(XnIp));

	if (startupParams.cfgKeyRegMax == 0)
		startupParams.cfgKeyRegMax = 4; // default 4 key pairs

	if (XnKeyPairs != nullptr)
		delete[] XnKeyPairs;

	XnKeyPairs = new XnKeyPair[startupParams.cfgKeyRegMax];
	SecureZeroMemory(XnKeyPairs, startupParams.cfgKeyRegMax * sizeof(XnKeyPair));

	if (startupParams.cfgQosDataLimitDiv4 == 0)
		startupParams.cfgQosDataLimitDiv4 = 64; // 256 bytes as default
}

XnIp* CXnIp::getConnection(IN_ADDR ina)
{
	XnIp* ret = &XnIPs[getConnectionIndex(ina)];
	if (ret->isValid(ina))
		return ret;
	else
		return nullptr;
}

/*
	Use this to get the connection index from an XLive API call
*/
int CXnIp::getConnectionIndex(IN_ADDR connectionIdentifier)
{
	return connectionIdentifier.s_addr >> 24;
}

void CXnIp::setTimeConnectionInteractionHappened(IN_ADDR ina)
{
	XnIp* xnIp = getConnection(ina);
	if (xnIp != nullptr)
		xnIp->lastConnectionInteractionTime = timeGetTime();
}

int CXnIp::handleRecvdPacket(XSocket* xsocket, sockaddr_in* lpFrom, WSABUF* lpBuffers, LPDWORD bytesRecvdCount)
{
	// check first if the packet received has the size bigger or equal to the XNet packet header first
	if (*bytesRecvdCount >= sizeof(XNetPacketHeader))
	{
		XNetPacketHeader* XNetPck = reinterpret_cast<XNetPacketHeader*>(lpBuffers->buf);
		switch (XNetPck->intHdr)
		{
		case 'BrOd':
		{
			XBroadcastPacket* broadcastPck = reinterpret_cast<XBroadcastPacket*>(lpBuffers->buf);
			if (*bytesRecvdCount >= sizeof(XBroadcastPacket)
				&& strncmp(broadcastPck->pckHeader.HdrStr, broadcastStrHdr, MAX_HDR_STR) == 0
				&& broadcastPck->data.name.sin_addr.s_addr == INADDR_BROADCAST)
			{
				if (*bytesRecvdCount > sizeof(XBroadcastPacket))
				{
					*bytesRecvdCount = *bytesRecvdCount - sizeof(XBroadcastPacket);
					char* buffer = new char[*bytesRecvdCount];
					memcpy(buffer, lpBuffers->buf + sizeof(XBroadcastPacket), *bytesRecvdCount);
					memcpy(lpBuffers->buf, buffer, *bytesRecvdCount);
					delete[] buffer;
					return 0;
				}
				// set the bytes received count to 0
				*bytesRecvdCount = 0;
				WSASetLastError(WSAEWOULDBLOCK);
				return SOCKET_ERROR;
			}
		}

		case 'XNeT':
		{
			XNetRequestPacket* XNetPck = reinterpret_cast<XNetRequestPacket*>(lpBuffers->buf);
			if (*bytesRecvdCount == sizeof(XNetRequestPacket)
				&& strncmp(XNetPck->pckHeader.HdrStr, requestStrHdr, MAX_HDR_STR) == 0)
			{
				switch (XNetPck->data.reqType)
				{
				case XnIp_ConnectionEstablishSecure:
					LOG_TRACE_NETWORK("handleRecvdPacket() - Received ConnectionEstablishSecure request from ip address {:x}, port: {}", htonl(lpFrom->sin_addr.s_addr), htons(lpFrom->sin_port));
					HandleConnectionPacket(xsocket, XNetPck, lpFrom); // save NAT info and send back a connection packet
					break;

				case XnIp_ConnectionCloseSecure:
					LOG_TRACE_NETWORK("handleRecvdPacket() - Received ConnectionCloseSecure request from ip address {:x}, port: {}", htonl(lpFrom->sin_addr.s_addr), htons(lpFrom->sin_port));
					//IN_ADDR ipIdentifier = GetConnectionIdentifierByRecvAddr(xsocket, lpFrom);
					//ipManager.UnregisterXnIpIdentifier(ipIdentifier);
					break;

				case XnIp_ConnectionPing:
				default:
					break;
				}

				// set the bytes received count to 0
				*bytesRecvdCount = 0;
				WSASetLastError(WSAEWOULDBLOCK);
				return SOCKET_ERROR;
			}
		}

		default:
			break;
		} // switch (XNetPck->intHdr)
	}

	IN_ADDR ipIdentifier = GetConnectionIdentifierByRecvAddr(xsocket, lpFrom);

	// Let the game know the packet received came from an unkown source
	if (ipIdentifier.s_addr == 0)
	{
		LOG_CRITICAL_NETWORK("handleRecvdPacket() - discarding packet with size: {}", *bytesRecvdCount);
		// set the bytes received count to 0 and recv address/identifier
		*bytesRecvdCount = 0;
		lpFrom->sin_addr.s_addr = 0;
		WSASetLastError(WSAEWOULDBLOCK);
		return SOCKET_ERROR;
	}

	lpFrom->sin_addr = ipIdentifier;
	setTimeConnectionInteractionHappened(ipIdentifier);
	XnIp* xnIp = getConnection(ipIdentifier);
	if (xnIp != nullptr)
	{
		xnIp->pckRecvd++;
	}

	return 0;
} 

void CXnIp::checkForLostConnections()
{
	int lostConnectionsCount = 0;
	for (int i = 0; i < GetMaxXnConnections(); i++)
	{
		XnIp* xnIp = &XnIPs[i];
		if (xnIp->bValid
			&& timeGetTime() - xnIp->lastConnectionInteractionTime >= XnIp_ConnectionTimeOut)
		{
			lostConnectionsCount++;
			UnregisterXnIpIdentifier(xnIp->connectionIdentifier);
		}
	}
	LOG_INFO_NETWORK("CXnIp::checkForLostConnections() - lost {} connections!", lostConnectionsCount);
}

void CXnIp::sendXNetRequest(XSocket* xsocket, IN_ADDR connectionIdentifier, int reqType)
{
	sockaddr_in sendToAddr;
	SecureZeroMemory(&sendToAddr, sizeof(sockaddr_in));
	
	XnIp* xnIp = getConnection(connectionIdentifier);
	if (xnIp != nullptr)
	{
		sendToAddr.sin_family = AF_INET;
		sendToAddr.sin_addr = connectionIdentifier;

		XNetRequestPacket connectionPacket;
		SecureZeroMemory(&connectionPacket.data, sizeof(XNetRequestPacket::XNetReq));

		XNetGetTitleXnAddr(&connectionPacket.data.xnaddr);
		connectionPacket.data.xnkid = xnIp->keyPair->xnkid;
		connectionPacket.data.reqType = reqType;

		xnIp->connectionPacketsSentCount++;

		int ret = XSocketSendTo((SOCKET)xsocket, (char*)&connectionPacket, sizeof(XNetRequestPacket), 0, (sockaddr*)&sendToAddr, sizeof(sendToAddr));
		LOG_INFO_NETWORK("sendConnectionRequest() secure packet sent socket handle: {}, connection index: {}, connection identifier: {:x}", xsocket->winSockHandle, getConnectionIndex(connectionIdentifier), sendToAddr.sin_addr.s_addr);
	}
	else
	{
		LOG_ERROR_NETWORK("sendConnectionRequest() - connection index: {}, identifier: {:x} is invalid!", getConnectionIndex(connectionIdentifier), connectionIdentifier.s_addr);
	}
}

/*
	Return connection identifier from received address 
	NOTE: connection has to be established
*/
IN_ADDR CXnIp::GetConnectionIdentifierByRecvAddr(XSocket* xsocket, sockaddr_in* fromAddr)
{
	IN_ADDR addrInval;
	addrInval.s_addr = 0;

	for (int i = 0; i < GetMaxXnConnections(); i++)
	{
		XnIp* xnIp = &XnIPs[i];
		if (xnIp->bValid
			&& XNetGetConnectStatus(xnIp->connectionIdentifier) == XNET_CONNECT_STATUS_CONNECTED)
		{
			// TODO: get rid of H2v only sockets
			switch (xsocket->getNetworkSocketPort())
			{
			case 1000:
				if (xsocket->sockAddrInEqual(fromAddr, &xnIp->NatAddrSocket1000))
					return xnIp->connectionIdentifier;
				break;

			case 1001:
				if (xsocket->sockAddrInEqual(fromAddr, &xnIp->NatAddrSocket1001))
					return xnIp->connectionIdentifier;
				break;

			default:
				// wtf?... unknown socket
				LOG_CRITICAL_NETWORK("GetConnectionIdentifierByRecvAddr() - unkown network socket!");
				return addrInval;
			}
		}
	}
	
	LOG_CRITICAL_NETWORK("GetConnectionIdentifierByRecvAddr() - received packet from unknown/unregistered source, ip address: {}:{}", inet_ntoa(fromAddr->sin_addr), htons(fromAddr->sin_port));
	return addrInval;
}

void CXnIp::SaveConnectionNatInfo(XSocket* xsocket, IN_ADDR connectionIdentifier, sockaddr_in* addr)
{
	LOG_INFO_NETWORK("SaveNatInfo() - socket: {}, connection index: {}, identifier: {:x}", xsocket->winSockHandle, getConnectionIndex(connectionIdentifier), connectionIdentifier.s_addr);

	XnIp* xnIp = getConnection(connectionIdentifier);
	if (xnIp != nullptr)
	{
		// TODO: handle dynamically
		/* 
		   Store NAT data
		   First we look at our socket's intended port.
		   port 1000 is mapped to the receiving address/port xnIp->NatAddrSocket1000 via the connection identifier.
		   port 1001 is mapped to the receiving address/port xnIp->NatAddrSocket1001 via the connection identifier.
		*/

		// TODO: get rid of H2v only sockets
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
			LOG_CRITICAL_NETWORK("SaveConnectionNatInfo() - unkown network socket!");
			break;

		}
	}
	else
	{
		LOG_ERROR_NETWORK("SaveNatInfo() - connection index: {} identifier: {:x} is invalid!", getConnectionIndex(connectionIdentifier), connectionIdentifier.s_addr);
	}
}

void CXnIp::HandleConnectionPacket(XSocket* xsocket, XNetRequestPacket* connectReqPacket, sockaddr_in* recvAddr)
{
	IN_ADDR connectionIdentifier;
	connectionIdentifier.s_addr = 0;

	int ret = CreateXnIpIdentifier(&connectReqPacket->data.xnaddr, &connectReqPacket->data.xnkid, &connectionIdentifier, true);
	if (ret == 0)
	{
		// TODO: get rid of H2v only sockets
		SaveConnectionNatInfo(xsocket, connectionIdentifier, recvAddr);

		XnIp* xnIp = &XnIPs[getConnectionIndex(connectionIdentifier)];
		if (xnIp->isValid(connectionIdentifier)
			&& xnIp->xnetstatus < XNET_CONNECT_STATUS_CONNECTED)
		{
			sendXNetRequest(xsocket, xnIp->connectionIdentifier, XnIp_ConnectionEstablishSecure); // establish 'secure' connection on the socket
			setTimeConnectionInteractionHappened(xnIp->connectionIdentifier);

			// TODO: handle dynamically
			if (!xsocket->sockAddrInIsNull(&xnIp->NatAddrSocket1000) 
				&& !xsocket->sockAddrInIsNull(&xnIp->NatAddrSocket1001))
				xnIp->xnetstatus = XNET_CONNECT_STATUS_CONNECTED; // if we have the NAT data for each port, set the status to CONNECTED
			else
				xnIp->xnetstatus = XNET_CONNECT_STATUS_PENDING;
		}

		// increase packets received count
		xnIp->pckRecvd++;
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

	XNADDR localXn;
	if (!GetLocalXNAddr(&localXn))
	{
		LOG_TRACE_NETWORK("CreateXnIpIdentifier() - XNADDR information is not populated!");
		return WSAEINVAL;
	}

	if (memcmp(&localXn.abEnet, pxna->abEnet, sizeof(XNADDR::abEnet)) == 0)
	{
		LOG_INFO_NETWORK("CreateXnIpIdentifier() - the specified XNADDR is the same with the local one, aborting connection.");
		outIpIdentifier->s_addr = IPADDR_LOOPBACK;
		return 0;
	}

	// clear any lost connections before creating another one
	checkForLostConnections();

	int firstUnusedConnectionIndex = 0;
	bool firstUnusedConnectionIndexFound = false;

	// check if the user is already in the system, and get the first 
	for (int i = 0; i < GetMaxXnConnections(); i++)
	{
		// compare current connection index with what we got passed to us
		if (XnIPs[i].bValid
			&& memcmp(&XnIPs[i].xnaddr.abEnet, pxna->abEnet, sizeof(XNADDR::abEnet)) == 0
			&& memcmp(&XnIPs[i].keyPair->xnkid, xnkid, sizeof(XNKID)) == 0)
		{
			if (outIpIdentifier) {
				*outIpIdentifier = XnIPs[i].connectionIdentifier;
				LOG_INFO_NETWORK("CreateXnIpIdentifier() - already present connection index: {}, identifier: {:x}", i, XnIPs[i].connectionIdentifier.s_addr);
			}

			// update with new information
			XnIPs[i].xnaddr = *pxna;

			setTimeConnectionInteractionHappened(XnIPs[i].connectionIdentifier);

			// if it is already in the system, return
			return 0;
		}

		if (XnIPs[i].bValid == false && firstUnusedConnectionIndexFound == false)
		{
			firstUnusedConnectionIndex = i; // save the first unused data index, in case the XnIp is not in the system, so we don't have to loop again to find a free spot
			firstUnusedConnectionIndexFound = true;
		}
	}

	// if we got this far, it means the connection is new
	if (firstUnusedConnectionIndexFound) // check if we found a new connection spot
	{
		XnKeyPair* keyPair = getKeyPair(xnkid);
		if (keyPair != nullptr)
		{
			XnIp* newXnIp = &XnIPs[firstUnusedConnectionIndex];
			SecureZeroMemory(newXnIp, sizeof(XnIp));

			newXnIp->xnaddr = *pxna;
			newXnIp->keyPair = keyPair;

			// if this is zero we are fucked
			int randIdentifier = (rand() % 0xFF) + 1; // 0 to 254 + 1
			randIdentifier <<= 8;
			LOG_INFO_NETWORK("CreateXnIpIdentifier() - new connection index {}, identifier {:x}", firstUnusedConnectionIndex, htonl(firstUnusedConnectionIndex | randIdentifier));

			if (outIpIdentifier)
				outIpIdentifier->s_addr = htonl(firstUnusedConnectionIndex | randIdentifier);

			newXnIp->xnetstatus = XNET_CONNECT_STATUS_IDLE;
			newXnIp->connectionIdentifier.s_addr = htonl(firstUnusedConnectionIndex | randIdentifier);
			newXnIp->bValid = true;

			setTimeConnectionInteractionHappened(newXnIp->connectionIdentifier);

			return 0;
		}
		else
		{
			return WSAEINVAL;
		}

	}
	else
	{
		LOG_TRACE_NETWORK("CreateXnIpIdentifier() - no more available connection spots!");
		return WSAENOMORE;
	}
}

void CXnIp::UnregisterXnIpIdentifier(const IN_ADDR ina)
{
	// TODO: let the other connection end know that the connection has to be closed
	XnIp* xnIp = getConnection(ina);
	if (xnIp != nullptr)
	{
		LOG_INFO_NETWORK("UnregisterXnIpIdentifier() - packets sent: {}, packets recv'd {}", xnIp->pckSent, xnIp->pckRecvd);
		LOG_INFO_NETWORK("UnregisterXnIpIdentifier() - Unregistered connection index: {}, identifier: {:x}", getConnectionIndex(ina), xnIp->connectionIdentifier.s_addr);
		SecureZeroMemory(xnIp, sizeof(XnIp));
	}
}

void CXnIp::UnregisterLocalConnectionInfo()
{
	SecureZeroMemory(&localUser, sizeof(XnIp));
}

int CXnIp::RegisterKey(XNKID* xnkid, XNKEY* xnkey)
{
	XnKeyPair* newKeyReg = nullptr;
	for (int i = 0; i < GetMaxXnKeyPairs(); i++)
	{
		if (memcmp(&XnKeyPairs[i].xnkid, xnkid, sizeof(XNKID)) == 0)
		{
			if (XnKeyPairs[i].bValid == true)
			{
				LOG_TRACE_NETWORK("RegisterKey() - XnKeyPair: xnkid {}, xnkey: {} already registered!", ByteToHexStr((BYTE*)xnkid, sizeof(XNKID)), ByteToHexStr((BYTE*)xnkey, sizeof(XNKEY)));
				return WSAEALREADY;
			}
		}

		if (newKeyReg == nullptr
			&& XnKeyPairs[i].bValid == false)
		{
			newKeyReg = &XnKeyPairs[i];
		}
	}

	// if the loop above doesn't return because the key is already present, populate with new data
	if (newKeyReg != nullptr)
	{
		newKeyReg->bValid = true;
		newKeyReg->xnkid = *xnkid;
		newKeyReg->xnkey = *xnkey;
		lastRegisteredKey = newKeyReg;
		return 0;
	}
	else
	{

		LOG_TRACE_NETWORK("RegisterKey() - reached max key registrations!", ByteToHexStr((BYTE*)xnkid, sizeof(XNKID)), ByteToHexStr((BYTE*)xnkey, sizeof(XNKEY)));
		return WSAENOMORE;
	}
}

void CXnIp::UnregisterKey(const XNKID* xnkid)
{
	for (int i = 0; i < GetMaxXnKeyPairs(); i++)
	{
		if (memcmp(&XnKeyPairs[i].xnkid, xnkid, sizeof(XNKID)) == 0)
		{
			// close any connections present on this key
			//for (int j = 0; j < GetMaxXnConnections(); j++)
			//{
			//	if (XnIPs[j].bValid
			//		&& XnIPs[j].keyPair == &XnKeyPairs[i])
			//		UnregisterXnIpIdentifier(XnIPs[j].connectionIdentifier);
			//}

			if (XnKeyPairs[i].bValid == true)
			{
				SecureZeroMemory(&XnKeyPairs[i], sizeof(XnKeyPair));
				return;
			}
		}
	}

	LOG_TRACE_NETWORK("XnKeyPair: xnkid {} is unknown!", ByteToHexStr((BYTE*)xnkid, sizeof(XNKID)));
}

XnKeyPair* CXnIp::getKeyPair(const XNKID* xnkid)
{
	for (int i = 0; i < GetMaxXnKeyPairs(); i++)
	{
		if (memcmp(&XnKeyPairs[i].xnkid, xnkid, sizeof(XNKID)) == 0
			&& XnKeyPairs[i].bValid)
		{
			return &XnKeyPairs[i];
		}
	}

	return nullptr;
}

// this function is just a hack
void CXnIp::getLastRegisteredKeys(XNKID* xnkid, XNKEY* xnkey)
{
	if (xnkid)
		SecureZeroMemory(xnkid, sizeof(XNKID));

	if (xnkey)
		SecureZeroMemory(xnkey, sizeof(XNKEY));

	if (lastRegisteredKey != nullptr)
	{
		if (lastRegisteredKey->bValid)
		{
			if (xnkid)
				*xnkid = lastRegisteredKey->xnkid;

			if (xnkey)
				*xnkey = lastRegisteredKey->xnkey;
		}
	}
}

void CXnIp::SetupLocalConnectionInfo(unsigned long xnaddr, const char* abEnet, const char* abOnline)
{
	SecureZeroMemory(&localUser, sizeof(XnIp));
	
	// TODO: this field is the lan address, make use of it at some point
	//localUser.xnaddr.ina.s_addr = saddr; 
	localUser.xnaddr.ina.s_addr = 0; 
	localUser.xnaddr.inaOnline.s_addr = xnaddr;
	localUser.xnaddr.wPortOnline = htons(H2Config_base_port);
	HexStrToBytes(std::string(abEnet, sizeof(XNADDR::abEnet) * 2), localUser.xnaddr.abEnet, sizeof(XNADDR::abEnet));
	HexStrToBytes(std::string(abOnline, sizeof(XNADDR::abOnline) * 2), localUser.xnaddr.abOnline, sizeof(XNADDR::abOnline));

	localUser.bValid = true;
}

BOOL CXnIp::GetLocalXNAddr(XNADDR* pxna)
{
	if (pxna)
		SecureZeroMemory(pxna, sizeof(XNADDR));

	if (localUser.bValid)
	{
		if (pxna)
			*pxna = localUser.xnaddr;

		return TRUE;
	}

	return FALSE;
}

// #51: XNetStartup
int WINAPI XNetStartup(const XNetStartupParams *pxnsp)
{
	ipManager.Initialize(pxnsp);
	return 0;
}

// #53: XNetRandom
INT WINAPI XNetRandom(BYTE * pb, UINT cb)
{
	static bool Rc4CryptInitialized = false;

	if (Rc4CryptInitialized == false)
	{
		LARGE_INTEGER key;
		QueryPerformanceCounter(&key);
		XeCryptRc4Key(&Rc4StateRand, (BYTE*)&key, sizeof(key));
		Rc4CryptInitialized = true;
	}

	XeCryptRc4Ecb(&Rc4StateRand, pb, cb);
	return 0;
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
	if (pxna == nullptr 
		|| pxnkid == nullptr
		|| pina == nullptr)
		return WSAEINVAL;

	int ret = ipManager.CreateXnIpIdentifier(pxna, pxnkid, pina, false);
	 
	LOG_INFO_NETWORK("XNetXnAddrToInAddr() - local-address: {:x}, online-address: {:x}", pxna->ina.s_addr, pxna->inaOnline.s_addr);
	return ret;
}

// #60: XNetInAddrToXnAddr
INT WINAPI XNetInAddrToXnAddr(const IN_ADDR ina, XNADDR* pxna, XNKID* pxnkid)
{
	LOG_INFO_NETWORK("XNetInAddrToXnAddr() - connection index: {}, identifier {:x}", ipManager.getConnectionIndex(ina), ina.s_addr);
	
	if (pxna)
		SecureZeroMemory(pxna, sizeof(XNADDR));

	if (pxnkid)
		SecureZeroMemory(pxnkid, sizeof(XNKID));

	XnIp* xnIp = ipManager.getConnection(ina);
	if (xnIp != nullptr)
	{
		if (pxna)
			*pxna = xnIp->xnaddr;

		if (pxnkid)
			*pxnkid = xnIp->keyPair->xnkid;

		return 0;
	}

	LOG_ERROR_NETWORK("XNetInAddrToXnAddr() - connection index: {}, identifier: {:x} are invalid!", ipManager.getConnectionIndex(ina), ina.s_addr);

	return WSAEINVAL;
}

// #63: XNetUnregisterInAddr
int WINAPI XNetUnregisterInAddr(const IN_ADDR ina)
{
	LOG_INFO_NETWORK("XNetUnregisterInAddr() - connection index {}, identifier: {:x}", ipManager.getConnectionIndex(ina), ina.s_addr);
	ipManager.UnregisterXnIpIdentifier(ina);
	return 0;
}

// #65: XNetConnect
int WINAPI XNetConnect(const IN_ADDR ina)
{
	LOG_INFO_NETWORK("XNetConnect() - connection index {}, identifier: {:x}", ipManager.getConnectionIndex(ina), ina.s_addr);

	XnIp* xnIp = ipManager.getConnection(ina);
	if (xnIp != nullptr)
	{
		if (xnIp->xnetstatus == XNET_CONNECT_STATUS_IDLE)
		{
			for (auto sockIt : ipManager.SocketPtrArray)
			{
				// TODO: handle dinamically, so it can be used by other games too
				if (sockIt->isUDP() // connect only UDP sockets
					&& (sockIt->getNetworkSocketPort() == 1000 // h2v sockets only atm
						|| sockIt->getNetworkSocketPort() == 1001)) 
				{
					ipManager.sendXNetRequest(sockIt, ina, XnIp_ConnectionEstablishSecure); // establish 'secure' connection on the sockets
					xnIp->xnetstatus = XNET_CONNECT_STATUS_PENDING;
				}
			}
		}

		return 0;
	}

	return WSAEINVAL;
}

// #66: XNetGetConnectStatus
int WINAPI XNetGetConnectStatus(const IN_ADDR ina)
{
	//LOG_INFO_NETWORK("XNetConnect(): connection index {}, identifier: {:x}", ipManager.getConnectionIndex(ina), ina.s_addr);
	XnIp* xnIp = ipManager.getConnection(ina);
	if (xnIp != nullptr)
	{
		/* 
			Mainly for H2v because it has P2P connection even on dedicated servers, if the connect status is checked by the game, it means the connection identifier is still used
			This prevents connection info being cleared even if no data has been received from the connection (probably the ports were not forwarded/ no data is sent at all between the peers) 
		*/
		ipManager.setTimeConnectionInteractionHappened(ina); 
		return xnIp->xnetstatus;
	}

	LOG_ERROR_NETWORK("XNetGetConnectStatus() - connection index: {}, identifier: {:x} is invalid!", ipManager.getConnectionIndex(ina), ina.s_addr);
	return XNET_CONNECT_STATUS_LOST;
}

// #73: XNetGetTitleXnAddr
DWORD WINAPI XNetGetTitleXnAddr(XNADDR * pxna)
{
	if (ipManager.GetLocalXNAddr(pxna))
		return XNET_GET_XNADDR_ETHERNET;
	else
		return XNET_GET_XNADDR_PENDING;
}


// #55: XNetRegisterKey //need #51
int WINAPI XNetRegisterKey(XNKID *pxnkid, XNKEY *pxnkey)
{
	LOG_INFO_NETWORK("XNetRegisterKey()");
	return ipManager.RegisterKey(pxnkid, pxnkey);
}


// #56: XNetUnregisterKey // need #51
int WINAPI XNetUnregisterKey(const XNKID* pxnkid)
{
	LOG_INFO_NETWORK("XNetUnregisterKey()");
	ipManager.UnregisterKey(pxnkid);

	return 0;
}

bool XnIp::isValid(IN_ADDR identifier)
{
	bool ret = bValid && identifier.s_addr == connectionIdentifier.s_addr;
	if (identifier.s_addr != connectionIdentifier.s_addr)
	{
		LOG_CRITICAL_NETWORK("XnIp::isValid() - CRITICAL ERROR: {:x} != {:x}", identifier.s_addr, connectionIdentifier.s_addr);
		return false;
	}

	return ret;
}
