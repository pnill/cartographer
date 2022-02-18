#include "stdafx.h"

#include "XHVEngine.h"

#include <DSound.h>

// #5008
int WINAPI XHVCreateEngine(PXHV_INIT_PARAMS pParams, PHANDLE phWorkerThread, XHVENGINE **ppEngine)
{
	return DSERR_NODRIVER;

	if (pParams->bCustomVADProvided)
	{
		LOG_TRACE(voice_log, "XHVCreateEngine() - bCustomVADProvided set");
	}

	if (ppEngine)
	{
		*ppEngine = new XHVENGINE;
		LOG_TRACE(voice_log, "XHVCreateEngine() - created engine");

		return S_OK;
	}

	return E_INVALIDARG;
}

BOOL XHVENGINE::IsHeadsetPresent(VOID *pThis, DWORD dwUserIndex)
{
	return false;
}

BOOL XHVENGINE::isRemoteTalking(VOID *pThis, XUID xuid)
{
	return false;
}

BOOL XHVENGINE::IsLocalTalking(VOID *pThis, DWORD dwUserIndex)
{
	return false;
}

LONG XHVENGINE::AddRef(/*CXHVEngine*/ VOID *pThis)
{
	return S_OK;
}

LONG XHVENGINE::Release(/*CXHVEngine*/ VOID *pThis)
{
	return S_OK;
}

HRESULT XHVENGINE::Lock(/*CXHVEngine*/ VOID *pThis, XHV_LOCK_TYPE lockType)
{
	return S_OK;
}

HRESULT XHVENGINE::StartLocalProcessingModes(VOID *pThis, DWORD dwUserIndex, /* CONST PXHV_PROCESSING_MODE*/ VOID *processingModes, DWORD dwNumProcessingModes)
{
	return S_OK;
}

HRESULT XHVENGINE::StopLocalProcessingModes(VOID *pThis, DWORD dwUserIndex, /*CONST PXHV_PROCESSING_MODE*/ VOID *processingModes, DWORD dwNumProcessingModes)
{
	return S_OK;
}

HRESULT XHVENGINE::StartRemoteProcessingModes(VOID *pThis, XUID a1, int a2, int a3)
{
	return S_OK;
}

HRESULT XHVENGINE::StopRemoteProcessingModes(VOID *pThis, XUID xuidRemoteTalker, /*CONST PXHV_PROCESSING_MODE*/ VOID* a2, int a3)
{
	return S_OK;
}

HRESULT XHVENGINE::SetMaxDecodePackets(VOID *pThis, int a1)
{
	return S_OK;
}

HRESULT XHVENGINE::RegisterLocalTalker(VOID *pThis, DWORD dwUserIndex)
{
	return S_OK;
}

HRESULT XHVENGINE::UnregisterLocalTalker(VOID *pThis, DWORD dwUserIndex)
{
	return S_OK;
}

HRESULT XHVENGINE::RegisterRemoteTalker(VOID *pThis, XUID a1, LPVOID reserved, LPVOID reserved2, LPVOID reserved3)
{
	return S_OK;
}

HRESULT XHVENGINE::UnregisterRemoteTalker(VOID *pThis, XUID xuid)
{
	return S_OK;
}

HRESULT XHVENGINE::GetRemoteTalkers(VOID *pThis, PDWORD pdwRemoteTalkersCount, PXUID pxuidRemoteTalkers)
{
	return S_OK;
}

DWORD XHVENGINE::GetDataReadyFlags(VOID *pThis)
{
	return S_OK;
}

HRESULT XHVENGINE::GetLocalChatData(VOID *pThis, DWORD dwUserIndex, PBYTE pbData, PDWORD pdwSize, PDWORD pdwPackets)
{
	// The game uses 10 bytes / voice packet, so we need to split them
	// To verify who is talking, add the XUID of the local talker in the first packet
	return E_PENDING;
	
	memset(pbData, 0, *pdwSize);

	*pdwPackets = *pdwSize / XHV_VOICECHAT_MODE_PACKET_SIZE;

	if (*pdwPackets > XHV_MAX_VOICECHAT_PACKETS)
		*pdwPackets = XHV_MAX_VOICECHAT_PACKETS;

	if (pdwSize && pdwPackets)
	{
		return S_OK;
	}
	else
	{
		if (pdwSize) *pdwSize = 0;
		if (pdwPackets) *pdwPackets = 0;
	}

	if (pdwSize) *pdwSize = 0;
	if (pdwPackets) *pdwPackets = 0;

	return E_PENDING;
}

HRESULT XHVENGINE::SetPlaybackPriority(VOID *pThis, XUID xuidRemoteTalker, DWORD dwUserIndex, int a3)
{
	return S_OK;
}


HRESULT XHVENGINE::SubmitIncomingChatData(VOID *pThis, XUID xuidRemoteTalker, const BYTE* pbData, PDWORD pdwSize)
{
	return S_OK;
}