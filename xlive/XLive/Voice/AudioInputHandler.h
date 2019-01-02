#pragma once

#include "Globals.h"
#include "AudioDevices.h"

class CAudioInputHandler
{
	CAudioInputHandler();


	~CAudioInputHandler();

private:
	PaStreamParameters  inputParameters;
	PaStream*           stream;

};