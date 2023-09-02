#include "stdafx.h"
#include "aim_assist.h"

#include "H2MOD.h"
#include "Blam/Engine/game/game_globals.h"
#include "Util/Hooks/Hook.h"

// sword-flying target clear patch
void __cdecl aim_assist_targeting_clear_hook(s_aim_assist_targetting_data* target_data)
{
	if (!s_main_game_globals::game_is_campaign() && !xbox_tickrate_is_enabled())
	{
		target_data->primary_auto_aim_level = 0.0;
		target_data->secondary_auto_aim_level = 0.0;
		target_data->target_object = -1;
		target_data->model_target = -1;
		target_data->target_player = -1;
		target_data->auto_aim_flags = 0;
	}
}

void aim_assist_apply_patches()
{
	PatchCall(Memory::GetAddress(0x169E59), aim_assist_targeting_clear_hook);
}