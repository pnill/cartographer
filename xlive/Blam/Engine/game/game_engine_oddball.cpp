#include "stdafx.h"
#include "game_engine_oddball.h"

e_game_engine_type c_oddball_engine::get_type()
{
	return _game_engine_type_oddball;
}

bool c_oddball_engine::setup()
{
	return INVOKE_TYPE(0x1102FC, 0xDC8AC, bool(__thiscall*)(c_game_engine*), this);
}

void c_oddball_engine::send_game_start_event(datum player_index)
{
	INVOKE_TYPE(0x10FF31, 0xDC4E1, void(__thiscall*)(c_game_engine*, datum), this, player_index);
}

void c_oddball_engine::function_13()
{
	return;
}

void c_oddball_engine::render_game_engine_hud_elements(uint32 user_index)
{
	INVOKE_TYPE(0x110579, 0xDCB29, void(__thiscall*)(c_game_engine*, uint32), this, user_index);
}

void c_oddball_engine::function_16(datum player_index)
{
	INVOKE_TYPE(0x110781, 0xDCD31, void(__thiscall*)(c_game_engine*, datum), this, player_index);
}

void c_oddball_engine::update()
{
	INVOKE_TYPE(0x110B6C, 0xDD11C, void(__thiscall*)(c_game_engine*), this);
}

real32 c_oddball_engine::get_player_speed_modifier(datum player_index)
{
	return INVOKE_TYPE(0x1103EB, 0xDC998, real32(__thiscall*)(c_game_engine*, datum), this, player_index);
}

void c_oddball_engine::update_object_color_change(datum object_index)
{
	INVOKE_TYPE(0x10FF65, 0xDC515, void(__thiscall*)(c_game_engine*, datum), this, object_index);
}

void c_oddball_engine::handle_object_taken_event(datum weapon_index, datum biped_index)
{
	INVOKE_TYPE(0x110460, 0xDCA10, void(__thiscall*)(c_game_engine*, datum, datum), this, weapon_index, biped_index);
}

void c_oddball_engine::handle_object_dropped_event(datum weapon_index, datum biped_index)
{
	INVOKE_TYPE(0x1104F3, 0xDCAA3, void(__thiscall*)(c_game_engine*, datum, datum), this, weapon_index, biped_index);
}

void c_oddball_engine::get_multiplayer_score_string(wchar_t* out_string)
{
	INVOKE_TYPE(0x10FFF0, 0xDC5A0, void(__thiscall*)(c_game_engine*, wchar_t*), this, out_string);
}

void c_oddball_engine::player_killed(datum killing_player, datum killed_player, bool suicide, int32 unk_index)
{
	INVOKE_TYPE(0x1107E1, 0xDCD91, void(__thiscall*)(c_game_engine*, datum, datum, bool, int32), this, killing_player, killed_player, suicide, unk_index);
}

void c_oddball_engine::function_33(datum player_index, void* unk)
{
	INVOKE_TYPE(0x110005, 0xDC5B5, void(__thiscall*)(c_game_engine*, datum, void*), this, player_index, unk);
}

bool c_oddball_engine::function_35(int32 unk_index)
{
	return false;
}

bool c_oddball_engine::test_variant_engine_flag(datum player_index, e_game_engine_variant_flag_test_type type)
{
	return INVOKE_TYPE(0x110856, 0xDCE06, bool(__thiscall*)(c_game_engine*, datum, int), this, player_index, type);
}

void c_oddball_engine::get_player_state_index(datum player_index, bool* always_returned_true)
{
	INVOKE_TYPE(0x1108DC, 0xDCE8C, void(__thiscall*)(c_game_engine*, datum, bool*), this, player_index, always_returned_true);
}

e_simulation_entity_type c_oddball_engine::get_game_engine_entity_type()
{
	return _simulation_entity_type_oddball_engine_globals;
}

void c_oddball_engine::function_41()
{
	return;
}

void c_oddball_engine::set_simulation_baseline_data(int32 unused, void* state_data)
{
	INVOKE_TYPE(0x11007C, 0xDC62C, void(__thiscall*)(c_game_engine*, int32, void*), this, unused, state_data);
}

void c_oddball_engine::build_simulation_update(uint32* unk, int32 unused, void* state_data)
{
	INVOKE_TYPE(0x1100AA, 0xDC65A, void(__thiscall*)(c_game_engine*, uint32*, int32, void*), this, unk, unused, state_data);
}

bool c_oddball_engine::apply_simulation_update(uint32 flags, int32 unused, void* state_data)
{
	return INVOKE_TYPE(0x1100E6, 0xDC696, bool(__thiscall*)(c_game_engine*, uint32, int32, void*), this, flags, unused, state_data);
}