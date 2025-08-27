#include "stdafx.h"
#include "game_engine_default.h"

e_game_engine_type c_game_engine::get_type()
{
	return _game_engine_type_none;
}

bool c_game_engine::setup()
{
	return false;
}

bool c_game_engine::cleanup()
{
	return false;
}

bool c_game_engine::function_4()
{
	return true;
}

bool c_game_engine::verify_netpoint(uint32 netpoint_index)
{
	return true;
}

void c_game_engine::player_join(datum player_index)
{

}

void c_game_engine::player_leave(datum player_index)
{

}

void c_game_engine::player_rejoin(datum player_index)
{

}

void c_game_engine::swap_player_indices(uint32 old_index, uint32 new_index)
{

}

void c_game_engine::player_team_change(datum player_index)
{

}

void c_game_engine::game_start()
{

}

void c_game_engine::function_14(datum player_index)
{

}

bool c_game_engine::player_can_interact_with_weapon(datum player_index, datum weapon_index)
{
	return true;
}

void c_game_engine::handle_player_objective_touch_interaction(datum player_index, datum object_index)
{

}

void c_game_engine::update()
{

}

real32 c_game_engine::get_player_speed_modifier(datum player_index)
{
	return 1.f;
}

uint32 c_game_engine::function_21(datum object_index)
{
	LOG_INFO_GAME("[{}]", __FUNCTION__);
	return UINT_MAX;
}

void c_game_engine::update_object_color_change(datum object_index)
{

}

void c_game_engine::function_23(datum index)
{

}

void c_game_engine::handle_object_taken_event(datum weapon_index, datum biped_index)
{
}

void c_game_engine::handle_object_dropped_event(datum weapon_index, datum biped_index)
{

}

int32 c_game_engine::get_sudden_death_timer(int32 time_remaining_in_ticks, bool unk, bool unk_2)
{
	return time_remaining_in_ticks;
}

e_game_team c_game_engine::get_primary_team_index()
{
	return _game_team_observer;
}

bool c_game_engine::is_team_enemy(e_game_team a, e_game_team b)
{
	return a != b;
}

void c_game_engine::get_multiplayer_score_string(wchar_t* out_string)
{
	INVOKE_TYPE(0x111251, 0x1001A9, void(__thiscall*)(c_game_engine*, wchar_t*), this, out_string);
}

void c_game_engine::function_30(int32 a1, int32 a2, int32 a3)
{
}

int32 c_game_engine::get_player_killed_event_id(datum player_index, datum player_index_2, bool a3)
{
	return NONE;
}

void c_game_engine::function_33(datum player_index, void* unk)
{

}

bool c_game_engine::function_34(datum player_index, void* unk)
{
	return true;
}

bool c_game_engine::test_variant_engine_flag(datum player_index, e_game_engine_variant_flag_test_type type)
{
	return INVOKE_TYPE(0x1112D1, 0x100229, bool(__thiscall*)(c_game_engine*, datum, e_game_engine_variant_flag_test_type), this, player_index, type);
}

void c_game_engine::function_37(int32 unk_always_1)
{

}

void c_game_engine::get_player_state_index(datum player_index, bool* always_returned_true)
{
	*always_returned_true = true;
}

bool c_game_engine::should_garbage_collect(datum object_index)
{
	return false;
}

e_simulation_entity_type c_game_engine::get_game_engine_entity_type()
{
	return _simulation_entity_type_slayer_engine_globals;
}

void c_game_engine::function_42(void* unk)
{
	INVOKE_TYPE(0x1112A7, 0x1001FF, void(__thiscall*)(c_game_engine*, void*), this, unk);
}

void c_game_engine::function_43(uint32 flags, void* unk, void* unk_2)
{
	INVOKE_TYPE(0x111750, 0x1006A8, void(__thiscall*)(c_game_engine*, uint32, void*, void*), this, flags, unk, unk_2);
}

bool c_game_engine::function_44(uint32 flags, void* unk)
{
	return INVOKE_TYPE(0x111965, 0x1008BD, bool(__thiscall*)(c_game_engine*, uint32, void*), this, flags, unk);
}

void c_game_engine::set_player_simulation_baseline_data(int32 unused, int32 unused_2, void* unk)
{
	INVOKE_TYPE(0x11127D, 0x1001D5, void(__thiscall*)(c_game_engine*, int32, int32, void*), this, unused, unused_2, unk);
}

void c_game_engine::build_player_simulation_update(int16 abs_player_index, int32* update_mask, int32 unused, void* unk_2)
{
	INVOKE_TYPE(0x11131C, 0x100274, void(__thiscall*)(c_game_engine*, int16, int32*, int32, void*), this, abs_player_index, update_mask, unused, unk_2);
}

bool c_game_engine::apply_player_simulation_update(int16 abs_player_index, int32* update_mask, int32 unused, void* unk)
{
	return INVOKE_TYPE(0x111541, 0x100499, bool(__thiscall*)(c_game_engine*, int16, int32*, int32, void*), this, abs_player_index, update_mask, unused, unk);
}

uint32 c_game_engine::get_territory_name(wchar_t* a1, int32 a2, int32 a3, wchar_t* a4, wchar_t* a5)
{
	return 0;
}