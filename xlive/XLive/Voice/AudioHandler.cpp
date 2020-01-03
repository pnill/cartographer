
#if COMPILE_WITH_VOICE

#include "AudioHandler.h"

h2log* CAudioHandler::logger = nullptr;

CAudioHandler::CAudioHandler(CAudioDevices* pAudioDevice)
{
	if (logger == nullptr)
		logger = h2log::create_console("PortAudio");
	m_CAudioErr = Pa_Initialize();

	if (m_CAudioErr != PaErrorCode::paNoError)
	{
		LOG_TRACE(logger, "Failed to initialize! Error code: {}", m_CAudioErr);
		return;
	}

	LOG_TRACE(logger, "PostAudio version: {:x}", Pa_GetVersion());
	LOG_TRACE(logger, "Version text: {}", Pa_GetVersionInfo()->versionText);

	audioDevices = pAudioDevice = new CAudioDevices;

	if (audioDevices->GetAudioClassError() != paNoError) {
		LOG_TRACE(logger, "CAudioDevices object failed to initialize, aborting.");
		return;
	}
}


CAudioHandler::~CAudioHandler()
{
	delete audioDevices;

	PaError err = Pa_Terminate();
	if (err != PaErrorCode::paNoError)
		LOG_TRACE(logger, "Pa_Terminate failed, error code: {}", err);
}
#endif
