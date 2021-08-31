#pragma once

typedef struct _SOCKET_INFORMATION 
{
	WSAOVERLAPPED Overlapped;
	SOCKET Socket;
	WSABUF DataBuf;
	DWORD BytesSEND;
	DWORD BytesRECV;
} SOCKET_INFORMATION, *LPSOCKET_INFORMATION;

class CXNetQoS
{
public:
	void Listener();
	bool IsListening();

	bool m_bStopListening = false;
	SOCKET m_ListenSocket = INVALID_SOCKET;
	WSAEVENT m_WsaEvent = WSA_INVALID_EVENT;

	unsigned int cbData = 0;
	PBYTE pbData = nullptr;

	static void CALLBACK HandleClient(DWORD dwError, DWORD cbTransferred, LPWSAOVERLAPPED lpOverlapped, DWORD dwFlags);
	static void CALLBACK SendBack(DWORD dwError, DWORD cbTransferred, LPWSAOVERLAPPED lpOverlapped, DWORD dwFlags);

	std::atomic<bool> m_listenerThreadRunning = false;
private:
	
};

#define XNET_XNQOSINFO_COMPLETE         0x01    // Qos has finished processing this entry
#define XNET_XNQOSINFO_TARGET_CONTACTED 0x02    // Target host was successfully contacted
#define XNET_XNQOSINFO_TARGET_DISABLED  0x04    // Target host has disabled its Qos listener
#define XNET_XNQOSINFO_DATA_RECEIVED    0x08    // Target host supplied Qos data
#define XNET_XNQOSINFO_PARTIAL_COMPLETE 0x10    // Qos has unfinished estimates for this entry

#define XNET_QOS_LISTEN_ENABLE              0x00000001 // Responds to queries on the given XNKID
#define XNET_QOS_LISTEN_DISABLE             0x00000002 // Rejects queries on the given XNKID
#define XNET_QOS_LISTEN_SET_DATA            0x00000004 // Sets the block of data to send back to queriers
#define XNET_QOS_LISTEN_SET_BITSPERSEC      0x00000008 // Sets max bandwidth that query reponses may consume
#define XNET_QOS_LISTEN_RELEASE             0x00000010 // Stops listening on given XNKID and releases memory

#define XNET_QOS_LOOKUP_RESERVED            0x00000000 // No flags defined yet for XNetQosLookup

#define XNET_QOS_SERVICE_LOOKUP_RESERVED    0x00000000 // No flags defined yet for XNetQosServiceLookup

