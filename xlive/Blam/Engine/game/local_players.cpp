#include "stdafx.h"
#include "local_players.h"

bool __cdecl players_user_is_active(int32 user_index)
{
	return INVOKE(0x5139B, 0x598BE, players_user_is_active, user_index);
}

datum __cdecl player_index_from_user_index(int32 user_index)
{
	return INVOKE(0x5141D, 0x5992B, player_index_from_user_index, user_index);
}