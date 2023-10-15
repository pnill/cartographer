#include "stdafx.h"
#include "game_statborg.h"

#include "H2MOD/Variants/Variants.h"
#include "Util/Hooks/Hook.h"

c_game_statborg* __cdecl game_engine_get_statborg(void)
{
	return INVOKE(0x6B8A7, 0x6AD32, game_engine_get_statborg);
}

int16 c_game_statborg::get_player_stat(int32 player_index, e_statborg_entry statborg_entry)
{
	return this->m_player_data[player_index].statistic[statborg_entry];
}

int16 c_game_statborg::get_team_stat(int32 team_index, e_statborg_entry statistic)
{
	return this->m_team_data[team_index].statistic[statistic];
}

typedef void(__thiscall* adjust_player_stat_t)(c_game_statborg* statborg, datum, e_statborg_entry, int16, int32, bool);
adjust_player_stat_t p_c_game_statborg__adjust_player_stat;
void c_game_statborg::adjust_player_stat(datum player_datum, e_statborg_entry statistic, int16 count, int32 game_result_statistic, bool adjust_team_stat)
{
	p_c_game_statborg__adjust_player_stat(this, player_datum, statistic, count, game_result_statistic, adjust_team_stat);
	return;
}

typedef void(__thiscall* adjust_team_stat_t)(c_game_statborg*, int32, e_statborg_entry, int16, int32);
adjust_team_stat_t p_c_game_statborg__adjust_team_stat;
void c_game_statborg::adjust_team_stat(int32 team_index, e_statborg_entry statistic, int16 count, int32 game_result_statistic)
{
	p_c_game_statborg__adjust_team_stat(this, team_index, statistic, count, game_result_statistic);
	return;
}


// Cartographer handler for adjust_player_stat member function
// TODO Revamp so we don't need to do this
void __fastcall c_game_statborg__adjust_player_stat(c_game_statborg* thisptr, DWORD _edx, datum player_datum, e_statborg_entry statistic, short count, int game_result_statistic, bool adjust_team_stat)
{
	bool handled = CustomVariantHandler::c_game_statborg__adjust_player_stat(ExecTime::_preEventExec, thisptr, player_datum, statistic, count, game_result_statistic, adjust_team_stat);
	if (!handled)
		p_c_game_statborg__adjust_player_stat(thisptr, player_datum, statistic, count, game_result_statistic, adjust_team_stat);
	CustomVariantHandler::c_game_statborg__adjust_player_stat(ExecTime::_postEventExec, thisptr, player_datum, statistic, count, game_result_statistic, adjust_team_stat);
	return;
}

// Cartographer handler for adjust_team_stat member function
// TODO Revamp so we don't need to do this
void __fastcall c_game_statborg__adjust_team_stat(c_game_statborg* statborg, DWORD _edx, int team_index, e_statborg_entry statistic, short count, int game_result_statistic)
{
	// Don't add to team score automatically if playing graverobber
	if (CustomVariantHandler::VariantEnabled(_id_graverobber)) 
	{ 
		if (graverobber_player_picking_up_skull() == false) { return; }
	}

	statborg->adjust_team_stat(team_index, statistic, count, game_result_statistic);
	return;
}


void game_statborg_apply_patches()
{
	DETOUR_ATTACH(p_c_game_statborg__adjust_player_stat, Memory::GetAddress<adjust_player_stat_t>(0xD03ED, 0x8C84C), c_game_statborg__adjust_player_stat);
	DETOUR_ATTACH(p_c_game_statborg__adjust_team_stat, Memory::GetAddress<adjust_team_stat_t>(0xD0252, 0x8C6B1), c_game_statborg__adjust_team_stat);
	return;
}