
#include "AudioDevices.h"
#include "H2MOD\Modules\OnScreenDebug\OnscreenDebug.h"

CAudioDevices::CAudioDevices()
{
	m_totalDevices = Pa_GetDeviceCount();
	if (m_totalDevices < 0)
	{
		TRACE_N("[PortAudio-API] ERROR: Pa_GetDeviceCount returned 0x%x", m_totalDevices);
		m_CAudioDeviceErr = m_totalDevices;
		return;
	}

	m_audioDeviceInfoArray = new DeviceInfo[m_totalDevices];

	for (int i = 0; i < m_totalDevices; i++)
	{
		const PaDeviceInfo* deviceInfo;
		deviceInfo = Pa_GetDeviceInfo(i);

		if (i == Pa_GetDefaultInputDevice())
		{
			TRACE_N("[PortAudio-API] INFO: Found default input device, index: %i", i);
			m_defaultInputDeviceIndex = i;
		}
		else if (i == Pa_GetDefaultOutputDevice())
		{
			TRACE_N("[PortAudio-API] INFO: Found default output device, index: %i", i);
			m_defaultOutputDeviceIndex = i;
		}

		DeviceInfo *device = &m_audioDeviceInfoArray[i];

		device->deviceInfo = deviceInfo;
		device->hostApiInfo = Pa_GetHostApiInfo(deviceInfo->hostApi);

		/* Get the device type */
		if (device->deviceInfo->maxInputChannels > 0)
			device->deviceType = DeviceType::Input;

		else if (device->deviceInfo->maxOutputChannels > 0)
			device->deviceType = DeviceType::Output;

		device->deviceError = paNoError;
	}

	if (!IsDeviceAvailable(Input))
		TRACE_N("[PortAudio-API] INFO: No audio input device found. Voice chat will not work!");

	if (!IsDeviceAvailable(Output))
		TRACE_N("[PortAudio-API] INFO: No audio output device found.");

	TRACE_N("[PortAudio-API] INFO: Total numver of audio devices = %d", m_totalDevices);
}

void CAudioDevices::SelectAudioOutput()
{

}

void CAudioDevices::SelectAudioInput()
{

}


CAudioDevices::~CAudioDevices()
{
	delete[] m_audioDeviceInfoArray;
}
