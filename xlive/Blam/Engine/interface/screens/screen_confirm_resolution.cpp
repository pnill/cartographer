#include "stdafx.h"
#include "screen_confirm_resolution.h"

void* __cdecl c_screen_confirm_resolution::load(c_screen_parameters* parameters)
{
    return INVOKE(0x24BAC8, 0x0, c_screen_confirm_resolution::load, parameters);
}
