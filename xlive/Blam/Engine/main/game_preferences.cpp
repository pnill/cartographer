#include "stdafx.h"
#include "game_preferences.h"

e_language get_current_language(void)
{
	return INVOKE(0x381FD, 0x2FAA6, get_current_language);
}

void __cdecl global_preferences_initialize(void)
{
	INVOKE(0x325FD, 0x25E06, global_preferences_initialize);
	return;
}

void __cdecl game_preferences_flag_dirty()
{
	INVOKE(0x323D2, 0x25994, game_preferences_flag_dirty);
}