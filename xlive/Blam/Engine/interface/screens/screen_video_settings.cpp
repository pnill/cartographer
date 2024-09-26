#include "stdafx.h"
#include "screen_video_settings.h"

void* c_screen_video_settings::load(s_screen_parameters* parameters)
{
	return INVOKE(0x21EDC7, 0x0, c_screen_video_settings::load, parameters);
}
