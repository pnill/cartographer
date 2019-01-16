#include <WinSock2.h>
#include "XLive\Networking\XLiveQoS.h"
#include "H2MOD\Modules\Config\Config.h"
#include <MSWSock.h>
#include <WS2tcpip.h>
#include <numeric>
#include "xnet.h"

using namespace std::chrono_literals;

H2MOD_QoS h2QoS;
extern XNetStartupParams g_XnetStartupParams;

void ClientQoSLookUp(UINT cxna, XNADDR *apxna[],UINT cProbes,IN_ADDR  aina[], XNQOS** pxnqos,DWORD dwBitsPerSec,XNQOS* pqos)
{
	//TRACE_GAME_NETWORK_N("ClientQoSLookup( cxna: %i, cProbes: %i, XNADDR array: %08X)", cxna,cProbes,apxna);

	while (pqos->cxnqosPending > 0)
	{
		if (cProbes > 0)
		{
			XNADDR *xn = apxna[ pqos->cxnqosPending - 1 ];
			XNQOSINFO *xnqosinfo = &pqos->axnqosinfo[ ( pqos->cxnqosPending - 1) ];
			memset(xnqosinfo, NULL, sizeof(XNQOSINFO));
			
			xnqosinfo->dwDnBitsPerSec = dwBitsPerSec;
			xnqosinfo->dwUpBitsPerSec = dwBitsPerSec;

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
			std::string prt = std::to_string(ntohs(xn->wPortOnline) + 10);

			//if (H2Config_debug_log)
			//	TRACE_GAME_NETWORK_N("[XNetQoSLookup] QoSLookup, addr=%s, port=%s", addr.c_str(), prt.c_str());

			// Resolve the server address and port
			iResult = getaddrinfo(addr.c_str(), prt.c_str(), &hints, &result);
			if (iResult != 0) {
				//if (H2Config_debug_log)
				//	TRACE_GAME_NETWORK_N("[XnetQoSLookup] getaddrinfo failed with error: %d\n", iResult);
				
				xnqosinfo->cProbesRecv = 0;
				xnqosinfo->cProbesXmit = 0;
				xnqosinfo->bFlags |= XNET_XNQOSINFO_TARGET_DISABLED;

				if(pqos->cxnqosPending > 0)
					pqos->cxnqosPending--;
				
				continue;
			}

			for (ptr = result; ptr != NULL; ptr = ptr->ai_next) {

				connectSocket = socket(ptr->ai_family, ptr->ai_socktype, ptr->ai_protocol);

				if (connectSocket == INVALID_SOCKET) {
					
					xnqosinfo->cProbesRecv = 0;
					xnqosinfo->cProbesXmit = 0;
					xnqosinfo->bFlags |= XNET_XNQOSINFO_TARGET_DISABLED;
					
					if(pqos->cxnqosPending > 0)
						pqos->cxnqosPending--;

					break;
				}

				/* We're only allowing 500ms for a response from server, or connection to server to be established. */
				int nTimeout = 500;
				setsockopt(connectSocket, SOL_SOCKET, SO_RCVTIMEO, (const char*)&nTimeout, sizeof(int));
				setsockopt(connectSocket, SOL_SOCKET, SO_SNDTIMEO, (const char*)&nTimeout, sizeof(int));

				// Connect to server.
				iResult = connect(connectSocket, ptr->ai_addr, (int)ptr->ai_addrlen);
				if (iResult == SOCKET_ERROR) {
					closesocket(connectSocket);
					connectSocket = INVALID_SOCKET;		
					xnqosinfo->cProbesRecv = 0;
					xnqosinfo->cProbesXmit = 0;
					xnqosinfo->bFlags |= XNET_XNQOSINFO_TARGET_DISABLED;

					if(pqos->cxnqosPending > 0)
						pqos->cxnqosPending--;
					
					continue;
				}
			}

			freeaddrinfo(result);
			
			if (connectSocket == INVALID_SOCKET)
			{
				//if (H2Config_debug_log)
				//	TRACE_GAME_NETWORK_N("[XNetQoSLookup] Unable to connect to server...");

				xnqosinfo->cProbesRecv = 0;
				xnqosinfo->cProbesXmit = 0;
				xnqosinfo->bFlags |= XNET_XNQOSINFO_TARGET_DISABLED;

				if(pqos->cxnqosPending > 0 )
					pqos->cxnqosPending--;

				continue;
			}

			int recvResult = 0;
			BYTE *recvBuf = new BYTE[g_XnetStartupParams.cfgQosDataLimitDiv4 * 4];
			int recvBufLen = g_XnetStartupParams.cfgQosDataLimitDiv4 * 4;
			memset(recvBuf, NULL, recvBufLen);

			std::vector<long long int> ping_storage;
			int probes = cProbes;
			do
			{
				//if (H2Config_debug_log)
				//	TRACE_GAME_NETWORK_N("[XNetQoSLookup][Client] Sending Probe %i..",probes);

				char send_data[3];
				ZeroMemory(send_data, 3);
				*(WORD*)&send_data = 0xAADD;

				auto started = std::chrono::high_resolution_clock::now();
				send(connectSocket, send_data, sizeof(send_data), 0);
				recvResult = recv(connectSocket, (char*)recvBuf, recvBufLen, 0);
				auto done = std::chrono::high_resolution_clock::now();

				//if(H2Config_debug_log)
				//	TRACE_GAME_NETWORK_N("[XNetQosLookup][Client] Probe got %i bytes of data back",RecvLen);

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

			if (recvResult <= 0)
			{
				//if (H2Config_debug_log)
				//	TRACE_GAME_NETWORK_N("[XNetQoSLookup][Socket: %08X] Disconnected!",connectSocket);

				xnqosinfo->cProbesRecv = 0;
				xnqosinfo->cProbesXmit = 0;
				xnqosinfo->bFlags |= XNET_XNQOSINFO_TARGET_DISABLED;

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
			xnqosinfo->bFlags |= (XNET_XNQOSINFO_TARGET_CONTACTED | XNET_XNQOSINFO_COMPLETE | XNET_XNQOSINFO_DATA_RECEIVED);

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

BOOL H2MOD_QoS::IsListening()
{
	return listenerThreadRunning;
}

LPFN_ACCEPTEX lpfnAcceptEx = nullptr;

void CALLBACK H2MOD_QoS::HandleClient(DWORD dwError, DWORD cbTransferred, LPWSAOVERLAPPED lpOverlapped, DWORD dwFlags)
{
	if (dwError)
		TRACE_GAME_NETWORK_N("[H2MOD-QoS] WSASend callback error %d", dwError);

	DWORD flags = 0;
	LPSOCKET_INFORMATION acceptSockInfo = reinterpret_cast<LPSOCKET_INFORMATION>(lpOverlapped);

	// delete memory allocated inside SendBack callback
	delete[] acceptSockInfo->DataBuf.buf;

	memset(&(acceptSockInfo->Overlapped), NULL, sizeof(WSAOVERLAPPED));
	acceptSockInfo->DataBuf.len = 3;
	acceptSockInfo->DataBuf.buf = acceptSockInfo->Buffer;

	if (WSARecv(acceptSockInfo->Socket, &(acceptSockInfo->DataBuf), 1, NULL, &flags, &(acceptSockInfo->Overlapped), h2QoS.SendBack) == SOCKET_ERROR)
	{
		if (WSAGetLastError() != WSA_IO_PENDING) {
			TRACE_GAME_NETWORK_N("[H2MOD-QoS] HandleClient callback-> WSARecv asynchronous I/O operation failed with error: %d", WSAGetLastError());
			closesocket(acceptSockInfo->Socket);
			delete acceptSockInfo;
		}
	}
}

void CALLBACK H2MOD_QoS::SendBack(DWORD dwError, DWORD cbTransferred, LPWSAOVERLAPPED lpOverlapped, DWORD dwFlags)
{					  
	LPSOCKET_INFORMATION acceptSockInfo = reinterpret_cast<LPSOCKET_INFORMATION>(lpOverlapped);

	acceptSockInfo->DataBuf.len = g_XnetStartupParams.cfgQosDataLimitDiv4 * 4;
	acceptSockInfo->DataBuf.buf = new char[acceptSockInfo->DataBuf.len];
	memset(acceptSockInfo->DataBuf.buf, NULL, acceptSockInfo->DataBuf.len);
	memcpy(acceptSockInfo->DataBuf.buf, h2QoS.pbData, acceptSockInfo->DataBuf.len);

	TRACE_GAME_NETWORK_N("[H2MOD-QoS] SendBack callback -> socket: %d", acceptSockInfo->Socket);

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

	TRACE_GAME_NETWORK_N("[H2MOD-QoS] SendBack callback -> magic received??? 0x%x", *(WORD*)&(acceptSockInfo->Buffer));

	int wsaError = 0;
	if (*(WORD*)&(acceptSockInfo->Buffer) == 0xAADD)
	{
		TRACE_GAME_NETWORK_N("[H2MOD-QoS] SendBack callback -> magic is right, sending data back on port %d", acceptSockInfo->Socket);

		DWORD flags = 0;
		ZeroMemory(&(acceptSockInfo->Overlapped), sizeof(WSAOVERLAPPED));
		int wsaError = WSASend(acceptSockInfo->Socket, &(acceptSockInfo->DataBuf), 1, NULL, flags, &(acceptSockInfo->Overlapped), h2QoS.HandleClient);

		TRACE_GAME_NETWORK_N("[H2MOD-QoS] SendBack callback -> WSASend error: %d\n", wsaError);

		if (wsaError == SOCKET_ERROR)
		{
			if (WSAGetLastError() != WSA_IO_PENDING)
			{
				TRACE_GAME_NETWORK_N("[H2MOD-QoS] SendBack callback -> WSASend() failed with error %d\n", WSAGetLastError());
				goto cleanup;
			}

			return;
		}

		else if (wsaError == ERROR_SUCCESS)
			return; // do not clear the data if operation is successful because it gets used by the callback too

		else
			goto cleanup;		
	}
	
cleanup:
	TRACE_GAME_NETWORK_N("[H2MOD-QoS] SendBack callback -> WSASend finished with error code %d (maybe client disconnected).", wsaError);
	delete[] acceptSockInfo->DataBuf.buf;
	closesocket(acceptSockInfo->Socket);
	delete acceptSockInfo;
}

void H2MOD_QoS::Listener()
{
	h2QoS.listenerThreadRunning = TRUE;

	WSAEVENT wsaEvent = WSACreateEvent();;
	SOCKET AcceptSocket = INVALID_SOCKET;
	char AcceptBuffer[2 * (sizeof(SOCKADDR_IN) + 16)];
	OVERLAPPED ListenOverlapped;
	LPSOCKET_INFORMATION lpSockInfo;

	while (true)
	{
		// check if the listen socket has been closed by XNetQoSListen
		if (listenSocket == INVALID_SOCKET)
			goto end;

		memset(&ListenOverlapped, NULL, sizeof(WSAOVERLAPPED));
		WSAResetEvent(wsaEvent);
		ListenOverlapped.hEvent = wsaEvent;

		if (lpfnAcceptEx == nullptr)
		{
			TRACE_GAME_NETWORK_N("[H2MOD-QoS] Pointer to AcceptEx function unavailable.");
			goto end;
		}

		if ((AcceptSocket = WSASocket(AF_INET, SOCK_STREAM, 0, NULL, 0, WSA_FLAG_OVERLAPPED)) == INVALID_SOCKET)
		{
			TRACE_GAME_NETWORK_N("[H2MOD-QoS] WSASocket failed with error: %d", WSAGetLastError());
			continue;
		}

		if (lpfnAcceptEx(listenSocket, AcceptSocket, (PVOID)AcceptBuffer, 0, sizeof(SOCKADDR_IN) + 16, sizeof(SOCKADDR_IN) + 16, NULL, &ListenOverlapped) == FALSE)
		{
			if (WSAGetLastError() != ERROR_IO_PENDING) {
				TRACE_GAME_NETWORK_N("[H2MOD-QoS] AcceptEx asynchronous I/O operation failed with error: %d", WSAGetLastError());
				closesocket(AcceptSocket);
				continue;
			}
		}

		// Wait for acceptex events
		while (true)
		{
			DWORD Index = WSAWaitForMultipleEvents(1, &wsaEvent, FALSE, WSA_INFINITE, TRUE);
			if (Index == WSA_WAIT_FAILED)
			{
				TRACE_GAME_NETWORK_N("WSAWaitForMultipleEvents() failed with error %d", WSAGetLastError());
				closesocket(AcceptSocket);
				continue;
			}

			if (Index != WAIT_IO_COMPLETION)
				break;
		}

		lpSockInfo = new SOCKET_INFORMATION;
		memset(lpSockInfo, NULL, sizeof(SOCKET_INFORMATION));
		lpSockInfo->Socket = AcceptSocket;
		memset(&(lpSockInfo->Overlapped), NULL, sizeof(WSAOVERLAPPED));
		lpSockInfo->BytesSEND = 0;
		lpSockInfo->BytesRECV = 0;
		lpSockInfo->DataBuf.len = 3; // size of buffer to be received
		lpSockInfo->DataBuf.buf = lpSockInfo->Buffer;

		DWORD flags = 0;

		TRACE_GAME_NETWORK_N("[H2MOD-QoS] WSARecv about to accept data on socket: %d", AcceptSocket);

		if (WSARecv(lpSockInfo->Socket, &(lpSockInfo->DataBuf), 1, NULL, &flags, &(lpSockInfo->Overlapped), h2QoS.SendBack) == SOCKET_ERROR)
		{
			if (WSAGetLastError() != WSA_IO_PENDING) 
			{
				TRACE_GAME_NETWORK_N("[H2MOD-QoS] WSARecv asynchronous I/O operation failed with error: %d", WSAGetLastError());
				closesocket(AcceptSocket);
				delete lpSockInfo;
			}
		}
	}


end:
	TRACE_GAME_NETWORK_N("[H2MOD-QoS] Listener thread about to terminate with error: %d", listenSocket);
	WSACloseEvent(wsaEvent);
	h2QoS.listenerThreadRunning = FALSE;
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
			if (cb > h2QoS.cbData)
				h2QoS.cbData = cb;

			ZeroMemory(h2QoS.pbData, cb);
			memcpy(h2QoS.pbData, pb, cb);
		}
	}

	if (dwFlags & XNET_QOS_LISTEN_ENABLE && h2QoS.IsListening() == FALSE)
	{
		h2QoS.listenSocket = WSASocket(AF_INET, SOCK_STREAM, 0, NULL, 0, WSA_FLAG_OVERLAPPED);
		if (h2QoS.listenSocket == INVALID_SOCKET)
		{
			TRACE_GAME_NETWORK_N("[H2MOD-QoS] Listener socket creation failed.");
			return ERROR_FUNCTION_FAILED;
		}

		SOCKADDR_IN serverInf;
		memset(&serverInf, NULL, sizeof(SOCKADDR_IN));
		serverInf.sin_family = AF_INET;
		serverInf.sin_addr.s_addr = INADDR_ANY; // anyone can connect (don't loopback to self)
		serverInf.sin_port = htons(H2Config_base_port + 10);

		if (bind(h2QoS.listenSocket, (SOCKADDR*)&serverInf, sizeof(serverInf)) == SOCKET_ERROR) 
		{
			TRACE_GAME_NETWORK_N("[H2MOD-QoS] Unable to bind listener socket!");
			return ERROR_FUNCTION_FAILED;
		}

		DWORD dwBytes;
		GUID GuidAcceptEx = WSAID_ACCEPTEX;

		DWORD wsaResult = WSAIoctl(h2QoS.listenSocket, SIO_GET_EXTENSION_FUNCTION_POINTER,
			&GuidAcceptEx, sizeof(GuidAcceptEx),
			&lpfnAcceptEx, sizeof(lpfnAcceptEx),
			&dwBytes, NULL, NULL);

		if (wsaResult == SOCKET_ERROR)
		{
			TRACE_GAME_NETWORK_N("[H2MOD-QoS] Failed to get AcceptEx function pointer! error: %d", WSAGetLastError());
			closesocket(h2QoS.listenSocket);
			return ERROR_FUNCTION_FAILED;
		}

		wsaResult = listen(h2QoS.listenSocket, SOMAXCONN); 
		if (wsaResult == SOCKET_ERROR)
		{
			TRACE_GAME_NETWORK_N("[H2MOD-QoS] listen failed! error: %d", WSAGetLastError());
			closesocket(h2QoS.listenSocket);
			return ERROR_FUNCTION_FAILED;
		}

		std::thread(&H2MOD_QoS::Listener, &h2QoS).detach();
	}

	if (dwFlags & XNET_QOS_LISTEN_DISABLE)
	{
		
	}

	if (dwFlags & XNET_QOS_LISTEN_RELEASE)
	{
		closesocket(h2QoS.listenSocket);
		h2QoS.listenSocket = INVALID_SOCKET;
		h2QoS.listenerThreadRunning = FALSE;
		
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
	ZeroMemory(pqos, sizeof(XNQOS) + (sizeof(XNQOSINFO)*(cxna - 1)));

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

	//return 0;
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
