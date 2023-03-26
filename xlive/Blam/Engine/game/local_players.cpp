#include "stdafx.h"
#include "local_players.h"

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