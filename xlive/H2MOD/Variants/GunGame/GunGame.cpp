#include "stdafx.h"
#include "GunGame.h"

#include "game/game.h"
#include "networking/Session/NetworkSession.h"
#include "H2MOD.h"

using namespace NetworkSession;

static int weapon_one = 0;
static int weapon_two = 0;
static int weapon_three = 0;
static int weapon_four = 0;
static int weapon_five = 0;
static int weapon_six = 0;
static int weapon_seven = 0;
static int weapon_eight = 0;
static int weapon_nine = 0;
static int weapon_ten = 0;
static int weapon_eleven = 0;
static int weapon_tweleve = 0;
static int weapon_thirteen = 0;
static int weapon_fourteen = 0;
static int weapon_fiffteen = 0;
static int weapon_sixteen = 0;


// TODO(PermaNull): Add additional levels with dual weilding

std::unordered_map<int, datum> GunGame::levelWeapon;
std::unordered_map<uint64, int> GunGame::gungamePlayers;

GunGame::GunGame()
{
}


void GunGame::InitWeaponLevels() {
	GunGame::levelWeapon[0] = e_weapons_datum_index::energy_blade_useless;
	GunGame::levelWeapon[1] = e_weapons_datum_index::needler;
	GunGame::levelWeapon[2] = e_weapons_datum_index::plasma_pistol;
	GunGame::levelWeapon[3] = e_weapons_datum_index::magnum;
	GunGame::levelWeapon[4] = e_weapons_datum_index::smg;
	GunGame::levelWeapon[5] = e_weapons_datum_index::plasma_rifle;
	GunGame::levelWeapon[6] = e_weapons_datum_index::brute_plasma_rifle;
	GunGame::levelWeapon[7] = e_weapons_datum_index::juggernaut_powerup;
	GunGame::levelWeapon[8] = e_weapons_datum_index::shotgun;
	GunGame::levelWeapon[9] = e_weapons_datum_index::brute_shot;
	GunGame::levelWeapon[10] = e_weapons_datum_index::covenant_carbine;
	GunGame::levelWeapon[11] = e_weapons_datum_index::battle_rifle;
	GunGame::levelWeapon[12] = e_weapons_datum_index::beam_rifle;
	GunGame::levelWeapon[13] = e_weapons_datum_index::sniper_rifle;
	GunGame::levelWeapon[14] = e_weapons_datum_index::rocket_launcher;	
}

void GunGame::ResetPlayerLevels() {
	gungamePlayers.clear();
}

void GunGame::Initialize()
{
	GunGame::InitWeaponLevels();
	if (NetworkSession::LocalPeerIsSessionHost())
	{
		GunGame::ResetPlayerLevels();
	}
}

void GunGame::Dispose()
{
	ResetPlayerLevels();
}

CustomVariantId GunGame::GetVariantId()
{
	return CustomVariantId::_id_gungame;
}

void GunGame::OnMapLoad(ExecTime execTime, s_game_options* gameOptions)
{
	switch (execTime)
	{
	case ExecTime::_preEventExec:
		break;

	case ExecTime::_postEventExec:
		LOG_TRACE_GAME("[h2mod-infection] Peer host init");

		switch (gameOptions->game_mode)
		{
			// cleanup when loading main menu
		case _game_mode_multiplayer:
			this->Initialize();
			break;
		/*case _main_menu:
			this->Dispose();
			break;*/
		default:
			break;
		}

		break;

	case ExecTime::_ExecTimeUnknown:
	default:
		LOG_TRACE_GAME("{} - unknown execTime", __FUNCTION__);
		break;
	}
}

void GunGame::OnPlayerDeath(ExecTime execTime, datum playerIdx)
{
	int absPlayerIdx = DATUM_INDEX_TO_ABSOLUTE_INDEX(playerIdx);
	datum playerUnitDatum = s_player::get_unit_index(playerIdx);

	switch (execTime)
	{
	case ExecTime::_preEventExec:
		// to note after the original function executes, the controlled unit by this player is set to NONE
		if (!game_is_predicted())
		{
			s_player::set_player_unit_grenade_count(playerIdx, _unit_grenade_type_fragmentation, 0, true);
			s_player::set_player_unit_grenade_count(playerIdx, _unit_grenade_type_plasma, 0, true);
		}
		break;

	case ExecTime::_postEventExec:
		break;

	case ExecTime::_ExecTimeUnknown:
	default:
		LOG_TRACE_GAME("{} - unknown execTime", __FUNCTION__);
		break;
	}
}

void GunGame::OnPlayerSpawn(ExecTime execTime, datum playerIdx)
{
	int absPlayerIdx = DATUM_INDEX_TO_ABSOLUTE_INDEX(playerIdx);
	datum playerUnitDatum = s_player::get_unit_index(playerIdx);

	switch (execTime)
	{
		// prespawn handler
	case ExecTime::_preEventExec:
		s_player::set_unit_character_type(playerIdx, _character_type_spartan);
		break;

		// postspawn handler
	case ExecTime::_postEventExec:
		// host only (dedicated server and client)
		if (!game_is_predicted())
		{
			LOG_TRACE_GAME(L"[H2Mod-GunGame]: {} player index: {}, player name: {1}", __FUNCTIONW__, absPlayerIdx, s_player::get_name(playerIdx));

			void* unit_object = object_try_and_get_and_verify_type(playerUnitDatum, FLAG(_object_type_biped));
			if (unit_object) {

				int level = 0;
				auto gungamePlayer = gungamePlayers.find(GetPlayerId(absPlayerIdx));
				if (gungamePlayer != gungamePlayers.end())
				{
					level = gungamePlayer->second;
				}
				else
				{
					gungamePlayers.insert(std::make_pair(GetPlayerId(absPlayerIdx), level));
				}

				LOG_TRACE_GAME(L"[H2Mod-GunGame]: {} - player index: {}, player name: {1} - Level: {2}", __FUNCTIONW__, absPlayerIdx, s_player::get_name(playerIdx), level);

				datum currentWeapon = GunGame::levelWeapon[level];

				if (level < 15) {
					call_give_player_weapon(absPlayerIdx, currentWeapon, 1);
				}
				else if (level == 15) {
					LOG_TRACE_GAME(L"[H2Mod-GunGame]: {} - {} on frag grenade level!", __FUNCTIONW__, s_player::get_name(playerIdx));
					s_player::set_player_unit_grenade_count(playerIdx, _unit_grenade_type_fragmentation, 99, true);
				}
				else if (level == 16) {
					LOG_TRACE_GAME(L"[H2Mod-GunGame]: {} - {} on plasma grenade level!", __FUNCTIONW__, s_player::get_name(playerIdx));
					s_player::set_player_unit_grenade_count(playerIdx, _unit_grenade_type_plasma, 99, true);
				}
			}
		}

		break;

	case ExecTime::_ExecTimeUnknown:
	default:
		LOG_TRACE_GAME("{} - unknown execTime", __FUNCTION__);
		break;
	}
}

bool GunGame::c_game_statborg__adjust_player_stat(ExecTime execTime, c_game_statborg* statborg, datum player_datum, e_statborg_entry statistic, short count, int game_results_statistic, bool adjust_team_stat)
{
	int absPlayerIdx = DATUM_INDEX_TO_ABSOLUTE_INDEX(player_datum);
	datum playerUnitDatum = s_player::get_unit_index(player_datum);
	unsigned long long playerId = GetPlayerId(absPlayerIdx);

	// in gungame we just keep track of the score
	bool handled = false;

	switch (execTime)
	{
	case ExecTime::_preEventExec:
		break;
		
	case ExecTime::_postEventExec:
		if (game_results_statistic == 7
			&& !game_is_predicted())
		{
			LOG_TRACE_GAME(L"[H2Mod-GunGame]: {} - player index: {}, player name: {}", __FUNCTIONW__, absPlayerIdx, s_player::get_name(player_datum));

			int level = GunGame::gungamePlayers[playerId];
			level++;

			if (level > 16)
				level = 0; // reset level, so we dont keep the player without weapons, in case the game doesnt end

			GunGame::gungamePlayers[playerId] = level;

			LOG_TRACE_GAME(L"[H2Mod-GunGame]: {} - player index: {} - new level: {} ", __FUNCTIONW__, absPlayerIdx, level);

			if (level < 15) {
				LOG_TRACE_GAME(L"[H2Mod-GunGame]: {} - {} on level {} giving them weapon...", __FUNCTIONW__, s_player::get_name(player_datum), level);

				datum LevelWeapon = GunGame::levelWeapon[level];
				s_player::set_player_unit_grenade_count(player_datum, _unit_grenade_type_fragmentation, 0, true);
				s_player::set_player_unit_grenade_count(player_datum, _unit_grenade_type_plasma, 0, true);
				call_give_player_weapon(absPlayerIdx, LevelWeapon, 1);
			}

			else if (level == 15) {
				LOG_TRACE_GAME(L"[H2Mod-GunGame]: {} - {} Level 15 - Frag Grenades!", __FUNCTIONW__, s_player::get_name(player_datum));
				s_player::set_player_unit_grenade_count(player_datum, _unit_grenade_type_fragmentation, 99, true);
			}

			else if (level == 16) {
				LOG_TRACE_GAME(L"[H2Mod-GunGame]: {} - {} Level 16 - Plasma Grenades!", __FUNCTIONW__, s_player::get_name(player_datum));
				s_player::set_player_unit_grenade_count(player_datum, _unit_grenade_type_fragmentation, 0, true);
				s_player::set_player_unit_grenade_count(player_datum, _unit_grenade_type_plasma, 99, true);
			}
		}

		break;

	case ExecTime::_ExecTimeUnknown:
	default:
		LOG_TRACE_GAME("{} - unknown execTime", __FUNCTION__);
		break;
	}

	return handled;
}
