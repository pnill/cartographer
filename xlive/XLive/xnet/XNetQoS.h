#pragma once
#include "stdafx.h"

typedef struct _SOCKET_INFORMATION 
{
	OVERLAPPED Overlapped;
	SOCKET Socket;
	WSABUF DataBuf;
	DWORD BytesSEND;
	DWORD BytesRECV;
} SOCKET_INFORMATION, *LPSOCKET_INFORMATION;

class CXNetQoS
{
public:
	void Listener();
	//void stopListening();
	bool IsListening();

	SOCKET m_ListenSocket;
	bool m_bStopListening = false;
	WSAEVENT m_WsaEvent;

	unsigned int cbData = 0;
	char* pbData = nullptr;

	static void CALLBACK HandleClient(DWORD dwError, DWORD cbTransferred, LPWSAOVERLAPPED lpOverlapped, DWORD dwFlags);
	static void CALLBACK SendBack(DWORD dwError, DWORD cbTransferred, LPWSAOVERLAPPED lpOverlapped, DWORD dwFlags);

	volatile std::atomic<BOOL> m_listenerThreadRunning = FALSE;
private:
	
};

#define XNET_XNQOSINFO_COMPLETE         0x01    // Qos has finished processing this entry
#define XNET_XNQOSINFO_TARGET_CONTACTED 0x02    // Target host was successfully contacted
#define XNET_XNQOSINFO_TARGET_DISABLED  0x04    // Target host has disabled its Qos listener
#define XNET_XNQOSINFO_DATA_RECEIVED    0x08    // Target host supplied Qos data
#define XNET_XNQOSINFO_PARTIAL_COMPLETE 0x10    // Qos has unfinished estimates for this entry
#define XNET_XNQOSINFO_UNK	0x11


#define XNET_QOS_LISTEN_ENABLE              0x00000001 // Responds to queries on the given XNKID
#define XNET_QOS_LISTEN_DISABLE             0x00000002 // Rejects queries on the given XNKID
#define XNET_QOS_LISTEN_SET_DATA            0x00000004 // Sets the block of data to send back to queriers
#define XNET_QOS_LISTEN_SET_BITSPERSEC      0x00000008 // Sets max bandwidth that query reponses may consume
#define XNET_QOS_LISTEN_RELEASE             0x00000010 // Stops listening on given XNKID and releases memory

#define XNET_QOS_LOOKUP_RESERVED            0x00000000 // No flags defined yet for XNetQosLookup

#define XNET_QOS_SERVICE_LOOKUP_RESERVED    0x00000000 // No flags defined yet for XNetQosServiceLookup

INT   WINAPI XNetQosListen(const XNKID * pxnkid,
	const BYTE * pb,
	UINT cb,
	DWORD dwBitsPerSec, DWORD dwFlags);
INT   WINAPI XNetQosLookup(UINT cxna,
	const XNADDR * apxna[],
	const XNKID * apxnkid[],
	const XNKEY * apxnkey[],
	UINT cina,
	const IN_ADDR aina[],
	const DWORD adwServiceId[],
	UINT cProbes, DWORD dwBitsPerSec, DWORD dwFlags, WSAEVENT hEvent, XNQOS ** ppxnqos);
INT   WINAPI XNetQosServiceLookup(DWORD dwFlags, WSAEVENT hEvent, XNQOS ** ppxnqos);
INT   WINAPI XNetQosRelease(XNQOS* pxnqos);
INT   WINAPI XNetQosGetListenStats(const XNKID * pxnkid, XNQOSLISTENSTATS * pQosListenStats);

