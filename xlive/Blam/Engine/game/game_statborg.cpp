#include "stdafx.h"
#include "game_statborg.h"

#include "H2MOD/Variants/Variants.h"
#include "Util/Hooks/Hook.h"

typedef void(__thiscall* adjust_player_stat_t)(c_game_statborg* statborg, datum player_datum, e_statborg_entry statistic, short count, int game_results_statistic, bool adjust_team_stat);
adjust_player_stat_t p_c_game_statborg__adjust_player_stat;

short c_game_statborg::get_player_stat(DWORD player_index, e_statborg_entry statborg_entry)
{
	return this->m_player_data[player_index].statistic[statborg_entry];
}

c_game_statborg* game_engine_get_statborg()
{
	typedef c_game_statborg* (__cdecl* game_engine_get_statborg_t)();
	auto p_game_engine_get_statborg = Memory::GetAddress<game_engine_get_statborg_t>(0x6B8A7, 0x6AD32);
	return p_game_engine_get_statborg();
}

void c_game_statborg::adjust_player_stat(datum player_datum, e_statborg_entry statistic, short count, int game_result_statistic, bool adjust_team_stat)
{
	p_c_game_statborg__adjust_player_stat(this, player_datum, statistic, count, game_result_statistic, adjust_team_stat);
}

// Cartographer handler for adjust_player_stat member function
// TODO Revamp so we don't need to do this
void __fastcall c_game_statborg__adjust_player_stat(c_game_statborg* thisptr, DWORD _edx, datum player_datum, e_statborg_entry statistic, short count, int game_result_statistic, bool adjust_team_stat)
{
	bool handled = CustomVariantHandler::c_game_statborg__adjust_player_stat(ExecTime::_preEventExec, thisptr, player_datum, statistic, count, game_result_statistic, adjust_team_stat);
	if (!handled)
		p_c_game_statborg__adjust_player_stat(thisptr, player_datum, statistic, count, game_result_statistic, adjust_team_stat);
	CustomVariantHandler::c_game_statborg__adjust_player_stat(ExecTime::_postEventExec, thisptr, player_datum, statistic, count, game_result_statistic, adjust_team_stat);
}

void game_statborg_apply_patches()
{
	DETOUR_ATTACH(p_c_game_statborg__adjust_player_stat, Memory::GetAddress<adjust_player_stat_t>(0xD03ED, 0x8C84C), c_game_statborg__adjust_player_stat);
}