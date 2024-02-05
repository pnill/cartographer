#include "stdafx.h"
#include "game_state_procs.h"

#include "Blam/Engine/main/interpolator.h"



typedef void(__cdecl* game_state_call_after_load_procs_t)(int32);
game_state_call_after_load_procs_t p_game_state_call_after_load_procs;
void __cdecl game_state_call_after_load_procs_hook(int32 a1)
{
	p_game_state_call_after_load_procs(a1);
	halo_interpolator_clear_buffers();
}

void game_state_procs_apply_patches(void)
{
	DETOUR_ATTACH(p_game_state_call_after_load_procs, Memory::GetAddress<game_state_call_after_load_procs_t>(0x8C269, 0xCB93A), game_state_call_after_load_procs_hook);
	return;
}
