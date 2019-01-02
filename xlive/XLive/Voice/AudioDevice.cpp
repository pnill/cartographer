
#include "H2MOD\Modules\OnScreenDebug\OnscreenDebug.h"
#include "AudioInputHandler.h"
#include "AudioDevice.h"



CAudioDevice::CAudioDevice()
{
	CAudioErr = Pa_Initialize();

	if (CAudioErr != PaErrorCode::paNoError)
	{
		TRACE_N("[PortAudio-API] ERROR: PortAudio failed to initialize! Voicechat turned off!");
		return;
	}

	/* Local variables */
	const PaDeviceInfo* deviceInfo;


	TRACE_N("[PortAudio-API] Info: PostAudio version: 0x%08X", Pa_GetVersion());
	TRACE_N("[PortAudio-API] Info: Version text: %s", Pa_GetVersionInfo()->versionText);

	totalDevices = Pa_GetDeviceCount();
	if (totalDevices < 0)
	{
		TRACE_N("[PortAudio-API] ERROR: Pa_GetDeviceCount returned 0x%x", totalDevices);
		CAudioErr = totalDevices;
		return;
	}

	TRACE_N("[PortAudio-API] Info: Number of devices = %d", totalDevices);

	for (int i = 0; i < totalDevices; i++)
	{
		deviceInfo = Pa_GetDeviceInfo(i);
		const PaHostApiInfo *hostInfo = Pa_GetHostApiInfo(deviceInfo->hostApi);

		if (i == hostInfo->defaultInputDevice && defaultInputFound == false)
		{
			char deviceName[512];
			sprintf_s(deviceName, "[PortAudio-API] Info: Found default input device at index %i, with the name %s", i, deviceInfo->name);
			addDebugText(deviceName);

			TRACE_N("[PortAudio-API] Info: Found default input device at index %i, with the name %s", i, deviceInfo->name);

			defaultInputDevice.Index = (PaDeviceIndex)i;
			defaultInputDevice.PaInfo = Pa_GetDeviceInfo(i);
			defaultInputFound = true;

			continue;
		}

		else if (i == hostInfo->defaultOutputDevice && defaultOutputFound == false)
		{
			char deviceName[512];
			sprintf_s(deviceName, "[PortAudio-API] Info: Found default output device at index %i, with the name %s", i, deviceInfo->name);
			addDebugText(deviceName);

			TRACE_N("[PortAudio-API] Info: Found default output device at index %i, with the name %s", i, deviceInfo->name);


			defaultOutputDevice.Index = (PaDeviceIndex)i;
			defaultOutputDevice.PaInfo = Pa_GetDeviceInfo(i);
			defaultOutputFound = true;

			continue;
		}
	}

	if (defaultInputFound == false || defaultOutputFound == false)
	{
		CAudioErr = PaErrorCode::paDeviceUnavailable;
		TRACE_N("[PortAudio-API] Info: No input or output devices found");
	}
}

CAudioDevice::~CAudioDevice()
{
	PaError err = Pa_Terminate();
	if (err != PaErrorCode::paNoError)
		TRACE_N("[PortAudio-API] ERROR: Pa_Terminate failed, error code: %i", err);
}
