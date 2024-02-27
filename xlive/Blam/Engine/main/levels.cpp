#include "stdafx.h"
#include "levels.h"

// unlocks all single player maps
int32 __cdecl get_last_single_player_level_id_unlocked_from_profile(void);


void levels_apply_patches(void)
{
	PatchCall(Memory::GetAddress(0x2422C8), get_last_single_player_level_id_unlocked_from_profile);
	return;
}

int32 __cdecl get_last_single_player_level_id_unlocked_from_profile(void)
{
	return 805; // return the id of the last level
}
