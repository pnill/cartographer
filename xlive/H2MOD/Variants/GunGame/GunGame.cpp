#include "stdafx.h"

#include "GunGame.h"
#include "Blam/Engine/Game/game/game.h"
#include "Blam/Engine/Game/networking/logic/network_life_cycle.h"
#include "Blam/Engine/Game/units/units.h"
#include "Blam/Engine/Game/networking/messages/network_message_type_collection.h"
#include "Blam/Engine/Networking/Session/NetworkSession.h"

#include "Util/ReadIniArguments.h"

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
std::unordered_map<unsigned long long, int> GunGame::gungamePlayers;

datum weaponDatums[36] = {
	0xE53D2AD8, 0xE5F02B8B, 0xE6322BCD, 0xE6AF2C4A,
	0xE79B2D36,0xE8172DB2,0xE8382DD3,0xE8742E0F,
	0xE8D32E6E,0xE9062EA1,0xE90C2EA7,0xE90C2EA7,
	0xE9732F0E,0xE9F62F91,0xEAD83073,0xEB4230DD,
	0xEB9E3139,0xEC3131CC,0xEC673202,0xEC9E3239,
	0xECD63271,0xED3F32DA,0xED753310,0xEDA2333D,
	0xEDD4336F,0xEE0933A4,0xEE3433CF,0xEE5233ED,
	0xEE5F33FA,0xEE7B3416,0xEE993434,0xEE9E3439,
	0xEED3346E,0xEEF1348C,0xEF1B34B6,0xF33838D2
};

GunGame::GunGame()
{
}

void GunGame::ReadWeaponLevels()
{
	FILE* gfp;
	gfp = fopen("gungame.ini", "r");

	if (gfp)
	{
		LOG_TRACE_GAME("[GunGame Enabled] - Opened GunGame.ini!");
		while (!feof(gfp))
		{
			char gstr[256];

			fgets(gstr, 256, gfp);

			gCHECK_ARG("weapon_one =", weapon_one);
			gCHECK_ARG("weapon_two =", weapon_two);
			gCHECK_ARG("weapon_three =", weapon_three);
			gCHECK_ARG("weapon_four =", weapon_four);
			gCHECK_ARG("weapon_five =", weapon_five);
			gCHECK_ARG("weapon_six =", weapon_six);
			gCHECK_ARG("weapon_seven =", weapon_seven);
			gCHECK_ARG("weapon_eight =", weapon_eight);
			gCHECK_ARG("weapon_nine =", weapon_nine);
			gCHECK_ARG("weapon_ten =", weapon_ten);
			gCHECK_ARG("weapon_eleven =", weapon_eleven);
			gCHECK_ARG("weapon_tweleve =", weapon_tweleve);
			gCHECK_ARG("weapon_thirteen =", weapon_thirteen);
			gCHECK_ARG("weapon_fourteen =", weapon_fourteen);
			gCHECK_ARG("weapon_fifteen =", weapon_fiffteen);
			gCHECK_ARG("weapon_sixteen =", weapon_sixteen);

		}

#ifdef __DEBUG
		LOG_TRACE_GAME("[GunGame] : {}", b_GunGame);

		LOG_TRACE_GAME("[GunGame] - weapon_one: {}", weapon_one);
		LOG_TRACE_GAME("[GunGame] - weapon_two: {}", weapon_two);
		LOG_TRACE_GAME("[GunGame] - weapon_three: {}", weapon_three);
#endif

		fclose(gfp);
	}
}

void GunGame::InitWeaponLevels() {
	if (weapon_one != 0) {
		LOG_TRACE_GAME("[H2Mod-GunGame] : Intialize() - weapon_one: {}", weapon_one);
		LOG_TRACE_GAME("[H2Mod-GunGame] : Intialize() - weapon_one enum:  {:x}", weaponDatums[weapon_one]);
		LOG_TRACE_GAME("[H2Mod-GunGame] : Intialize() - weapon_two enum:  {:x}", weaponDatums[weapon_two]);
		GunGame::levelWeapon[0] = weaponDatums[weapon_one];
		GunGame::levelWeapon[1] = weaponDatums[weapon_two];
		GunGame::levelWeapon[2] = weaponDatums[weapon_three];
		GunGame::levelWeapon[3] = weaponDatums[weapon_four];
		GunGame::levelWeapon[4] = weaponDatums[weapon_five];
		GunGame::levelWeapon[5] = weaponDatums[weapon_six];
		GunGame::levelWeapon[6] = weaponDatums[weapon_seven];
		GunGame::levelWeapon[7] = weaponDatums[weapon_eight];
		GunGame::levelWeapon[8] = weaponDatums[weapon_nine];
		GunGame::levelWeapon[9] = weaponDatums[weapon_ten];
		GunGame::levelWeapon[10] = weaponDatums[weapon_eleven];
		GunGame::levelWeapon[11] = weaponDatums[weapon_tweleve];
		GunGame::levelWeapon[12] = weaponDatums[weapon_thirteen];
		GunGame::levelWeapon[13] = weaponDatums[weapon_fourteen];
		GunGame::levelWeapon[14] = weaponDatums[weapon_fiffteen];
	} else {
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

		switch (gameOptions->m_engine_type)
		{
			// cleanup when loading main menu
		case _multiplayer:
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
	datum playerUnitDatum = s_player::GetPlayerUnitDatumIndex(absPlayerIdx);

	switch (execTime)
	{
	case ExecTime::_preEventExec:
		// to note after the original function executes, the controlled unit by this player is set to NONE
		if (!s_game_globals::game_is_predicted())
		{
			units::carto_set_player_unit_grenades_count(absPlayerIdx, e_grenades::Fragmentation, 0, true);
			units::carto_set_player_unit_grenades_count(absPlayerIdx, e_grenades::Plasma, 0, true);
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
	datum playerUnitDatum = s_player::GetPlayerUnitDatumIndex(absPlayerIdx);

	switch (execTime)
	{
		// prespawn handler
	case ExecTime::_preEventExec:
		s_player::SetUnitBipedType(absPlayerIdx, s_player::e_character_type::Spartan);
		break;

		// postspawn handler
	case ExecTime::_postEventExec:
		// host only (dedicated server and client)
		if (!s_game_globals::game_is_predicted())
		{
			LOG_TRACE_GAME(L"[H2Mod-GunGame]: {} player index: {}, player name: {1}", __FUNCTIONW__, absPlayerIdx, s_player::GetName(absPlayerIdx));

			char* unit_object = (char*)object_try_and_get_and_verify_type(playerUnitDatum, FLAG(e_object_type::biped));
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

				LOG_TRACE_GAME(L"[H2Mod-GunGame]: {} - player index: {}, player name: {1} - Level: {2}", __FUNCTIONW__, absPlayerIdx, s_player::GetName(absPlayerIdx), level);

				datum currentWeapon = GunGame::levelWeapon[level];

				if (level < 15) {
					units::carto_call_give_player_weapon(absPlayerIdx, currentWeapon, 1);
				}
				else if (level == 15) {
					LOG_TRACE_GAME(L"[H2Mod-GunGame]: {} - {} on frag grenade level!", __FUNCTIONW__, s_player::GetName(absPlayerIdx));
					units::carto_set_player_unit_grenades_count(absPlayerIdx, e_grenades::Fragmentation, 99, true);
				}
				else if (level == 16) {
					LOG_TRACE_GAME(L"[H2Mod-GunGame]: {} - {} on plasma grenade level!", __FUNCTIONW__, s_player::GetName(absPlayerIdx));
					units::carto_set_player_unit_grenades_count(absPlayerIdx, e_grenades::Plasma, 99, true);
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

bool GunGame::OnPlayerScore(ExecTime execTime, void* thisptr, unsigned short a2, int a3, int a4, int a5, char a6)
{
	int absPlayerIdx = a2;
	datum playerUnitDatum = s_player::GetPlayerUnitDatumIndex(absPlayerIdx);
	unsigned long long playerId = GetPlayerId(absPlayerIdx);

	// in gungame we just keep track of the score
	bool handled = false;

	switch (execTime)
	{
	case ExecTime::_preEventExec:
		break;
		
	case ExecTime::_postEventExec:
		if (a5 == 7 
			&& !s_game_globals::game_is_predicted())
		{
			LOG_TRACE_GAME(L"[H2Mod-GunGame]: {} - player index: {}, player name: {}", __FUNCTIONW__, absPlayerIdx, s_player::GetName(absPlayerIdx));

			int level = GunGame::gungamePlayers[playerId];
			level++;

			if (level > 16)
				level = 0; // reset level, so we dont keep the player without weapons, in case the game doesnt end

			GunGame::gungamePlayers[playerId] = level;

			LOG_TRACE_GAME(L"[H2Mod-GunGame]: {} - player index: {} - new level: {} ", __FUNCTIONW__, absPlayerIdx, level);

			if (level < 15) {
				LOG_TRACE_GAME(L"[H2Mod-GunGame]: {} - {} on level {} giving them weapon...", __FUNCTIONW__, s_player::GetName(absPlayerIdx), level);

				datum LevelWeapon = GunGame::levelWeapon[level];
				units::carto_set_player_unit_grenades_count(absPlayerIdx, e_grenades::Fragmentation, 0, true);
				units::carto_set_player_unit_grenades_count(absPlayerIdx, e_grenades::Plasma, 0, true);
				units::carto_call_give_player_weapon(absPlayerIdx, LevelWeapon, 1);
			}

			else if (level == 15) {
				LOG_TRACE_GAME(L"[H2Mod-GunGame]: {} - {} Level 15 - Frag Grenades!", __FUNCTIONW__, s_player::GetName(absPlayerIdx));
				units::carto_set_player_unit_grenades_count(absPlayerIdx, e_grenades::Fragmentation, 99, true);
			}

			else if (level == 16) {
				LOG_TRACE_GAME(L"[H2Mod-GunGame]: {} - {} Level 16 - Plasma Grenades!", __FUNCTIONW__, s_player::GetName(absPlayerIdx));
				units::carto_set_player_unit_grenades_count(absPlayerIdx, e_grenades::Fragmentation, 0, true);
				units::carto_set_player_unit_grenades_count(absPlayerIdx, e_grenades::Plasma, 99, true);
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
