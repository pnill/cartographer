
#include "AudioHandler.h"

CAudioHandler::CAudioHandler(CAudioDevices* pAudioDevice) 
{
	if (h2mod->Server)
		return;

	m_CAudioErr = Pa_Initialize();

	if (m_CAudioErr != PaErrorCode::paNoError)
	{
		TRACE_N("[PortAudio-API] ERROR: PortAudio failed to initialize! Error code: %i", m_CAudioErr);
		return;
	}

	TRACE_N("[PortAudio-API] INFO: PostAudio version: 0x%08X", Pa_GetVersion());
	TRACE_N("[PortAudio-API] INFO: Version text: %s", Pa_GetVersionInfo()->versionText);

	audioDevices = pAudioDevice = new CAudioDevices;

	if (audioDevices->GetAudioClassError() != paNoError) {
		TRACE_N("CAudioHandler: CAudioDevices object failed to initialize, aborting.");
		return;
	}


}


CAudioHandler::~CAudioHandler()
{
	if (h2mod->Server)
		return;

	delete audioDevices;

	PaError err = Pa_Terminate();
	if (err != PaErrorCode::paNoError)
		TRACE_N("[PortAudio-API] ERROR: Pa_Terminate failed, error code: %i", err);
}