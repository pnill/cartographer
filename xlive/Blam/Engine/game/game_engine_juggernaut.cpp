#include "stdafx.h"
#include "game_engine_juggernaut.h"

e_game_engine_type c_juggernaut_engine::get_type()
{
	return _game_engine_type_juggernaut;
}

bool c_juggernaut_engine::setup()
{
	return INVOKE_TYPE(0xD3231, 0xCF7F1, bool(__thiscall*)(c_game_engine*), this);
}

void c_juggernaut_engine::player_join(datum player_index)
{
	INVOKE_TYPE(0xD3872, 0xCFE32, void(__thiscall*)(c_game_engine*, datum), this, player_index);
}

void c_juggernaut_engine::send_game_start_event(datum player_index)
{
	INVOKE_TYPE(0xD3249, 0xCF809, void(__thiscall*)(c_game_engine*, datum), this, player_index);
}

void c_juggernaut_engine::player_leave(datum player_index)
{
	INVOKE_TYPE(0xD38A2, 0xCFE62, void(__thiscall*)(c_game_engine*, datum), this, player_index);
}

void c_juggernaut_engine::swap_player_indices(uint32 old_index, uint32 new_index)
{
	INVOKE_TYPE(0xD327E, 0xCF83E, void(__thiscall*)(c_game_engine*, uint32, uint32), this, old_index, new_index);
}

void c_juggernaut_engine::function_13()
{
	INVOKE_TYPE(0xD327D, 0xCF83D, void(__thiscall*)(c_game_engine*), this);
}

void c_juggernaut_engine::function_14(datum player_index)
{
	INVOKE_TYPE(0xD32DE, 0xCF89E, void(__thiscall*)(c_game_engine*, datum), this, player_index);
}

void c_juggernaut_engine::render_game_engine_hud_elements(uint32 user_index)
{
	INVOKE_TYPE(0xD358E, 0xCFB4E, void(__thiscall*)(c_game_engine*, uint32), this, user_index);
}

void c_juggernaut_engine::function_16(datum player_index)
{
	INVOKE_TYPE(0xD33F9, 0xCF9B9, void(__thiscall*)(c_game_engine*, datum), this, player_index);
}

void c_juggernaut_engine::update()
{
	INVOKE_TYPE(0xD3A71, 0xD0031, void(__thiscall*)(c_game_engine*), this);
}

real32 c_juggernaut_engine::get_player_speed_modifier(datum player_index)
{
	return INVOKE_TYPE(0xD3463, 0xCFA23, real32(__thiscall*)(c_game_engine*, datum), this, player_index);
}

bool c_juggernaut_engine::is_team_enemy(e_game_team a, e_game_team b)
{
	return INVOKE_TYPE(0xD3281, 0xCF841, bool(__thiscall*)(c_game_engine*, e_game_team, e_game_team), this, a, b);
}

void c_juggernaut_engine::player_killed(datum killing_player, datum killed_player, bool suicide, int32 unk_index)
{
	INVOKE_TYPE(0xD38D2, 0xCFE92, void(__thiscall*)(c_game_engine*, datum, datum, bool, int32), this, killing_player, killed_player, suicide, unk_index);
}

bool c_juggernaut_engine::function_35(int32 unk_index)
{
	return unk_index == 1;
}

bool c_juggernaut_engine::test_variant_engine_flag(datum player_index, e_game_engine_variant_flag_test_type type)
{
	return INVOKE_TYPE(0xD34E0, 0xCFAA0, bool(__thiscall*)(c_game_engine*, datum, e_game_engine_variant_flag_test_type), this, player_index, type);
}

void c_juggernaut_engine::get_player_state_index(datum player_index, bool* always_returned_true)
{
	INVOKE_TYPE(0xD3564, 0xCFB24, void(__thiscall*)(c_game_engine*, datum, bool*), this, player_index, always_returned_true);
}

e_simulation_entity_type c_juggernaut_engine::get_game_engine_entity_type()
{
	return _simulation_entity_type_juggernaut_engine_globals;
}

void c_juggernaut_engine::function_41()
{
	return;
}

void c_juggernaut_engine::set_simulation_baseline_data(int32 unused, void* state_data)
{
	INVOKE_TYPE(0xD32E8, 0xCF8A8, void(__thiscall*)(c_game_engine*, int32, void*), this, unused, state_data);
}

void c_juggernaut_engine::build_simulation_update(uint32* unk, int32 unused, void* state_data)
{
	INVOKE_TYPE(0xD331A, 0xCF8DA, void(__thiscall*)(c_game_engine*, uint32*, int32, void*), this, unk, unused, state_data);
}

bool c_juggernaut_engine::apply_simulation_update(uint32 flags, int32 unused, void* state_data)
{
	return INVOKE_TYPE(0xD3676, 0xCFC36, bool(__thiscall*)(c_game_engine*, uint32, int32, void*), this, flags, unused, state_data);
}