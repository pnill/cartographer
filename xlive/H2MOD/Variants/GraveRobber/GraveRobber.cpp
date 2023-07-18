#include "stdafx.h"
#include "GraveRobber.h"

#include "H2MOD.h"
#include "Blam/Engine/game/game_globals.h"
#include "Blam/Engine/game/game_engine_util.h"
#include "Blam/Engine/game/game_statborg.h"
#include "Blam/Engine/Networking/Session/NetworkSession.h"
#include "H2MOD/Modules/CustomMenu/CustomLanguage.h"
#include "H2MOD/Modules/HaloScript/HaloScript.h"
#include "H2MOD/Modules/Shell/Config.h"
#include "H2MOD/Utils/Utils.h"

bool firstPlayerSpawn;
bool player_is_picking_up_skull = false;

const wchar_t* headhunterSoundTable[e_language_ids::_lang_id_end][e_graverobber_sounds::_graverobber_end]
{
	{SND_HEADHUNTER_EN, SND_SKULL_SCORED_EN},
	{SND_HEADHUNTER_JP, SND_SKULL_SCORED_JP},
	{SND_HEADHUNTER_GE, SND_SKULL_SCORED_GE},
	{SND_HEADHUNTER_FR, SND_SKULL_SCORED_FR},
	{SND_HEADHUNTER_ES, SND_SKULL_SCORED_ES},
	{SND_HEADHUNTER_IT, SND_SKULL_SCORED_IT},
	{SND_HEADHUNTER_KO, SND_SKULL_SCORED_KO},
	{SND_HEADHUNTER_CH, SND_SKULL_SCORED_CH}
};

void GraveRobber::TriggerSound(e_graverobber_sounds sound, int sleep)
{
	const int language_id = *Memory::GetAddress<int*>(0x412818);

	if (Memory::IsDedicatedServer()) return;

	if (headhunterSoundTable[language_id][sound] != nullptr)
	{
		LOG_TRACE_GAME(L"[h2mod-graverobber] Triggering sound {}", headhunterSoundTable[language_id][sound]);
		h2mod->custom_sound_play(headhunterSoundTable[language_id][sound], sleep);
	}
}

void GraveRobber::SpawnPlayerClientSetup()
{
	if (firstPlayerSpawn)
	{
		TriggerSound(_snd_head_hunter, 1000);
		firstPlayerSpawn = false;
	}
}

void GraveRobber::SpawnSkull(datum unit_datum)
{
	const s_biped_data_definition* biped_unit = (s_biped_data_definition*)object_try_and_get_and_verify_type(unit_datum, FLAG(e_object_type::biped));

	if (biped_unit != NULL)
	{
		s_object_placement_data nObject;

		Engine::Objects::create_new_placement_data(&nObject, e_weapons_datum_index::ball, -1, 0);

		nObject.position = biped_unit->position;
		nObject.translational_velocity = biped_unit->translational_velocity;

		datum new_object_datum = Engine::Objects::object_new(&nObject);
		if (!DATUM_IS_NONE(new_object_datum))
			Engine::Objects::simulation_action_object_create(new_object_datum);
	}
}


void GraveRobber::PickupSkull(datum player_datum, datum skull_datum)
{
	if (DATUM_IS_NONE(skull_datum)) { return; }

	int player_index = DATUM_INDEX_TO_ABSOLUTE_INDEX(player_datum);
	s_player* player = s_player::GetPlayer(player_index);

	c_game_statborg* game_statborg = game_engine_get_statborg();
	if (!s_game_globals::game_is_predicted())
	{
		player_is_picking_up_skull = true;
		game_statborg->adjust_player_stat(player_datum, statborg_entry_score, 1, -1, true);
		if (game_engine_has_teams())
		{
			if (game_statborg->get_team_stat(player->properties[0].player_team, statborg_entry_score) == s_game_globals::get_game_variant()->score_to_win_round)
			{
				game_engine_end_round_with_winner(player->properties[0].player_team, false);
			}
		}
		else
		{
			if (game_statborg->get_player_stat(player_index, statborg_entry_score) == s_game_globals::get_game_variant()->score_to_win_round)
			{
				game_engine_end_round_with_winner(player_datum, false);
			}
		}

		player_is_picking_up_skull = false;
	}
	
	HaloScript::ObjectDestroy(skull_datum);

	if (!Memory::IsDedicatedServer())
	{
		for (int i = 0; i < k_number_of_users; i++)
		{
			if (DATUM_INDEX_TO_ABSOLUTE_INDEX(h2mod->get_player_datum_index_from_controller_index(i)) == player_index)
			{
				TriggerSound(_snd_skull_scored, 500);
				break;
			}
		}
	}
}

void GraveRobber::InitializeClient()
{
	h2mod->disable_sounds(FLAG(_sound_type_slayer) | ALL_SOUNDS_NO_SLAYER);
	firstPlayerSpawn = true;
}

void GraveRobber::Initialize()
{
	if (!Memory::IsDedicatedServer())
	{
		GraveRobber::InitializeClient();
	}
}

void GraveRobber::Dispose() 
{
	/*Unused*/
}

CustomVariantId GraveRobber::GetVariantId()
{
	return CustomVariantId::_id_graverobber;
}

void GraveRobber::OnMapLoad(ExecTime execTime, s_game_options* options)
{
	switch (execTime)
	{
	case ExecTime::_preEventExec:
		break;

	case ExecTime::_postEventExec:
		switch (options->game_mode)
		{
		case _game_mode_multiplayer:
			this->Initialize();
			break;
		case _game_mode_ui_shell:
			break;
		default:
			break;
		}
		break;

	default:
		LOG_TRACE_GAME("{} - unknown execTime", __FUNCTION__);
		break;
	}
}

void GraveRobber::OnPlayerSpawn(ExecTime execTime, datum playerIdx)
{
	switch (execTime)
	{
		// prespawn handler
	case ExecTime::_preEventExec:
		break;

		// postspawn handler
	case ExecTime::_postEventExec:
		if (!Memory::IsDedicatedServer())
			GraveRobber::SpawnPlayerClientSetup();
		break;
	default:
		LOG_TRACE_GAME("{} - unknown execTime", __FUNCTION__);
		break;
	}
}

void GraveRobber::OnPlayerDeath(ExecTime execTime, datum playerIdx)
{
	const int player_index = DATUM_INDEX_TO_ABSOLUTE_INDEX(playerIdx);
	const datum unit_datum = s_player::GetPlayerUnitDatumIndex(player_index);

	switch (execTime)
	{
	case ExecTime::_preEventExec:
		// to note after the original function executes, the controlled unit by this player is set to NONE
		if (!s_game_globals::game_is_predicted() && unit_datum != object_get_damage_owner(unit_datum))
			GraveRobber::SpawnSkull(unit_datum);
		break;

	case ExecTime::_postEventExec:
		break;
	default:
		LOG_TRACE_GAME("{} - unknown execTime", __FUNCTION__);
		break;
	}
}

// Just return true since the player shouldn't be able to score normally
// Only way to score is picking up skulls which is triggered in GraveRobber::PickupSkull
bool GraveRobber::c_game_statborg__adjust_player_stat(ExecTime execTime, c_game_statborg* statborg, datum player_datum, e_statborg_entry statistic, short count, int game_results_statistic, bool adjust_team_stat)
{
	return true;
}

bool GraveRobber::OnAutoPickupHandler(ExecTime execTime, datum playerIdx, datum objectIdx)
{
	const s_weapon_data_definition* weaponObject = object_get_fast_unsafe<s_weapon_data_definition>(objectIdx);

	bool handled = false;

	switch (execTime)
	{
	case ExecTime::_preEventExec:

		if (DATUM_INDEX_TO_ABSOLUTE_INDEX(weaponObject->tag_definition_index) == DATUM_INDEX_TO_ABSOLUTE_INDEX(e_weapons_datum_index::ball))
		{
			GraveRobber::PickupSkull(playerIdx, objectIdx);
			handled = true;
		}
		break;

	case ExecTime::_postEventExec:
		break;

	default:
		LOG_TRACE_GAME("{} - unknown execTime", __FUNCTION__);
		break;
	}

	return handled;
}

bool graverobber_player_picking_up_skull()
{
	return player_is_picking_up_skull;
}