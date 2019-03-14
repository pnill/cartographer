#include <WinSock2.h>
#include <MSWSock.h>
#include <WS2tcpip.h>
#include <numeric>
#include "xnet.h"
#include "XLive\Networking\XNetQoS.h"
#include "H2MOD\Modules\Config\Config.h"
#include "H2MOD\Modules\OnScreenDebug\OnscreenDebug.h"

using namespace std::chrono_literals;

CXNetQoS c_xnetqos;
extern XNetStartupParams g_XnetStartupParams;

void ClientQoSLookUp(UINT cxna, XNADDR *apxna[], UINT cProbes, IN_ADDR aina[], XNQOS** apxnqos, DWORD dwBitsPerSec, XNQOS* pqos)
{
	//TRACE_GAME_NETWORK_N("ClientQoSLookup( cxna: %i, cProbes: %i, XNADDR array: %08X)", cxna,cProbes,apxna);

	while (pqos->cxnqosPending > 0)
	{
		if (cProbes > 0)
		{
			XNADDR *xn = apxna[ pqos->cxnqosPending - 1 ];
			XNQOSINFO *xnqosinfo = &pqos->axnqosinfo[ ( pqos->cxnqosPending - 1) ];
		
			//if (H2Config_debug_log)
			//	TRACE_GAME_NETWORK_N("[XNetQoSLookup] Looping cxnas for probes pqos->xnqosPending: %i", pqos->cxnqosPending);

			SOCKET connectSocket = INVALID_SOCKET;
			struct addrinfo *result = NULL,	*ptr = NULL, hints;
			int iResult;

			ZeroMemory(&hints, sizeof(hints));
			hints.ai_family = AF_UNSPEC;
			hints.ai_socktype = SOCK_STREAM;
			hints.ai_protocol = IPPROTO_TCP;

			std::string addr = inet_ntoa(xn->ina);
			std::string port = std::to_string(ntohs(xn->wPortOnline) + 10);

			//if (H2Config_debug_log)
			//	TRACE_GAME_NETWORK_N("[XNetQoSLookup] QoSLookup, addr=%s, port=%s", addr.c_str(), prt.c_str());

			// Resolve the server address and port
			iResult = getaddrinfo(addr.c_str(), port.c_str(), &hints, &result);
			if (iResult != 0) {
				//if (H2Config_debug_log)
				//	TRACE_GAME_NETWORK_N("[XnetQoSLookup] getaddrinfo failed with error: %d\n", iResult);
				
				xnqosinfo->cProbesRecv = 0;
				xnqosinfo->cProbesXmit = 0;
				xnqosinfo->bFlags = XNET_XNQOSINFO_TARGET_DISABLED;

				if(pqos->cxnqosPending > 0)
					pqos->cxnqosPending--;
				
				continue;
			}

			for (ptr = result; ptr != NULL; ptr = ptr->ai_next) {

				connectSocket = socket(ptr->ai_family, ptr->ai_socktype, ptr->ai_protocol);
				if (connectSocket == INVALID_SOCKET) {
					continue;
				}

				/* We're only allowing 500ms for a response from server, or connection to server to be established. */
				DWORD nTimeout = 500;
				setsockopt(connectSocket, SOL_SOCKET, SO_RCVTIMEO, (const char*)&nTimeout, sizeof(DWORD));
				setsockopt(connectSocket, SOL_SOCKET, SO_SNDTIMEO, (const char*)&nTimeout, sizeof(DWORD));

				// Connect to server.
				iResult = connect(connectSocket, ptr->ai_addr, (int)ptr->ai_addrlen);
				if (iResult == SOCKET_ERROR) {
					closesocket(connectSocket);
					connectSocket = INVALID_SOCKET;		
					continue;
				}

				break; // if we get here, we must have connected successfully
			}

			freeaddrinfo(result);
			
			if (connectSocket == INVALID_SOCKET || iResult == SOCKET_ERROR)
			{
				//if (H2Config_debug_log)
				//	TRACE_GAME_NETWORK_N("[XNetQoSLookup] Unable to connect to server...");

				xnqosinfo->cProbesRecv = 0;
				xnqosinfo->cProbesXmit = 0;
				xnqosinfo->bFlags = XNET_XNQOSINFO_TARGET_DISABLED;

				if (pqos->cxnqosPending > 0 )
					pqos->cxnqosPending--;

				continue;
			}

			int recvResult = 0;
			int recvBufLen = g_XnetStartupParams.cfgQosDataLimitDiv4 * 4;
			BYTE *recvBuf = new BYTE[recvBufLen];
			memset(recvBuf, NULL, recvBufLen);

			std::vector<long long int> ping_storage;
			int probes = cProbes;
			do
			{
				//if (H2Config_debug_log)
				//	TRACE_GAME_NETWORK_N("[XNetQoSLookup][Client] Sending Probe %i..",probes);

				char send_data[4];
				ZeroMemory(send_data, 4);
				*(DWORD*)&send_data = 0xAABBCCDD;

				auto started = std::chrono::high_resolution_clock::now();
				send(connectSocket, send_data, sizeof(send_data), 0);
				recvResult = recv(connectSocket, (char*)recvBuf, recvBufLen, 0);
				auto done = std::chrono::high_resolution_clock::now();

				//if(H2Config_debug_log)
				//	TRACE_GAME_NETWORK_N("[XNetQosLookup][Client] Probe got %i bytes of data back", RecvLen);

				std::chrono::milliseconds d = std::chrono::duration_cast<std::chrono::milliseconds>(done - started);

				long long int diff = d.count();

				//if (H2Config_debug_log)
				//	TRACE_GAME_NETWORK_N("[XNetQoSLookup][Client] Probe %i finished ping: %lld", probes, diff);

				ping_storage.push_back(diff);

				probes--;
			} while (probes > 0);

			/*int WSAError = WSAGetLastError();
			if (RecvLen == -1 || WSAError == WSAETIMEDOUT || WSAError == WSAECONNRESET || WSAError == WSAESHUTDOWN || WSAError == WSAEINVAL || connectSocket == INVALID_SOCKET || connectSocket == SOCKET_ERROR)
			{
				closesocket(connectSocket);
				if (H2Config_debug_log)
					TRACE_GAME_NETWORK_N("[XNetQoSLookup][Socket: %08X] Winsock Error Occured: %i - Socket Closed", connectSocket, WSAError);

			}*/

			closesocket(connectSocket);

			if (recvResult <= 0 || recvResult != recvBufLen)
			{
				//if (H2Config_debug_log)
				//	TRACE_GAME_NETWORK_N("[XNetQoSLookup][Socket: %08X] Disconnected!",connectSocket);

				xnqosinfo->cProbesRecv = 0;
				xnqosinfo->cProbesXmit = 0;
				xnqosinfo->bFlags = XNET_XNQOSINFO_TARGET_DISABLED;

				if (pqos->cxnqosPending > 0)
					pqos->cxnqosPending--;

				continue;
			}

			auto ping_result = std::minmax_element(ping_storage.begin(),ping_storage.end());
			long long min_ping = ping_storage[ (ping_result.first- ping_storage.begin()) ];
			long long max_ping = ping_storage[ (ping_result.second - ping_storage.begin())];
			WORD average = (std::accumulate(ping_storage.begin(), ping_storage.end(), 0) / ping_storage.size());

			ping_storage.clear();

			//if (H2Config_debug_log)
			//	TRACE_GAME_NETWORK_N("[XNetQoSLookup][Client] Finished Probes max_ping: %i, min_ping: %i, average: %i", (WORD)max_ping, (WORD)min_ping,average);
				
			xnqosinfo->wRttMinInMsecs = (WORD)min_ping;
			xnqosinfo->wRttMedInMsecs = (WORD)average;
			xnqosinfo->cProbesRecv = 4;
			xnqosinfo->cProbesXmit = 4;
			xnqosinfo->pbData = recvBuf;
			xnqosinfo->cbData = recvBufLen;
			xnqosinfo->dwUpBitsPerSec = dwBitsPerSec;
			xnqosinfo->dwDnBitsPerSec = dwBitsPerSec;
			xnqosinfo->bFlags = (XNET_XNQOSINFO_TARGET_CONTACTED | XNET_XNQOSINFO_COMPLETE | XNET_XNQOSINFO_DATA_RECEIVED);

			if (pqos->cxnqosPending > 0)
				pqos->cxnqosPending--;
		}
	}

	for (int i = 0; i < cxna; i++)
	{
		// XNADDR
		delete apxna[i];
	}

	// XNADDR** 
	delete[] apxna;
}

BOOL CXNetQoS::IsListening()
{
	return m_listenerThreadRunning;
}

LPFN_ACCEPTEX lpfnAcceptEx = nullptr;

void CALLBACK CXNetQoS::HandleClient(DWORD dwError, DWORD cbTransferred, LPWSAOVERLAPPED lpOverlapped, DWORD dwFlags)
{
	if (dwError)
		TRACE_GAME_NETWORK_N("[H2MOD-QoS] HandleClient -> callback error %d", dwError);

	LPSOCKET_INFORMATION acceptSockInfo = reinterpret_cast<LPSOCKET_INFORMATION>(lpOverlapped);
	TRACE_GAME_NETWORK_N("[H2MOD-QoS] HandleClient callback -> socket: %d", acceptSockInfo->Socket);

	if (dwError != 0)
	{
		TRACE_GAME_NETWORK_N("[H2MOD-QoS] HandleClient callback -> I/O operation failed with error: %d", dwError);
	}

	if (cbTransferred == 0)
	{
		TRACE_GAME_NETWORK_N("[H2MOD-QoS] HandleClient callback -> no data received!", dwError);
	}
	
	// delete memory allocated inside SendBack callback
	delete[] acceptSockInfo->DataBuf.buf;

	memset(&(acceptSockInfo->Overlapped), NULL, sizeof(WSAOVERLAPPED));
	acceptSockInfo->DataBuf.len = 4;
	acceptSockInfo->DataBuf.buf = acceptSockInfo->Buffer;

	DWORD flags = 0;
	if (WSARecv(acceptSockInfo->Socket, &(acceptSockInfo->DataBuf), 1, NULL, &flags, &(acceptSockInfo->Overlapped), SendBack) == SOCKET_ERROR)
	{
		if (WSAGetLastError() != WSA_IO_PENDING) {
			TRACE_GAME_NETWORK_N("[H2MOD-QoS] HandleClient callback -> WSARecv asynchronous I/O operation failed with error: %d", WSAGetLastError());
			closesocket(acceptSockInfo->Socket);
			delete acceptSockInfo;
		}
	}
}

void CALLBACK CXNetQoS::SendBack(DWORD dwError, DWORD cbTransferred, LPWSAOVERLAPPED lpOverlapped, DWORD dwFlags)
{					  
	LPSOCKET_INFORMATION acceptSockInfo = reinterpret_cast<LPSOCKET_INFORMATION>(lpOverlapped);

	acceptSockInfo->DataBuf.len = g_XnetStartupParams.cfgQosDataLimitDiv4 * 4;
	acceptSockInfo->DataBuf.buf = new char[acceptSockInfo->DataBuf.len];

	memset(acceptSockInfo->DataBuf.buf, NULL, acceptSockInfo->DataBuf.len);
	memcpy(acceptSockInfo->DataBuf.buf, c_xnetqos.pbData, acceptSockInfo->DataBuf.len);

	//TRACE_GAME_NETWORK_N("[H2MOD-QoS] SendBack callback -> socket: %d", acceptSockInfo->Socket);

	if (dwError != 0)
	{
		TRACE_GAME_NETWORK_N("[H2MOD-QoS] SendBack callback -> I/O operation failed with error: %d", dwError);
		goto cleanup;
	}

	if (cbTransferred == 0)
	{
		TRACE_GAME_NETWORK_N("[H2MOD-QoS] SendBack callback -> no data received!", dwError);
		goto cleanup;
	}

	//TRACE_GAME_NETWORK_N("[H2MOD-QoS] SendBack callback -> magic received??? 0x%x", *(DWORD*)&(acceptSockInfo->Buffer));

	int wsaError = 0;
	if (*(DWORD*)&(acceptSockInfo->Buffer) == 0xAABBCCDD)
	{
		TRACE_GAME_NETWORK_N("[H2MOD-QoS] SendBack callback -> magic is right, sending data back on port %d", acceptSockInfo->Socket);

		DWORD flags = 0;
		ZeroMemory(&(acceptSockInfo->Overlapped), sizeof(WSAOVERLAPPED));
		wsaError = WSASend(acceptSockInfo->Socket, &(acceptSockInfo->DataBuf), 1, NULL, flags, &(acceptSockInfo->Overlapped), HandleClient);

		TRACE_GAME_NETWORK_N("[H2MOD-QoS] SendBack callback -> WSASend error: %d", wsaError);

		if (wsaError == SOCKET_ERROR)
		{
			if (WSAGetLastError() != WSA_IO_PENDING)
			{
				TRACE_GAME_NETWORK_N("[H2MOD-QoS] SendBack callback -> WSASend() failed with error %d", WSAGetLastError());
				goto cleanup;
			}

			return;
		}

		else if (wsaError == ERROR_SUCCESS)
			return; // do not clear the data if operation is successful because it gets used by the callback

		else
			goto cleanup;		
	}
	
cleanup:
	TRACE_GAME_NETWORK_N("[H2MOD-QoS] SendBack callback -> WSASend finished with error code %d (maybe client disconnected).", wsaError);
	closesocket(acceptSockInfo->Socket);
	delete[] acceptSockInfo->DataBuf.buf;
	delete acceptSockInfo;
}

void CXNetQoS::Listener()
{
	m_listenerThreadRunning = TRUE;

	SOCKADDR_IN serverInf;
	memset(&serverInf, NULL, sizeof(SOCKADDR_IN));
	serverInf.sin_family = AF_INET;
	serverInf.sin_addr.s_addr = INADDR_ANY; // anyone can connect (don't loopback to self)
	serverInf.sin_port = htons(H2Config_base_port + 10);

	m_ListenSocket = WSASocket(AF_INET, SOCK_STREAM, 0, NULL, 0, WSA_FLAG_OVERLAPPED);
	if (m_ListenSocket == INVALID_SOCKET)
	{
		m_ListenSocket = INVALID_SOCKET;
		TRACE_GAME_NETWORK_N("[H2MOD-QoS] Listener socket creation failed.");
	}

	if (bind(c_xnetqos.m_ListenSocket, (SOCKADDR*)&serverInf, sizeof(serverInf)) == SOCKET_ERROR)
	{
		closesocket(m_ListenSocket);
		m_ListenSocket = INVALID_SOCKET;
		TRACE_GAME_NETWORK_N("[H2MOD-QoS] Unable to bind listener socket!");
	}

	DWORD dwBytes;
	GUID GuidAcceptEx = WSAID_ACCEPTEX;

	DWORD result = WSAIoctl(c_xnetqos.m_ListenSocket, SIO_GET_EXTENSION_FUNCTION_POINTER,
		&GuidAcceptEx, sizeof(GuidAcceptEx),
		&lpfnAcceptEx, sizeof(lpfnAcceptEx),
		&dwBytes, NULL, NULL);

	if (result == SOCKET_ERROR)
	{
		TRACE_GAME_NETWORK_N("[H2MOD-QoS] Failed to get AcceptEx function pointer! error: %d", WSAGetLastError());
		closesocket(m_ListenSocket);
		m_ListenSocket = INVALID_SOCKET;
	}

	result = listen(m_ListenSocket, SOMAXCONN);
	if (result == SOCKET_ERROR)
	{
		TRACE_GAME_NETWORK_N("[H2MOD-QoS] Listen failed! error: %d", WSAGetLastError());
		closesocket(m_ListenSocket);
		m_ListenSocket = INVALID_SOCKET;
	}

	m_WsaEvent = WSACreateEvent();
	SOCKET acceptSocket = INVALID_SOCKET;
	char AcceptBuffer[2 * (sizeof(SOCKADDR_IN) + 16)];
	OVERLAPPED ListenOverlapped;
	LPSOCKET_INFORMATION lpSockInfo;

	while (true)
	{
		// check if the listen socket has been closed by XNetQoSListen
		if (m_ListenSocket == INVALID_SOCKET)
			goto end;

		memset(&ListenOverlapped, NULL, sizeof(WSAOVERLAPPED));
		WSAResetEvent(m_WsaEvent);
		ListenOverlapped.hEvent = m_WsaEvent;

		if (lpfnAcceptEx == nullptr)
		{
			TRACE_GAME_NETWORK_N("[H2MOD-QoS] Pointer to AcceptEx function unavailable.");
			goto end;
		}

		if ((acceptSocket = WSASocket(AF_INET, SOCK_STREAM, 0, NULL, 0, WSA_FLAG_OVERLAPPED)) == INVALID_SOCKET)
		{
			TRACE_GAME_NETWORK_N("[H2MOD-QoS] WSASocket failed with error: %d", WSAGetLastError());
			continue;
		}

		if (lpfnAcceptEx(m_ListenSocket, acceptSocket, (PVOID)AcceptBuffer, 0, sizeof(SOCKADDR_IN) + 16, sizeof(SOCKADDR_IN) + 16, NULL, &ListenOverlapped) == FALSE)
		{
			if (WSAGetLastError() != ERROR_IO_PENDING) {
				TRACE_GAME_NETWORK_N("[H2MOD-QoS] AcceptEx asynchronous I/O operation failed with error: %d", WSAGetLastError());
				closesocket(acceptSocket);
				continue;
			}
		}

		// Wait for acceptex events
		while (true)
		{
			DWORD Index = WSAWaitForMultipleEvents(1, &m_WsaEvent, FALSE, WSA_INFINITE, TRUE);

			if (m_bStopListening == true) {
				TRACE_GAME_NETWORK_N("[H2MOD-QoS] Listener thread signaled to terminate thread");
				closesocket(acceptSocket);
				closesocket(m_ListenSocket);
				goto end;
			}

			if (Index == WSA_WAIT_FAILED)
			{
				TRACE_GAME_NETWORK_N("WSAWaitForMultipleEvents() failed with error %d", WSAGetLastError());
				closesocket(acceptSocket);
				continue;
			}

			if (Index != WAIT_IO_COMPLETION) 
				break;
		}

		lpSockInfo = new SOCKET_INFORMATION;
		memset(lpSockInfo, NULL, sizeof(SOCKET_INFORMATION));
		lpSockInfo->Socket = acceptSocket;
		memset(&(lpSockInfo->Overlapped), NULL, sizeof(WSAOVERLAPPED));
		lpSockInfo->BytesSEND = 0;
		lpSockInfo->BytesRECV = 0;
		lpSockInfo->DataBuf.len = 4; // size of buffer to be received from clients
		lpSockInfo->DataBuf.buf = lpSockInfo->Buffer;

		DWORD flags = 0;

		TRACE_GAME_NETWORK_N("[H2MOD-QoS] WSARecv about to accept data on socket: %d", acceptSocket);

		if (WSARecv(lpSockInfo->Socket, &(lpSockInfo->DataBuf), 1, NULL, &flags, &(lpSockInfo->Overlapped), SendBack) == SOCKET_ERROR)
		{
			if (WSAGetLastError() != WSA_IO_PENDING) 
			{
				TRACE_GAME_NETWORK_N("[H2MOD-QoS] WSARecv asynchronous I/O operation failed with error: %d", WSAGetLastError());
				closesocket(acceptSocket);
				delete lpSockInfo;
			}
		}
	}


end:
	TRACE_GAME_NETWORK_N("[H2MOD-QoS] Listener thread about to terminate, last WSA error code: %i", WSAGetLastError());
	WSACloseEvent(m_WsaEvent);
	m_bStopListening = false;
	m_listenerThreadRunning = FALSE;
}



// #69: XNetQosListen
DWORD WINAPI XNetQosListen(XNKID *pxnkid, PBYTE pb, UINT cb, DWORD dwBitsPerSec, DWORD dwFlags)
{
	//TRACE_GAME_NETWORK_N("XNetQosListen  (pxnkid = %X, pb = %X, cb = %d, bitsPerSec = %d, flags = %X)",
	//	pxnkid, pb, cb, dwBitsPerSec, dwFlags);

	if (dwFlags & XNET_QOS_LISTEN_SET_DATA)
	{
		if (cb > 0)
		{
			if (cb > c_xnetqos.cbData)
				c_xnetqos.cbData = cb;

			ZeroMemory(c_xnetqos.pbData, cb);
			memcpy(c_xnetqos.pbData, pb, cb);
		}
	}

	if (dwFlags & XNET_QOS_LISTEN_ENABLE && c_xnetqos.IsListening() == FALSE)
	{
		std::thread(&CXNetQoS::Listener, &c_xnetqos).detach();
	}

	if (dwFlags & XNET_QOS_LISTEN_DISABLE)
	{
		
	}

	if (dwFlags & XNET_QOS_LISTEN_RELEASE)
	{
		c_xnetqos.m_bStopListening = true;
		memset(c_xnetqos.pbData, NULL, c_xnetqos.cbData);
		// signal the thread it has to unblock, and based on m_bStopListening it will decide wheter terminate the thread or not
		WSASetEvent(c_xnetqos.m_WsaEvent);
		
		TRACE_GAME_NETWORK_N("[H2MOD-QoS] Listener released!");
	}

	return S_OK;
}

// #70: XNetQosLookup
DWORD WINAPI XNetQosLookup(UINT cxna, XNADDR * apxna[], XNKID * apxnkid[], XNKEY * apxnkey[], UINT cina, IN_ADDR aina[], DWORD adwServiceId[], UINT cProbes, DWORD dwBitsPerSec, DWORD dwFlags, WSAEVENT hEvent, XNQOS** pxnqos)
{
	TRACE_GAME_NETWORK_N("XNetQosLookup ( cxna: %i, cina: %i, cProbes: %i, dwBitsPerSec: %i, hEvent: %X)",
		cxna, cina, cProbes, dwBitsPerSec, hEvent);
	TRACE_GAME_NETWORK_N("XNetQoSLookup( apxna: %X, apxnkid: %X, apxnkey: %X, aina: %X, adwServiceId: %X, pxnqos: %X)",
		apxna, apxnkid, apxnkey, aina, adwServiceId, pxnqos);
	//void ClientQoSLookUp(UINT cxna, XNADDR* apxna[],UINT cProbes,IN_ADDR  aina[], XNQOS** pxnqos,DWORD dwBitsPerSec)

	XNADDR** apxna_copy = new XNADDR*[cxna];
	for (DWORD i = 0; i < cxna; i++)
	{
		XNADDR* xn = apxna[i];
		apxna_copy[i] = new XNADDR;
		memcpy(apxna_copy[i], xn, sizeof(XNADDR));
	}

	*pxnqos = (XNQOS*)new char[sizeof(XNQOS) + (sizeof(XNQOSINFO) * (cxna - 1))];

	XNQOS* pqos = *pxnqos;
	ZeroMemory(pqos, sizeof(XNQOS) + (sizeof(XNQOSINFO) * (cxna - 1)));

	pqos->cxnqos = cxna;
	pqos->cxnqosPending = cxna;

	/*
	This is gonna hit some CPUs hard when there's a lot of servers on the list, we'll probably want to queue this a bit and only allow X number of threads to run at a time.
	We want to abuse the CPU where possible considering more modern systems will have decent CPUs so we'll be able to force things to happen faster but still want to keep compatibility with older setups.
	*/

	std::thread(ClientQoSLookUp, cxna, apxna_copy, cProbes, aina, pxnqos, dwBitsPerSec, pqos).detach();

	return ERROR_SUCCESS;
}

// #71: XNetQosServiceLookup
DWORD WINAPI XNetQosServiceLookup(DWORD a1, DWORD a2, DWORD a3)
{
	TRACE_GAME_NETWORK_N("XNetQosServiceLookup");

	// not connected to LIVE - abort now
	// - wants a3 return ASYNC struct
	return ERROR_INVALID_PARAMETER;
}

// #72: XNetQosRelease
INT WINAPI XNetQosRelease(XNQOS* pxnqos)
{
	for (int i = 0; i < pxnqos->cxnqos; i++)
	{
		if (pxnqos->axnqosinfo[i].cbData > 0 && pxnqos->axnqosinfo[i].pbData)
			delete[] pxnqos->axnqosinfo[i].pbData;

		//delete[] &pxnqos->axnqosinfo[i];
		//XNQOSINFO *xnqos = &pqos->axnqosinfo[ ( pqos->cxnqosPending - 1) ];
	}

	/* We need to clean-up all XNetQoSLookup data here, listener data should be cleaned up inside the Listen function Only. */
	TRACE_GAME_NETWORK_N("XNetQosRelease");
	return ERROR_SUCCESS;
}

// #77
// TODO: Check if halo2 uses this at all and implement it.
DWORD WINAPI XNetQosGetListenStats(DWORD a1, DWORD a2)
{
	TRACE_GAME_NETWORK_N("XNetQosGetListenStats");
	return ERROR_SUCCESS;
}
