
#if COMPILE_WITH_VOICE

#include "AudioDevices.h"
#include "AudioHandler.h"
#include "H2MOD\Modules\OnScreenDebug\OnscreenDebug.h"

CAudioDevices::CAudioDevices()
{
	if (h2mod->Server)
		return;

	m_totalDevices = Pa_GetDeviceCount();
	if (m_totalDevices < 0)
	{
		LOG_TRACE(CAudioHandler::logger, "Pa_GetDeviceCount returned {:x}", m_totalDevices);
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
			LOG_TRACE(CAudioHandler::logger, "Found default input device, index: {}", i);
			m_defaultInputDeviceIndex = i;
		}
		else if (i == Pa_GetDefaultOutputDevice())
		{
			LOG_TRACE(CAudioHandler::logger, "Found default output device, index: {}", i);
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
		LOG_TRACE(CAudioHandler::logger, "No audio input device found. Voice chat will not work!");

	if (!IsDeviceAvailable(Output))
		LOG_TRACE(CAudioHandler::logger, "No audio output device found.");

	LOG_TRACE(CAudioHandler::logger, "Total numver of audio devices = {}", m_totalDevices);
	LOG_TRACE(CAudioHandler::logger, "CAudioDevices: Successful initialization!");
}

void CAudioDevices::SelectAudioOutput()
{

}

void CAudioDevices::SelectAudioInput()
{

}


CAudioDevices::~CAudioDevices()
{
	if (h2mod->Server)
		return;

	delete[] m_audioDeviceInfoArray;
}

#endif