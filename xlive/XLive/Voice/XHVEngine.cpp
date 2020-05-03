#include "stdafx.h"
#include "XHVEngine.h"

#include "Globals.h"
#include "AudioDevices.h"
#include "AudioHandler.h"

#include "H2MOD\Modules\Config\Config.h"

std::vector<XUID> remotetalkers;

#if COMPILE_WITH_VOICE
CAudioHandler* p_CAudioHandler = nullptr;
CAudioDevices* p_CAudioDevices = nullptr;
#endif

//#5008
int WINAPI XHVCreateEngine(PXHV_INIT_PARAMS pParams, PHANDLE phWorkerThread, PIXHV2ENGINE *ppEngine)
{
	LOG_TRACE_XLIVE("XHVCreateEngine  (pParams = {0:p}, phWorkerThread = {1:p}, pEngine = {2:p})",
		(void*)pParams, (void*)phWorkerThread, (void*)ppEngine);

	// disable until ready
	H2Config_voice_chat = false;

#if COMPILE_WITH_VOICE
	if (H2Config_voice_chat)
		p_CAudioHandler = new CAudioHandler(p_CAudioDevices);
#endif

	if (pParams->bCustomVADProvided)
	{
		LOG_TRACE_GAME("XHV Engine - bCustomVADProvided set");
	}

	if (phWorkerThread)
	{
		*phWorkerThread = CreateMutex(0, 0, 0);

		LOG_TRACE_XLIVE("- Handle = {:p}", (void*)*phWorkerThread);
	}


	if (ppEngine)
	{
		*ppEngine = new IXHV2ENGINE;
		LOG_TRACE_XLIVE("- hv2Engine = {:p}", (void*)*ppEngine);

		return ERROR_SUCCESS;
	}

	return ERROR_FUNCTION_FAILED;
}

BOOL IXHV2ENGINE::IsHeadsetPresent(VOID *pThis, DWORD dwUserIndex) {
	//LOG_TRACE_XLIVE("IXHV2Engine::IsHeadsetPresent");

	return H2Config_voice_chat
#if COMPILE_WITH_VOICE
		&& p_CAudioHandler->audioDevices->IsDeviceAvailable(Input)
#endif
		;
}

BOOL IXHV2ENGINE::isRemoteTalking(VOID *pThis, XUID xuid) {
	//LOG_TRACE_XLIVE("IXHV2Engine::isRemoteTalking");

	return H2Config_voice_chat && xuidIsTalkingMap[xuid];
}

BOOL IXHV2ENGINE::IsLocalTalking(VOID *pThis, DWORD dwUserIndex) {
	//LOG_TRACE_XLIVE("IXHV2Engine::isTalking(dwUserIndex = %d)", dwUserIndex);
	//check the xuid map
	XUID id = xFakeXuid[0];
	BOOL isTalking = xuidIsTalkingMap[id];
	return H2Config_voice_chat 
#if COMPILE_WITH_VOICE
		&& p_CAudioHandler->audioDevices->IsDeviceAvailable(Input) ? xuidIsTalkingMap[id] : false;
#endif
	;
}

LONG IXHV2ENGINE::AddRef(/*CXHVEngine*/ VOID *pThis)
{
	LOG_TRACE_XLIVE("IXHV2Engine::AddRef");
	return S_OK;
}

LONG IXHV2ENGINE::Release(/*CXHVEngine*/ VOID *pThis)
{
	LOG_TRACE_XLIVE("IXHV2Engine::Release");

	if (!remotetalkers.empty())
		remotetalkers.clear();

#if COMPILE_WITH_VOICE
	if (H2Config_voice_chat)
		delete p_CAudioHandler;
#endif

	IXHV2ENGINE* pIXHV2Engine = reinterpret_cast<IXHV2ENGINE*>(pThis);
	delete pIXHV2Engine;

	return S_OK;
}


HRESULT IXHV2ENGINE::Lock(/*CXHVEngine*/ VOID *pThis, XHV_LOCK_TYPE lockType)
{

	if (lockType == XHV_LOCK_TYPE_LOCK)
	{
		locked = true;
	}

	if (lockType == XHV_LOCK_TYPE_TRYLOCK && locked == false)
	{
		locked = true;
	}

	if (lockType == XHV_LOCK_TYPE_UNLOCK)
	{
		locked = false;
	}

	return S_OK;
}



HRESULT IXHV2ENGINE::StartLocalProcessingModes(VOID *pThis, DWORD dwUserIndex, /* CONST PXHV_PROCESSING_MODE*/ VOID *processingModes, DWORD dwNumProcessingModes)
{
	return S_OK;
}



HRESULT IXHV2ENGINE::StopLocalProcessingModes(VOID *pThis, DWORD dwUserIndex, /*CONST PXHV_PROCESSING_MODE*/ VOID *processingModes, DWORD dwNumProcessingModes)
{
	return S_OK;
}



HRESULT IXHV2ENGINE::StartRemoteProcessingModes(VOID *pThis, XUID a1, int a2, int a3)
{
	LOG_TRACE_GAME("[h2mod-voice] StartRemoteProcessingModes XUID: {}", a1);
	return S_OK;
}



HRESULT IXHV2ENGINE::StopRemoteProcessingModes(VOID *pThis, XUID xuidRemoteTalker, /*CONST PXHV_PROCESSING_MODE*/ VOID* a2, int a3)
{
	LOG_TRACE_GAME("[h2mod-voice] StopRemoteProcessingModes xuidRemoteTalker: {}", xuidRemoteTalker);
	return S_OK;
}



HRESULT IXHV2ENGINE::NullSub(VOID *pThis, int a1)
{
	LOG_TRACE_XLIVE("IXHV2Engine::NullSub  (a1 = {:x})", a1);
	return ERROR_SUCCESS;
}



HRESULT IXHV2ENGINE::RegisterLocalTalker(VOID *pThis, DWORD dwUserIndex)
{
	return S_OK;
}



HRESULT IXHV2ENGINE::UnregisterLocalTalker(VOID *pThis, DWORD dwUserIndex)
{
	//LOG_TRACE_XLIVE("IXHV2Engine::UnregisterLocalTalker  (dwUserIndex = %d)",
	//	dwUserIndex);

	return S_OK;
}


HRESULT IXHV2ENGINE::RegisterRemoteTalker(VOID *pThis, XUID a1, LPVOID reserved, LPVOID reserved2, LPVOID reserved3)
{
	//LOG_TRACE_GAME_N("[h2mod-voice] IXHV2Engine::RegisterRemoteTalker XUID: %lld", a1);
	remotetalkers.push_back(a1);
	return S_OK;
}


HRESULT IXHV2ENGINE::UnregisterRemoteTalker(VOID *pThis, XUID xuid)
{
	//LOG_TRACE_GAME_N("[h2mod-voice] IXHV2Engine::UnregisterRemoteTalker  (a1 = %X, a2 = %X)",
	//	a1);

	for (auto it = remotetalkers.begin(); it != remotetalkers.end(); ++it)
	{
		if (*it = xuid) {
			remotetalkers.erase(it);
			break;
		}
	}

	return S_OK;
}


HRESULT IXHV2ENGINE::GetRemoteTalkers(VOID *pThis, PDWORD pdwRemoteTalkersCount, PXUID pxuidRemoteTalkers)
{

	//LOG_TRACE_GAME_N("[h2mod-voice] - GetRemoteTalkers");
	*pdwRemoteTalkersCount = remotetalkers.size();
	int i = 0;
	for (auto it = remotetalkers.begin(); it != remotetalkers.end(); ++it)
	{
		pxuidRemoteTalkers[i] = *it;
		i++;
	}

	return S_OK;
}

DWORD IXHV2ENGINE::GetDataReadyFlags(VOID *pThis)
{
	int ret = 0x00;

	if (H2Config_voice_chat && xuidIsTalkingMap[xFakeXuid[0]])
	{
		//LOG_TRACE_GAME_N("[h2mod-voice] GetDataReadyFlags - called");
		ret = 0x1;
	}
	// 0x1 = user 0
	// 0xF = user 0-3
	// 0xFF = user 0-7
	return ret;
}


HRESULT IXHV2ENGINE::GetLocalChatData(VOID *pThis, DWORD dwUserIndex, PBYTE pbData, PDWORD pdwSize, PDWORD pdwPackets)
{
	// The game uses 10 bytes / voice packet, so we need to split them
	// To verify who is talking, add the XUID of the local talker in the first packet
	if (H2Config_voice_chat)
	{
		*pdwPackets = *pdwSize / XHV_VOICECHAT_MODE_PACKET_SIZE;

		if (*pdwPackets > XHV_MAX_VOICECHAT_PACKETS)
			*pdwPackets = XHV_MAX_VOICECHAT_PACKETS;

		if (pdwSize && pdwPackets && xuidIsTalkingMap[xFakeXuid[0]])
		{
			return S_OK;
		}
		else
		{
			if (pdwSize) *pdwSize = 0;
			if (pdwPackets) *pdwPackets = 0;
		}
	}

	if (pdwSize) *pdwSize = 0;
	if (pdwPackets) *pdwPackets = 0;

	return E_PENDING;
}



HRESULT IXHV2ENGINE::SetPlaybackPriority(VOID *pThis, XUID xuidRemoteTalker, DWORD dwUserIndex, int a3)
{
	return S_OK;
}


HRESULT IXHV2ENGINE::SubmitIncomingChatData(VOID *pThis, XUID xuidRemoteTalker, const BYTE* pbData, PDWORD pdwSize)
{
	//LOG_TRACE_GAME_N("[h2mod-voice][SubmitIncomingChatData] - XUID in packet: %lld", test);
	//LOG_TRACE_GAME_N("[h2mod-voice][SubmitIncomingChatData] - Trying to unmute tsID: %i  xuid: %lld", tsID, xuidRemoteTalker);

	return S_OK;
}


IXHV2ENGINE::IXHV2ENGINE()
{
	funcTablePtr = &(funcPtr[0]);


	funcPtr[0] = (HV2FUNCPTR)&IXHV2ENGINE::AddRef;
	funcPtr[1] = (HV2FUNCPTR)&IXHV2ENGINE::Release;
	funcPtr[2] = (HV2FUNCPTR)&IXHV2ENGINE::Lock;
	funcPtr[3] = (HV2FUNCPTR)&IXHV2ENGINE::StartLocalProcessingModes;
	funcPtr[4] = (HV2FUNCPTR)&IXHV2ENGINE::StopLocalProcessingModes;
	funcPtr[5] = (HV2FUNCPTR)&IXHV2ENGINE::StartRemoteProcessingModes;
	funcPtr[6] = (HV2FUNCPTR)&IXHV2ENGINE::StopRemoteProcessingModes;
	funcPtr[7] = (HV2FUNCPTR)&IXHV2ENGINE::NullSub;

	funcPtr[8] = (HV2FUNCPTR)&IXHV2ENGINE::RegisterLocalTalker;
	funcPtr[9] = (HV2FUNCPTR)&IXHV2ENGINE::UnregisterLocalTalker;
	funcPtr[10] = (HV2FUNCPTR)&IXHV2ENGINE::RegisterRemoteTalker;
	funcPtr[11] = (HV2FUNCPTR)&IXHV2ENGINE::UnregisterRemoteTalker;

	funcPtr[12] = (HV2FUNCPTR)&IXHV2ENGINE::GetRemoteTalkers;
	funcPtr[13] = (HV2FUNCPTR)&IXHV2ENGINE::IsHeadsetPresent;
	funcPtr[14] = (HV2FUNCPTR)&IXHV2ENGINE::IsLocalTalking;
	funcPtr[15] = (HV2FUNCPTR)&IXHV2ENGINE::isRemoteTalking;

	funcPtr[16] = (HV2FUNCPTR)&IXHV2ENGINE::GetDataReadyFlags;
	funcPtr[17] = (HV2FUNCPTR)&IXHV2ENGINE::GetLocalChatData;
	funcPtr[18] = (HV2FUNCPTR)&IXHV2ENGINE::SetPlaybackPriority;
	funcPtr[19] = (HV2FUNCPTR)&IXHV2ENGINE::SubmitIncomingChatData;

}
