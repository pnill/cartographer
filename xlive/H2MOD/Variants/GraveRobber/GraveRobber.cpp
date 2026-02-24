#include "stdafx.h"
#include "GraveRobber.h"

#include "H2MOD.h"

#include "game/game.h"
#include "game/game_engine_util.h"
#include "game/game_statborg.h"
#include "game/players.h"
#include "game/game_options.h"
#include "items/weapons.h"
#include "main/game_preferences.h"
#include "networking/network_event.h"
#include "simulation/game_interface/simulation_game_action.h"
#include "shell/shell.h"

bool firstPlayerSpawn;
bool player_is_picking_up_skull = false;

const wchar_t *const headhunterSoundTable[k_language_count][e_graverobber_sounds::_graverobber_end]
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

	if (shell_is_dedicated_server()) return;

	if (headhunterSoundTable[language_id][sound] != nullptr)
	{
		event(_event_verbose, "h2mod:graverobber: Triggering sound %ws", headhunterSoundTable[language_id][sound]);
		H2MOD::custom_sound_play(headhunterSoundTable[language_id][sound], sleep);
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
	const object_datum* biped_unit = (object_datum*)object_try_and_get_and_verify_type(unit_datum, _object_mask_biped);

	if (biped_unit != NULL)
	{
		object_placement_data nObject;

		object_placement_data_new(&nObject, e_weapons_datum_index::ball, -1, 0);

		nObject.position = biped_unit->object.position;
		nObject.translational_velocity = biped_unit->object.translational_velocity;

		datum new_object_datum = object_new(&nObject);
		if (new_object_datum != NONE)
			simulation_action_object_create(new_object_datum);
	}
}


void GraveRobber::PickupSkull(datum player_index, datum skull_datum)
{
	if (skull_datum == NONE) { return; }

	player_datum* player = (player_datum*)datum_get(player_data_get(), player_index);

	c_game_statborg* game_statborg = game_engine_get_statborg();
	if (!game_is_predicted())
	{
		player_is_picking_up_skull = true;
		game_statborg->adjust_player_stat(player_index, _statborg_entry_round_score, 1, NONE, true);
		if (game_engine_has_teams())
		{
			if (game_statborg->get_team_stat(player->configuration.team_index, _statborg_entry_round_score) == current_game_variant()->score_to_win_round)
			{
				game_engine_end_round_with_winner(player->configuration.team_index, false);
			}
		}
		else
		{
			const uint16 player_abs_index = DATUM_INDEX_TO_ABSOLUTE_INDEX(player_index);
			if (game_statborg->get_player_stat(player_abs_index, _statborg_entry_round_score) == current_game_variant()->score_to_win_round)
			{
				game_engine_end_round_with_winner(player_index, false);
			}
		}

		player_is_picking_up_skull = false;
	}
	
	object_delete(skull_datum);

	if (!shell_is_dedicated_server())
	{
		for (int i = 0; i < k_number_of_users; i++)
		{
			if (player_index_from_user_index(i) == player_index)
			{
				TriggerSound(_snd_skull_scored, 500);
				break;
			}
		}
	}
}

void GraveRobber::InitializeClient()
{
	H2MOD::disable_score_announcer_sounds(FLAG(_sound_type_slayer) | ALL_SOUNDS_NO_SLAYER);
	firstPlayerSpawn = true;
}

void GraveRobber::Initialize()
{
	if (!shell_is_dedicated_server())
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
		event(_event_verbose, "h2mod:graverobber: %s - unknown execTime", __FUNCTION__);
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
		if (!shell_is_dedicated_server())
			GraveRobber::SpawnPlayerClientSetup();
		break;
	default:
		event(_event_verbose, "h2mod:graverobber: %s - unknown execTime", __FUNCTION__);
		break;
	}
}

void GraveRobber::OnPlayerDeath(ExecTime execTime, datum player_index)
{
	const player_datum* player = (player_datum*)datum_get(player_data_get(), player_index);

	switch (execTime)
	{
	case ExecTime::_preEventExec:
		// to note after the original function executes, the controlled unit by this player is set to NONE
		if (!game_is_predicted() && player->unit_index != object_get_damage_owner(player->unit_index))
			GraveRobber::SpawnSkull(player->unit_index);
		break;

	case ExecTime::_postEventExec:
		break;
	default:
		event(_event_verbose, "h2mod:graverobber: %s - unknown execTime", __FUNCTION__);
		break;
	}
}

bool GraveRobber::OnAutoPickupHandler(ExecTime execTime, datum playerIdx, datum objectIdx)
{
	const weapon_datum* weapon = weapon_get(objectIdx);

	bool handled = false;

	switch (execTime)
	{
	case ExecTime::_preEventExec:

		if (DATUM_INDEX_TO_ABSOLUTE_INDEX(weapon->definition_index) == DATUM_INDEX_TO_ABSOLUTE_INDEX(e_weapons_datum_index::ball))
		{
			GraveRobber::PickupSkull(playerIdx, objectIdx);
			handled = true;
		}
		break;

	case ExecTime::_postEventExec:
		break;

	default:
		event(_event_verbose, "h2mod:graverobber: %s - unknown execTime", __FUNCTION__);
		break;
	}

	return handled;
}

bool graverobber_player_picking_up_skull()
{
	return player_is_picking_up_skull;
}