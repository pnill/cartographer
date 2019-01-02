
#include "H2MOD\Modules\OnScreenDebug\OnscreenDebug.h"
#include "AudioInputHandler.h"
#include "AudioDevices.h"



CAudioDevices::CAudioDevices()
{
	m_CAudioErr = Pa_Initialize();

	if (m_CAudioErr != PaErrorCode::paNoError)
	{
		TRACE_N("[PortAudio-API] ERROR: PortAudio failed to initialize! Error code: %i", m_CAudioErr);
		return;
	}


	TRACE_N("[PortAudio-API] INFO: PostAudio version: 0x%08X", Pa_GetVersion());
	TRACE_N("[PortAudio-API] INFO: Version text: %s", Pa_GetVersionInfo()->versionText);

	m_totalDevices = Pa_GetDeviceCount();
	if (m_totalDevices < 0)
	{
		TRACE_N("[PortAudio-API] ERROR: Pa_GetDeviceCount returned 0x%x", m_totalDevices);
		m_CAudioErr = m_totalDevices;
		return;
	}

	m_audioDeviceInfoArray = new DeviceInfo[m_totalDevices];

	for (int i = 0; i < m_totalDevices; i++)
	{
		const PaDeviceInfo* deviceInfo;
		deviceInfo = Pa_GetDeviceInfo(i);

		if (i == Pa_GetDefaultInputDevice())
		{
			TRACE_N("[PortAudio-API] INFO: Foind default input device, index: %i", i);
			m_defaultInputDeviceIndex = i;
		}
		else if (i == Pa_GetDefaultOutputDevice())
		{
			TRACE_N("[PortAudio-API] INFO: Foind default output device, index: %i", i);
			m_defaultOutputDeviceIndex = i;
		}

		DeviceInfo *device = &m_audioDeviceInfoArray[i];

		device->deviceInfo = deviceInfo;
		device->hostApiInfo = Pa_GetHostApiInfo(deviceInfo->hostApi);

		if (device->deviceInfo->maxInputChannels > 0)
			device->deviceType = DeviceType::Input;
		else
			device->deviceType = DeviceType::Output;

		device->deviceError = paNoError;
	}


	TRACE_N("[PortAudio-API] INFO: Total numver of audio devices = %d", m_totalDevices);
	
}

CAudioDevices::~CAudioDevices()
{
	PaError err = Pa_Terminate();
	if (err != PaErrorCode::paNoError)
		TRACE_N("[PortAudio-API] ERROR: Pa_Terminate failed, error code: %i", err);

	delete[] m_audioDeviceInfoArray;
}
