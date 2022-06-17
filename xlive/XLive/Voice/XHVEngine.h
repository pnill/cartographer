#pragma once

//
// Each packet reported by voice chat mode is the following size (including the
// XHV_CODEC_HEADER)
//

#define XHV_VOICECHAT_MODE_PACKET_SIZE          (10) // 10 bytes - size of voice packet (in newer version of xlive, a packet has to be at least 42 bytes)

//
// When supplying a buffer to GetLocalChatData, you won't have to supply a
// buffer any larger than the following number of packets (or
// XHV_MAX_VOICECHAT_PACKETS * XHV_VOICECHAT_MODE_PACKET_SIZE bytes)
//

#define XHV_MAX_VOICECHAT_PACKETS               (10)

//
// Data Ready Flags.  These flags are set when there is local data waiting to be
// consumed (e.g. through GetLocalChatData).  GetLocalDataFlags() allows you to
// get the current state of these flags without entering XHV's critical section.
// Each mask is 4 bits, one for each local talker.  The least significant bit in
// each section indicates data is available for user index 0, while the most
// significant bit indicates user index 3.
//

#define XHV_VOICECHAT_DATA_READY_MASK           (0xF)
#define XHV_VOICECHAT_DATA_READY_OFFSET         (0)

//
// This header appears at the beginning of each blob of data reported by voice
// chat mode
//

#pragma pack(push, 1)
typedef struct XHV_CODEC_HEADER
{
	WORD                                        bMsgNo : 4;
	WORD                                        wSeqNo : 11;
	WORD                                        bFriendsOnly : 1;
} XHV_CODEC_HEADER, *PXHV_CODEC_HEADER;
#pragma pack (pop)


typedef DWORD                                   XHV_LOCK_TYPE;

typedef DWORD                                   XHV_PROCESSING_MODE, *PXHV_PROCESSING_MODE;
typedef DWORD                                   XHV_PLAYBACK_PRIORITY;
typedef VOID(*PFNMICRAWDATAREADY)(
	IN  DWORD                                   dwUserIndex,
	IN  PVOID                                   pvData,
	IN  DWORD                                   dwSize,
	IN  PBOOL                                   pVoiceDetected
	);

typedef struct XHV_INIT_PARAMS
{
	DWORD                                       dwMaxRemoteTalkers;
	DWORD                                       dwMaxLocalTalkers;
	PXHV_PROCESSING_MODE                        localTalkerEnabledModes;
	DWORD                                       dwNumLocalTalkerEnabledModes;
	PXHV_PROCESSING_MODE                        remoteTalkerEnabledModes;
	DWORD                                       dwNumRemoteTalkerEnabledModes;
	BOOL                                        bCustomVADProvided;
	BOOL                                        bRelaxPrivileges;
	PFNMICRAWDATAREADY                          pfnMicrophoneRawDataReady;
	HWND                                        hwndFocus;
} XHV_INIT_PARAMS, *PXHV_INIT_PARAMS;

#define XHV_LOCK_TYPE_LOCK                      0
#define XHV_LOCK_TYPE_TRYLOCK                   1
#define XHV_LOCK_TYPE_UNLOCK                    2
#define XHV_LOCK_TYPE_COUNT                     3

class XHVENGINE
{
public:
	virtual LONG	STDMETHODCALLTYPE AddRef();	// 00
	virtual LONG	STDMETHODCALLTYPE Release();	// 04
	virtual HRESULT STDMETHODCALLTYPE Lock(XHV_LOCK_TYPE lockType);	// 08
	virtual HRESULT STDMETHODCALLTYPE StartLocalProcessingModes(DWORD dwUserIndex, /* CONST PXHV_PROCESSING_MODE*/ VOID* processingModes, DWORD dwNumProcessingModes);
	virtual HRESULT STDMETHODCALLTYPE StopLocalProcessingModes(DWORD dwUserIndex, /*CONST PXHV_PROCESSING_MODE*/ VOID* processingModes, DWORD dwNumProcessingModes);
	virtual HRESULT STDMETHODCALLTYPE StartRemoteProcessingModes(XUID xuidRemoteTalker, VOID* a2, int a3);
	virtual HRESULT STDMETHODCALLTYPE StopRemoteProcessingModes(XUID xuidRemoteTalker, /*CONST PXHV_PROCESSING_MODE*/ VOID* a2, int a3);	// 18
	virtual HRESULT STDMETHODCALLTYPE SetMaxDecodePackets(DWORD dwMaxDecodePackets);	// 1C
	virtual HRESULT STDMETHODCALLTYPE RegisterLocalTalker(DWORD dwUserIndex);
	virtual HRESULT STDMETHODCALLTYPE UnregisterLocalTalker(DWORD dwUserIndex);
	virtual HRESULT STDMETHODCALLTYPE RegisterRemoteTalker(XUID xuid, LPVOID reserved, LPVOID reserved2, LPVOID reserved3);	// 28
	virtual HRESULT STDMETHODCALLTYPE UnregisterRemoteTalker(XUID xuid);
	virtual HRESULT STDMETHODCALLTYPE GetRemoteTalkers(PDWORD pdwRemoteTalkersCount, PXUID pxuidRemoteTalkers);	// 30	
	virtual BOOL	STDMETHODCALLTYPE IsHeadsetPresent(DWORD dwUserIndex);
	virtual BOOL	STDMETHODCALLTYPE IsLocalTalking(DWORD dwUserIndex);
	virtual BOOL	STDMETHODCALLTYPE IsRemoteTalking(XUID xuidRemoteTalker);
	virtual DWORD	STDMETHODCALLTYPE GetDataReadyFlags();
	virtual HRESULT STDMETHODCALLTYPE GetLocalChatData(DWORD dwUserIndex, PBYTE pbData, PDWORD pdwSize, PDWORD pdwPackets);
	virtual HRESULT STDMETHODCALLTYPE SetPlaybackPriority(XUID xuidRemoteTalker, DWORD dwUserIndex, int a3);
	virtual HRESULT STDMETHODCALLTYPE SubmitIncomingChatData(XUID xuidRemoteTalker, const BYTE* pbData, PDWORD pdwSize);	// 4C
};