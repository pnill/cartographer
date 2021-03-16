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

void CXnIp::LogConnectionsDetails(sockaddr_in* address, int errorCode)
{
	LOG_CRITICAL(onscreendebug_log, "{} - tried to add XNADDR in the system, caused error: {}", __FUNCTION__, errorCode);

	if (address != nullptr)
		LOG_CRITICAL(onscreendebug_log, "{} - received connection request from {}:{} we can't fulfil.", __FUNCTION__, inet_ntoa(address->sin_addr), htons(address->sin_port));

	int keysRegisteredCount = GetRegisteredKeyCount();

	if (keysRegisteredCount > 0)
	{
		LOG_CRITICAL(onscreendebug_log, "{} - registered key count: {}", __FUNCTION__, keysRegisteredCount);
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
				LOG_CRITICAL(onscreendebug_log, "{} - hold up pendejo, wtf - connection index: {}, we have free connection registry and couldn't use it.", __FUNCTION__, i);
			}
		}
	}
	else
	{
		LOG_CRITICAL(onscreendebug_log, "{} - no keys are registered, cannot create connections with no registered keys!", __FUNCTION__);
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
				//case XnIp_ConnectionDeclareConnected:
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

	int ret = CreateXnIpIdentifier(&connectReqPkt->data.xnaddr, &connectReqPkt->data.xnkid, &connectionIdentifier, connectReqPkt->data.reqType);
	if (ret == 0)
	{
		// TODO: get rid of H2v only sockets

		XnIp* xnIp = &XnIPs[getConnectionIndex(connectionIdentifier)];
		//if (xnIp->isValid(connectionIdentifier)) // this check should be irelevant
		{
			switch (connectReqPkt->data.reqType)
			{
			case XnIp_ConnectionEstablishSecure:
				// first save the port we received connection request from
				if (xnIp->connectStatus < XNET_CONNECT_STATUS_CONNECTED)
				{
					SaveNatInfo(xsocket, connectionIdentifier, recvAddr);
					if (xnIp->natIsUpdated()) // check if the last connect packet was sent from the last socket to have NAT data saved
					{
						// this check is present in XNetConnect as well, just adding it to be more clear
						if (xnIp->connectStatus < XNET_CONNECT_STATUS_PENDING)
							XNetConnect(connectionIdentifier); // we want to send connect back only if we have all the NAT data for each socket, and only if the state is IDLE

						// TODO: not 100% necessary for now
						// if a game uses XNetConnect to connect it wont set the STATUS to connected until this is received by the other part
						//else if (xnIp->connectStatus == XNET_CONNECT_STATUS_PENDING) // if we got this far it means we received last NAT and connect packet
							//gXnIp.sendXNetRequest(xsocket, connectionIdentifier, XnIp_ConnectionDeclareConnected);

						xnIp->connectStatus = XNET_CONNECT_STATUS_CONNECTED;

					}
				}
				else 
				{
					LOG_TRACE_NETWORK("{} - wtf how did we end up here, while status connected and trying to establish a secure connection lol", __FUNCTION__);
				}

				break;

			// this will prevent the connection to send packets before we know we can send them
			/*case XnIp_ConnectionDeclareConnected:
				if (xnIp->natIsUpdated())
				{
					switch (xnIp->connectStatus)
					{
					case XNET_CONNECT_STATUS_PENDING:
						xnIp->connectStatus = XNET_CONNECT_STATUS_CONNECTED; // if we have received the NAT data for each port, set the status to CONNECTED
						break;
					case XNET_CONNECT_STATUS_CONNECTED:
						LOG_TRACE_NETWORK("{} - received XNetDeclareConnected request, but we are already connected!", __FUNCTION__);
						break;

					default:
						LOG_TRACE_NETWORK("{} - how are we even receiving ConnectionDeclareConnected packets if we are not even pending one??", __FUNCTION__);
						break;
					}
				}
				else
				{
					LOG_TRACE_NETWORK("{} - we didn't even receive the NAT data for all sockets, wtf", __FUNCTION__);
				}
				break;*/

			default:
				break;
			}

			setTimeConnectionInteractionHappened(xnIp->connectionIdentifier);
			// increase packets received count
			xnIp->pckRecvd++;
			if (bytesRecvdCount)
				xnIp->bytesRecvd += *bytesRecvdCount;
		}
	}
	else
	{
		LOG_TRACE_NETWORK("{} - secure connection cannot be established!" __FUNCTION__);
		LogConnectionsDetails(recvAddr, ret);
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

XnIp* CXnIp::XnIpLookUp(const XNADDR* pxna, const XNKID* xnkid, bool* firstUnusedIndexFound, int* firstUnusedIndex)
{
	if (firstUnusedIndexFound != nullptr
		&& firstUnusedIndex != nullptr)
	{
		*firstUnusedIndex = 0;
		*firstUnusedIndexFound = false;
	}

	// check if the user is already in the system, and get the first 
	for (int i = 0; i < GetMaxXnConnections(); i++)
	{
		if (firstUnusedIndexFound != nullptr 
			&& firstUnusedIndex != nullptr)
		{
			if (*firstUnusedIndexFound == false
				&& !XnIPs[i].bValid)
			{
				*firstUnusedIndex = i; // get the first unused connection spot, if we are required to
				*firstUnusedIndexFound = true;
			}
		}

		// compare current connection index with what we got passed to us
		if (XnIPs[i].bValid
			&& memcmp(XnIPs[i].xnaddr.abEnet, pxna->abEnet, sizeof(XNADDR::abEnet)) == 0 // check the MAC address
			// && memcmp(XnIPs[i].xnaddr.abOnline, pxna->abOnline, sizeof(XNADDR::abOnline) == 0)
			&& memcmp(XnIPs[i].keyPair->xnkid.ab, xnkid->ab, sizeof(XNKID::ab)) == 0)
		{
			// if it is already in the system, return
			return &XnIPs[i];
		}
	}

	return nullptr;
}

int CXnIp::registerNewXnIp(int connectionIndex, const XNADDR* pxna, const XNKID* pxnkid, IN_ADDR* outIpIdentifier)
{
	XnKeyPair* keyPair = getKeyPair(pxnkid);
	if (keyPair != nullptr)
	{
		XnIp* newXnIp = &XnIPs[connectionIndex];
		SecureZeroMemory(newXnIp, sizeof(*newXnIp));

		newXnIp->xnaddr = *pxna;
		newXnIp->keyPair = keyPair;

		// if this is zero we are fucked
		int randIdentifier = (rand() % 0xFF) + 1; // 0 to 254 + 1
		randIdentifier <<= CHAR_BIT;
		LOG_INFO_NETWORK("{} - new connection index {}, identifier {:X}", __FUNCTION__, connectionIndex, htonl(connectionIndex | randIdentifier));

		if (outIpIdentifier)
			outIpIdentifier->s_addr = htonl(connectionIndex | randIdentifier);

		newXnIp->connectStatus = XNET_CONNECT_STATUS_IDLE;

		newXnIp->connectionIdentifier.s_addr = htonl(connectionIndex | randIdentifier);
		newXnIp->bValid = true;

		setTimeConnectionInteractionHappened(newXnIp->connectionIdentifier);

		return 0;
	}
	else
	{
		return WSAEINVAL;
	}
}

int CXnIp::CreateXnIpIdentifier(const XNADDR* pxna, const XNKID* pxnkid, IN_ADDR* outIpIdentifier, eXnip_ConnectRequestType requestType)
{
	/*
		Update: 1/31/2020
		- Creates an identifier to be used by WinSock calls from a XNADDR address
		- It gets created when we received a XNet request connection packet, or when the game calls XNetXnAddrToInAddr
		- If XNetInAddrToXnAddr passes an invalid identifier, it will return WSAEINVAL
	*/

#define NO_MORE_CONNECT_SPOTS(_function, _err) \
	{ \
		LOG_TRACE_NETWORK("{} - no more available connection spots!", (_function)); \
		return(_err); \
	} \

	XNADDR localXn;
	if (GetLocalUserXn() == nullptr)
	{
		LOG_TRACE_NETWORK("{} - XNADDR information is not populated!", __FUNCTION__);
		return WSAEINVAL;
	}

	if (memcmp(localXn.abEnet, pxna->abEnet, sizeof(XNADDR::abEnet)) == 0)
	{
		// some retarded games use the MAC address as a unique player identifier
		// but in 2020 we want to connect from the same PC multiple game instances, and this has become a unique account identifier
		// but even then, we cant allow xbox addresses with the same abEnet identifier
		LOG_CRITICAL_NETWORK("{} - the specified XNADDR is the same with the local one, aborting connection.", __FUNCTION__);
		outIpIdentifier->s_addr = IPADDR_LOOPBACK;
		return 0;
	}

	// HACK: clear any lost connections before creating another one
	checkForLostConnections();

	int firstUnusedConnectionIndex = 0;
	bool firstUnusedConnectionIndexFound = false;

	XnIp* pXnIpAlreadyRegistered = XnIpLookUp(pxna, pxnkid, &firstUnusedConnectionIndexFound, &firstUnusedConnectionIndex);

	// check if the user is already in the system
	if (pXnIpAlreadyRegistered != nullptr)
	{
		if (
			(requestType == XnIp_ConnectionEstablishSecure								// if this function is called after a packet is received and the request type is ConnectionEstablishSecure
			&& pXnIpAlreadyRegistered->connectStatus == XNET_CONNECT_STATUS_CONNECTED)	// and we are already connected, it means the other side tries to re-connect, so we just disconnect, then re-connect, but only if we are already connected
			
			|| (pXnIpAlreadyRegistered->xnaddr.ina.s_addr != pxna->ina.s_addr			// if there are differences between XNADDRs (like the port or even the IP address) but the MAC address is the same, clear and create another connection
			|| pXnIpAlreadyRegistered->xnaddr.inaOnline.s_addr != pxna->inaOnline.s_addr
			|| pXnIpAlreadyRegistered->xnaddr.wPortOnline != pxna->wPortOnline)
			)
		{
			int connectionIndexToReuse = getConnectionIndex(pXnIpAlreadyRegistered->connectionIdentifier);
			XNetUnregisterInAddr(pXnIpAlreadyRegistered->connectionIdentifier); // unregister the connection
			return registerNewXnIp(connectionIndexToReuse, pxna, pxnkid, outIpIdentifier); // register a new one, if it gets at this point, it shouldn't fail at all
		}

		if (outIpIdentifier)
		{
			*outIpIdentifier = pXnIpAlreadyRegistered->connectionIdentifier;
			LOG_INFO_NETWORK("{} - already present connection identifier: {:X}", __FUNCTION__, pXnIpAlreadyRegistered->connectionIdentifier.s_addr);
		}

		return 0;
	}
	else if (firstUnusedConnectionIndexFound) // check if we can create another XnIp identifier, if not already present
	{
		return registerNewXnIp(firstUnusedConnectionIndex, pxna, pxnkid, outIpIdentifier);
	}
	else
	{
		NO_MORE_CONNECT_SPOTS(__FUNCTION__, WSAENOMORE);
	}

#undef NO_MORE_CONNECT_SPOTS
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
		if (memcmp(XnKeyPairs[i].xnkid.ab, pxnkid->ab, sizeof(XNKID::ab)) == 0)
		{
			if (XnKeyPairs[i].bValid == true)
			{
				LOG_TRACE_NETWORK("{} - XnKeyPair: xnkid {}, xnkey: {} already registered!", __FUNCTION__, ByteToHexStr(pxnkid->ab, sizeof(pxnkid->ab)), ByteToHexStr(pxnkey->ab, sizeof(pxnkey->ab)));
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
		memcpy(newKeyReg->xnkid.ab, pxnkid->ab, sizeof(XNKID::ab));
		memcpy(newKeyReg->xnkey.ab, pxnkey->ab, sizeof(XNKEY::ab));
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
		if (memcmp(XnKeyPairs[i].xnkid.ab, pxnkid->ab, sizeof(XNKID::ab)) == 0)
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
				SecureZeroMemory(&XnKeyPairs[i], sizeof(XnKeyPairs[i]));
				return;
			}
		}
	}

	LOG_TRACE_NETWORK("{} - xnkid {} is unknown!", __FUNCTION__, ByteToHexStr(pxnkid->ab, sizeof(pxnkid->ab)));
}

XnKeyPair* CXnIp::getKeyPair(const XNKID* pxnkid)
{
	for (int i = 0; i < GetMaxXnKeyPairs(); i++)
	{
		if (XnKeyPairs[i].bValid
			&& memcmp(XnKeyPairs[i].xnkid.ab, pxnkid->ab, sizeof(XNKID::ab)) == 0
			)
		{
			return &XnKeyPairs[i];
		}
	}

	LOG_CRITICAL_NETWORK("{} - unknown session key id - {}", __FUNCTION__, ByteToHexStr(pxnkid->ab, sizeof(pxnkid->ab)));
	return nullptr;
}

void CXnIp::sendXNetRequest(XSocket* xsocket, IN_ADDR connectionIdentifier, eXnip_ConnectRequestType reqType)
{
	sockaddr_in sendToAddr;
	SecureZeroMemory(&sendToAddr, sizeof(sockaddr_in));

	XnIp* xnIp = gXnIp.getConnection(connectionIdentifier);
	if (xnIp != nullptr)
	{
		sendToAddr.sin_family = AF_INET;
		sendToAddr.sin_addr = connectionIdentifier;
		sendToAddr.sin_port = xsocket->getNetworkOrderSocketPort();

		XNetRequestPacket connectionPacket;
		SecureZeroMemory(&connectionPacket.data, sizeof(XNetRequestPacket::XNetReq));

		connectionPacket.data.reqType = reqType;
		memcpy(connectionPacket.data.xnkid.ab, xnIp->keyPair->xnkid.ab, sizeof(XNKID::ab));
		switch (reqType)
		{
		case XnIp_ConnectionEstablishSecure:
			XNetGetTitleXnAddr(&connectionPacket.data.xnaddr);
			break;

		case XnIp_ConnectionDeclareConnected:
		case XnIp_ConnectionCloseSecure:
		case XnIp_ConnectionPong:
		case XnIp_ConnectionPing:
			LOG_INFO_NETWORK("{} - unimplemented requests!", __FUNCTION__);
			break;

		default:
			// this shouldn't be executed ever
			LOG_ERROR_NETWORK("{} - invalid request type", __FUNCTION__);
			break;
		}

		xnIp->connectionPacketsSentCount++;

		int ret = xsocket->udpSend((char*)&connectionPacket, sizeof(XNetRequestPacket), 0, (sockaddr*)&sendToAddr, sizeof(sendToAddr));
		LOG_INFO_NETWORK("{} - secure packet sent socket handle: {}, connection index: {}, connection identifier: {:x}", __FUNCTION__, xsocket->winSockHandle, gXnIp.getConnectionIndex(connectionIdentifier), sendToAddr.sin_addr.s_addr);
	}
	else
	{
		LOG_ERROR_NETWORK("{} - connection index: {}, identifier: {:x} is invalid!", __FUNCTION__, gXnIp.getConnectionIndex(connectionIdentifier), connectionIdentifier.s_addr);
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
		XNetRandom(pxnkid->ab, sizeof(pxnkid->ab));
		XNetRandom(pxnkey->ab, sizeof(pxnkey->ab));
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

	int ret = gXnIp.CreateXnIpIdentifier(pxna, pxnkid, pina, XnIp_ConnectionRequestInvalid);

	if (ret != 0)
	{
		gXnIp.LogConnectionsDetails(nullptr, ret);
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
		SecureZeroMemory(pxnkid->ab, sizeof(pxnkid->ab));

	XnIp* xnIp = gXnIp.getConnection(ina);
	if (xnIp != nullptr)
	{
		if (pxna)
			*pxna = xnIp->xnaddr;

		if (pxnkid)
			memcpy(pxnkid->ab, xnIp->keyPair->xnkid.ab, sizeof(XNKID::ab));

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
		if (xnIp->connectStatus == XNET_CONNECT_STATUS_IDLE)
		{
			for (auto sockIt : XSocket::Sockets)
			{
				// TODO: handle dinamically, so it can be used by other games too
				if (sockIt->isUDP() // connect only UDP sockets
					&& H2v_socketsToConnect.find(sockIt->getHostOrderSocketPort()) != H2v_socketsToConnect.end())
				{
					gXnIp.sendXNetRequest(sockIt, ina, XnIp_ConnectionEstablishSecure); // establish 'secure' connection on the sockets
				}
			}
			xnIp->connectStatus = XNET_CONNECT_STATUS_PENDING; // after we sent, set the state to PENDING
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
		gXnIp.setTimeConnectionInteractionHappened(ina);
		return xnIp->connectStatus;
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