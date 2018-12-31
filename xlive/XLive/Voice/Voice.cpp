#include "Globals.h"
#include "3rdparty/opus/include/opus.h"
#include "xlivedefs.h"
#include "Voice.h"
#include "H2MOD\Modules\Config\Config.h"

IXHV2ENGINE hv2Engine;
std::vector<XUID> remotetalkers;

//#5008
int WINAPI XHVCreateEngine(PXHV_INIT_PARAMS pParams, PHANDLE phWorkerThread, PIXHV2ENGINE *ppEngine)
{
	TRACE("XHVCreateEngine  (pParams = %X, phWorkerThread = %X, pEngine = %X)",
		pParams, phWorkerThread, ppEngine);

	if (pParams->bCustomVADProvided)
	{
		TRACE_GAME_N("XHV Engine - bCustomVADProvided set");
	}
	if (phWorkerThread)
	{
		*phWorkerThread = CreateMutex(0, 0, 0);

		TRACE("- Handle = %X", *phWorkerThread);
	}


	if (ppEngine)
	{
		*ppEngine = (PIXHV2ENGINE)&hv2Engine;

		TRACE("- hv2Engine = %X", *ppEngine);
	}


	return ERROR_SUCCESS;
}

BOOL IXHV2ENGINE::IsHeadsetPresent(VOID *pThis, DWORD dwUserIndex) {
	//TRACE("IXHV2Engine::IsHeadsetPresent");

	//TODO: add logic that actually validates their audio device is connected
	//TODO: handle it being connected and disconnected
	return  H2Config_voice_chat;
}

BOOL IXHV2ENGINE::isRemoteTalking(VOID *pThis, XUID xuid) {
	//TRACE("IXHV2Engine::isRemoteTalking");
	//TODO: the xuid given here is the real xuid, the one we provide in xlive.ini is the hex of that the above value
	//so we need to convert it...
	return H2Config_voice_chat && xuidIsTalkingMap[xuid];
}

BOOL IXHV2ENGINE::IsLocalTalking(VOID *pThis, DWORD dwUserIndex) {
	//TRACE("IXHV2Engine::isTalking(dwUserIndex = %d)", dwUserIndex);
	//check the xuid map
	XUID id = xFakeXuid[0];
	BOOL isTalking = xuidIsTalkingMap[id];
	return H2Config_voice_chat && microphoneEnabled ? xuidIsTalkingMap[id] : false;
}

LONG IXHV2ENGINE::AddRef(/*CXHVEngine*/ VOID *pThis)
{
	TRACE("IXHV2Engine::AddRef");


	return S_OK;
}



LONG IXHV2ENGINE::Release(/*CXHVEngine*/ VOID *pThis)
{
	TRACE("IXHV2Engine::Release");

	if (!remotetalkers.empty())
		remotetalkers.clear();

	return 0;
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

	TRACE_GAME_N("[h2mod-voice] StartRemoteProcessingModes XUID: %lld", a1);
	return S_OK;
}



HRESULT IXHV2ENGINE::StopRemoteProcessingModes(VOID *pThis, XUID xuidRemoteTalker, /*CONST PXHV_PROCESSING_MODE*/ VOID* a2, int a3)
{

	TRACE_GAME_N("[h2mod-voice] StopRemoteProcessingModes xuidRemoteTalker: %lld", xuidRemoteTalker);
	//TRACE( "IXHV2Engine::Dummy7  (xuid = %X, a2 = %X, a3 = %X, a4 = %X)",
	//	xuidRemoteTalker, a2, a3);


	return S_OK;
}



HRESULT IXHV2ENGINE::NullSub(VOID *pThis, int a1)
{
	TRACE("IXHV2Engine::NullSub  (a1 = %X)", a1);


	return ERROR_SUCCESS;
}



HRESULT IXHV2ENGINE::RegisterLocalTalker(VOID *pThis, DWORD dwUserIndex)
{
	return S_OK;
}



HRESULT IXHV2ENGINE::UnregisterLocalTalker(VOID *pThis, DWORD dwUserIndex)
{
	//TRACE("IXHV2Engine::UnregisterLocalTalker  (dwUserIndex = %d)",
	//	dwUserIndex);

	return S_OK;
}


HRESULT IXHV2ENGINE::RegisterRemoteTalker(VOID *pThis, XUID a1, LPVOID reserved, LPVOID reserved2, LPVOID reserved3)
{
	//TRACE_GAME_N("[h2mod-voice] IXHV2Engine::RegisterRemoteTalker XUID: %lld", a1);
	remotetalkers.push_back(a1);
	return S_OK;
}


HRESULT IXHV2ENGINE::UnregisterRemoteTalker(VOID *pThis, XUID xuid)
{
	//TRACE_GAME_N("[h2mod-voice] IXHV2Engine::UnregisterRemoteTalker  (a1 = %X, a2 = %X)",
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

	//TRACE_GAME_N("[h2mod-voice] - GetRemoteTalkers");
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

	if (xuidIsTalkingMap[xFakeXuid[0]])
	{
		//TRACE_GAME_N("[h2mod-voice] GetDataReadyFlags - called");
		ret = 0x1;
	}
	// 0x1 = user 0
	// 0xF = user 0-3
	// 0xFF = user 0-7
	return ret;
}


HRESULT IXHV2ENGINE::GetLocalChatData(VOID *pThis, DWORD dwUserIndex, PBYTE pbData, PDWORD pdwSize, PDWORD pdwPackets)
{
    *pdwPackets = *pdwSize / XHV_VOICECHAT_MODE_PACKET_SIZE;

	if (*pdwPackets > XHV_MAX_VOICECHAT_PACKETS)
		*pdwPackets = XHV_MAX_VOICECHAT_PACKETS;

	if (pdwSize && pdwPackets && xuidIsTalkingMap[xFakeXuid[0]])
	{
		//char dummy_data[0xC] = { 0x01 };
		//memset(dummy_data, 0x00, 0xC);
		//*(XUID*)&dummy_data = xFakeXuid[0];
		//*(int*)(&dummy_data+4) = rand();
		//memcpy(pbData, dummy_data, 0x0C);
		//*pdwSize = 0xC;
		//*pdwPackets = 1;

		return S_OK;
	}
	else
	{
		if (pdwSize) *pdwSize = 0;
		if (pdwPackets) *pdwPackets = 0;
	}

	return E_PENDING;
}



HRESULT IXHV2ENGINE::SetPlaybackPriority(VOID *pThis, XUID xuidRemoteTalker, DWORD dwUserIndex, int a3)
{
	return S_OK;
}


HRESULT IXHV2ENGINE::SubmitIncomingChatData(VOID *pThis, XUID xuidRemoteTalker, const BYTE* pbData, PDWORD pdwSize)
{
	XUID test = *(XUID*)pbData;
	//TRACE_GAME_N("[h2mod-voice][SubmitIncomingChatData] - XUID in packet: %lld", test);
	//TRACE_GAME_N("[h2mod-voice][SubmitIncomingChatData] - Trying to unmute tsID: %i  xuid: %lld", tsID, xuidRemoteTalker);

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

