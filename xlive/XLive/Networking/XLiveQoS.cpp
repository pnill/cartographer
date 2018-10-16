#include <WinSock2.h>
#include <windows.h>
#include "XLive\Networking\XLiveQoS.h"
#include "H2MOD\Modules\Config\Config.h"
#include "xliveless.h"
#include "xlivedefs.h"
#include <WS2tcpip.h>
#include "Globals.h"
#include <thread>
#include <chrono>
#include <functional>
#include <numeric>

using namespace std::chrono_literals;

H2MOD_QoS h2QoS;

void StartQoSThread()
{
		h2QoS.startListening();
}

void StartListenerThread(PBYTE pbData, UINT cbData)
{
	if (cbData > 0)
	{

		/* We're racing... clean this up (eventually) - PermaNull*/
		if (h2QoS.cbData > 0 && h2QoS.pbData != NULL)
		{
			h2QoS.cbData = 0;
			delete[] h2QoS.pbData;
		}

		PBYTE pb = new BYTE[cbData];
		ZeroMemory(pb, cbData);
		memcpy(pb, pbData, cbData);

		h2QoS.pbData = pb;
		h2QoS.cbData = cbData;
	}

	if(!h2QoS.IsListening())
		std::thread(StartQoSThread).detach();

	
}


//DWORD WINAPI XNetQosLookup (UINT cxna, XNADDR * apxna[], XNKID * apxnkid[],XNKEY * apxnkey[], UINT cina, IN_ADDR aina[], DWORD adwServiceId[], UINT cProbes, DWORD dwBitsPerSec, DWORD dwFlags, WSAEVENT hEvent, XNQOS** pxnqos)
void ClientQoSLookUp(UINT cxna, XNADDR *apxna[],UINT cProbes,IN_ADDR  aina[], XNQOS** pxnqos,DWORD dwBitsPerSec,XNQOS* pqos)
{

	TRACE_GAME_N("ClientQoSLookup( cxna: %i, cProbes: %i, XNADDR array: %08X)", cxna,cProbes,apxna);
	//*pxnqos = new XNQOS;

	/**pxnqos = (XNQOS*)malloc(sizeof(XNQOS) + (sizeof(XNQOSINFO) * (cxna)));

	XNQOS* pqos = &**pxnqos;
	ZeroMemory(pqos, sizeof(XNQOS) + (sizeof(XNQOSINFO)*(cxna)));

	pqos->cxnqosPending = cxna;
	pqos->cxnqos = cxna;
		*/	
	while (pqos->cxnqosPending)
	{


		if (cProbes > 0)
		{

			if (pqos->cxnqosPending == 0)
				break;

			XNADDR *xn = apxna[pqos->cxnqosPending-1];
			XNQOSINFO *xnqos = &pqos->axnqosinfo[ ( pqos->cxnqosPending - 1) ];
			

			if (H2Config_debug_log)
				TRACE_GAME_N("[XNetQoSLookup] Looping cxnas for probes pqos->xnqosPending: %i", pqos->cxnqosPending);

			SOCKET connectSocket = INVALID_SOCKET;
			WSADATA wsaData;
			struct addrinfo *result = NULL,	*ptr = NULL, hints;
			int iResult;
			int recvbuflen = 255;

			// Initialize Winsock
			iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
			if (iResult != 0) {
				if (H2Config_debug_log)
					TRACE_GAME_N("WSAStartup failed with error: %d\n", iResult);
			}

			ZeroMemory(&hints, sizeof(hints));
			hints.ai_family = AF_UNSPEC;
			hints.ai_socktype = SOCK_STREAM;
			hints.ai_protocol = IPPROTO_TCP;
			std::string addr = inet_ntoa(xn->ina);
			std::string prt = std::to_string(ntohs(xn->wPortOnline) + 10);

			if (H2Config_debug_log)
				TRACE_GAME_N("[XNetQoSLookup] QoSLookup, addr=%s, port=%s", addr.c_str(), prt.c_str());

			// Resolve the server address and port
			iResult = getaddrinfo(addr.c_str(), prt.c_str(), &hints, &result);
			if (iResult != 0) {
				if (H2Config_debug_log)
					TRACE_GAME_N("[XnetQoSLookup] getaddrinfo failed with error: %d\n", iResult);
				//WSACleanup();
				
				xnqos->cProbesRecv = 0;
				xnqos->wRttMedInMsecs = 1000;
				xnqos->wRttMinInMsecs = 1000;
				xnqos->dwUpBitsPerSec = 0;
				xnqos->dwDnBitsPerSec = 0;
				xnqos->bFlags = XNET_XNQOSINFO_TARGET_DISABLED;

				if(pqos->cxnqosPending > 0)
					pqos->cxnqosPending--;
				
				break;
				continue;
			}

			for (ptr = result; ptr != NULL; ptr = ptr->ai_next) {


				connectSocket = socket(ptr->ai_family, ptr->ai_socktype,
					ptr->ai_protocol);


				if (connectSocket == INVALID_SOCKET) {
					xnqos->wRttMinInMsecs = 2000;
					xnqos->wRttMedInMsecs = 2000;
					xnqos->bReserved = 0;
					xnqos->dwUpBitsPerSec = 0;
					xnqos->dwDnBitsPerSec = 0;
					xnqos->bFlags = XNET_XNQOSINFO_TARGET_DISABLED;
					
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
					xnqos->wRttMinInMsecs = 3000;
					xnqos->wRttMedInMsecs = 3000;
					xnqos->bReserved = 0;
					xnqos->dwUpBitsPerSec = 0;
					xnqos->dwDnBitsPerSec = 0;
					xnqos->bFlags = XNET_XNQOSINFO_TARGET_DISABLED;

					if(pqos->cxnqosPending > 0)
						pqos->cxnqosPending--;
					
					continue;
				}
			}

			freeaddrinfo(result);
			
			if (connectSocket == INVALID_SOCKET)
			{
				if (H2Config_debug_log)
					TRACE_GAME_N("[XNetQoSLookup] Unable to connect to server...");

				xnqos->wRttMinInMsecs = 4000;
				xnqos->wRttMedInMsecs = 4000;
				xnqos->bReserved = 0;
				xnqos->dwUpBitsPerSec = 0;
				xnqos->dwDnBitsPerSec = 0;
				xnqos->bFlags = XNET_XNQOSINFO_TARGET_DISABLED;

					if(pqos->cxnqosPending > 0 )
						pqos->cxnqosPending--;

				continue;
			}

			int RecvLen = 0;
			char recvbuf[500];

		

			std::vector<long long int> ping_storage;
			int probes = cProbes;
			do
			{
				if (H2Config_debug_log)
					TRACE_GAME_N("[XNetQoSLookup][Client] Sending Probe %i..",probes);

				auto started = std::chrono::high_resolution_clock::now();

				char send_data[3];
				ZeroMemory(send_data, 3);
				*(WORD*)&send_data = 0xAADD;

				send(connectSocket, send_data, 3, 0);

				RecvLen = recv(connectSocket, recvbuf, recvbuflen, 0);

				if(H2Config_debug_log)
					TRACE_GAME_N("[XNetQosLookup][Client] Probe got %i bytes of data back",RecvLen);

				auto done = std::chrono::high_resolution_clock::now();

				std::chrono::milliseconds d = std::chrono::duration_cast<std::chrono::milliseconds>(done - started);

				long long int diff = d.count();

				if (H2Config_debug_log)
					TRACE_GAME_N("[XNetQoSLookup][Client] Probe %i finished ping: %lld", probes, diff);

				ping_storage.push_back(diff);

				probes--;
			} while (probes > 0);


			if (RecvLen <= 0) 
			{
				closesocket(connectSocket);
				if (H2Config_debug_log)
					TRACE_GAME_N("[XNetQoSLookup][Socket: %08X] Disconnected!",connectSocket);


				xnqos->wRttMinInMsecs = 4000;
				xnqos->wRttMedInMsecs = 4000;
				xnqos->bReserved = 0;
				xnqos->dwUpBitsPerSec = 0;
				xnqos->dwDnBitsPerSec = 0;
				xnqos->bFlags = XNET_XNQOSINFO_TARGET_DISABLED;

				pqos->cxnqosPending--;
				continue;
				
			}

			/*int WSAError = WSAGetLastError();
			if (RecvLen == -1 || WSAError == WSAETIMEDOUT || WSAError == WSAECONNRESET || WSAError == WSAESHUTDOWN || WSAError == WSAEINVAL || connectSocket == INVALID_SOCKET || connectSocket == SOCKET_ERROR)
			{
				closesocket(connectSocket);
				if (H2Config_debug_log)
					TRACE_GAME_N("[XNetQoSLookup][Socket: %08X] Winsock Error Occured: %i - Socket Closed", connectSocket, WSAError);

			}*/



			/* This gets cleaned up in XNetQosRelease() - XNQOS struct is passed there */
			PBYTE pb = new BYTE[RecvLen];
			ZeroMemory(pb, RecvLen);
			memcpy(pb, recvbuf, RecvLen);

			auto ping_result = std::minmax_element(ping_storage.begin(),ping_storage.end());
			long long min_ping = ping_storage[ (ping_result.first- ping_storage.begin())];
			long long max_ping = ping_storage[ (ping_result.second - ping_storage.begin()) ];
			WORD average = (std::accumulate(ping_storage.begin(), ping_storage.end(), 0) / ping_storage.size());

			ping_storage.clear();

			if (H2Config_debug_log)
				TRACE_GAME_N("[XNetQoSLookup][Client] Finished Probes max_ping: %i, min_ping: %i, average: %i", (WORD)max_ping, (WORD)min_ping,average);
				
			
			xnqos->wRttMinInMsecs = (WORD)min_ping;
			xnqos->wRttMedInMsecs = (WORD)average;
			xnqos->cProbesRecv = 4;
			xnqos->cProbesXmit = 4;
			xnqos->pbData = pb;
			xnqos->cbData = RecvLen;
			xnqos->bReserved = 0;
			xnqos->dwUpBitsPerSec = dwBitsPerSec;
			xnqos->dwDnBitsPerSec = dwBitsPerSec;
			xnqos->bFlags = XNET_XNQOSINFO_TARGET_CONTACTED | XNET_XNQOSINFO_COMPLETE | XNET_XNQOSINFO_DATA_RECEIVED;

			closesocket(connectSocket);

			if (pqos->cxnqosPending > 0)
			{
				if(H2Config_debug_log)
					TRACE_GAME_N("[XNetQoSLookup] Finished Probing: %s, %i hosts left to probe.", addr.c_str(), pqos->cxnqosPending);
				
				pqos->cxnqosPending--;
			}

		}



	}

	
	//pqos->cxnqosPending = cProbes;

	/**pxnqos = new XNQOS;
	XNQOS* pqos = *pxnqos;
	pqos->cxnqos = 1;
	pqos->cxnqosPending = 0;
	memset(pqos->axnqosinfo, 0x00, sizeof(XNQOSINFO));
	pqos->axnqosinfo->bReserved = 0;
	pqos->axnqosinfo->cProbesXmit = cProbes;
	pqos->axnqosinfo->cProbesRecv = cProbes;

	if (cProbes > 0)
	{
		pqos->axnqosinfo->wRttMedInMsecs = 5;
		pqos->axnqosinfo->wRttMinInMsecs = 10;
		pqos->axnqosinfo->dwUpBitsPerSec = dwBitsPerSec;
		pqos->axnqosinfo->dwDnBitsPerSec = dwBitsPerSec;
		pqos->axnqosinfo->bFlags = XNET_XNQOSINFO_TARGET_CONTACTED | XNET_XNQOSINFO_COMPLETE | XNET_XNQOSINFO_DATA_RECEIVED;
	}
	else
	{
		pqos->axnqosinfo->wRttMedInMsecs = 5;
		pqos->axnqosinfo->wRttMinInMsecs = 10;
		pqos->axnqosinfo->dwUpBitsPerSec = dwBitsPerSec;
		pqos->axnqosinfo->dwDnBitsPerSec = dwBitsPerSec;
		pqos->axnqosinfo->bFlags = XNET_XNQOSINFO_TARGET_CONTACTED | XNET_XNQOSINFO_COMPLETE;
	}*/

		

}

void handleClientConnection(SOCKET socket) {
	char recvbuf[2];
	int iResult = 1;

	if (H2Config_debug_log)
		TRACE_GAME_N("[XNetQoSLookup] handleClientConnection(Socket: %08X)",socket);
	
	/* We're only allowing 500ms of wait-time between packets, if the latency is higher than this the game would be unplayable either way.*/
	int nTimeout = 500; 
	setsockopt(socket, SOL_SOCKET, SO_RCVTIMEO, (const char*)&nTimeout, sizeof(int));
	setsockopt(socket, SOL_SOCKET, SO_SNDTIMEO, (const char*)&nTimeout, sizeof(int));


	do {

		iResult = recv(socket, recvbuf, 2, 0);
		
		/*int WSAError = WSAGetLastError();
		if (WSAError == WSAETIMEDOUT || WSAError == WSAECONNRESET || WSAError == WSAESHUTDOWN || WSAError == WSAEINVAL || iResult == SOCKET_ERROR)
		{
			closesocket(socket);
			if (H2Config_debug_log)
				TRACE_GAME_N("[XNetQoSLookup][Socket: %08X] Winsock Error Occured: %i", socket,WSAError);
			break;
		}*/

		if (iResult <= 0) { 
			if (H2Config_debug_log)
			TRACE_GAME_N("[XnetQoSLookup][Socket: %08X] Disconnected.",socket);
			closesocket(socket);
			break; 
		}

		if (*(WORD*)recvbuf == 0xAADD)
		{
			if (H2Config_debug_log)
				TRACE_GAME_N("[XNetQoSLookup][Socket: %08X] Got client magic, sending data back!",socket);

			char *mSendBuf = new char[h2QoS.cbData];
			ZeroMemory(mSendBuf, h2QoS.cbData);

			memcpy(mSendBuf, h2QoS.pbData, h2QoS.cbData);

			send(socket, mSendBuf, h2QoS.cbData, 0);

			delete[] mSendBuf;
		}

		std::this_thread::sleep_for(1ms);
	} while (1);


}

BOOL H2MOD_QoS::IsListening()
{
	return listenerThreadRunning;
}

void H2MOD_QoS::startListening()
{

	if (IsListening())
		return;

	listenerThreadRunning = TRUE;

	struct addrinfo *result = NULL;
	struct addrinfo hints;
	int iResult;
	int port = H2Config_base_port + 10;           /* port number to use */
	int client;                  /* file descriptor for socket */

	WSADATA wsaData;

	// Initialize Winsock
	iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (iResult != 0) {
		if (H2Config_debug_log)
			TRACE_GAME_N("WSAStartup failed with error: %d", iResult);
		return;
	}

	ZeroMemory(&hints, sizeof(hints));
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP;
	hints.ai_flags = AI_PASSIVE;

	// Resolve the server address and port
	iResult = getaddrinfo(NULL, std::to_string(port).c_str(), &hints, &result);
	if (iResult != 0) {
		if (H2Config_debug_log)
			TRACE_GAME_N("getaddrinfo failed with error: %d, port:", iResult, std::to_string(port).c_str());
		//WSACleanup();
		return;
	}

	// Create a SOCKET for connecting to server
	serverSocket = socket(result->ai_family, result->ai_socktype, result->ai_protocol);
	if (serverSocket == INVALID_SOCKET) {
		if (H2Config_debug_log)
			TRACE_GAME_N("socket failed with error: %ld", WSAGetLastError());
		freeaddrinfo(result);
		serverSocket = NULL;
		//WSACleanup();
		return;
	}

	/* bind socket to the port */
	iResult = bind(serverSocket, result->ai_addr, (int)result->ai_addrlen);
	if (iResult == SOCKET_ERROR) {
		if (H2Config_debug_log)
			TRACE_GAME_N("unable to bind to socket: %s", strerror(errno));
		freeaddrinfo(result);
		closesocket(serverSocket);
		serverSocket = NULL;
		return;
	}

	freeaddrinfo(result);

	/* listen for clients on the socket */
	iResult = listen(serverSocket, SOMAXCONN);
	if (iResult == SOCKET_ERROR) {
		if (H2Config_debug_log)
			TRACE_GAME_N("Error trying to listen on port: %s", strerror(errno));
		closesocket(serverSocket);
		serverSocket = NULL;
		return;
	}

	if (H2Config_debug_log)
		TRACE_GAME_N("Listening on port: %d", port);

	while (listenerThreadRunning) {
		/* wait for a client to connect */
		client = accept(serverSocket, 0, 0);
		if (client == -1) {
			if (H2Config_debug_log)
				TRACE_GAME_N("accept failed: %s", strerror(errno));
			//try to accept again
			//TODO: if it fails too many times, exit
			continue;
		}

		/* 
			This is probably a really bad idea, easy method to DoS servers if natural play doesn't already as a person's CPU will start spawning 1 thread per connection.

			Need to come up with a better implementation because without this we won't be able to process data coming from the server, 
			It may be best to attempt a connection first and start timing after the connection on the client end for the ping portion.
		*/

		std::thread t1(&handleClientConnection, client);
		t1.detach();

		std::this_thread::sleep_for(1ms);
	}

	listenerThreadRunning = FALSE;
}

// #69: XNetQosListen
DWORD WINAPI XNetQosListen(XNKID *pxnkid, PBYTE pb, UINT cb, DWORD dwBitsPerSec, DWORD dwFlags)
{
	TRACE("XNetQosListen  (pxnkid = %X, pb = %X, cb = %d, bitsPerSec = %d, flags = %X)",
		pxnkid, pb, cb, dwBitsPerSec, dwFlags);

	if ((dwFlags & XNET_QOS_LISTEN_ENABLE) && (dwFlags & XNET_QOS_LISTEN_SET_DATA))
	{
		TRACE("XnetQoSListen == XNET_QOS_LISTEN_ENABLE && XNetQoSListen == XNET_QOS_LISTEN_SET_DATA");
		StartListenerThread(pb, cb);
	}

	if (dwFlags & XNET_QOS_LISTEN_DISABLE)
	{
		TRACE("XnetQoSListen == XNET_QOS_LISTEN_DISABLE");
	}

	if (dwFlags & XNET_QOS_LISTEN_RELEASE)
	{
		TRACE("XnetQosListen == XNET_QOS_LISTEN_RELEASE");

	}

	if ((dwFlags & XNET_QOS_LISTEN_ENABLE) && !(dwFlags & XNET_QOS_LISTEN_SET_DATA))
	{
		StartListenerThread(pb, cb);
	}

	//std::thread(StartQoSThread,pb,cb).detach();

	//	if(dwFlags == XNET_QOS_LISTEN_ENABLE)
	//		std::thread(StartQoSThread).detach();

	return 0;
}

// #70: XNetQosLookup
DWORD WINAPI XNetQosLookup(UINT cxna, XNADDR * apxna[], XNKID * apxnkid[], XNKEY * apxnkey[], UINT cina, IN_ADDR aina[], DWORD adwServiceId[], UINT cProbes, DWORD dwBitsPerSec, DWORD dwFlags, WSAEVENT hEvent, XNQOS** pxnqos)
{


	TRACE("XNetQosLookup ( cxna: %i, cina: %i, cProbes: %i, dwBitsPerSec: %i, hEvent: %X)",
		cxna, cina, cProbes, dwBitsPerSec, hEvent);
	TRACE("XNetQoSLookup( apxna: %X, apxnkid: %X, apxnkey: %X, aina: %X, adwServiceId: %X, pxnqos: %X)",
		apxna, apxnkid, apxnkey, aina, adwServiceId, pxnqos);
	//void ClientQoSLookUp(UINT cxna, XNADDR* apxna[],UINT cProbes,IN_ADDR  aina[], XNQOS** pxnqos,DWORD dwBitsPerSec)

	//XNADDR **axpna_copy = (XNADDR**)malloc(cxna * sizeof(XNADDR*));
	XNADDR** axpna_copy = (XNADDR**)malloc(cxna * sizeof(XNADDR*));

	for (int i = 0; i < cxna; i++)
	{
		XNADDR* xn = apxna[i];
		axpna_copy[i] = new XNADDR;
		memcpy(axpna_copy[i], xn, sizeof(XNADDR));
	}


	*pxnqos = (XNQOS*)malloc(sizeof(XNQOS) + (sizeof(XNQOSINFO) * (cxna - 1)));

	XNQOS* pqos = &**pxnqos;
	ZeroMemory(pqos, sizeof(XNQOS) + (sizeof(XNQOSINFO)*(cxna - 1)));

	pqos->cxnqosPending = cxna;
	pqos->cxnqos = cxna;
	pqos->axnqosinfo[0].wRttMedInMsecs = 500;
	pqos->axnqosinfo[0].wRttMinInMsecs = 100;
	pqos->axnqosinfo[0].dwDnBitsPerSec = dwBitsPerSec;
	pqos->axnqosinfo[0].dwUpBitsPerSec = dwBitsPerSec;
	pqos->axnqosinfo[0].bFlags = XNET_XNQOSINFO_TARGET_CONTACTED | XNET_XNQOSINFO_COMPLETE;



	/*
	This is gonna hit some CPUs hard when there's a lot of servers on the list, we'll probably want to queue this a bit and only allow X number of threads to run at a time.
	We want to abuse the CPU where possible considering more modern systems will have decent CPUs so we'll be able to force things to happen faster but still want to keep compatibility with older setups.
	*/

	std::thread(ClientQoSLookUp, cxna, axpna_copy, cProbes, aina, pxnqos, dwBitsPerSec, pqos).detach();


	/* Memory Leak  - FIX ME! (Need to do some kind of garbage collection somewhere and store data like this in an array to be cleared later */
	/*
	*pxnqos = new XNQOS;
	XNQOS* pqos = *pxnqos;
	pqos->cxnqos = 1;
	pqos->cxnqosPending = 0;
	memset(pqos->axnqosinfo, 0x00, sizeof(XNQOSINFO));
	pqos->axnqosinfo->bReserved = 0;
	pqos->axnqosinfo->cProbesXmit = cProbes;
	pqos->axnqosinfo->cProbesRecv = cProbes;

	if (cProbes > 0)
	{
	pqos->axnqosinfo->wRttMedInMsecs = 5;
	pqos->axnqosinfo->wRttMinInMsecs = 10;
	pqos->axnqosinfo->dwUpBitsPerSec = dwBitsPerSec;
	pqos->axnqosinfo->dwDnBitsPerSec = dwBitsPerSec;
	pqos->axnqosinfo->bFlags = XNET_XNQOSINFO_TARGET_CONTACTED | XNET_XNQOSINFO_COMPLETE | XNET_XNQOSINFO_DATA_RECEIVED;
	}
	else
	{
	pqos->axnqosinfo->wRttMedInMsecs = 5;
	pqos->axnqosinfo->wRttMinInMsecs = 10;
	pqos->axnqosinfo->dwUpBitsPerSec = dwBitsPerSec;
	pqos->axnqosinfo->dwDnBitsPerSec = dwBitsPerSec;
	pqos->axnqosinfo->bFlags = XNET_XNQOSINFO_TARGET_CONTACTED | XNET_XNQOSINFO_COMPLETE;

	}*/

	return 0;
}

// #71: XNetQosServiceLookup
DWORD WINAPI XNetQosServiceLookup(DWORD a1, DWORD a2, DWORD a3)
{
	TRACE("XNetQosServiceLookup");


	// not connected to LIVE - abort now
	// - wants a3 return ASYNC struct
	return ERROR_INVALID_PARAMETER;

	//return 0;
}

// #72: XNetQosRelease
INT WINAPI XNetQosRelease(XNQOS* pxnqos)
{

	for (int i = 0; i == pxnqos->cxnqos; i++)
	{
		if (pxnqos->axnqosinfo[i].cbData > 0)
			delete[] pxnqos->axnqosinfo[i].pbData;

		delete[] & pxnqos->axnqosinfo[i];
		//XNQOSINFO *xnqos = &pqos->axnqosinfo[ ( pqos->cxnqosPending - 1) ];

	}
	/* We need to clean-up all XNetQoSLookup data here, listener data should be cleaned up inside the Listen function Only. */
	TRACE("XNetQosRelease");
	return 0;
}

// #77
// TODO: Check if halo2 uses this at all and implement it.
DWORD WINAPI XNetQosGetListenStats(DWORD a1, DWORD a2)
{
	TRACE("XNetQosGetListenStats");
	return 0;
}
