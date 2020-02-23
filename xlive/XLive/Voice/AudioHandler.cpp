
#if COMPILE_WITH_VOICE

#include "AudioHandler.h"

#include "H2MOD\Modules\Startup\Startup.h"

CAudioHandler::CAudioHandler()
{
	m_CAudioErr = Pa_Initialize();

	if (m_CAudioErr != PaErrorCode::paNoError)
	{
		LOG_ERROR(voice_log, "AudioHandler failed to initialize! Error code: {}", m_CAudioErr);
		return;
	}

	LOG_INFO(voice_log, "PostAudio version: {:x}", Pa_GetVersion());
	LOG_INFO(voice_log, "Version text: {}", Pa_GetVersionInfo()->versionText);

	audioDevices = new CAudioDevices;

	if (audioDevices->GetAudioInterfaceError() != paNoError) {
		LOG_ERROR(voice_log, "CAudioDevices failed to initialize, aborting.");
		return;
	}
}

CAudioHandler::~CAudioHandler()
{
	delete audioDevices;

	PaError err = Pa_Terminate();
	if (err != PaErrorCode::paNoError)
		LOG_TRACE(voice_log, "Pa_Terminate failed, error code: {}", err);
}
#endif
