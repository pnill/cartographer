#include "stdafx.h"
#include "XHVEngine.h"

#include <DSound.h>

// #5008
HRESULT WINAPI XHVCreateEngine(PXHV_INIT_PARAMS pParams, PHANDLE phWorkerThread, XHVENGINE **ppEngine)
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

BOOL XHVENGINE::IsHeadsetPresent(DWORD dwUserIndex)
{
	return false;
}

BOOL XHVENGINE::IsRemoteTalking(XUID xuid)
{
	return false;
}

BOOL XHVENGINE::IsLocalTalking(DWORD dwUserIndex)
{
	return false;
}

LONG XHVENGINE::AddRef()
{
	return S_OK;
}

LONG XHVENGINE::Release()
{
	return S_OK;
}

HRESULT XHVENGINE::Lock(XHV_LOCK_TYPE lockType)
{
	return S_OK;
}

HRESULT XHVENGINE::StartLocalProcessingModes(DWORD dwUserIndex, /* CONST PXHV_PROCESSING_MODE*/ VOID* processingModes, DWORD dwNumProcessingModes)
{
	return S_OK;
}

HRESULT XHVENGINE::StopLocalProcessingModes(DWORD dwUserIndex, /*CONST PXHV_PROCESSING_MODE*/ VOID* processingModes, DWORD dwNumProcessingModes)
{
	return S_OK;
}

HRESULT XHVENGINE::StartRemoteProcessingModes(XUID a1, VOID* a2, int a3)
{
	return S_OK;
}

HRESULT XHVENGINE::StopRemoteProcessingModes(XUID xuidRemoteTalker, /*CONST PXHV_PROCESSING_MODE*/ VOID* a2, int a3)
{
	return S_OK;
}

HRESULT XHVENGINE::SetMaxDecodePackets(DWORD dwMaxDecodePackets)
{
	return S_OK;
}

HRESULT XHVENGINE::RegisterLocalTalker(DWORD dwUserIndex)
{
	return S_OK;
}

HRESULT XHVENGINE::UnregisterLocalTalker(DWORD dwUserIndex)
{
	return S_OK;
}

HRESULT XHVENGINE::RegisterRemoteTalker(XUID a1, LPVOID reserved, LPVOID reserved2, LPVOID reserved3)
{
	return S_OK;
}

HRESULT XHVENGINE::UnregisterRemoteTalker(XUID xuid)
{
	return S_OK;
}

HRESULT XHVENGINE::GetRemoteTalkers(PDWORD pdwRemoteTalkersCount, PXUID pxuidRemoteTalkers)
{
	return S_OK;
}

DWORD XHVENGINE::GetDataReadyFlags()
{
	return S_OK;
}

HRESULT XHVENGINE::GetLocalChatData(DWORD dwUserIndex, PBYTE pbData, PDWORD pdwSize, PDWORD pdwPackets)
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

HRESULT XHVENGINE::SetPlaybackPriority(XUID xuidRemoteTalker, DWORD dwUserIndex, int a3)
{
	return S_OK;
}

HRESULT XHVENGINE::SubmitIncomingChatData(XUID xuidRemoteTalker, const BYTE* pbData, PDWORD pdwSize)
{
	return S_OK;
}