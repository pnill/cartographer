#include "stdafx.h"
#include "screen_bungie_news.h"

#include "interface/user_interface.h"

/* public code */

void* __cdecl c_screen_bungie_news::load(c_screen_parameters* parameters)
{
	return INVOKE(0xB101, 0x0, c_screen_bungie_news::load, parameters);
}
