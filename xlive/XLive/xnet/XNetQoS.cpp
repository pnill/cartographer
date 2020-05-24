#include "stdafx.h"
#include <MSWSock.h>
#include <WS2tcpip.h>
#include "xnet.h"
#include "XLive\xnet\XNetQoS.h"
#include "IpManagement\XnIp.h"
#include "H2MOD\Modules\Config\Config.h"

using namespace std::chrono_literals;

CXNetQoS XNetQoS;

void ClientQoSLookUp(UINT cxna, XNADDR *apxna[], UINT cProbes, IN_ADDR aina[], XNQOS** apxnqos, DWORD dwBitsPerSec, XNQOS* pqos)
{
	//LOG_TRACE_NETWORK_N("ClientQoSLookup( cxna: %i, cProbes: %i, XNADDR array: %08X)", cxna,cProbes,apxna);

	while (pqos->cxnqosPending > 0)
	{
		if (cProbes > 0)
		{
			XNADDR *xn = apxna[pqos->cxnqosPending - 1];
			XNQOSINFO *xnqosinfo = &pqos->axnqosinfo[(pqos->cxnqosPending - 1)];

			//	LOG_TRACE_NETWORK_N("[XNetQoSLookup] Looping cxnas for probes pqos->xnqosPending: {}", pqos->cxnqosPending);

			SOCKET connectSocket = INVALID_SOCKET;
			struct addrinfo *result = NULL, *ptr = NULL, hints;
			int iResult;

			SecureZeroMemory(&hints, sizeof(hints));
			hints.ai_family = AF_INET;
			hints.ai_socktype = SOCK_STREAM;
			hints.ai_protocol = IPPROTO_TCP;

			std::string addr = inet_ntoa(xn->ina);
			std::string port = std::to_string(ntohs(xn->wPortOnline) + 10);

			//	LOG_TRACE_NETWORK_N("[XNetQoSLookup] QoSLookup, addr={0}, port={1}", addr.c_str(), prt.c_str());

			// Resolve the server address and port
			iResult = getaddrinfo(addr.c_str(), port.c_str(), &hints, &result);
			if (iResult != 0) {
				//	LOG_TRACE_NETWORK_N("[XnetQoSLookup] getaddrinfo failed with error: {}\n", iResult);

				xnqosinfo->cProbesRecv = 0;
				xnqosinfo->cProbesXmit = 0;
				xnqosinfo->bFlags |= XNET_XNQOSINFO_TARGET_DISABLED;

				if (pqos->cxnqosPending > 0)
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
				//	LOG_TRACE_NETWORK_N("[XNetQoSLookup] Unable to connect to server...");

				xnqosinfo->cProbesRecv = 0;
				xnqosinfo->cProbesXmit = 0;
				xnqosinfo->bFlags |= XNET_XNQOSINFO_TARGET_DISABLED;

				if (pqos->cxnqosPending > 0)
					pqos->cxnqosPending--;

				continue;
			}

			int recvResult = 0;
			int recvBufLen = ipManager.GetReqQoSBufferSize();
			BYTE *recvBuf = new BYTE[recvBufLen];
			SecureZeroMemory(recvBuf, recvBufLen);

			std::vector<long long int> ping_storage;
			int probes = cProbes;
			do
			{
				//	LOG_TRACE_NETWORK_N("[XNetQoSLookup][Client] Sending Probe {}..",probes);

				char send_data[4];
				SecureZeroMemory(send_data, 4);
				*(DWORD*)&send_data = 0xAABBCCDD;

				auto started = std::chrono::high_resolution_clock::now();
				send(connectSocket, send_data, sizeof(send_data), 0);
				recvResult = recv(connectSocket, (char*)recvBuf, recvBufLen, 0);
				auto done = std::chrono::high_resolution_clock::now();

				//	LOG_TRACE_NETWORK_N("[XNetQosLookup][Client] Probe got {} bytes of data back", RecvLen);

				std::chrono::milliseconds d = std::chrono::duration_cast<std::chrono::milliseconds>(done - started);

				long long int diff = d.count();

				//	LOG_TRACE_NETWORK_N("[XNetQoSLookup][Client] Probe {} finished ping: %lld", probes, diff);

				ping_storage.push_back(diff);

				probes--;
			} while (probes > 0);

			/*int WSAError = WSAGetLastError();
			if (RecvLen == -1 || WSAError == WSAETIMEDOUT || WSAError == WSAECONNRESET || WSAError == WSAESHUTDOWN || WSAError == WSAEINVAL || connectSocket == INVALID_SOCKET || connectSocket == SOCKET_ERROR)
			{
				closesocket(connectSocket);
					LOG_TRACE_NETWORK_N("[XNetQoSLookup][Socket: %08X] Winsock Error Occured: %i - Socket Closed", connectSocket, WSAError);

			}*/

			closesocket(connectSocket);

			if (recvResult <= 0 || recvResult != recvBufLen)
			{
				//	LOG_TRACE_NETWORK_N("[XNetQoSLookup][Socket: %08X] Disconnected!",connectSocket);

				xnqosinfo->cProbesRecv = 0;
				xnqosinfo->cProbesXmit = 0;
				xnqosinfo->bFlags |= XNET_XNQOSINFO_TARGET_DISABLED;

				if (pqos->cxnqosPending > 0)
					pqos->cxnqosPending--;

				continue;
			}

			auto ping_result = std::minmax_element(ping_storage.begin(), ping_storage.end());
			long long min_ping = ping_storage[(ping_result.first - ping_storage.begin())];
			long long max_ping = ping_storage[(ping_result.second - ping_storage.begin())];
			unsigned int average = (std::accumulate(ping_storage.begin(), ping_storage.end(), 0) / ping_storage.size());

			ping_storage.clear();

			//	LOG_TRACE_NETWORK_N("[XNetQoSLookup][Client] Finished Probes max_ping: {0}, min_ping: {1}, average: {2}", (WORD)max_ping, (WORD)min_ping,average);

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

	for (UINT i = 0; i < cxna; i++)
	{
		// XNADDR
		delete apxna[i];
	}

	// XNADDR**
	delete[] apxna;
}

bool CXNetQoS::IsListening()
{
	return m_listenerThreadRunning;
}

LPFN_ACCEPTEX lpfnAcceptEx = nullptr;

void CALLBACK CXNetQoS::HandleClient(DWORD dwError, DWORD cbTransferred, LPWSAOVERLAPPED lpOverlapped, DWORD dwFlags)
{
	LPSOCKET_INFORMATION acceptSockInfo = reinterpret_cast<LPSOCKET_INFORMATION>(lpOverlapped);
	//LOG_TRACE_NETWORK_N("CXNetQoS::HandleClient() - socket: {}", acceptSockInfo->Socket);

	if (dwError != ERROR_SUCCESS)
	{
		LOG_TRACE_NETWORK("CXNetQoS::HandleClient() - dwError: {}", dwError);
		goto cleanup;
	}

	if (cbTransferred == 0)
	{
		LOG_TRACE_NETWORK("CXNetQoS::HandleClient() - no bytes sent!");
		goto cleanup;
	}

	// delete memory allocated inside SendBack callback
	delete[] acceptSockInfo->DataBuf.buf;

	acceptSockInfo->DataBuf.len = 4;
	acceptSockInfo->DataBuf.buf = new char[acceptSockInfo->DataBuf.len];
	SecureZeroMemory(&acceptSockInfo->Overlapped, sizeof(WSAOVERLAPPED));

	DWORD flags = 0;
	DWORD recvResult = WSARecv(acceptSockInfo->Socket, &acceptSockInfo->DataBuf, 1, NULL, &flags, &acceptSockInfo->Overlapped, SendBack);

	if (recvResult == ERROR_SUCCESS)
		return;

	else if (recvResult == SOCKET_ERROR)
	{
		if (WSAGetLastError() == WSA_IO_PENDING)
			return;

		LOG_TRACE_NETWORK("CXNetQoS::HandleClient() - WSARecv operation failed with error: {}", WSAGetLastError());
		goto cleanup;
	}

cleanup:
	closesocket(acceptSockInfo->Socket);
	delete[] acceptSockInfo->DataBuf.buf;
	delete acceptSockInfo;
}

void CALLBACK CXNetQoS::SendBack(DWORD dwError, DWORD cbTransferred, LPWSAOVERLAPPED lpOverlapped, DWORD dwFlags)
{
	LPSOCKET_INFORMATION acceptSockInfo = reinterpret_cast<LPSOCKET_INFORMATION>(lpOverlapped);

	//LOG_TRACE_NETWORK_N("[H2MOD-QoS] SendBack callback -> socket: %d", acceptSockInfo->Socket);

	if (dwError != ERROR_SUCCESS)
	{
		LOG_TRACE_NETWORK("CXNetQoS::SendBack() - dwError: {}, cleaning up", dwError);
		goto cleanup;
	}

	if (cbTransferred == 0)
	{
		LOG_TRACE_NETWORK("CXNetQoS::SendBack() - no data received!");
		goto cleanup;
	}

	if (*(DWORD*)(&acceptSockInfo->DataBuf.buf[0]) == 0xAABBCCDD)
	{
		//LOG_TRACE_NETWORK_N("CXNetQoS::SendBack() - magic is right, sending data back on socket {}", acceptSockInfo->Socket);

		delete[] acceptSockInfo->DataBuf.buf;

		// copy the data passed by the game
		acceptSockInfo->DataBuf.len = ipManager.GetReqQoSBufferSize();
		acceptSockInfo->DataBuf.buf = new CHAR[ipManager.GetReqQoSBufferSize()];
		memcpy(acceptSockInfo->DataBuf.buf, XNetQoS.pbData, ipManager.GetReqQoSBufferSize());

		DWORD flags = 0;
		SecureZeroMemory(&acceptSockInfo->Overlapped, sizeof(WSAOVERLAPPED));
		DWORD sendResult = WSASend(acceptSockInfo->Socket, &acceptSockInfo->DataBuf, 1, NULL, flags, &acceptSockInfo->Overlapped, HandleClient);

		//LOG_TRACE_NETWORK_N("CXNetQoS::SendBack() - WSASend returned: {}", wsaError);

		if (sendResult == ERROR_SUCCESS)
			return; // everything went fine, return and don't clear the allocated memory, because it's currently used by the callback

		else if (sendResult == SOCKET_ERROR)
		{
			if (WSAGetLastError() == WSA_IO_PENDING)
				return; // the operation will be executed asynchronously, don't clear memory because it will be used

			//LOG_TRACE_NETWORK("CXNetQoS::SendBack() - WSASend() failed with error {}", WSAGetLastError());
			goto cleanup;
		}
	}
	else
	{
		//LOG_TRACE_NETWORK("CXNetQoS::SendBack() - Unknown data received! (wtf)");
		goto cleanup;
	}

cleanup:
	closesocket(acceptSockInfo->Socket);
	delete[] acceptSockInfo->DataBuf.buf;
	delete acceptSockInfo;
}

void CXNetQoS::Listener()
{
	m_listenerThreadRunning = true;

	sockaddr_in serverAddr;
	SecureZeroMemory(&serverAddr, sizeof(sockaddr_in));

	serverAddr.sin_family = AF_INET;
	serverAddr.sin_addr.s_addr = INADDR_ANY; // anyone can ping
	serverAddr.sin_port = htons(H2Config_base_port + 10);

	// use scopes, to avoid: Compiler Warning (level 1) C4533, due to usage of goto
	{
		m_ListenSocket = WSASocket(AF_INET, SOCK_STREAM, 0, NULL, 0, WSA_FLAG_OVERLAPPED);
		if (m_ListenSocket == INVALID_SOCKET)
		{
			LOG_TRACE_NETWORK("CXNetQoS::Listener() - Listener socket creation failed.");
			goto listenerThreadCleanup;
		}
	}

	{
		int result = bind(m_ListenSocket, (sockaddr*)&serverAddr, sizeof(serverAddr));
		if (result == SOCKET_ERROR)
		{
			LOG_TRACE_NETWORK("CXNetQoS::Listener() - Unable to bind listener socket!");
			goto listenerThreadCleanup;
		}
	}

	{
		DWORD dwBytes = 0;
		GUID GuidAcceptEx = WSAID_ACCEPTEX;

		int result = WSAIoctl(m_ListenSocket, SIO_GET_EXTENSION_FUNCTION_POINTER,
			&GuidAcceptEx, sizeof(GuidAcceptEx),
			&lpfnAcceptEx, sizeof(lpfnAcceptEx),
			&dwBytes, NULL, NULL);

		if (result == SOCKET_ERROR)
		{
			LOG_TRACE_NETWORK("CXNetQoS::Listener() - failed to get AcceptEx function pointer, WSAIoctl error: {}", WSAGetLastError());
			goto listenerThreadCleanup;
		}
	}

	{
		int result = listen(m_ListenSocket, SOMAXCONN);
		if (result == SOCKET_ERROR)
		{
			LOG_TRACE_NETWORK("CXNetQoS::Listener() - listen error: {}", WSAGetLastError());
			goto listenerThreadCleanup;
		}
	}
	
	SOCKET acceptSocket = INVALID_SOCKET;
	char AcceptBuffer[2 * (sizeof(sockaddr_in) + 16)];

	WSAOVERLAPPED acceptOvelapped;
	LPSOCKET_INFORMATION lpSockInfo;

	while (true)
	{
		SecureZeroMemory(&acceptOvelapped, sizeof(WSAOVERLAPPED));

		if (m_bStopListening)
			goto listenerThreadCleanup;

		WSAResetEvent(m_WsaEvent);
		acceptOvelapped.hEvent = m_WsaEvent;
		acceptSocket = WSASocket(AF_INET, SOCK_STREAM, 0, NULL, 0, WSA_FLAG_OVERLAPPED);
		if (acceptSocket == INVALID_SOCKET)
		{
			LOG_TRACE_NETWORK("CXNetQoS::Listener() - WSASocket failed with error: {}", WSAGetLastError());
			continue;
		}

		SecureZeroMemory(AcceptBuffer, sizeof(AcceptBuffer));
		if (lpfnAcceptEx(m_ListenSocket, acceptSocket, (PVOID)AcceptBuffer, 0, sizeof(sockaddr_in) + 16, sizeof(sockaddr_in) + 16, NULL, &acceptOvelapped) == FALSE)
		{
			if (WSAGetLastError() != ERROR_IO_PENDING) {
				LOG_TRACE_NETWORK("CXNetQoS::Listener() - AcceptEx operation failed with error: {}", WSAGetLastError());
				closesocket(acceptSocket);
				continue;
			}
		}

		// Wait for AcceptEx events
		while (true)
		{
			DWORD ret = WSAWaitForMultipleEvents(1, &m_WsaEvent, FALSE, WSA_INFINITE, TRUE);

			if (m_bStopListening) {
				LOG_TRACE_NETWORK("CXNetQoS::Listener() - Listener thread signaled to terminate");
				closesocket(acceptSocket);
				goto listenerThreadCleanup;
			}

			if (ret == WSA_WAIT_FAILED)
			{
				//LOG_TRACE_NETWORK("WSAWaitForMultipleEvents() failed with error {}", WSAGetLastError());
				closesocket(acceptSocket);
				continue;
			}

			if (ret != WAIT_IO_COMPLETION)
				break;
		}

		lpSockInfo = new SOCKET_INFORMATION;
		SecureZeroMemory(lpSockInfo, sizeof(SOCKET_INFORMATION));

		lpSockInfo->BytesSEND = 0;
		lpSockInfo->BytesRECV = 0;
		lpSockInfo->Socket = acceptSocket;

		// buffer prep
		lpSockInfo->DataBuf.len = 4;
		lpSockInfo->DataBuf.buf = new char[lpSockInfo->DataBuf.len];

		DWORD flags = 0;
		//LOG_TRACE_NETWORK("CXNetQoS::Listener() - WSARecv about to receive data on socket: {}", acceptSocket);
		DWORD recvResult = WSARecv(lpSockInfo->Socket, &lpSockInfo->DataBuf, 1, NULL, &flags, &lpSockInfo->Overlapped, SendBack);
		if (recvResult == SOCKET_ERROR)
			continue;

		else if (recvResult == SOCKET_ERROR)
		{
			if (WSAGetLastError() == WSA_IO_PENDING)
				continue;

			//LOG_TRACE_NETWORK("CXNetQoS::Listener() - WSARecv operation failed with error: {}", WSAGetLastError());
			closesocket(acceptSocket);
			delete lpSockInfo;
		}
	}

listenerThreadCleanup:
	LOG_TRACE_NETWORK("CXNetQoS::Listener() - Listener thread about to terminate, last WSA error code: {}", WSAGetLastError());
	if (m_ListenSocket != INVALID_SOCKET)
	{
		closesocket(m_ListenSocket);
		m_ListenSocket = INVALID_SOCKET;
	}

	WSACloseEvent(m_WsaEvent);
	m_WsaEvent = WSA_INVALID_EVENT;
	
	m_bStopListening = false;
	m_listenerThreadRunning = false;
	return;
}

// #69: XNetQosListen
DWORD WINAPI XNetQosListen(XNKID *pxnkid, PBYTE pb, UINT cb, DWORD dwBitsPerSec, DWORD dwFlags)
{
	//LOG_TRACE_NETWORK_N("XNetQosListen  (pxnkid = %X, pb = %X, cb = %d, bitsPerSec = %d, flags = %X)",
	//	pxnkid, pb, cb, dwBitsPerSec, dwFlags);

	if (XNetQoS.pbData == nullptr)
	{
		XNetQoS.pbData = new BYTE[ipManager.GetReqQoSBufferSize()];
		SecureZeroMemory(XNetQoS.pbData, ipManager.GetReqQoSBufferSize());
	}

	if (dwFlags & XNET_QOS_LISTEN_SET_DATA)
	{
		if (cb > 0)
		{
			if (cb > XNetQoS.cbData)
				XNetQoS.cbData = cb;

			SecureZeroMemory(XNetQoS.pbData, ipManager.GetReqQoSBufferSize());
			memcpy(XNetQoS.pbData, pb, cb);
		}
	}

	if ((dwFlags & XNET_QOS_LISTEN_ENABLE) && XNetQoS.IsListening() == false)
	{
		XNetQoS.m_WsaEvent = WSACreateEvent();
		
		if (XNetQoS.m_WsaEvent != WSA_INVALID_EVENT)
			std::thread(&CXNetQoS::Listener, &XNetQoS).detach();
	}

	if (dwFlags & XNET_QOS_LISTEN_DISABLE)
	{

	}

	if ((dwFlags & XNET_QOS_LISTEN_RELEASE) && XNetQoS.IsListening() != false)
	{
		if (XNetQoS.m_WsaEvent != WSA_INVALID_EVENT)
		{
			XNetQoS.m_bStopListening = true;
			// signal the thread it has to unblock, and based on m_bStopListening it will decide wheter terminate the thread or not
			WSASetEvent(XNetQoS.m_WsaEvent);
			LOG_TRACE_NETWORK("XNetQosListen() - Listener released!");
		}
	}

	return 0;
}

// #70: XNetQosLookup
DWORD WINAPI XNetQosLookup(UINT cxna, XNADDR * apxna[], XNKID * apxnkid[], XNKEY * apxnkey[], UINT cina, IN_ADDR aina[], DWORD adwServiceId[], UINT cProbes, DWORD dwBitsPerSec, DWORD dwFlags, WSAEVENT hEvent, XNQOS** pxnqos)
{
	LOG_TRACE_NETWORK("XNetQosLookup ( cxna: {0}, cina: {1}, cProbes: {2}, dwBitsPerSec: {3}, hEvent: {3:p})",
		cxna, cina, cProbes, dwBitsPerSec, (void*)hEvent);
	LOG_TRACE_NETWORK("XNetQoSLookup( apxna: {0:p}, apxnkid: {1:p}, apxnkey: {2:p}, aina: {3:p}, adwServiceId: {4:p}, pxnqos: {5:p})",
		(void*)apxna, (void*)apxnkid, (void*)apxnkey, (void*)aina, (void*)adwServiceId, (void*)pxnqos);
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
	SecureZeroMemory(pqos, sizeof(XNQOS) + (sizeof(XNQOSINFO) * (cxna - 1)));

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
	LOG_TRACE_NETWORK("XNetQosServiceLookup()");

	// not connected to LIVE - abort now
	// - wants a3 return ASYNC struct
	return ERROR_INVALID_PARAMETER;
}

// #72: XNetQosRelease
INT WINAPI XNetQosRelease(XNQOS* pxnqos)
{
	for (unsigned int i = 0; i < pxnqos->cxnqos; i++)
	{
		// this may crash other games, doesn't for H2PC
		if (pxnqos->axnqosinfo[i].cbData > 0 && pxnqos->axnqosinfo[i].pbData)
			delete[] pxnqos->axnqosinfo[i].pbData;
	}

	// TODO: find a way to stop the tread writing to this memory block
	// delete[] pxnqos; 

	/* We need to clean-up all XNetQoSLookup data here, listener data should be cleaned up inside the Listen function Only. */
	LOG_TRACE_NETWORK("XNetQosRelease()");
	return ERROR_SUCCESS;
}

// TODO: Check if halo2 uses this at all and implement it.
// #77
DWORD WINAPI XNetQosGetListenStats(DWORD a1, DWORD a2)
{
	LOG_TRACE_NETWORK("XNetQosGetListenStats()");
	return ERROR_SUCCESS;
}
