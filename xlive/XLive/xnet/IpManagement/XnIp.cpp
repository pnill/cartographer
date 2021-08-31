#include "stdafx.h"

#include "XnIp.h"
#include "..\..\Cryptography\Rc4.h"

#include "H2MOD\Modules\Config\Config.h"
#include "H2MOD\Modules\Utils\Utils.h"
#include "H2MOD\Modules\Startup\Startup.h"
#include "H2MOD\Modules\Console\ConsoleCommands.h"

#include "..\NIC.h"

#include "..\net_utils.h"

CXnIp gXnIp;
XECRYPT_RC4_STATE Rc4StateRand;

h2log* critical_network_errors_log = nullptr;

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

	// initielize critical network logs
	// TODO: disable after all network problems are addressed
	if (network_log == nullptr)
	{
		critical_network_errors_log = h2log::create("Critical Network Errors", prepareLogFileName(L"criticalNetworkErrors"), true, 0);

		LOG_CRITICAL_NETWORK(DLL_VERSION_STR "\n");
		LOG_CRITICAL_NETWORK("{} - initialized critical network log!", __FUNCTION__);
	}
}

void CXnIp::LogConnectionsToConsole()
{
	if (GetRegisteredKeyCount() > 0)
	{
		commands->output(L"XNet connections: ");
		LOG_CRITICAL_NETWORK(L"XNet connections: ");

		for (int i = 0; i < GetMaxXnConnections(); i++)
		{
			std::wstring logString;
			if (XnIPs[i].bValid)
			{
				XnIp* xnIp = &XnIPs[i];
				logString +=
					L"		Index: " + std::to_wstring(i) + L" " +
					L"Packets sent: " + std::to_wstring(xnIp->pckSent) + L" " +
					L"Packets received: " + std::to_wstring(xnIp->pckRecvd) + L" " +
					L"Connect status: " + std::to_wstring(xnIp->connectStatus) + L" " +
					L"Connection initiator: " + (xnIp->connectionInitiator ? L"yes" : L"no") + L" " +
					L"Time since last interaction: " + std::to_wstring((float)(timeGetTime() - xnIp->lastConnectionInteractionTime) / 1000.f) + L" " +
					L"Time since last packet received: " + std::to_wstring((float)(timeGetTime() - xnIp->lastPacketReceivedTime) / 1000.f);
			}
			else
			{
				logString += 
					L"Unused connection index: " + std::to_wstring(i);
			}
			commands->output(logString);
			LOG_CRITICAL_NETWORK(logString);
		}
	}
	else
	{
		commands->output(L"Cannot log XNet connections when no keys are registerd (you need to host/be in a game)");
		LOG_CRITICAL_NETWORK(L"Cannot log XNet connections when no keys are registerd (you need to host/be in a game)");
	}
}

void CXnIp::LogConnectionsErrorDetails(sockaddr_in* address, int errorCode, const XNKID* receivedKey)
{
	LOG_CRITICAL_NETWORK("{} - tried to add XNADDR in the system, caused error: {}", __FUNCTION__, errorCode);

	if (address != nullptr)
		LOG_CRITICAL_NETWORK("{} - received connection request from {}:{} we can't fulfil.", __FUNCTION__, inet_ntoa(address->sin_addr), htons(address->sin_port));

	int keysRegisteredCount = GetRegisteredKeyCount();

	if (keysRegisteredCount > 0)
	{
		for (int i = 0; i < keysRegisteredCount; i++)
		{
			if (XnKeyPairs[i].bValid)
			{
				LOG_TRACE_NETWORK("{} - registered session ID key: {}", __FUNCTION__, ByteToHexStr(XnKeyPairs[i].xnkid.ab, sizeof(XnKeyPairs[i].xnkey.ab)));
			}
		}

		XnKeyPair* matchingKey = getKeyPair(receivedKey);

		if (matchingKey == nullptr)
		{
			LOG_CRITICAL_NETWORK("{} - received key does not match any registered key!!", __FUNCTION__);
			return;
		}

		LOG_CRITICAL_NETWORK("{} - registered key count: {}", __FUNCTION__, keysRegisteredCount);
		for (int i = 0; i < GetMaxXnConnections(); i++)
		{
			if (XnIPs[i].bValid)
			{
				XnIp* xnIp = &XnIPs[i];
				float connectionLastPacketReceivedSeconds = (float)(timeGetTime() - xnIp->lastConnectionInteractionTime) / 1000.f;
				float connectionLastInteractionSeconds = (float)(timeGetTime() - xnIp->lastConnectionInteractionTime) / 1000.f;
				LOG_CRITICAL_NETWORK("{} - connection index: {}, packets sent: {}, packets received: {}, time since last interaction: {:.4f} seconds, time since last packet receive: {:.4f} seconds",
					__FUNCTION__,
					i,
					xnIp->pckSent,
					xnIp->pckRecvd,
					connectionLastInteractionSeconds,
					connectionLastPacketReceivedSeconds);
			}
			else
			{
				// we shouldn't ever get at this point
				LOG_CRITICAL_NETWORK("{} - hold up pendejo, wtf - connection index: {}, we have free connection registry and couldn't use it.", __FUNCTION__, i);
			}
		}
	}
	else
	{
		LOG_CRITICAL_NETWORK("{} - no keys are registered, cannot create connections with no registered keys!!", __FUNCTION__);
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
			}
		}
		break;

		case 'XNeT':
		{
			XNetRequestPacket* XNetPck = reinterpret_cast<XNetRequestPacket*>(lpBuffers->buf);
			if (*bytesRecvdCount == sizeof(XNetRequestPacket)
				&& strncmp(XNetPck->pckHeader.HdrStr, requestStrHdr, MAX_HDR_STR) == 0)
			{
				LOG_TRACE_NETWORK("{} - Received XNetRequest type: {} from ip address {:X}, port: {}", 
					__FUNCTION__, 
					(int)XNetPck->data.reqType, 
					ntohl(lpFrom->sin_addr.s_addr), 
					ntohs(lpFrom->sin_port));

				HandleXNetRequestPacket(xsocket, XNetPck, lpFrom, bytesRecvdCount); // save NAT info and send back a connection packet

				// set the bytes received count to 0
				// pool another packet after, so we keep the game fed with data
				*bytesRecvdCount = 0;
				return SOCKET_ERROR;
			}
		}
		break;

		default:
			break;
		} // switch (XNetPck->intHdr)
	}

	IN_ADDR ipIdentifier = GetConnectionIdentifierByRecvAddr(xsocket, lpFrom);

	// Let the game know the packet received came from an unkown source
	if (ipIdentifier.s_addr == 0)
	{
		LOG_ERROR_NETWORK("{} - discarding packet with size: {}", __FUNCTION__, *bytesRecvdCount);
		// set the bytes received count to 0 and recv address/identifier
		*bytesRecvdCount = 0;
		ZeroMemory(lpFrom, sizeof(*lpFrom));
		return SOCKET_ERROR;
	}

	lpFrom->sin_addr = ipIdentifier;
	updatePacketReceivedCounters(ipIdentifier, *bytesRecvdCount);

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
			&& xnIp->natIsUpdated())
		{
			// TODO: get rid of H2v only sockets
			switch (xsocket->getHostOrderSocketPort())
			{
			case 1000:
				if (xsocket->sockAddrInEqual(fromAddr, xnIp->getNatAddr(H2v_sockets::Sock1000)))
					return xnIp->connectionIdentifier;
				break;

			case 1001:
				if (xsocket->sockAddrInEqual(fromAddr, xnIp->getNatAddr(H2v_sockets::Sock1001)))
					return xnIp->connectionIdentifier;
				break;

			default:
				// wtf?... unknown socket
				LOG_CRITICAL_NETWORK("{} - unkown network socket!", __FUNCTION__);
				return addrInval;
			}
		}
	}
	
	LOG_ERROR_NETWORK("{} - received packet from unknown/unregistered source, ip address: {}:{}", __FUNCTION__, inet_ntoa(fromAddr->sin_addr), htons(fromAddr->sin_port));
	return addrInval;
}

void CXnIp::SaveNatInfo(XSocket* xsocket, IN_ADDR connectionIdentifier, sockaddr_in* addr)
{
	LOG_INFO_NETWORK("{} - socket: {}, connection index: {}, identifier: {:X}", __FUNCTION__, xsocket->winSockHandle, getConnectionIndex(connectionIdentifier), connectionIdentifier.s_addr);

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
			xnIp->updateNat(H2v_sockets::Sock1000, addr);
			break;

		case 1001:
			//LOG_TRACE_NETWORK("SaveConnectionNatInfo() xnIp->NatAddrSocket1001 mapping port 1001 - port: {}, connection identifier: {:x}", htons(addr->sin_port), xnIp->connectionIdentifier.s_addr);
			xnIp->updateNat(H2v_sockets::Sock1001, addr);
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

void CXnIp::HandleXNetRequestPacket(XSocket* xsocket, const XNetRequestPacket* reqPacket, sockaddr_in* recvAddr, LPDWORD lpBytesRecvdCount)
{
	IN_ADDR connectionIdentifier;
	connectionIdentifier.s_addr = 0;

	int ret = CreateXnIpIdentifierFromPacket(&reqPacket->data.xnaddr, &reqPacket->data.xnkid, reqPacket, &connectionIdentifier);
	// if CreateXnIpIdentifierFromPacket is successful, we created another connection or we returned an already present one
	if (ret == 0)
	{
		// TODO: get rid of H2v only sockets

		XnIp* xnIp = &XnIPs[getConnectionIndex(connectionIdentifier)];
		//if (xnIp->isValid(connectionIdentifier)) // this check should be irelevant
		{
			switch (reqPacket->data.reqType)
			{
			case XnIp_ConnectionUpdateNAT:
				if (xnIp->connectStatus <= XNET_CONNECT_STATUS_PENDING)
				{
					if (!xnIp->natIsUpdated())
					{
						SaveNatInfo(xsocket, connectionIdentifier, recvAddr);
						xnIp->connectStatus = XNET_CONNECT_STATUS_PENDING;

						// we need to fully update NAT before anything else
						if (xnIp->natIsUpdated())
						{
							if (XNIP_TEST_BIT(reqPacket->data.flags, XnIp_HasEndpointNATData)) // don't send back if the other end has our NAT data already
							{
								SendXNetRequest(xsocket, connectionIdentifier, XnIp_ConnectionEstablishSecure); // send EstablishSecure after we saved all NAT
							}
							else
							{
								SendXNetRequestAllSockets(connectionIdentifier, XnIp_ConnectionUpdateNAT);
							}
						}
					}
					else
					{
						LOG_ERROR_NETWORK("{} - already present NAT.", __FUNCTION__);
					}
				}
				break;

			case XnIp_ConnectionEstablishSecure:
				// first save the port we received connection request from
				if (xnIp->connectStatus < XNET_CONNECT_STATUS_CONNECTED)
				{
					if (xnIp->natIsUpdated()) // check if the last connect packet was sent from the last socket to have NAT data saved
					{
						// bellow TODO comment not relevant anymore but will keep for history
						{
							// TODO: what if we just stored all NAT data, but we dont send XnIp_ConnectionEstablishSecure back because the game already did by calling XNetConnect()
							// which could have resulted in dropped packets because:
							// 1: the other side doesn't have the ports forwarded but we do have them, and we received the NAT data but we think we already sent EstacnlishSecure packet
						}

						// when 2 peers try to connect to each other at the same time
						// determine which peer should handle the connecting by who has a bigger MAC address
						// or if the other side doesn't consider itself as a connection initiator, just send
						// if the other connection cannot receive packets (ports are not forwarded), connectReqPkt->data.connectionInitiator will always be true

						// prevent this being sent twice, this will happen if the game Calls XNetConnect at the same time on each endpoint
						// and both ends have the ports open
						if (!XNIP_TEST_BIT(xnIp->flags, XnIp::XnIp_ConnectDeclareConnectedRequestSent))
						{
							if (!reqPacket->data.connectionInitiator
								|| memcmp(GetLocalUserXn()->xnaddr.abEnet, reqPacket->data.xnaddr.abEnet, sizeof(XNADDR::abEnet)) > 0
								)
							{
								SendXNetRequest(xsocket, connectionIdentifier, XnIp_ConnectionDeclareConnected);
							}
							else
							{
								SendXNetRequest(xsocket, connectionIdentifier, XnIp_ConnectionEstablishSecure);
							}

							XNIP_SET_BIT(xnIp->flags, XnIp::XnIp_ConnectDeclareConnectedRequestSent);
						}
					}
					else
					{
						LOG_CRITICAL_NETWORK("{} - attempt at establishing secure connection, but we don't have NAT data!", __FUNCTION__);
					}
				}
				break;

			// this will prevent the connection to send packets before we know we can send them
			case XnIp_ConnectionDeclareConnected:

				// if NAT isn't updated already, we might not have the ports forwarded
				/*if (!xnIp->natIsUpdated())
				{
					SaveNatInfo(xsocket, connectionIdentifier, recvAddr);

					if (!xnIp->logErrorOnce && xnIp->otherSideNonceKeyReceived)
					{
						LOG_CRITICAL_NETWORK("{} - XnIp_ConnectionDeclareConnected - wtf, NAT wasn't updated but the nonce key is.");
						xnIp->logErrorOnce = true;
					}

					// we might not even have the nonceKey
					if (!xnIp->otherSideNonceKeyReceived)
					{
						memcpy(xnIp->connectionNonceOtherSide, reqPacket->data.nonceKey, sizeof(XnIp::connectionNonceOtherSide));
						xnIp->otherSideNonceKeyReceived = true;
					}
				} */

				// check again if the situation changed, and NAT data got updated
				if (xnIp->natIsUpdated())
				{
					switch (xnIp->connectStatus)
					{
					case XNET_CONNECT_STATUS_PENDING:
						gXnIp.SendXNetRequest(xsocket, connectionIdentifier, XnIp_ConnectionDeclareConnected);
						xnIp->connectStatus = XNET_CONNECT_STATUS_CONNECTED;
						break;

					default:
						break;
					}
				}
				else
				{
					LOG_CRITICAL_NETWORK("{} - XnIp_ConnectionDeclareConnected but NAT isn't updated!", __FUNCTION__);
				}
				break;

			case XnIp_ConnectionCloseSecure:
				//HandleDisconnectPacket(xsocket, XNetPck, lpFrom);
				//LOG_TRACE_NETWORK("{} - Received ConnectionCloseSecure request from ip address {:x}, port: {}", __FUNCTION__, htonl(lpFrom->sin_addr.s_addr), htons(lpFrom->sin_port));
				break;

			case XnIp_ConnectionPing:
			case XnIp_ConnectionPong:
				LOG_CRITICAL_NETWORK("{} - unimplemented request type: {}", __FUNCTION__, reqPacket->data.reqType);
				break;

			default:
				break;
			}

			// increase packets received count
			int bytesReceivedCount = 0;
			if (lpBytesRecvdCount)
				bytesReceivedCount = *lpBytesRecvdCount;
			updatePacketReceivedCounters(connectionIdentifier, bytesReceivedCount);
		}
	}
	else
	{
		LOG_TRACE_NETWORK("{} - secure connection cannot be established!" __FUNCTION__);
		LogConnectionsErrorDetails(recvAddr, ret, &reqPacket->data.xnkid);
		// TODO: send back the connection cannot be established
	}
}

void CXnIp::HandleDisconnectPacket(XSocket* xsocket, const XNetRequestPacket* disconnectReqPck, sockaddr_in* recvAddr)
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
	/*
		Update: 1/31/2020
		- Creates an identifier to be used by WinSock calls from a XNADDR address
		- It gets created when we received a XNet request connection packet, or when the game calls XNetXnAddrToInAddr
		- If XNetInAddrToXnAddr passes an invalid identifier, it will return WSAEINVAL
	*/

	XnKeyPair* keyPair = getKeyPair(pxnkid);
	if (keyPair != nullptr)
	{
		XnIp* newXnIp = &XnIPs[connectionIndex];
		SecureZeroMemory(newXnIp, sizeof(*newXnIp));

		newXnIp->xnaddr = *pxna;
		newXnIp->keyPair = keyPair;

		XNetRandom(newXnIp->connectionNonce, sizeof(XnIp::connectionNonce));

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

// TODO: add separate function that doesn't create XnIp identification from packet
// for now if reqPacket is null it means this is not called after a connection packet is received
// basically reqPacket = nullptr, XNetXnAddrToInAddr() context, if reqPacket != nullptr, HandleXNetRequestPacket() context
int CXnIp::CreateXnIpIdentifierFromPacket(const XNADDR* pxna, const XNKID* pxnkid, const XNetRequestPacket* reqPacket, IN_ADDR* outIpIdentifier)
{
#define NO_MORE_CONNECT_SPOTS(_function, _err) \
	{ \
		LOG_CRITICAL_NETWORK("{} - no more available connection spots!", (_function)); \
		return(_err); \
	} \

	XnIp* localConnectionInfo = GetLocalUserXn();
	if (localConnectionInfo == nullptr)
	{
		LOG_TRACE_NETWORK("{} - XNADDR information is not populated!", __FUNCTION__);
		return WSAEADDRNOTAVAIL;
	}

	if (memcmp(localConnectionInfo->xnaddr.abEnet, pxna->abEnet, sizeof(XNADDR::abEnet)) == 0)
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

	// check if we can create another XnIp identifier, if not already present
	// and if there's any free spots for a new connection
	if (firstUnusedConnectionIndexFound 
		&& pXnIpAlreadyRegistered == nullptr) 
	{
		int result = registerNewXnIp(firstUnusedConnectionIndex, pxna, pxnkid, outIpIdentifier);
		if (result == 0 // check if registerNewXnIp was successful
			&& reqPacket != nullptr) 
		{
			XnIp* newlyCreatedXnIp = getConnection(*outIpIdentifier);
			memcpy(newlyCreatedXnIp->connectionNonceOtherSide, reqPacket->data.nonceKey, sizeof(XnIp::connectionNonceOtherSide));
			newlyCreatedXnIp->otherSideNonceKeyReceived = true;

			LOG_TRACE_NETWORK("{} - updated noOnce key for connection identifier: {:X}, n0nceKey: {}", 
				__FUNCTION__, 
				newlyCreatedXnIp->connectionIdentifier.s_addr,
				ByteToHexStr(newlyCreatedXnIp->connectionNonceOtherSide, 8)
			);

			// if we received and created this identification after we received a packet, the other side tries to initiate a connection
			newlyCreatedXnIp->connectionInitiator = true;
		}
		else if (result == 0) // check if registerNewXnIp was successful
		{
			XnIp* newlyCreatedXnIp = getConnection(*outIpIdentifier);

			// if we didn't create the new identification from a packet, we consider our side the connection initiator
			newlyCreatedXnIp->connectionInitiator = false;
		}
		return result;
	}
	else if (pXnIpAlreadyRegistered != nullptr)
	{
		// if this function is called after a packet is received and the request type is ConnectionEstablishSecure
		// and the connection Nonce (random number created when registering a new connection), it means the other side tries to re-connect, we clear the old identification, then create a new one
		// or if there are differences between XNADDRs (like the port or even the IP address) but the MAC address is the same, clear and create another connection


		// TODO FIXME: for a small subset of players pXnIpAlreadyRegistered->connectionNonceOtherSide is different from reqPacket->data.nonceKey when connection attempt is in progress
		// causing the connection to fail
		if (
			(reqPacket != nullptr
			&& pXnIpAlreadyRegistered->otherSideNonceKeyReceived
			&& memcmp(pXnIpAlreadyRegistered->connectionNonceOtherSide, reqPacket->data.nonceKey, sizeof(XnIp::connectionNonce)) != 0)
			|| 
			(pXnIpAlreadyRegistered->xnaddr.ina.s_addr != pxna->ina.s_addr
			|| pXnIpAlreadyRegistered->xnaddr.inaOnline.s_addr != pxna->inaOnline.s_addr
			|| pXnIpAlreadyRegistered->xnaddr.wPortOnline != pxna->wPortOnline)
			)
		{
			int newConnectionIndex = getConnectionIndex(pXnIpAlreadyRegistered->connectionIdentifier);
			if (firstUnusedConnectionIndexFound) // if we have another connect spot, use it
				newConnectionIndex = firstUnusedConnectionIndex; // we prefer using another connection index if possible

			XNetUnregisterInAddr(pXnIpAlreadyRegistered->connectionIdentifier); // unregister the connection
			int result = registerNewXnIp(newConnectionIndex, pxna, pxnkid, outIpIdentifier); // register a new one, if it gets at this point, it shouldn't fail at all
			if (result == 0
				&& reqPacket != nullptr) // check if registerNewXnIp was successful
			{
				XnIp* newlyCreatedXnIp = getConnection(*outIpIdentifier);
				memcpy(newlyCreatedXnIp->connectionNonceOtherSide, reqPacket->data.nonceKey, sizeof(XnIp::connectionNonceOtherSide));
				newlyCreatedXnIp->otherSideNonceKeyReceived = true;

				// if we received and created this identification after we received a packet, the other side tries to initiate a connection
				// the game can't know this new connection is about to be created, we just received the packet from the other side
				newlyCreatedXnIp->connectionInitiator = true;

				LOG_TRACE_NETWORK("{} - otherSideNonceKeyReceived: {} != reqPacket->data.nonceKey: {}",
					__FUNCTION__,
					ByteToHexStr(pXnIpAlreadyRegistered->connectionNonceOtherSide, 8),
					ByteToHexStr(reqPacket->data.nonceKey, 8)
				);
			}
			else if (result == 0) // check if registerNewXnIp was successful
			{
				XnIp* newlyCreatedXnIp = getConnection(*outIpIdentifier);

				// if we didn't create the new identification from a packet, we consider our side the connection initiator
				// the game will know this is a new connection, because this function has been called in a XNetXnAddrToInAddr context
				newlyCreatedXnIp->connectionInitiator = false;
			}
			return result;
		}

		// update the nonce key if we got this far, and this function has been called after we received a connection packet
		if (reqPacket != nullptr
			&& !pXnIpAlreadyRegistered->otherSideNonceKeyReceived)
		{
			LOG_TRACE_NETWORK("{} - updated noOnce key for connection identifier: {:X}, n0ncekey: {}", 
				__FUNCTION__, 
				pXnIpAlreadyRegistered->connectionIdentifier.s_addr,
				ByteToHexStr(reqPacket->data.nonceKey, 8)
			);
			memcpy(pXnIpAlreadyRegistered->connectionNonceOtherSide, reqPacket->data.nonceKey, sizeof(XnIp::connectionNonceOtherSide));
			pXnIpAlreadyRegistered->otherSideNonceKeyReceived = true;
		}

		if (outIpIdentifier)
		{
			*outIpIdentifier = pXnIpAlreadyRegistered->connectionIdentifier;
			LOG_INFO_NETWORK("{} - already present connection identifier: {:X}", __FUNCTION__, pXnIpAlreadyRegistered->connectionIdentifier.s_addr);
		}

		return 0;
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
				LOG_TRACE_NETWORK("{} - XnKeyPair: xnkid {}, xnkey: {} already registered!", 
					__FUNCTION__, 
					ByteToHexStr(pxnkid->ab, sizeof(pxnkid->ab)), 
					ByteToHexStr(pxnkey->ab, sizeof(pxnkey->ab))
				);
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
		LOG_CRITICAL_NETWORK("{} - reached max key registrations!", __FUNCTION__);
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

	LOG_CRITICAL_NETWORK("{} - xnkid {} is unknown!", __FUNCTION__, ByteToHexStr(pxnkid->ab, sizeof(pxnkid->ab)));
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

void CXnIp::SendXNetRequestAllSockets(IN_ADDR connectionIdentifier, eXnip_ConnectRequestType reqType)
{
	// send for each UDP socket, the other side may not have the NAT data
	for (auto sockIt : XSocket::Sockets)
	{
		// TODO: handle dinamically, so it can be used by other games too
		if (sockIt->isUDP() // connect only UDP sockets
			&& H2v_socketsToConnect.find(sockIt->getHostOrderSocketPort()) != H2v_socketsToConnect.end())
		{
			gXnIp.SendXNetRequest(sockIt, connectionIdentifier, reqType);
		}
	}
}

void CXnIp::SendXNetRequest(XSocket* xsocket, IN_ADDR connectionIdentifier, eXnip_ConnectRequestType reqType)
{
	sockaddr_in sendToAddr;
	SecureZeroMemory(&sendToAddr, sizeof(sockaddr_in));

	XnIp* xnIp = gXnIp.getConnection(connectionIdentifier);
	if (xnIp != nullptr)
	{
		sendToAddr.sin_family = AF_INET;
		sendToAddr.sin_addr = connectionIdentifier;
		sendToAddr.sin_port = xsocket->getNetworkOrderSocketPort();

		XNetRequestPacket reqPacket;

		reqPacket.data.reqType = reqType;
		memcpy(reqPacket.data.xnkid.ab, xnIp->keyPair->xnkid.ab, sizeof(XNKID::ab));
		memcpy(reqPacket.data.nonceKey, xnIp->connectionNonce, sizeof(XnIp::connectionNonce));
		XNetGetTitleXnAddr(&reqPacket.data.xnaddr);

		switch (reqType)
		{
		case XnIp_ConnectionUpdateNAT:
		case XnIp_ConnectionEstablishSecure:
			if (xnIp->natIsUpdated())
				XNIP_SET_BIT(reqPacket.data.flags, XnIp_HasEndpointNATData);
			reqPacket.data.connectionInitiator = !xnIp->connectionInitiator;
			break;

		case XnIp_ConnectionDeclareConnected:
			break;

		case XnIp_ConnectionCloseSecure:
		case XnIp_ConnectionPong:
		case XnIp_ConnectionPing:
			LOG_CRITICAL_NETWORK("{} - unimplemented requests!", __FUNCTION__);
			break;

		default:
			// this shouldn't be executed ever
			LOG_ERROR_NETWORK("{} - invalid request type", __FUNCTION__);
			break;
		}

		xnIp->connectionPacketsSentCount++;

		int ret = xsocket->udpSend((char*)&reqPacket, sizeof(XNetRequestPacket), 0, (sockaddr*)&sendToAddr, sizeof(sendToAddr));
		LOG_INFO_NETWORK("{} - secure packet sent socket handle: {}, connection index: {}, connection identifier: {:x}, n0nceKey: {}", 
			__FUNCTION__, 
			xsocket->winSockHandle, 
			gXnIp.getConnectionIndex(connectionIdentifier), 
			sendToAddr.sin_addr.s_addr,
			ByteToHexStr(reqPacket.data.nonceKey, 8)
		);
	}
	else
	{
		LOG_CRITICAL_NETWORK("{} - connection index: {}, identifier: {:x} is invalid!", __FUNCTION__, gXnIp.getConnectionIndex(connectionIdentifier), connectionIdentifier.s_addr);
	}
}

void CXnIp::SetupLocalConnectionInfo(unsigned long xnaddr, unsigned long lanaddr, const char* abEnet, const char* abOnline)
{
	SecureZeroMemory(&ipLocal, sizeof(ipLocal));

	ipLocal.xnaddr.ina = CNic::GetBestInterfaceRouteAddressFromIp("0.0.0.0"); // pass INADDR_ANY

	if (Ipv4AddressIsReservedOrLocalhost(ipLocal.xnaddr.ina))
	{
		LOG_TRACE_NETWORK("{} - IP address is private - {}", __FUNCTION__, inet_ntoa(ipLocal.xnaddr.ina));
	}
	else
	{
		// fall back to localhost if what GetBestIpToIpRoute found is not a private/local host ip address (maybe we are not under a NAT gateway)
		LOG_TRACE_NETWORK("{} - GetBestIpToIpRoute() returned public IP address, maybe we are not under a NAT device, fall back to localhost IP address!", __FUNCTION__);
		ipLocal.xnaddr.ina.s_addr = inet_addr("127.0.0.1");
	}

	// override what GetBestIpToIpRoute found if lanaddr is different than 0
	if (lanaddr != 0)
	{
		ipLocal.xnaddr.ina.s_addr = lanaddr; // check if the lanaddr is specified already
		LOG_TRACE_NETWORK("{} - lanaddr already specified: {}", __FUNCTION__, inet_ntoa(ipLocal.xnaddr.ina));
	}

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
	if (lostConnectionsCount > 0)
		LOG_CRITICAL_NETWORK("{} - lost {} connections!", __FUNCTION__, lostConnectionsCount);
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

	int ret = gXnIp.CreateXnIpIdentifierFromPacket(pxna, pxnkid, nullptr, pina);

	if (ret != 0)
	{
		gXnIp.LogConnectionsErrorDetails(nullptr, ret, pxnkid);
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
	if (xnIp == nullptr)
	{
		LOG_CRITICAL_NETWORK("{} - connection index: {}, identifier: {:X} are invalid!", __FUNCTION__, gXnIp.getConnectionIndex(ina), ina.s_addr);
		return WSAEINVAL;
	}

	if (pxna)
		*pxna = xnIp->xnaddr;

	if (pxnkid)
		memcpy(pxnkid->ab, xnIp->keyPair->xnkid.ab, sizeof(XNKID::ab));

	return 0;
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
	if (xnIp == nullptr)
		return WSAEINVAL;
	
	// send connect packets only if the state is idle
	if (xnIp->connectStatus == XNET_CONNECT_STATUS_IDLE)
	{
		if (!xnIp->natIsUpdated())
		{
			gXnIp.SendXNetRequestAllSockets(ina, XnIp_ConnectionUpdateNAT);
			xnIp->connectStatus = XNET_CONNECT_STATUS_PENDING; // after we sent, set the state to PENDING
		}
	}

	return 0;
}

// #66: XNetGetConnectStatus
int WINAPI XNetGetConnectStatus(const IN_ADDR ina)
{
	//LOG_INFO_NETWORK("{} : connection index {}, identifier: {:x}", ipManager.getConnectionIndex(ina), ina.s_addr);
	XnIp* xnIp = gXnIp.getConnection(ina);
	if (xnIp == nullptr)
	{
		LOG_CRITICAL_NETWORK("{} - connection index: {}, identifier: {:X} is invalid!", __FUNCTION__, gXnIp.getConnectionIndex(ina), ina.s_addr);
		return XNET_CONNECT_STATUS_LOST;
	}

	if (xnIp->connectStatus == XNET_CONNECT_STATUS_CONNECTED)
	{
		gXnIp.setTimeConnectionInteractionHappened(ina);
	}
	else if (xnIp->connectStatus < XNET_CONNECT_STATUS_CONNECTED
		&& timeGetTime() - xnIp->lastConnectionInteractionTime >= XnIp_ConnectionTimeOut)
	{
		return XNET_CONNECT_STATUS_LOST;
	}

	return xnIp->connectStatus;
}

// #73: XNetGetTitleXnAddr
DWORD WINAPI XNetGetTitleXnAddr(XNADDR * pxna)
{
	XnIp* localUserXnIp = gXnIp.GetLocalUserXn();

	if (localUserXnIp == nullptr)
		return XNET_GET_XNADDR_PENDING;
	
	*pxna = localUserXnIp->xnaddr;
	return XNET_GET_XNADDR_ETHERNET;
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