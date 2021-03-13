#include "stdafx.h"

#include "XnIp.h"
#include "..\..\Cryptography\Rc4.h"
#include "H2MOD\Modules\Config\Config.h"

#include "H2MOD\Modules\Utils\Utils.h"

CXnIp gXnIp;
XECRYPT_RC4_STATE Rc4StateRand;

void CXnIp::Initialize(const XNetStartupParams* netStartupParams)
{
	memcpy(&startupParams, netStartupParams, sizeof(XNetStartupParams));

	if (startupParams.cfgSecRegMax == 0)
		startupParams.cfgSecRegMax = 32; // default 32 XNet connections

	if (XnIPs != nullptr)
		delete[] XnIPs;

	XnIPs = new	XnIp[startupParams.cfgSecRegMax];
	SecureZeroMemory(XnIPs, startupParams.cfgSecRegMax * sizeof(*XnIPs));

	if (startupParams.cfgKeyRegMax == 0)
		startupParams.cfgKeyRegMax = 4; // default 4 key pairs

	if (XnKeyPairs != nullptr)
		delete[] XnKeyPairs;

	XnKeyPairs = new XnKeyPair[startupParams.cfgKeyRegMax];
	SecureZeroMemory(XnKeyPairs, startupParams.cfgKeyRegMax * sizeof(*XnKeyPairs));

	if (startupParams.cfgQosDataLimitDiv4 == 0)
		startupParams.cfgQosDataLimitDiv4 = 256 / 4; // 256 bytes as default

	if (startupParams.cfgSockDefaultRecvBufsizeInK <= 0)
		startupParams.cfgSockDefaultRecvBufsizeInK = SOCK_UDP_MIN_RECV_BUFFER_K_UNITS;

	if (startupParams.cfgSockDefaultSendBufsizeInK <= 0)
		startupParams.cfgSockDefaultSendBufsizeInK = SOCK_UDP_MIN_SEND_BUFFER_K_UNITS;
}

void CXnIp::LogConnectionsDetails(sockaddr_in* address)
{
	if (address != nullptr)
		LOG_CRITICAL(onscreendebug_log, "{} - received connection request from {}:{} we can't fulfil", __FUNCTION__, inet_ntoa(address->sin_addr), htons(address->sin_port));

	for (int i = 0; i < GetMaxXnConnections(); i++)
	{
		if (XnIPs[i].bValid)
		{
			XnIp* xnIp = &XnIPs[i];
			float connectionLastInteractionSeconds = (float)(timeGetTime() - xnIp->lastConnectionInteractionTime) / 1000.f;
			LOG_CRITICAL(onscreendebug_log, "{} - connection index: {}, packets sent: {}, packets received: {}, time since last interaction: {:.4f} seconds", __FUNCTION__, i, xnIp->pckSent, xnIp->pckRecvd, connectionLastInteractionSeconds);
		}
		else
		{
			LOG_CRITICAL(onscreendebug_log, "{} hold up pendejo, wtf - connection index: {}, we have free connection registry and couldn't use it.", __FUNCTION__, i);
		}
	}
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
					char* buffer = (char*)_alloca(*bytesRecvdCount); // allocate on stack
					memcpy(buffer, lpBuffers->buf + sizeof(XBroadcastPacket), *bytesRecvdCount);
					memcpy(lpBuffers->buf, buffer, *bytesRecvdCount);
					return 0;
				}
				// set the bytes received count to 0
				*bytesRecvdCount = 0;
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
					LOG_TRACE_NETWORK("{} - Received ConnectionEstablishSecure request from ip address {:X}, port: {}", __FUNCTION__, ntohl(lpFrom->sin_addr.s_addr), ntohs(lpFrom->sin_port));
					HandleConnectionPacket(xsocket, XNetPck, lpFrom, bytesRecvdCount); // save NAT info and send back a connection packet
					break;

				case XnIp_ConnectionCloseSecure:
					//HandleDisconnectPacket(xsocket, XNetPck, lpFrom);
					//LOG_TRACE_NETWORK("{} - Received ConnectionCloseSecure request from ip address {:x}, port: {}", __FUNCTION__, htonl(lpFrom->sin_addr.s_addr), htons(lpFrom->sin_port));
					break;

				case XnIp_ConnectionPing:
				case XnIp_ConnectionPong:
					LOG_ERROR_NETWORK("{} - unimplemented request type: {}", __FUNCTION__, XNetPck->data.reqType);
					break;
				default:
					break;
				}

				// set the bytes received count to 0
				*bytesRecvdCount = 0;
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
		LOG_CRITICAL_NETWORK("{} - discarding packet with size: {}", __FUNCTION__, *bytesRecvdCount);
		// set the bytes received count to 0 and recv address/identifier
		*bytesRecvdCount = 0;
		ZeroMemory(lpFrom, sizeof(*lpFrom));
		return SOCKET_ERROR;
	}

	lpFrom->sin_addr = ipIdentifier;
	setTimeConnectionInteractionHappened(ipIdentifier);
	XnIp* xnIp = getConnection(ipIdentifier);
	if (xnIp != nullptr)
	{
		xnIp->pckRecvd++;
		xnIp->bytesRecvd += *bytesRecvdCount;
	}

	return 0;
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
			switch (xsocket->getHostOrderSocketPort())
			{
			case 1000:
				if (xsocket->sockAddrInEqual(fromAddr, xnIp->getNatAddr(H2vSocket1000)))
					return xnIp->connectionIdentifier;
				break;

			case 1001:
				if (xsocket->sockAddrInEqual(fromAddr, xnIp->getNatAddr(H2vSocket1001)))
					return xnIp->connectionIdentifier;
				break;

			default:
				// wtf?... unknown socket
				LOG_CRITICAL_NETWORK("{} - unkown network socket!", __FUNCTION__);
				return addrInval;
			}
		}
	}
	
	LOG_CRITICAL_NETWORK("{} - received packet from unknown/unregistered source, ip address: {}:{}", __FUNCTION__, inet_ntoa(fromAddr->sin_addr), htons(fromAddr->sin_port));
	return addrInval;
}

void CXnIp::SaveNatInfo(XSocket* xsocket, IN_ADDR connectionIdentifier, sockaddr_in* addr)
{
	LOG_INFO_NETWORK("{} - socket: {}, connection index: {}, identifier: {:x}", __FUNCTION__, xsocket->winSockHandle, getConnectionIndex(connectionIdentifier), connectionIdentifier.s_addr);

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
		switch (xsocket->getHostOrderSocketPort())
		{
		case 1000:
			//LOG_TRACE_NETWORK("SaveConnectionNatInfo() xnIp->NatAddrSocket1000 mapping port 1000 - port: {}, connection identifier: {:x}", htons(addr->sin_port), xnIp->connectionIdentifier.s_addr);
			xnIp->updateNat(H2vSocket1000, addr);
			break;

		case 1001:
			//LOG_TRACE_NETWORK("SaveConnectionNatInfo() xnIp->NatAddrSocket1001 mapping port 1001 - port: {}, connection identifier: {:x}", htons(addr->sin_port), xnIp->connectionIdentifier.s_addr);
			xnIp->updateNat(H2vSocket1001, addr);
			break;

		default:
			LOG_CRITICAL_NETWORK("{} - unkown network socket!", __FUNCTION__);
			break;

		}
	}
	else
	{
		LOG_ERROR_NETWORK("{} - connection index: {} identifier: {:X} is invalid!", __FUNCTION__, getConnectionIndex(connectionIdentifier), connectionIdentifier.s_addr);
	}
}

void CXnIp::HandleConnectionPacket(XSocket* xsocket, XNetRequestPacket* connectReqPkt, sockaddr_in* recvAddr, LPDWORD bytesRecvdCount)
{
	IN_ADDR connectionIdentifier;
	connectionIdentifier.s_addr = 0;

	int ret = CreateXnIpIdentifier(&connectReqPkt->data.xnaddr, &connectReqPkt->data.xnkid, &connectionIdentifier, true);
	if (ret == 0)
	{
		// TODO: get rid of H2v only sockets
		SaveNatInfo(xsocket, connectionIdentifier, recvAddr);

		XnIp* xnIp = &XnIPs[getConnectionIndex(connectionIdentifier)];
		if (xnIp->isValid(connectionIdentifier)
			&& xnIp->xnetstatus < XNET_CONNECT_STATUS_CONNECTED)
		{
			xsocket->sendXNetRequest(xnIp->connectionIdentifier, XnIp_ConnectionEstablishSecure); // establish 'secure' connection on the socket
			setTimeConnectionInteractionHappened(xnIp->connectionIdentifier);

			// TODO FIXME: handle dynamically
			if (xnIp->natIsUpdated())
				xnIp->xnetstatus = XNET_CONNECT_STATUS_CONNECTED; // if we have received the NAT data for each port, set the status to CONNECTED
			else
				xnIp->xnetstatus = XNET_CONNECT_STATUS_PENDING; // otherwise we keep it pending
		}

		// increase packets received count
		xnIp->pckRecvd++;
		if (bytesRecvdCount)
			xnIp->bytesRecvd += *bytesRecvdCount;
	}
	else
	{
		LogConnectionsDetails(recvAddr); // TODO: disable after connection bug is fixed
		LOG_TRACE_NETWORK("{} - secure connection cannot be established!, __FUNCTION__");
		// TODO: send back the connection cannot be established
	}
}

void CXnIp::HandleDisconnectPacket(XSocket* xsocket, XNetRequestPacket* disconnectReqPck, sockaddr_in* recvAddr)
{
	IN_ADDR ipIdentifier = GetConnectionIdentifierByRecvAddr(xsocket, recvAddr);

	if (ipIdentifier.s_addr == 0)
	{
		LOG_INFO_NETWORK("{} - cannot disconnect, connection unknown!", __FUNCTION__);
	}

	XnIp* xnIp = getConnection(ipIdentifier);
	if (xnIp != nullptr)
	{
		// TODO: 
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
	if (GetLocalUserXn() == nullptr)
	{
		LOG_TRACE_NETWORK("{} - XNADDR information is not populated!", __FUNCTION__);
		return WSAEINVAL;
	}

	if (memcmp(&localXn.abEnet, pxna->abEnet, sizeof(XNADDR::abEnet)) == 0)
	{
		// some retarded games use the MAC address as a unique player identifier
		// but in 2020 we want to connect from the same PC multiple game instances, and this has become a unique account identifier
		// but even then, we cant allow xbox addresses with the same abEnet identifier
		LOG_CRITICAL_NETWORK("{} - the specified XNADDR is the same with the local one, aborting connection.", __FUNCTION__);
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
			&& memcmp(&XnIPs[i].xnaddr.abEnet, pxna->abEnet, sizeof(XNADDR::abEnet)) == 0 // check the MAC address
			&& memcmp(&XnIPs[i].keyPair->xnkid, xnkid, sizeof(XNKID)) == 0)
		{
			if (outIpIdentifier) {
				*outIpIdentifier = XnIPs[i].connectionIdentifier;
				LOG_INFO_NETWORK("{} - already present connection index: {}, identifier: {:X}", __FUNCTION__, i, XnIPs[i].connectionIdentifier.s_addr);
			}

			// if there are differences between XNADDRs (like the port or even the IP address) but the MAC address is the same, update and set the connection status to IDLE and discard NAT data
			if (memcmp(&XnIPs[i].xnaddr, pxna, sizeof(XNADDR)))
			{
				XnIPs[i].natDiscard();
				XnIPs[i].xnaddr = *pxna;
				XnIPs[i].xnetstatus = XNET_CONNECT_STATUS_IDLE; // setting this to IDLE will tell the game to call XNetConnect (or in the case this gets called after receiving a connect packet, it'll update the state to PENDING)
			}
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
			SecureZeroMemory(newXnIp, sizeof(*newXnIp));

			newXnIp->xnaddr = *pxna;
			newXnIp->keyPair = keyPair;

			// if this is zero we are fucked
			int randIdentifier = (rand() % 0xFF) + 1; // 0 to 254 + 1
			randIdentifier <<= 8;
			LOG_INFO_NETWORK("{} - new connection index {}, identifier {:x}", __FUNCTION__, firstUnusedConnectionIndex, htonl(firstUnusedConnectionIndex | randIdentifier));

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
			LOG_CRITICAL(onscreendebug_log, "{} - unknown key when trying to create new connection!", __FUNCTION__);
			return WSAEINVAL;
		}

	}
	else
	{
		//LOG_TRACE_NETWORK("{} - no more available connection spots!", __FUNCTION__);
		LOG_CRITICAL(onscreendebug_log, "{} - no more available connection spots!", __FUNCTION__); // TODO: undo and remove this line after connection bug is fixed
		return WSAENOMORE;
	}
}

void CXnIp::UnregisterXnIpIdentifier(const IN_ADDR ina)
{
	// TODO: let the other connection end know that the connection has to be closed
	XnIp* xnIp = getConnection(ina);
	if (xnIp != nullptr)
	{
		LOG_INFO_NETWORK("{} - packets sent: {}, packets recv'd {}", __FUNCTION__, xnIp->pckSent, xnIp->pckRecvd);
		LOG_INFO_NETWORK("{} - Unregistered connection index: {}, identifier: {:X}", __FUNCTION__, getConnectionIndex(ina), xnIp->connectionIdentifier.s_addr);
		SecureZeroMemory(xnIp, sizeof(*xnIp));
	}
}

int CXnIp::RegisterKey(XNKID* pxnkid, XNKEY* pxnkey)
{
	XnKeyPair* newKeyReg = nullptr;
	for (int i = 0; i < GetMaxXnKeyPairs(); i++)
	{
		if (memcmp(&XnKeyPairs[i].xnkid, pxnkid, sizeof(XNKID)) == 0)
		{
			if (XnKeyPairs[i].bValid == true)
			{
				LOG_TRACE_NETWORK("{} - XnKeyPair: xnkid {}, xnkey: {} already registered!", __FUNCTION__, ByteToHexStr((BYTE*)pxnkid, sizeof(*pxnkid)), ByteToHexStr((BYTE*)pxnkey, sizeof(*pxnkey)));
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
		newKeyReg->xnkid = *pxnkid;
		newKeyReg->xnkey = *pxnkey;
		lastRegisteredKey = newKeyReg;
		return 0;
	}
	else
	{
		LOG_TRACE_NETWORK("{} - reached max key registrations!", __FUNCTION__);
		return WSAENOMORE;
	}
}

void CXnIp::UnregisterKey(const XNKID* pxnkid)
{
	for (int i = 0; i < GetMaxXnKeyPairs(); i++)
	{
		if (memcmp(&XnKeyPairs[i].xnkid, pxnkid, sizeof(XNKID)) == 0)
		{
			// close any connections present on this key
			//for (int j = 0; j < GetMaxXnConnections(); j++)
			//{
			//	if (XnIPs[j].bValid
			//		&& XnIPs[j].keyPair == &XnKeyPairs[i])
			//		UnregisterXnIpIdentifier(XnIPs[j].connectionIdentifier);
			//}

			if (&XnKeyPairs[i] == lastRegisteredKey)
				lastRegisteredKey = nullptr;

			if (XnKeyPairs[i].bValid == true)
			{
				if (lastRegisteredKey == &XnKeyPairs[i])
					lastRegisteredKey = nullptr;

				SecureZeroMemory(&XnKeyPairs[i], sizeof(XnKeyPairs[i]));
				return;
			}
		}
	}

	LOG_TRACE_NETWORK("{} - xnkid {} is unknown!", __FUNCTION__, ByteToHexStr((BYTE*)pxnkid, sizeof(*pxnkid)));
}

XnKeyPair* CXnIp::getKeyPair(const XNKID* pxnkid)
{
	for (int i = 0; i < GetMaxXnKeyPairs(); i++)
	{
		if (XnKeyPairs[i].bValid
			&& memcmp(&XnKeyPairs[i].xnkid, pxnkid, sizeof(XNKID)) == 0
			)
		{
			return &XnKeyPairs[i];
		}
	}

	//LOG_CRITICAL_NETWORK("{} - unknown session key id - {}", __FUNCTION__, ByteToHexStr((BYTE*)pxnkid, sizeof(*pxnkid)));
	LOG_CRITICAL(onscreendebug_log, "{} - unknown session key id - {}", __FUNCTION__, ByteToHexStr((BYTE*)pxnkid, sizeof(*pxnkid))); // TODO: undo after connection bug is fixed
	return nullptr;
}

// this function is just a hack
void CXnIp::getLastRegisteredKeys(XNKID* xnkid, XNKEY* xnkey)
{
	if (xnkid)
		SecureZeroMemory(xnkid, sizeof(*xnkid));

	if (xnkey)
		SecureZeroMemory(xnkey, sizeof(*xnkey));

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

void CXnIp::SetupLocalConnectionInfo(unsigned long xnaddr, unsigned long lanaddr, const char* abEnet, const char* abOnline)
{
	SecureZeroMemory(&ipLocal, sizeof(ipLocal));
	
	ipLocal.xnaddr.ina.s_addr = lanaddr;
	ipLocal.xnaddr.inaOnline.s_addr = xnaddr;
	ipLocal.xnaddr.wPortOnline = htons(H2Config_base_port);
	HexStrToBytes(std::string(abEnet, sizeof(XNADDR::abEnet) * 2), ipLocal.xnaddr.abEnet, sizeof(XNADDR::abEnet));
	HexStrToBytes(std::string(abOnline, sizeof(XNADDR::abOnline) * 2), ipLocal.xnaddr.abOnline, sizeof(XNADDR::abOnline));

	ipLocal.bValid = true;
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
	LOG_INFO_NETWORK("{} - lost {} connections!", __FUNCTION__, lostConnectionsCount);
}

// #51: XNetStartup
int WINAPI XNetStartup(const XNetStartupParams *pxnsp)
{
	gXnIp.Initialize(pxnsp);
	return 0;
}

// #52: XNetCleanup
INT WINAPI XNetCleanup()
{
	LOG_TRACE_NETWORK("XNetCleanup()");

	XSocket::socketsDisposeAll();

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
		XNetRandom((BYTE*)pxnkid, sizeof(*pxnkid));
		XNetRandom((BYTE*)pxnkey, sizeof(*pxnkey));
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

	int ret = gXnIp.CreateXnIpIdentifier(pxna, pxnkid, pina, false);

	if (ret != 0)
	{
		gXnIp.LogConnectionsDetails();
	}
	 
	LOG_INFO_NETWORK("{} - local-address: {:X}, online-address: {:X}", __FUNCTION__, pxna->ina.s_addr, pxna->inaOnline.s_addr);
	return ret;
}

// #60: XNetInAddrToXnAddr
INT WINAPI XNetInAddrToXnAddr(const IN_ADDR ina, XNADDR* pxna, XNKID* pxnkid)
{
	LOG_INFO_NETWORK("XNetInAddrToXnAddr() - connection index: {}, identifier {:x}", gXnIp.getConnectionIndex(ina), ina.s_addr);
	
	if (pxna)
		SecureZeroMemory(pxna, sizeof(*pxna));

	if (pxnkid)
		SecureZeroMemory(pxnkid, sizeof(*pxnkid));

	XnIp* xnIp = gXnIp.getConnection(ina);
	if (xnIp != nullptr)
	{
		if (pxna)
			*pxna = xnIp->xnaddr;

		if (pxnkid)
			*pxnkid = xnIp->keyPair->xnkid;

		return 0;
	}

	LOG_ERROR_NETWORK("{} - connection index: {}, identifier: {:X} are invalid!", __FUNCTION__, gXnIp.getConnectionIndex(ina), ina.s_addr);

	return WSAEINVAL;
}

// #63: XNetUnregisterInAddr
int WINAPI XNetUnregisterInAddr(const IN_ADDR ina)
{
	LOG_INFO_NETWORK("{} - connection index {}, identifier: {:X}", __FUNCTION__, gXnIp.getConnectionIndex(ina), ina.s_addr);
	gXnIp.UnregisterXnIpIdentifier(ina);
	return 0;
}

// #65: XNetConnect
int WINAPI XNetConnect(const IN_ADDR ina)
{
	LOG_INFO_NETWORK("{} - connection index {}, identifier: {:X}", __FUNCTION__, gXnIp.getConnectionIndex(ina), ina.s_addr);

	XnIp* xnIp = gXnIp.getConnection(ina);
	if (xnIp != nullptr)
	{
		// send connect packets only if the state is idle
		if (xnIp->xnetstatus == XNET_CONNECT_STATUS_IDLE)
		{
			for (auto sockIt : XSocket::Sockets)
			{
				// TODO: handle dinamically, so it can be used by other games too
				if (sockIt->isUDP() // connect only UDP sockets
					&& H2v_socketsToConnect.find(sockIt->getHostOrderSocketPort()) != H2v_socketsToConnect.end())
				{
					sockIt->sendXNetRequest(ina, XnIp_ConnectionEstablishSecure); // establish 'secure' connection on the sockets
				}
			}
			xnIp->xnetstatus = XNET_CONNECT_STATUS_PENDING; // after we sent, set the state to PENDING
		}

		return 0;
	}

	return WSAEINVAL;
}

// #66: XNetGetConnectStatus
int WINAPI XNetGetConnectStatus(const IN_ADDR ina)
{
	//LOG_INFO_NETWORK("{} : connection index {}, identifier: {:x}", ipManager.getConnectionIndex(ina), ina.s_addr);
	XnIp* xnIp = gXnIp.getConnection(ina);
	if (xnIp != nullptr)
	{
		/* 
			Mainly for H2v because it has P2P connection even on dedicated servers, if the connect status is checked by the game, it means the connection identifier is still used
			This prevents connection info being cleared even if no data has been received from the connection (probably the ports were not forwarded/ no data is sent at all between the peers) 
		*/
		gXnIp.setTimeConnectionInteractionHappened(ina); 
		return xnIp->xnetstatus;
	}

	LOG_ERROR_NETWORK("{} - connection index: {}, identifier: {:X} is invalid!", __FUNCTION__, gXnIp.getConnectionIndex(ina), ina.s_addr);
	return XNET_CONNECT_STATUS_LOST;
}

// #73: XNetGetTitleXnAddr
DWORD WINAPI XNetGetTitleXnAddr(XNADDR * pxna)
{
	XnIp* localUserXnIp = gXnIp.GetLocalUserXn();

	if (localUserXnIp)
	{
		*pxna = localUserXnIp->xnaddr;
		return XNET_GET_XNADDR_ETHERNET;
	}
	else
	{
		return XNET_GET_XNADDR_PENDING;
	}
}


// #55: XNetRegisterKey //need #51
int WINAPI XNetRegisterKey(XNKID *pxnkid, XNKEY *pxnkey)
{
	LOG_INFO_NETWORK("XNetRegisterKey()");
	return gXnIp.RegisterKey(pxnkid, pxnkey);
}


// #56: XNetUnregisterKey // need #51
int WINAPI XNetUnregisterKey(const XNKID* pxnkid)
{
	LOG_INFO_NETWORK("XNetUnregisterKey()");
	gXnIp.UnregisterKey(pxnkid);

	return 0;
}