#include "stdafx.h"

#include "XnIp.h"
#include "..\..\Cryptography\Rc4.h"

#include "H2MOD\Utils\Utils.h"
#include "H2MOD\Modules\Shell\Config.h"
#include "H2MOD\Modules\Shell\Startup\Startup.h"

#include "..\NIC.h"
#include "..\net_utils.h"

// TODO singleton?
CXnIp gXnIp;
// local user xbox network address
XnIp CXnIp::m_ipLocal;

XECRYPT_RC4_STATE gRc4StateRand;

// network log for critical errors
h2log* critical_network_errors_log = nullptr;

void CXnIp::Initialize(const XNetStartupParams* netStartupParams)
{
	memcpy(&m_startupParams, netStartupParams, sizeof(XNetStartupParams));

	if (m_startupParams.cfgSecRegMax == 0)
		m_startupParams.cfgSecRegMax = 32; // default 32 XNet connections

	if (m_XnIPs != nullptr)
		delete[] m_XnIPs;

	m_XnIPs = new XnIp[m_startupParams.cfgSecRegMax];
	ZeroMemory(m_XnIPs, m_startupParams.cfgSecRegMax * sizeof(*m_XnIPs));

	if (m_startupParams.cfgKeyRegMax == 0)
		m_startupParams.cfgKeyRegMax = 4; // default 4 key pairs

	if (m_XnKeyPairs != nullptr)
		delete[] m_XnKeyPairs;

	m_XnKeyPairs = new XnKeyPair[m_startupParams.cfgKeyRegMax];
	ZeroMemory(m_XnKeyPairs, m_startupParams.cfgKeyRegMax * sizeof(*m_XnKeyPairs));

	if (m_startupParams.cfgQosDataLimitDiv4 == 0)
		m_startupParams.cfgQosDataLimitDiv4 = 256 / 4; // 256 bytes as default

	if (m_startupParams.cfgSockDefaultRecvBufsizeInK <= 0)
		m_startupParams.cfgSockDefaultRecvBufsizeInK = SOCK_UDP_MIN_RECV_BUFFER_K_UNITS;

	if (m_startupParams.cfgSockDefaultSendBufsizeInK <= 0)
		m_startupParams.cfgSockDefaultSendBufsizeInK = SOCK_UDP_MIN_SEND_BUFFER_K_UNITS;

	// initielize critical network logs
	// TODO: disable after all network problems are addressed
	if (network_log == nullptr)
	{
		critical_network_errors_log = h2log::create("Critical Network Errors", prepareLogFileName(L"criticalNetworkErrors"), true, 0);

		LOG_CRITICAL_NETWORK(DLL_VERSION_STR);
		LOG_CRITICAL_NETWORK("{} - initialized critical network log!", __FUNCTION__);
	}
}

// gets the actual connection index from a connection identifier
int CXnIp::GetConnectionIndex(IN_ADDR connectionIdentifier)
{
	return (int)(connectionIdentifier.s_addr >> 24);
}

XnIp* CXnIp::GetConnection(const IN_ADDR ina) const
{
	XnIp* xnIp = &m_XnIPs[GetConnectionIndex(ina)];
	return xnIp->IsValid(ina) ? xnIp : nullptr;
}

void CXnIp::SetTimeConnectionInteractionHappened(IN_ADDR ina)
{
	XnIp* xnIp = GetConnection(ina);
	if (xnIp != nullptr)
		xnIp->lastConnectionInteractionTime = _Shell::QPCToTimeNowMsec();
}

void CXnIp::UpdatePacketReceivedCounters(IN_ADDR ipIdentifier, unsigned int bytesRecvdCount)
{
	SetTimeConnectionInteractionHappened(ipIdentifier);
	XnIp* xnIp = GetConnection(ipIdentifier);
	if (xnIp != nullptr)
	{
		xnIp->pckStats.PckRecvdStatsUpdate(1, bytesRecvdCount);
		xnIp->lastPacketReceivedTime = _Shell::QPCToTimeNowMsec();
		GetLocalUserXn()->pckStats.PckRecvdStatsUpdate(1, bytesRecvdCount);
	}
}

void CXnIp::LogConnectionsToConsole(IOutput* output)
{
	if (GetRegisteredKeyCount() > 0)
	{
		const char* xnet_connections_str = "XNet connections: ";
		LOG_CRITICAL_NETWORK(xnet_connections_str);
		if (output)
			output->OutputFmt(StringFlag_None, "# %s", xnet_connections_str);

		for (int i = 0; i < GetMaxXnConnections(); i++)
		{
			std::string logString;
			if (m_XnIPs[i].bValid)
			{
				XnIp* xnIp = &m_XnIPs[i];

				const XnIpPckTransportStats* pckStats;
				xnIp->PckGetStats(&pckStats);

				logString +=
					"		Index: " + std::to_string(i) + " " +
					"Packets sent: " + std::to_string(pckStats->pckBytesSent) + " " +
					"Packets received: " + std::to_string(pckStats->pckBytesRecvd) + " " +
					"Packets sent avg/sec: " + std::to_string(pckStats->pckAvgSentPerSec) + " " +
					"Packets recvd avg/sec: " + std::to_string(pckStats->pckAvgRecvdPerSec) + " " +
					"Connect status: " + std::to_string(xnIp->connectStatus) + " " +
					"Connection initiator: " + (xnIp->connectionInitiator ? "yes" : "no") + " " +
					"Time since last interaction: " + std::to_string((float)(_Shell::QPCToTimeNowMsec() - xnIp->lastConnectionInteractionTime) / 1000.f) + " " +
					"Time since last packet received: " + std::to_string((float)(_Shell::QPCToTimeNowMsec() - xnIp->lastPacketReceivedTime) / 1000.f);
			}
			else
			{
				logString +=
					"Unused connection index: " + std::to_string(i);
			}

			LOG_CRITICAL_NETWORK(logString);
			if (output)
				output->OutputFmt(StringFlag_None, "# %s", logString.c_str());
		}
	}
	else
	{
		const char* err_message = "cannot log XNet connections when no keys are registerd (you need to host/be in a game)";
		LOG_CRITICAL_NETWORK(err_message);
		if (output)
			output->OutputFmt(StringFlag_None, "# %s", err_message);
	}
}

void CXnIp::LogConnectionsErrorDetails(const sockaddr_in* address, int errorCode, const XNKID* receivedKey)
{
	LOG_CRITICAL_NETWORK("{} - tried to add XNADDR in the system, caused error: {}", __FUNCTION__, errorCode);

	if (address != nullptr)
		LOG_CRITICAL_NETWORK("{} - received connection request from {}:{} we can't fulfil.", __FUNCTION__, inet_ntoa(address->sin_addr), htons(address->sin_port));

	int keysRegisteredCount = GetRegisteredKeyCount();

	if (keysRegisteredCount > 0)
	{
		for (int i = 0; i < keysRegisteredCount; i++)
		{
			if (m_XnKeyPairs[i].bValid)
			{
				LOG_CRITICAL_NETWORK("{} - registered session ID key: {}",
					__FUNCTION__,
					ByteToHexStr(m_XnKeyPairs[i].xnkid.ab, sizeof(m_XnKeyPairs[i].xnkid.ab)).c_str());
			}
		}

		XnKeyPair* matchingKey = GetKeyPair(receivedKey);

		if (matchingKey == nullptr)
		{
			LOG_CRITICAL_NETWORK("{} - received key does not match any registered key!!", __FUNCTION__);
			return;
		}

		LOG_CRITICAL_NETWORK("{} - registered key count: {}", __FUNCTION__, keysRegisteredCount);
		for (int i = 0; i < GetMaxXnConnections(); i++)
		{
			if (m_XnIPs[i].bValid)
			{
				XnIp* xnIp = &m_XnIPs[i];

				const XnIpPckTransportStats* pckStats;
				xnIp->PckGetStats(&pckStats);

				float connectionLastInteractionSeconds = (float)(_Shell::QPCToTimeNowMsec() - xnIp->lastConnectionInteractionTime) / 1000.f;
				float connectionLastPacketReceivedSeconds = (float)(_Shell::QPCToTimeNowMsec() - xnIp->lastConnectionInteractionTime) / 1000.f;
				LOG_CRITICAL_NETWORK("{} - connection index: {}, packets sent: {}, packets received: {}, time since last interaction: {:.4f} seconds, time since last packet receive: {:.4f} seconds",
					__FUNCTION__,
					i,
					pckStats->pckSent,
					pckStats->pckRecvd,
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

int CXnIp::HandleRecvdPacket(XSocket* xsocket, sockaddr_in* lpFrom, WSABUF* lpBuffers, DWORD dwBufferCount, LPDWORD lpBytesRecvdCount)
{
	// check first if the packet received has the size bigger or equal to the XNet packet header first
	if (*lpBytesRecvdCount >= sizeof(XNetPacketHeader))
	{
		XNetPacketHeader* XNetPck = reinterpret_cast<XNetPacketHeader*>(lpBuffers[0].buf);
		switch (XNetPck->intHdr)
		{
		case 'BrOd':
		{
			XBroadcastPacket* broadcastPck = reinterpret_cast<XBroadcastPacket*>(lpBuffers[0].buf);
			if (*lpBytesRecvdCount >= sizeof(XBroadcastPacket)
				&& strncmp(broadcastPck->pckHeader.HdrStr, broadcastStrHdr, MAX_HDR_STR) == 0
				&& broadcastPck->data.name.sin_addr.s_addr == INADDR_BROADCAST)
			{
				if (*lpBytesRecvdCount > sizeof(XBroadcastPacket))
				{
					*lpBytesRecvdCount = *lpBytesRecvdCount - sizeof(XBroadcastPacket);
					memmove(lpBuffers[0].buf, lpBuffers[0].buf + sizeof(XBroadcastPacket), *lpBytesRecvdCount);
					lpFrom->sin_addr = broadcastPck->data.name.sin_addr;
					lpFrom->sin_port = xsocket->GetNetworkOrderSocketPort();
					return 0;
				}
			}
		}
		break;

		case 'XNeT':
		{
			XNetRequestPacket* XNetPck = reinterpret_cast<XNetRequestPacket*>(lpBuffers[0].buf);
			if (*lpBytesRecvdCount == sizeof(XNetRequestPacket)
				&& strncmp(XNetPck->pckHeader.HdrStr, requestStrHdr, MAX_HDR_STR) == 0)
			{
				LOG_INFO_NETWORK("{} - Received XNetRequest type: {} from ip address {:X}, port: {}",
					__FUNCTION__,
					(int)XNetPck->data.reqType,
					ntohl(lpFrom->sin_addr.s_addr),
					ntohs(lpFrom->sin_port));

				HandleXNetRequestPacket(xsocket, XNetPck, lpFrom, lpBytesRecvdCount); // save NAT info and send back a connection packet

				// set the bytes received count to 0
				// should pool another packet after, so we keep the game fed with data
				*lpBytesRecvdCount = 0;
				return SOCKET_ERROR;
			}
		}
		break;

		default:
			break;
		} // switch (XNetPck->intHdr)
	}

	IN_ADDR ipIdentifier;

	// Let the game know the packet received came from an unkown source
	if (!GetEstablishedConnectionIdentifierByRecvAddr(xsocket, lpFrom, &ipIdentifier))
	{
		lpFrom->sin_addr = ipIdentifier;
		lpFrom->sin_port = xsocket->GetNetworkOrderSocketPort(); // Virtual socket ID, not the actual port
		UpdatePacketReceivedCounters(ipIdentifier, *lpBytesRecvdCount);
		return 0;
	}
	else
	{
		// set the bytes received count to 0 and recv address/identifier
		// when the packet comes from an unknown source
		LOG_ERROR_NETWORK("{} - discarding packet with size: {}", __FUNCTION__, *lpBytesRecvdCount);
		*lpBytesRecvdCount = 0;
		ZeroMemory(lpFrom, sizeof(*lpFrom));
		return SOCKET_ERROR;
	}
}

// Outputs the connection identifier
// Returns error if connection is not found or connected
int CXnIp::GetEstablishedConnectionIdentifierByRecvAddr(XSocket* xsocket, const sockaddr_in* fromAddr, IN_ADDR* outConnectionIdentifier)
{
	outConnectionIdentifier->s_addr = 0;

	for (int i = 0; i < GetMaxXnConnections(); i++)
	{
		XnIp* xnIp = &m_XnIPs[i];
		if (xnIp->bValid
			&& xnIp->NatIsUpdated())
		{
			// TODO: get rid of H2v only sockets
			switch (xsocket->GetHostOrderSocketPort())
			{
			case 1000:
				if (xsocket->SockAddrInEqual(fromAddr, xnIp->GetNatAddr(H2v_sockets::Sock1000)))
				{
					*outConnectionIdentifier = xnIp->connectionIdentifier;
					return 0;
				}
				break;

			case 1001:
				if (xsocket->SockAddrInEqual(fromAddr, xnIp->GetNatAddr(H2v_sockets::Sock1001)))
				{
					*outConnectionIdentifier = xnIp->connectionIdentifier;
					return 0;
				}
				break;

			default:
				// wtf?... unknown socket
				LOG_CRITICAL_NETWORK("{} - unkown network socket!", __FUNCTION__);
				return WSAEINVAL;
			} // switch (xsocket->getHostOrderSocketPort())
		}
	}

	LOG_ERROR_NETWORK("{} - received packet from unknown/unregistered source, ip address: {}:{}", __FUNCTION__, inet_ntoa(fromAddr->sin_addr), htons(fromAddr->sin_port));
	return WSAEINVAL;
}

void CXnIp::SaveNatInfo(XSocket* xsocket, IN_ADDR connectionIdentifier, const sockaddr_in* addr)
{
	LOG_TRACE_NETWORK("{} - socket: {}, connection index: {}, identifier: {:X}", __FUNCTION__, xsocket->winSockHandle, GetConnectionIndex(connectionIdentifier), connectionIdentifier.s_addr);

	XnIp* xnIp = GetConnection(connectionIdentifier);
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
		switch (xsocket->GetHostOrderSocketPort())
		{
		case 1000:
			//LOG_TRACE_NETWORK("SaveConnectionNatInfo() xnIp->NatAddrSocket1000 mapping port 1000 - port: {}, connection identifier: {:x}", htons(addr->sin_port), xnIp->connectionIdentifier.s_addr);
			xnIp->UpdateNat(H2v_sockets::Sock1000, addr);
			break;

		case 1001:
			//LOG_TRACE_NETWORK("SaveConnectionNatInfo() xnIp->NatAddrSocket1001 mapping port 1001 - port: {}, connection identifier: {:x}", htons(addr->sin_port), xnIp->connectionIdentifier.s_addr);
			xnIp->UpdateNat(H2v_sockets::Sock1001, addr);
			break;

		default:
			LOG_CRITICAL_NETWORK("{} - unkown network socket!", __FUNCTION__);
			break;
		} // switch (xsocket->getHostOrderSocketPort())
	}
	else
	{
		LOG_ERROR_NETWORK("{} - connection index: {} identifier: {:X} is invalid!", __FUNCTION__, GetConnectionIndex(connectionIdentifier), connectionIdentifier.s_addr);
	}
}

void CXnIp::HandleConnectionPacket(XSocket* xsocket, XnIp* xnIp, const XNetRequestPacket* reqPacket, const sockaddr_in* recvAddr, LPDWORD lpBytesRecvdCount)
{
	if (!xnIp->otherSideNonceKeyReceived)
	{
		// if the nonce id isn't updated
		// it means that locally the new connection was (most likely) registered by XNetXnAddrToInAddr API
		// and we are the connection initiators

		LOG_TRACE_NETWORK("{} - updated noOnce key for connection identifier: {:X}, n0ncekey: {}",
			__FUNCTION__,
			xnIp->connectionIdentifier.s_addr,
			ByteToHexStr(reqPacket->data.nonceKey, 8)
		);
		memcpy(xnIp->connectionNonceOtherSide, reqPacket->data.nonceKey, sizeof(XnIp::connectionNonceOtherSide));
		xnIp->otherSideNonceKeyReceived = true;
	}

	switch (reqPacket->data.reqType)
	{
	case XnIp_ConnectionUpdateNAT:
		if (xnIp->connectStatus <= XNET_CONNECT_STATUS_PENDING)
		{
			if (!xnIp->NatIsUpdated())
			{
				SaveNatInfo(xsocket, xnIp->connectionIdentifier, recvAddr);
				xnIp->connectStatus = XNET_CONNECT_STATUS_PENDING;

				// we need to fully update NAT before anything else
				if (xnIp->NatIsUpdated())
				{
					if (XNIP_TEST_BIT(reqPacket->data.flags, XnIp_HasEndpointNATData)) // don't send back if the other end has our NAT data already
					{
						SendXNetRequest(xsocket, xnIp->connectionIdentifier, XnIp_ConnectionEstablishSecure); // send EstablishSecure after we saved all NAT
					}
					else
					{
						SendXNetRequestAllSockets(xnIp->connectionIdentifier, XnIp_ConnectionUpdateNAT);
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
			if (xnIp->NatIsUpdated()) // check if the last connect packet was sent from the last socket to have NAT data saved
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
						SendXNetRequest(xsocket, xnIp->connectionIdentifier, XnIp_ConnectionDeclareConnected);
					}
					else
					{
						SendXNetRequest(xsocket, xnIp->connectionIdentifier, XnIp_ConnectionEstablishSecure);
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
		if (xnIp->NatIsUpdated())
		{
			switch (xnIp->connectStatus)
			{
			case XNET_CONNECT_STATUS_PENDING:
				LOG_TRACE_NETWORK("{} - connection identifier: {:X} successfuly connected", __FUNCTION__, xnIp->connectionIdentifier.s_addr);
				gXnIp.SendXNetRequest(xsocket, xnIp->connectionIdentifier, XnIp_ConnectionDeclareConnected);
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
	} // switch (reqPacket->data.reqType)
}

void CXnIp::HandleXNetRequestPacket(XSocket* xsocket, const XNetRequestPacket* reqPacket, const sockaddr_in* recvAddr, LPDWORD lpBytesRecvdCount)
{
	IN_ADDR connectionIdentifier;
	connectionIdentifier.s_addr = 0;

	int ret = CreateOrGetXnIpIdentifierFromPacket(&reqPacket->data.xnaddr, &reqPacket->data.xnkid, reqPacket, &connectionIdentifier);
	// if CreateXnIpIdentifierFromPacket is successful, we created another connection or we returned an already present one

	if (ret == 0)
	{
		// TODO: get rid of H2v only sockets

		if (connectionIdentifier.s_addr == INADDR_LOOPBACK)
			return;

		XnIp* xnIp = &m_XnIPs[GetConnectionIndex(connectionIdentifier)];
		switch (reqPacket->data.reqType)
		{
		case XnIp_ConnectionUpdateNAT:
		case XnIp_ConnectionEstablishSecure:
		case XnIp_ConnectionDeclareConnected:
		case XnIp_ConnectionCloseSecure:
			HandleConnectionPacket(xsocket, xnIp, reqPacket, recvAddr, lpBytesRecvdCount);
			break;

			// TODO:
		case XnIp_ConnectionPing:
		case XnIp_ConnectionPong:
			LOG_CRITICAL_NETWORK("{} - unimplemented request type: {}", __FUNCTION__, reqPacket->data.reqType);
			break;

		default:
			break;
		} // switch (reqPacket->data.reqType)

		// increase packets received count
		int bytesReceivedCount = 0;
		if (lpBytesRecvdCount)
			bytesReceivedCount = *lpBytesRecvdCount;

		UpdatePacketReceivedCounters(connectionIdentifier, bytesReceivedCount);
	}
	else
	{
		LOG_TRACE_NETWORK("{} - secure connection cannot be established!" __FUNCTION__);
		LogConnectionsErrorDetails(recvAddr, ret, &reqPacket->data.xnkid);
		// TODO: send back the connection cannot be established
	}
}

void CXnIp::HandleDisconnectPacket(XSocket* xsocket, const XNetRequestPacket* disconnectReqPck, const sockaddr_in* recvAddr)
{
	IN_ADDR ipIdentifier;
	if (GetEstablishedConnectionIdentifierByRecvAddr(xsocket, recvAddr, &ipIdentifier))
	{
		LOG_INFO_NETWORK("{} - cannot disconnect, connection unknown!", __FUNCTION__);
		return;
	}

	XnIp* xnIp = GetConnection(ipIdentifier);
	if (xnIp != nullptr)
	{
		// TODO: 
	}
}

XnIp* CXnIp::XnIpLookUp(const XNADDR* pxna, const XNKID* xnkid)
{
	// check if the user is already in the system, and get the first 
	for (int i = 0; i < GetMaxXnConnections(); i++)
	{
		// compare current connection index with what we got passed to us
		if (
			m_XnIPs[i].bValid
			&& !memcmp(m_XnIPs[i].xnaddr.abEnet, pxna->abEnet, sizeof(XNADDR::abEnet)) // check the MAC address
			// && !memcmp(m_XnIPs[i].xnaddr.abOnline, pxna->abOnline, sizeof(XNADDR::abOnline))
			&& !memcmp(m_XnIPs[i].keyPair->xnkid.ab, xnkid->ab, sizeof(XNKID::ab))
			)
		{
			// if it is already in the system, return
			return &m_XnIPs[i];
		}
	}

	return nullptr;
}

int CXnIp::RegisterNewXnIp(const XNADDR* pxna, const XNKID* pxnkid, IN_ADDR* outIpIdentifier)
{
	/*
		Update: 1/31/2020
		- Creates an identifier to be used by WinSock calls from a XNADDR address
		- It gets created when we received a XNet request connection packet, or when the game calls XNetXnAddrToInAddr
		- If XNetInAddrToXnAddr passes an invalid identifier, it will return WSAEINVAL
	*/

	XnKeyPair* keyPair = GetKeyPair(pxnkid);
	if (keyPair != nullptr)
	{
		for (int i = 0; i < GetMaxXnConnections(); i++)
		{
			if (!m_XnIPs[i].bValid)
			{
				XnIp* newXnIp = &m_XnIPs[i];
		ZeroMemory(newXnIp, sizeof(*newXnIp));

		memcpy(&newXnIp->xnaddr, pxna, sizeof(*pxna));
		newXnIp->keyPair = keyPair;

		XNetRandom(newXnIp->connectionNonce, sizeof(XnIp::connectionNonce));

				// if this is zero we are fucked
				ULONG randIdentifier = (rand() % 0xFF) + 1; // 0 to 254 + 1
				randIdentifier <<= CHAR_BIT;
				LOG_INFO_NETWORK("{} - new connection index {}, identifier {:X}", __FUNCTION__, i, htonl(i | randIdentifier));

				newXnIp->connectionIdentifier.s_addr = htonl(i | randIdentifier);
				newXnIp->lastConnectionInteractionTime = _Shell::QPCToTimeNowMsec();
				newXnIp->connectStatus = XNET_CONNECT_STATUS_IDLE;
				newXnIp->pckStats.PckDataSampleUpdate();
				newXnIp->bValid = true;

				if (outIpIdentifier)
					*outIpIdentifier = newXnIp->connectionIdentifier;

				return 0;
			}
		}

		// if we get this far, no more connection spots available
		return WSAENOMORE;
	}
	else
	{
		return WSAEINVAL;
	}
}

// TODO: add separate function that doesn't create XnIp identification from packet
// for now if reqPacket is null it means this is not called after a connection packet is received
// basically reqPacket = nullptr, XNetXnAddrToInAddr() context, if reqPacket != nullptr, HandleXNetRequestPacket() context
int CXnIp::CreateOrGetXnIpIdentifierFromPacket(const XNADDR* pxna, const XNKID* pxnkid, const XNetRequestPacket* reqPacket, IN_ADDR* outIpIdentifier)
{
	const XnIp* localConnectionInfo = GetLocalUserXn();
	if (!gXnIp.GetLocalUserXn()->bValid)
	{
		LOG_TRACE_NETWORK("{} - XNADDR information is not populated!", __FUNCTION__);
		return WSAEADDRNOTAVAIL;
	}

	if (!memcmp(localConnectionInfo->xnaddr.abEnet, pxna->abEnet, sizeof(XNADDR::abEnet)))
	{
		// some retarded games use the MAC address as a unique player identifier
		// but in 2020 we want to connect from the same PC multiple game instances, and this has become a unique account identifier
		// but even then, we cant allow xbox addresses with the same abEnet identifier
		LOG_CRITICAL_NETWORK("{} - the specified XNADDR is the same with the local one, aborting connection.", __FUNCTION__);
		LOG_CRITICAL_NETWORK("{} - local abEnet: {} == packet abEnet: {}",
			__FUNCTION__, ByteToHexStr(localConnectionInfo->xnaddr.abEnet, 6), ByteToHexStr(pxna->abEnet, 6));
		outIpIdentifier->s_addr = IPADDR_LOOPBACK;
		return 0;
	}

	// HACK: clear lost connections (if any) before creating another one
	ClearLostConnections();

	XnIp* registeredConnection = XnIpLookUp(pxna, pxnkid);

	bool fromPacket = reqPacket != nullptr;
	bool isAlreadyRegistered = registeredConnection != nullptr;

	/* register connection lambda */
	const auto registerConnection = [this](const XNADDR* pxna, const XNKID* pxnkid, IN_ADDR* outIpId, bool fromPacket, const XNetRequestPacket* reqPacket) -> int {
		int result = RegisterNewXnIp(pxna, pxnkid, outIpId);
		// first check if RegisterNewXnIp was successful
		if (result == 0)
		{
			XnIp* newlyCreatedXnIp = GetConnection(*outIpId);
			// if we received and created this identification after we received a packet, the other side tries to initiate a connection
			newlyCreatedXnIp->connectionInitiator = fromPacket ? true : false; // easier to deduce/read even if easier to write just fromPacket

			// if we got a request to create the new connection
			// update accordingly with the data received
			if (fromPacket)
			{
				memcpy(newlyCreatedXnIp->connectionNonceOtherSide, reqPacket->data.nonceKey, sizeof(XnIp::connectionNonceOtherSide));
				newlyCreatedXnIp->otherSideNonceKeyReceived = true;

			LOG_TRACE_NETWORK("{} - updated noOnce key for connection identifier: {:X}, n0nceKey: {}",
				__FUNCTION__,
					newlyCreatedXnIp->connectionIdentifier.s_addr,
					ByteToHexStr(newlyCreatedXnIp->connectionNonceOtherSide, 8).c_str()
				);
			}
		}

		return result;
	};

	if (!isAlreadyRegistered)
	{
		return registerConnection(pxna, pxnkid, outIpIdentifier, fromPacket, reqPacket);
	}
	else
	{
		// if this function is called after a packet is received and the request type is ConnectionEstablishSecure
		// and the connection Nonce/Id (random number created when registering a new connection), 
		// it means the other side tries to re-connect, we clear the old conenction slot, then register a new one
		// or if there are differences between XNADDRs (like the port or even the IP address) but the "MAC" address is the same, clear and create another connection

		// TODO FIXME: for a small subset of players pXnIpAlreadyRegistered->connectionNonceOtherSide is different from reqPacket->data.nonceKey when connection attempt is in progress
		// causing the connection to fail
		bool connectionIdMismatch = (reqPacket != nullptr
			&& registeredConnection->otherSideNonceKeyReceived
			&& memcmp(registeredConnection->connectionNonceOtherSide, reqPacket->data.nonceKey, sizeof(XnIp::connectionNonce)));

		if (connectionIdMismatch)
		{
			gXnIp.UnregisterXnIpIdentifier(registeredConnection->connectionIdentifier); // unregister the connection
			return registerConnection(pxna, pxnkid, outIpIdentifier, fromPacket, reqPacket);
		}

		// if we just received a request from a known connection
		// just output the identifier
		if (outIpIdentifier)
		{
			*outIpIdentifier = registeredConnection->connectionIdentifier;
			LOG_INFO_NETWORK("{} - already present connection identifier: {:X}", __FUNCTION__, registeredConnection->connectionIdentifier.s_addr);
		}

		return 0;
	}
}

void CXnIp::UnregisterXnIpIdentifier(const IN_ADDR ina)
{
	// TODO: let the other connection end know that the connection has to be closed
	XnIp* xnIp = GetConnection(ina);
	if (xnIp != nullptr)
	{
		const XnIpPckTransportStats* pckStats;
		xnIp->PckGetStats(&pckStats);
		LOG_INFO_NETWORK("{} - packets sent: {}, packets recv'd {}", __FUNCTION__,
			pckStats->pckSent,
			pckStats->pckRecvd);
		LOG_INFO_NETWORK("{} - Unregistered connection index: {}, identifier: {:X}", __FUNCTION__, GetConnectionIndex(ina), xnIp->connectionIdentifier.s_addr);
		SecureZeroMemory(xnIp, sizeof(*xnIp));
	}
}

int CXnIp::RegisterKey(XNKID* pxnkid, XNKEY* pxnkey)
{
	XnKeyPair* newKeyReg = nullptr;
	for (int i = 0; i < GetMaxXnKeyPairs(); i++)
	{
		if (!memcmp(m_XnKeyPairs[i].xnkid.ab, pxnkid->ab, sizeof(XNKID::ab)))
		{
			if (m_XnKeyPairs[i].bValid == true)
			{
				LOG_TRACE_NETWORK("{} - XnKeyPair: xnkid {}, xnkey: {} already registered!",
					__FUNCTION__,
					ByteToHexStr(pxnkid->ab, sizeof(pxnkid->ab)).c_str(),
					ByteToHexStr(pxnkey->ab, sizeof(pxnkey->ab)).c_str()
				);
				return WSAEALREADY;
			}
		}

		if (newKeyReg == nullptr
			&& m_XnKeyPairs[i].bValid == false)
		{
			newKeyReg = &m_XnKeyPairs[i];
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
		if (!memcmp(m_XnKeyPairs[i].xnkid.ab, pxnkid->ab, sizeof(XNKID::ab)))
		{
			// close any connections present on this key
			//for (int j = 0; j < GetMaxXnConnections(); j++)
			//{
			//	if (XnIPs[j].bValid
			//		&& XnIPs[j].keyPair == &XnKeyPairs[i])
			//		UnregisterXnIpIdentifier(XnIPs[j].connectionIdentifier);
			//}

			if (m_XnKeyPairs[i].bValid == true)
			{
				SecureZeroMemory(&m_XnKeyPairs[i], sizeof(m_XnKeyPairs[i]));
				return;
			}
		}
	}

	LOG_CRITICAL_NETWORK("{} - xnkid {} is unknown!", __FUNCTION__, ByteToHexStr(pxnkid->ab, sizeof(pxnkid->ab)).c_str());
}

XnKeyPair* CXnIp::GetKeyPair(const XNKID* pxnkid)
{
	for (int i = 0; i < GetMaxXnKeyPairs(); i++)
	{
		if (m_XnKeyPairs[i].bValid
			&& !memcmp(m_XnKeyPairs[i].xnkid.ab, pxnkid->ab, sizeof(XNKID::ab))
			)
		{
			return &m_XnKeyPairs[i];
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
		if (sockIt->IsUDP() // connect only UDP sockets
			&& H2v_socketsToConnect.find(sockIt->GetHostOrderSocketPort()) != H2v_socketsToConnect.end())
		{
			gXnIp.SendXNetRequest(sockIt, connectionIdentifier, reqType);
		}
	}
}

void CXnIp::SendXNetRequest(XSocket* xsocket, IN_ADDR connectionIdentifier, eXnip_ConnectRequestType reqType)
{
	sockaddr_in sendToAddr;
	ZeroMemory(&sendToAddr, sizeof(sockaddr_in));

	XnIp* xnIp = gXnIp.GetConnection(connectionIdentifier);
	if (xnIp != nullptr)
	{
		sendToAddr.sin_family = AF_INET;
		sendToAddr.sin_addr = connectionIdentifier;
		sendToAddr.sin_port = xsocket->GetNetworkOrderSocketPort();

		XNetRequestPacket reqPacket;

		reqPacket.data.reqType = reqType;
		memcpy(reqPacket.data.xnkid.ab, xnIp->keyPair->xnkid.ab, sizeof(XNKID::ab));
		memcpy(reqPacket.data.nonceKey, xnIp->connectionNonce, sizeof(XnIp::connectionNonce));
		XNetGetTitleXnAddr(&reqPacket.data.xnaddr);

		switch (reqType)
		{
		case XnIp_ConnectionUpdateNAT:
		case XnIp_ConnectionEstablishSecure:
			if (xnIp->NatIsUpdated())
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

		int ret = xsocket->UdpSend((char*)&reqPacket, sizeof(XNetRequestPacket), 0, (sockaddr*)&sendToAddr, sizeof(sendToAddr));
		LOG_INFO_NETWORK("{} - secure packet sent socket handle: {}, connection index: {}, connection identifier: {:x}, n0nceKey: {}",
			__FUNCTION__,
			xsocket->winSockHandle,
			gXnIp.GetConnectionIndex(connectionIdentifier),
			sendToAddr.sin_addr.s_addr,
			ByteToHexStr(reqPacket.data.nonceKey, 8).c_str()
		);
	}
	else
	{
		LOG_CRITICAL_NETWORK("{} - connection index: {}, identifier: {:x} is invalid!", __FUNCTION__, gXnIp.GetConnectionIndex(connectionIdentifier), connectionIdentifier.s_addr);
	}
}

void CXnIp::UnregisterLocalConnectionInfo()
{
	ZeroMemory(&m_ipLocal, sizeof(m_ipLocal));
}

XnIp* CXnIp::GetLocalUserXn()
{
	return &m_ipLocal;
}

void CXnIp::SetupLocalConnectionInfo(unsigned long xnaddr, unsigned long lanaddr, unsigned short baseport, const char* abEnet, const char* abOnline)
{
	SecureZeroMemory(&m_ipLocal, sizeof(m_ipLocal));

	m_ipLocal.xnaddr.ina = CNic::GetBestInterfaceRouteAddressFromIp("0.0.0.0"); // pass INADDR_ANY

	if (Ipv4AddressIsReservedOrLocalhost(m_ipLocal.xnaddr.ina))
	{
		LOG_TRACE_NETWORK("{} - IP address is private - {}", __FUNCTION__, inet_ntoa(m_ipLocal.xnaddr.ina));
	}
	else
	{
		// fall back to localhost if what GetBestIpToIpRoute found is not a private/local host ip address (maybe we are not under a NAT gateway)
		LOG_TRACE_NETWORK("{} - GetBestIpToIpRoute() returned public IP address, maybe we are not under a NAT device, falling back to localhost IP address!", __FUNCTION__);
		m_ipLocal.xnaddr.ina.s_addr = inet_addr("127.0.0.1");
	}

	// override what GetBestIpToIpRoute found if lanaddr is different than 0
	if (lanaddr != 0)
	{
		m_ipLocal.xnaddr.ina.s_addr = lanaddr; // check if the lanaddr is specified already
		LOG_TRACE_NETWORK("{} - lanaddr already specified: {}", __FUNCTION__, inet_ntoa(m_ipLocal.xnaddr.ina));
	}


	m_ipLocal.xnaddr.inaOnline.s_addr = xnaddr;

	m_ipLocal.xnaddr.wPortOnline = htons(baseport);
	HexStrToBytes(std::string(abEnet, sizeof(XNADDR::abEnet) * 2), m_ipLocal.xnaddr.abEnet, sizeof(XNADDR::abEnet));
	HexStrToBytes(std::string(abOnline, sizeof(XNADDR::abOnline) * 2), m_ipLocal.xnaddr.abOnline, sizeof(XNADDR::abOnline));
	m_ipLocal.pckStats.PckDataSampleUpdate();

	m_ipLocal.bValid = true;
}

void CXnIp::ClearLostConnections()
{
	int lostConnectionsCount = 0;
	for (int i = 0; i < GetMaxXnConnections(); i++)
	{
		XnIp* xnIp = &m_XnIPs[i];
		if (xnIp->bValid
			&& _Shell::QPCToTimeNowMsec() - xnIp->lastConnectionInteractionTime >= XnIp_ConnectionTimeOut)
		{
			lostConnectionsCount++;
			UnregisterXnIpIdentifier(xnIp->connectionIdentifier);
		}
	}
	if (lostConnectionsCount > 0)
		LOG_CRITICAL_NETWORK("{} - lost {} connections!", __FUNCTION__, lostConnectionsCount);
}

#pragma region XNet APIs

// #51: XNetStartup
int WINAPI XNetStartup(const XNetStartupParams* pxnsp)
{
	LOG_TRACE_NETWORK("XNetStartup()");
	gXnIp.Initialize(pxnsp);
	return 0;
}

// #52: XNetCleanup
INT WINAPI XNetCleanup()
{
	LOG_TRACE_NETWORK("XNetCleanup()");

	XSocket::SocketsDisposeAll();

	if (critical_network_errors_log != nullptr)
	{
		delete critical_network_errors_log;
		critical_network_errors_log = nullptr;
	}

	return 0;
}

// #53: XNetRandom
INT WINAPI XNetRandom(BYTE* pb, UINT cb)
{
	static bool Rc4CryptInitialized = false;

	if (Rc4CryptInitialized == false)
	{
		LARGE_INTEGER key;
		QueryPerformanceCounter(&key);
		XeCryptRc4Key(&gRc4StateRand, (BYTE*)&key, sizeof(key));
		Rc4CryptInitialized = true;
	}

	XeCryptRc4Ecb(&gRc4StateRand, pb, cb);
	return 0;
}

// #54: XNetCreateKey
INT WINAPI XNetCreateKey(XNKID* pxnkid, XNKEY* pxnkey)
{
	LOG_TRACE_NETWORK("XNetCreateKey()");
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
INT WINAPI XNetXnAddrToInAddr(const XNADDR* pxna, const XNKID* pxnkid, IN_ADDR* pina)
{
	if (pxna == nullptr
		|| pxnkid == nullptr
		|| pina == nullptr)
		return WSAEINVAL;

	int ret = gXnIp.CreateOrGetXnIpIdentifierFromPacket(pxna, pxnkid, nullptr, pina);

	if (ret != 0)
	{
		gXnIp.LogConnectionsErrorDetails(nullptr, ret, pxnkid);
	}

	LOG_TRACE_NETWORK("{} - local-address: {:X}, online-address: {:X}", __FUNCTION__, pxna->ina.s_addr, pxna->inaOnline.s_addr);
	return ret;
}

// #60: XNetInAddrToXnAddr
INT WINAPI XNetInAddrToXnAddr(const IN_ADDR ina, XNADDR* pxna, XNKID* pxnkid)
{
	LIMITED_LOG(15, LOG_TRACE_NETWORK, "{} - connection index: {}, identifier {:x}", __FUNCTION__, gXnIp.GetConnectionIndex(ina), ina.s_addr);

	if (pxna)
		ZeroMemory(pxna, sizeof(*pxna));

	if (pxnkid)
		ZeroMemory(pxnkid->ab, sizeof(pxnkid->ab));

	XnIp* xnIp = gXnIp.GetConnection(ina);
	if (xnIp == nullptr)
	{
		LOG_CRITICAL_NETWORK("{} - connection index: {}, identifier: {:X} are invalid!", __FUNCTION__, gXnIp.GetConnectionIndex(ina), ina.s_addr);
		return WSAEINVAL;
	}

	if (pxna)
		memcpy(pxna, &xnIp->xnaddr, sizeof(*pxna));

	if (pxnkid)
		memcpy(pxnkid->ab, xnIp->keyPair->xnkid.ab, sizeof(XNKID::ab));

	return 0;
}

// #63: XNetUnregisterInAddr
int WINAPI XNetUnregisterInAddr(const IN_ADDR ina)
{
	LOG_TRACE_NETWORK("{} - connection index {}, identifier: {:X}", __FUNCTION__, gXnIp.GetConnectionIndex(ina), ina.s_addr);
	gXnIp.UnregisterXnIpIdentifier(ina);
	return 0;
}

// #65: XNetConnect
int WINAPI XNetConnect(const IN_ADDR ina)
{
	LOG_TRACE_NETWORK("{} - connection index {}, identifier: {:X}", __FUNCTION__, gXnIp.GetConnectionIndex(ina), ina.s_addr);

	XnIp* xnIp = gXnIp.GetConnection(ina);
	if (xnIp == nullptr)
		return WSAEINVAL;

	// send connect packets only if the state is idle
	if (xnIp->connectStatus == XNET_CONNECT_STATUS_IDLE)
	{
		if (!xnIp->NatIsUpdated())
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
	LIMITED_LOG(15, LOG_TRACE_NETWORK, "{} : connection index {}, identifier: {:x}", __FUNCTION__, gXnIp.GetConnectionIndex(ina), ina.s_addr);

	XnIp* xnIp = gXnIp.GetConnection(ina);
	if (xnIp == nullptr)
	{
		LOG_CRITICAL_NETWORK("{} - connection index: {}, identifier: {:X} is invalid!", __FUNCTION__, gXnIp.GetConnectionIndex(ina), ina.s_addr);
		return XNET_CONNECT_STATUS_LOST;
	}

	if (xnIp->connectStatus == XNET_CONNECT_STATUS_CONNECTED)
	{
		gXnIp.SetTimeConnectionInteractionHappened(ina);
	}
	else if (xnIp->connectStatus < XNET_CONNECT_STATUS_CONNECTED
		&& _Shell::QPCToTimeNowMsec() - xnIp->lastConnectionInteractionTime >= XnIp_ConnectionTimeOut)
	{
		return XNET_CONNECT_STATUS_LOST;
	}

	return xnIp->connectStatus;
}

// #73: XNetGetTitleXnAddr
DWORD WINAPI XNetGetTitleXnAddr(XNADDR* pxna)
{
	LIMITED_LOG(15, LOG_TRACE_NETWORK, "{}", __FUNCTION__);
	ZeroMemory(pxna, sizeof(*pxna));

	XnIp* localUserXnIp = gXnIp.GetLocalUserXn();
	if (!localUserXnIp->bValid)
		return XNET_GET_XNADDR_PENDING;

	memcpy(pxna, &localUserXnIp->xnaddr, sizeof(*pxna));
	return XNET_GET_XNADDR_ETHERNET;
}


// #55: XNetRegisterKey
int WINAPI XNetRegisterKey(XNKID* pxnkid, XNKEY* pxnkey)
{
	LOG_TRACE_NETWORK("XNetRegisterKey()");
	return gXnIp.RegisterKey(pxnkid, pxnkey);
}

// #56: XNetUnregisterKey
int WINAPI XNetUnregisterKey(const XNKID* pxnkid)
{
	LOG_TRACE_NETWORK("XNetUnregisterKey()");
	gXnIp.UnregisterKey(pxnkid);

	return 0;
}

#pragma endregion