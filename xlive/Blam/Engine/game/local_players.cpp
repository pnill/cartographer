#include "stdafx.h"
#include "local_players.h"

s_local_player_properties* s_local_player_properties::get()
{
	return Memory::GetAddress<s_local_player_properties*>(0x51A628, 0x5204A4);
}

s_local_player_properties* get_local_player_properties(DWORD controller_index)
{
	return &s_local_player_properties::get()[controller_index];
}

bool local_user_has_player(int user_index)
{
	typedef bool(__cdecl* local_user_has_player_t)(int user_index);
	auto p_local_user_has_player = Memory::GetAddress<local_user_has_player_t>(0x5139B);
	return p_local_user_has_player(user_index);
}

datum local_user_get_player_idx(int user_index)
{
	typedef datum(__cdecl* local_user_get_player_idx_t)(int user_index);
	auto p_local_user_has_player = Memory::GetAddress<local_user_get_player_idx_t>(0x5141D);
	return p_local_user_has_player(user_index);
}
