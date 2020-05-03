#include "stdafx.h"
#include "XHVEngine.h"

#include "Globals.h"
#include "AudioHandler.h"

#include "DSound.h"

#include "H2MOD\Modules\Config\Config.h"

std::vector<XUID> remotetalkers;

#if COMPILE_WITH_VOICE
CAudioHandler* pAudioHandler = nullptr;
#endif

//#5008
int WINAPI XHVCreateEngine(PXHV_INIT_PARAMS pParams, PHANDLE phWorkerThread, PIXHV2ENGINE *ppEngine)
{
#if COMPILE_WITH_VOICE
	if (H2Config_voice_chat) 
		pAudioHandler = new CAudioHandler();
	else
#endif
		return DSERR_NODRIVER;

	if (pParams->bCustomVADProvided)
	{
		LOG_TRACE(voice_log, "XHVCreateEngine() - bCustomVADProvided set");
	}

	if (ppEngine)
	{
		*ppEngine = new IXHV2ENGINE;
		LOG_TRACE(voice_log, "XHVCreateEngine() - created engine");

		return S_OK;
	}

	return E_INVALIDARG;
}

BOOL IXHV2ENGINE::IsHeadsetPresent(VOID *pThis, DWORD dwUserIndex) {
	//LOG_TRACE("IsHeadsetPresent()");

	return H2Config_voice_chat
#if COMPILE_WITH_VOICE
		&& pAudioHandler->audioDevices->IsDeviceAvailable(Input)
		&& pAudioHandler->audioDevices->IsDeviceAvailable(Output)
#endif
		;
}

BOOL IXHV2ENGINE::isRemoteTalking(VOID *pThis, XUID xuid) {
	//LOG_TRACE(voice_log, "IXHV2Engine::isRemoteTalking()");

	return H2Config_voice_chat && xuidIsTalkingMap[xuid];
}

BOOL IXHV2ENGINE::IsLocalTalking(VOID *pThis, DWORD dwUserIndex) {
	//LOG_TRACE(voice_log, "IXHV2Engine::isTalking());
	//check the xuid map
	XUID id = usersSignInInfo[dwUserIndex].xuid;
	BOOL isTalking = xuidIsTalkingMap[id];
	return H2Config_voice_chat 
#if COMPILE_WITH_VOICE
		&& pAudioHandler->audioDevices->IsDeviceAvailable(Input) ? xuidIsTalkingMap[id] : false;
#endif
	;
}

LONG IXHV2ENGINE::AddRef(/*CXHVEngine*/ VOID *pThis)
{
	LOG_TRACE(voice_log, "IXHV2Engine::AddRef");
	return S_OK;
}

LONG IXHV2ENGINE::Release(/*CXHVEngine*/ VOID *pThis)
{
	LOG_TRACE(voice_log, "IXHV2Engine::Release");

	if (!remotetalkers.empty())
		remotetalkers.clear();

#if COMPILE_WITH_VOICE
	if (H2Config_voice_chat)
		delete pAudioHandler;
#endif

	delete this;

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
	LOG_TRACE(voice_log, "[h2mod-voice] StartRemoteProcessingModes XUID: {}", a1);
	return S_OK;
}



HRESULT IXHV2ENGINE::StopRemoteProcessingModes(VOID *pThis, XUID xuidRemoteTalker, /*CONST PXHV_PROCESSING_MODE*/ VOID* a2, int a3)
{
	LOG_TRACE(voice_log, "[h2mod-voice] StopRemoteProcessingModes xuidRemoteTalker: {}", xuidRemoteTalker);
	return S_OK;
}

HRESULT IXHV2ENGINE::NullSub(VOID *pThis, int a1)
{
	LOG_TRACE(voice_log, "IXHV2Engine::NullSub  (a1 = {:x})", a1);
	return S_OK;
}



HRESULT IXHV2ENGINE::RegisterLocalTalker(VOID *pThis, DWORD dwUserIndex)
{
	return S_OK;
}

HRESULT IXHV2ENGINE::UnregisterLocalTalker(VOID *pThis, DWORD dwUserIndex)
{
	//LOG_TRACE(voice_log, "UnregisterLocalTalker()");

	return S_OK;
}

HRESULT IXHV2ENGINE::RegisterRemoteTalker(VOID *pThis, XUID a1, LPVOID reserved, LPVOID reserved2, LPVOID reserved3)
{
	LOG_TRACE(voice_log, "RegisterRemoteTalker XUID: {}", a1);
	remotetalkers.push_back(a1);
	return S_OK;
}


HRESULT IXHV2ENGINE::UnregisterRemoteTalker(VOID *pThis, XUID xuid)
{
	LOG_TRACE(voice_log, "UnregisterRemoteTalker() XUID: {}", xuid);

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

	//LOG_TRACE(voice_log, "[h2mod-voice] - GetRemoteTalkers");
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
	LOG_TRACE(voice_log, "GetDataReadyFlags()");
	int ret = 0;

	if (H2Config_voice_chat && xuidIsTalkingMap[usersSignInInfo[0].xuid])
	{
		//LOG_TRACE(voice_log, GetDataReadyFlags()");
		ret |= 1 << 0; // only 1 user with GFWL
	}
	// 0x01 = user 0
	// 0x0F = user 0-3
	return ret;
}


HRESULT IXHV2ENGINE::GetLocalChatData(VOID *pThis, DWORD dwUserIndex, PBYTE pbData, PDWORD pdwSize, PDWORD pdwPackets)
{
	// The game uses 10 bytes / voice packet, so we need to split them
	// To verify who is talking, add the XUID of the local talker in the first packet
	
	memset(pbData, 0, *pdwSize);

	if (H2Config_voice_chat)
	{
		*pdwPackets = *pdwSize / XHV_VOICECHAT_MODE_PACKET_SIZE;

		if (*pdwPackets > XHV_MAX_VOICECHAT_PACKETS)
			*pdwPackets = XHV_MAX_VOICECHAT_PACKETS;

		if (pdwSize && pdwPackets && xuidIsTalkingMap[usersSignInInfo[0].xuid])
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
	//LOG_TRACE(voice_log, "SubmitIncomingChatData() - XUID in packet: %lld", test);
	//LOG_TRACE(voice_log, "SubmitIncomingChatData() - Trying to unmute xuid: %lld", xuidRemoteTalker);

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
