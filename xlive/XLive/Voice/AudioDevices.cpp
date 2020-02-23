
#if COMPILE_WITH_VOICE

#include "AudioDevices.h"
#include "AudioHandler.h"

#include "H2MOD/Modules/Startup/Startup.h"

CAudioDevices::CAudioDevices()
{
	m_deviceCount = Pa_GetDeviceCount();
	if (m_deviceCount < 0)
	{
		LOG_TRACE(voice_log, "Pa_GetDeviceCount returned {:x}", m_deviceCount);
		m_lastDeviceErr = m_deviceCount;
		return;
	}

	m_audioDeviceInfoArray = new DeviceInfo[m_deviceCount];

	for (int i = 0; i < m_deviceCount; i++)
	{
		const PaDeviceInfo* deviceInfo = nullptr;
		deviceInfo = Pa_GetDeviceInfo(i);

		if (i == Pa_GetDefaultInputDevice())
		{
			LOG_TRACE(voice_log, "Found default input device, index: {}", i);
			m_defaultInputDeviceIndex = i;
		}
		else if (i == Pa_GetDefaultOutputDevice())
		{
			LOG_INFO(voice_log, "Found default output device, index: {}", i);
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
		LOG_ERROR(voice_log, "No audio input device found. Voice chat will not work!");

	if (!IsDeviceAvailable(Output))
		LOG_ERROR(voice_log, "No audio output device found.");

	LOG_INFO(voice_log, "Total numver of audio devices = {}", m_deviceCount);
	LOG_INFO(voice_log, "CAudioDevices: Successful initialization!");
}

CAudioDevices::~CAudioDevices()
{
	delete[] m_audioDeviceInfoArray;
}


void CAudioDevices::SelectAudioOutput()
{

}

void CAudioDevices::SelectAudioInput()
{

}

#endif