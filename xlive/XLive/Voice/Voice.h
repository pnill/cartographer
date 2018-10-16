#pragma once

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

class IXHV2ENGINE
{
public:
	IXHV2ENGINE::IXHV2ENGINE();
	// 2F0 bytes = actual size
	// - note: check all INT return values - may not be true

	LONG AddRef(VOID *pThis);	// 00
	LONG Release(VOID *pThis);	// 04
	HRESULT Lock(VOID *pThis, XHV_LOCK_TYPE lockType);	// 08

	HRESULT StartLocalProcessingModes(VOID *pThis, DWORD dwUserIndex, /* CONST PXHV_PROCESSING_MODE*/ VOID *processingModes, DWORD dwNumProcessingModes);
	HRESULT StopLocalProcessingModes(VOID *pThis, DWORD dwUserIndex, /*CONST PXHV_PROCESSING_MODE*/ VOID *processingModes, DWORD dwNumProcessingModes);

	HRESULT StartRemoteProcessingModes(VOID *pThis, XUID xuidRemoteTalker, int a2, int a3);
	HRESULT StopRemoteProcessingModes(VOID *pThis, XUID xuidRemoteTalker, /*CONST PXHV_PROCESSING_MODE*/ VOID* a2, int a3);	// 18

	HRESULT NullSub(VOID *pThis, int a1);	// 1C

	HRESULT RegisterLocalTalker(VOID *pThis, DWORD dwUserIndex);
	HRESULT UnregisterLocalTalker(VOID *pThis, DWORD dwUserIndex);

	HRESULT RegisterRemoteTalker(VOID *pThis, XUID a1, LPVOID reserved, LPVOID reserved2, LPVOID reserved3);	// 28
	HRESULT UnregisterRemoteTalker(VOID *pThis, XUID a1);

	HRESULT GetRemoteTalkers(VOID *pThis, PDWORD pdwRemoteTalkersCount, PXUID pxuidRemoteTalkers);	// 30	
	BOOL IsLocalTalking(VOID *pThis, DWORD dwUserIndex);
	BOOL isRemoteTalking(VOID *pThis, XUID xuidRemoteTalker);
	BOOL IsHeadsetPresent(VOID *pThis, DWORD dwUserIndex);

	DWORD GetDataReadyFlags(VOID *pThis);

	HRESULT GetLocalChatData(VOID *pThis, DWORD dwUserIndex, PBYTE pbData, PDWORD pdwSize, PDWORD pdwPackets);
	HRESULT SetPlaybackPriority(VOID *pThis, XUID xuidRemoteTalker, DWORD dwUserIndex, int a3);

	HRESULT SubmitIncomingChatData(VOID *pThis, XUID xuidRemoteTalker, const BYTE* pbData, PDWORD pdwSize);	// 4C

	typedef void (IXHV2ENGINE::*HV2FUNCPTR)(void);

	HV2FUNCPTR *funcTablePtr;
	HV2FUNCPTR funcPtr[100];
	HV2FUNCPTR func2;

	bool locked = false;
};

typedef IXHV2ENGINE *PIXHV2ENGINE;

extern IXHV2ENGINE hv2Engine;
