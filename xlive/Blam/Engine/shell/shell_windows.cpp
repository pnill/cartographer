#include "stdafx.h"
#include "shell_windows.h"

bool __cdecl game_is_minimized(void)
{
	return INVOKE(0x28729, 0x248AB, game_is_minimized);
}
