#include "stdafx.h"
#include "game_statborg.h"

short c_game_statborg::get_player_score_by_type(DWORD player_index, byte score_type)
{
	return this->m_player_data[player_index].score[score_type];
}

c_game_statborg* game_engine_get_statborg()
{
	typedef c_game_statborg* (__cdecl* game_engine_get_statborg_t)();
	auto p_game_engine_get_statborg = Memory::GetAddress<game_engine_get_statborg_t>(0x6B8A7, 0x6AD32);
	return p_game_engine_get_statborg();
}
