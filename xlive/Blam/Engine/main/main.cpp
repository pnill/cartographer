#include "stdafx.h"
#include "main.h"

bool __cdecl cinematic_sound_sync_complete(void)
{
	return INVOKE(0x39480, 0x40FA1, cinematic_sound_sync_complete);
}

void main_game_reset_map()
{
	// ### TODO server offset
	*Memory::GetAddress<bool*>(0x48224E, 0x1FA4E) = true;
}
