#pragma once
#include "xlivedefs.h"
#include <atomic>
void ClientQoSLookUp(UINT cxna, XNADDR* apxna[], UINT cProbes, IN_ADDR  aina[], XNQOS** pxnqos, DWORD dwBitsPerSec,XNQOS* pqos);
void StartListenerThread(PBYTE pb, UINT cb);

class H2MOD_QoS
{
	public:
		void startListening();
		void stopListening();
		BOOL IsListening();
		PBYTE pbData = NULL;
		UINT cbData = 0;

	private:
		volatile std::atomic<BOOL> listenerThreadRunning = FALSE;
		//void shutdownServerSocket();
		SOCKET serverSocket;
};