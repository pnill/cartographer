#include "stdafx.h"
#include "screen_audio_settings.h"

void* c_screen_audio_settings::load(s_screen_parameters* parameters)
{
	return INVOKE(0x21ED20, 0x0, c_screen_audio_settings::load, parameters);
}
