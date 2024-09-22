#include "stdafx.h"
#include "screen_bungie_news.h"

void* c_screen_bungie_news::load(s_screen_parameters* parameters)
{
	return INVOKE(0xB101, 0x0, c_screen_bungie_news::load, parameters);
}
