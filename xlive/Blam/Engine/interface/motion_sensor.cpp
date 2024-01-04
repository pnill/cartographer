#include "stdafx.h"
#include "motion_sensor.h"

#include "Blam/Engine/game/game_time.h"
#include "Blam/Engine/game/players.h"
#include "Util/Hooks/Hook.h"

#define k_radar_size_multiplier 1.f

real32 g_motion_sensor_delta_accumulator = 0.f;

void __cdecl motion_sensor_update_examine_nearby_players()
{
	INVOKE(0x22BAE6, 0, motion_sensor_update_examine_nearby_players);
}

void __cdecl motion_sensor_update_sort_examined_objects()
{
	INVOKE(0x22BE3A, 0, motion_sensor_update_sort_examined_objects);
}

void __cdecl motion_sensor_update_examine_near_by_units()
{
	INVOKE(0x22BEAD, 0, motion_sensor_update_examine_near_by_units);
}

void __cdecl motion_sensor_update_move_examined_objects_onto_sample_array(int local_user_index)
{
	INVOKE(0x22C11C, 0, motion_sensor_update_move_examined_objects_onto_sample_array, local_user_index);
}

s_motion_sensor_globals* get_motion_sensor_globals()
{
	return *Memory::GetAddress< s_motion_sensor_globals**>(0x97710, 0);
}

real32* get_motion_sensor_doppler_scale()
{
	return Memory::GetAddress<real32*>(0x97710C, 0);
}

void set_motion_sensor_doppler_scale(real32 value)
{
	*Memory::GetAddress<real32*>(0x97710C, 0) = value;
}

void __cdecl motion_sensor_update()
{
	INVOKE(0x22C882, 0, motion_sensor_update);
}

void motion_sensor_update_with_delta(real32 delta)
{

	//motion_sensor_update();

	//if (delta > 0.f)
	//{
	//	s_motion_sensor_globals* motion_sensor_globals = get_motion_sensor_globals();

	//	real32 passed_tick_time = fmodf(game_ticks_to_seconds(time_globals::get_game_time()), 2.099999904632568f);
	//	if (passed_tick_time >= 2.0374999f)
	//	{
	//		set_motion_sensor_doppler_scale(passed_tick_time);
	//	}
	//	else
	//	{
	//		real32 scaled_tick_time = 1.0f / (passed_tick_time + 0.0625) * 1.1f;
	//		set_motion_sensor_doppler_scale(scaled_tick_time);
	//	}

	//	if (motion_sensor_globals->field_BC4)
	//	{
	//		motion_sensor_update_examine_nearby_players();
	//		//g_motion_sensor_delta_accumulator -= delta;
	//		motion_sensor_globals->field_BC4--;// = time_globals::seconds_to_ticks_round(g_motion_sensor_delta_accumulator);
	//	}
	//	else
	//	{
	//		motion_sensor_update_sort_examined_objects();
	//		motion_sensor_update_examine_nearby_players();
	//		motion_sensor_update_examine_near_by_units();
	//		//g_motion_sensor_delta_accumulator = 0.5f;
	//		motion_sensor_globals->field_BC4 = time_globals::seconds_to_ticks_round(0.5);
	//	}

	//	if (motion_sensor_globals->current_sample_index)
	//	{
	//		motion_sensor_globals->current_sample_index--;
	//	}
	//	else
	//	{
	//		motion_sensor_globals->current_sample_index = 9;
	//	}


	//	for (int i = 0; i < 4; i++)
	//	{
	//		if (players_user_is_active(i))
	//			motion_sensor_update_move_examined_objects_onto_sample_array(i);
	//	}
	//}
}

void radar_patch()
{
	WriteValue<byte>(Memory::GetAddress(0x2849C4), (byte)D3DBLEND_INVSRCCOLOR);
}

void motion_sensor_apply_patches()
{
	if (Memory::IsDedicatedServer()) { return; }

	radar_patch();
	motion_sensor_fix_size();

	// Remove hud motion sensor update from game tick
	//NopFill(Memory::GetAddress(0x220c8f), 5);
}

void motion_sensor_fix_size()
{
	float og_res = *Memory::GetAddress<float*>(0x3E6A9C);
	WriteValue<float>(Memory::GetAddress(0x3E6A9C), og_res * k_radar_size_multiplier);
}