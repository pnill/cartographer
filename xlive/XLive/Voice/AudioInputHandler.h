#pragma once

#include "Globals.h"
#include "AudioDevice.h"

class CAudioInputHandler
{
	CAudioInputHandler();


	~CAudioInputHandler();

private:
	PaStreamParameters  inputParameters;
	PaStream*           stream;

};