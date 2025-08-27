#include "stdafx.h"
#include "game_engine_slayer.h"

e_game_engine_type c_slayer_engine::get_type()
{
	return _game_engine_type_slayer;
}

bool c_slayer_engine::setup()
{
	return INVOKE_TYPE(0x110BD1, 0xDD181, bool(__thiscall*)(c_game_engine*), this);
}

void c_slayer_engine::player_join(datum player_index)
{
	INVOKE_TYPE(0x110C05, 0xDD185, void(__thiscall*)(c_game_engine*, datum), this, player_index);
}

void c_slayer_engine::send_game_start_event(datum player_index)
{
	INVOKE_TYPE(0x110C2B, 0xDD1DB, void(__thiscall*)(c_game_engine*, datum), this, player_index);
}

void c_slayer_engine::swap_player_indices(uint32 old_index, uint32 new_index)
{
	INVOKE_TYPE(0x110E0A, 0xDD3BA, void(__thiscall*)(c_game_engine*, uint32, uint32), this, old_index, new_index);
}

void c_slayer_engine::function_13()
{
	return;
}

void c_slayer_engine::render_game_engine_hud_elements(uint32 user_index)
{
	INVOKE_TYPE(0x111129, 0x100081, void(__thiscall*)(c_game_engine*, uint32), this, user_index);
}

void c_slayer_engine::function_16(datum player_index)
{
	return;
}

void c_slayer_engine::player_killed(datum killing_player, datum killed_player, bool suicide, int32 unk_index)
{
	INVOKE_TYPE(0x110C60, 0xDD210, void(__thiscall*)(c_game_engine*, datum, datum, bool, int32), this, killing_player, killed_player, suicide, unk_index);
}

bool c_slayer_engine::function_35(int32 unk_index)
{
	return unk_index == 1;
}

e_simulation_entity_type c_slayer_engine::get_game_engine_entity_type()
{
	return _simulation_entity_type_slayer_engine_globals;
}

void c_slayer_engine::function_41()
{
	return;
}

void c_slayer_engine::set_simulation_baseline_data(int32 unused, void* state_data)
{
	//memset(state_data, 0, sizeof(int32) * 8);
	//this->function_42(state_data);

	INVOKE_TYPE(0x110D7A, 0xDD32A, void(__thiscall*)(c_game_engine*, int32, void*), this, unused, state_data);
}

void c_slayer_engine::build_simulation_update(uint32* unk, int32 unused, void* state_data)
{
	//int32 flag_test = *(int32*)unk & 0x1F;
	//int32 unk_result = 0;
	//if(flag_test)
	//{
	//	this->function_43(flag_test, &unk_result, state_data);
	//	*(int32*)unk = unk_result;
	//}
	//else
	//{
	//	*(int32*)unk = 0;
	//}

	INVOKE_TYPE(0x110DA8, 0xDD358, void(__thiscall*)(c_game_engine*, void*, int32, void*), this, unk, unused, state_data);
}

bool c_slayer_engine::apply_simulation_update(uint32 flags, int32 unused, void* state_data)
{
	//if((flags & 0x1F) != 0)
	//{
	//	return this->function_44(flags & 0x1F, state_data) != 0;
	//}
	//return true;

	return INVOKE_TYPE(0x110DE4, 0xDD394, bool(__thiscall*)(c_game_engine*, uint32, int32, void*), this, flags, unused, state_data);
}