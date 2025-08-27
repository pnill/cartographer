#include "stdafx.h"
#include "game_engine_territories.h"

e_game_engine_type c_territories_engine::get_type()
{
	return _game_engine_type_territories;
}

bool c_territories_engine::setup()
{
	return INVOKE_TYPE(0x10C9F1, 0xD8FA7, bool(__thiscall*)(c_game_engine*), this);
}

bool c_territories_engine::function_4()
{
	return INVOKE_TYPE(0x10DB06, 0xDA0B6, bool(__thiscall*)(c_game_engine*), this);
}

void c_territories_engine::send_game_start_event(datum player_index)
{
	INVOKE_TYPE(0x10C916, 0xD8ECC, void(__thiscall*)(c_game_engine*, datum), this, player_index);
}

void c_territories_engine::swap_player_indices(uint32 old_index, uint32 new_index)
{
	INVOKE_TYPE(0x10C948, 0xD8EFF, void(__thiscall*)(c_game_engine*, uint32, uint32), this, old_index, new_index);
}

void c_territories_engine::player_team_change(datum player_index)
{
	INVOKE_TYPE(0x10DB83, 0xDA133, void(__thiscall*)(c_game_engine*, datum), this, player_index);
}

void c_territories_engine::function_13()
{
	INVOKE_TYPE(0x10CABE, 0xD9074, void(__thiscall*)(c_game_engine*), this);
}

void c_territories_engine::function_14(datum player_index)
{
	return;
}

void c_territories_engine::render_game_engine_hud_elements(uint32 user_index)
{
	INVOKE_TYPE(0x10CDCF, 0xD9385, void(__thiscall*)(c_game_engine*, uint32), this, user_index);
}

void c_territories_engine::function_16(datum player_index)
{
	return;
}

bool c_territories_engine::player_can_interact_with_weapon(datum player_index, datum weapon_index)
{
	return INVOKE_TYPE(0x10C94B, 0xD8F01, bool(__thiscall*)(c_game_engine*, datum, datum), this, player_index, weapon_index);
}

void c_territories_engine::update()
{
	INVOKE_TYPE(0x10DE8A, 0xDA43A, void(__thiscall*)(c_game_engine*), this);
}

void c_territories_engine::update_object_color_change(datum object_index)
{
	INVOKE_TYPE(0x10D807, 0xD9DB7, void(__thiscall*)(c_game_engine*, datum), this, object_index);
}

void c_territories_engine::player_killed(datum killing_player, datum killed_player, bool suicide, int32 unk_index)
{
	return;
}

void c_territories_engine::function_33(datum player_index, void* unk)
{
	INVOKE_TYPE(0x10CC5A, 0xD9210, void(__thiscall*)(c_game_engine*, datum, void*), this, player_index, unk);
}

bool c_territories_engine::function_35(int32 unk_index)
{
	return false;
}

e_simulation_entity_type c_territories_engine::get_game_engine_entity_type()
{
	return _simulation_entity_type_territories_engine_globals;
}

void c_territories_engine::function_41()
{
	return;
}

void c_territories_engine::set_simulation_baseline_data(int32 unused, void* state_data)
{
	INVOKE_TYPE(0x10C9AF, 0xD8F65, void(__thiscall*)(c_game_engine*, int32, void*), this, unused, state_data);
}

void c_territories_engine::build_simulation_update(uint32* unk, int32 unused, void* state_data)
{
	INVOKE_TYPE(0x10D420, 0xD99D0, void(__thiscall*)(c_game_engine*, uint32*, int32, void*), this, unk, unused, state_data);
}

bool c_territories_engine::apply_simulation_update(uint32 flags, int32 unused, void* state_data)
{
	return INVOKE_TYPE(0x10D815, 0xD9DC5, bool(__thiscall*)(c_game_engine*, uint32, int32, void*), this, flags, unused, state_data);
}

uint32 c_territories_engine::get_territory_name(wchar_t* a1, int32 a2, int32 a3, wchar_t* a4, wchar_t* a5)
{
	return INVOKE_TYPE(0x10D302, 0xD98B8, uint32(__thiscall*)(c_game_engine*, wchar_t*, int32, int32, wchar_t*, wchar_t*), this, a1, a2, a3, a4, a5);
}