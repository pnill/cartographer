#pragma once

#include "Globals.h"
#include "AudioDevices.h"

/* Select sample format. */
#if 0
#define PA_SAMPLE_TYPE  paFloat32
typedef float SAMPLE;
#define SAMPLE_SILENCE 0.0f
#define PRINTF_S_FORMAT "%.8f"
#elif 0
#define PA_SAMPLE_TYPE  paInt16
typedef short SAMPLE;
#define SAMPLE_SILENCE 0
#define PRINTF_S_FORMAT "%d"
#elif 0
#define PA_SAMPLE_TYPE  paInt8
typedef char SAMPLE;
#define SAMPLE_SILENCE 0
#define PRINTF_S_FORMAT "%d"
#else
#define PA_SAMPLE_TYPE  paUInt8
typedef unsigned char SAMPLE;
#define SAMPLE_SILENCE 128
#define PRINTF_S_FORMAT "%d"
#endif


class CAudioHandler
{
public:
	CAudioHandler(CAudioDevices*);

	CAudioDevices* audioDevices;
	PaError m_CAudioErr;
	

	~CAudioHandler();

private:
	PaStreamParameters  inputParameters, outputParameters;
	PaStream*           stream;

};