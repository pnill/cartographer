#pragma once

#include "Globals.h"

class CAudioDevice
{
public:
	PaError				CAudioErr = paNoError;
	bool				defaultInputFound = false;
	bool				defaultOutputFound = false;

	CAudioDevice();
	~CAudioDevice();

private:

	int					totalDevices;


	struct
	{
		PaDeviceIndex		Index;
		const PaDeviceInfo	*PaInfo = nullptr;
		PaError				deviceError = paNoError;
	}
	defaultInputDevice, defaultOutputDevice;
	PaStreamParameters  inputParameters, outputParameters;

};

