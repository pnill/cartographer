#include "stdafx.h"
#include "GunGame.h"

#include "game/game.h"
#include "game/game_options.h"
#include "game/players.h"
#include "networking/network_event.h"
#include "networking/session/network_session.h"
#include "simulation/game_interface/simulation_game_action.h"
#include "units/bipeds.h"

#include "H2MOD.h"

// TODO(PermaNull): Add additional levels with dual weilding

const e_weapons_datum_index k_level_weapons[15] =
{
	e_weapons_datum_index::energy_blade_useless,
	e_weapons_datum_index::needler,
	e_weapons_datum_index::plasma_pistol,
	e_weapons_datum_index::magnum,
	e_weapons_datum_index::smg,
	e_weapons_datum_index::plasma_rifle,
	e_weapons_datum_index::brute_plasma_rifle,
	e_weapons_datum_index::juggernaut_powerup,
	e_weapons_datum_index::shotgun,
	e_weapons_datum_index::brute_shot,
	e_weapons_datum_index::covenant_carbine,
	e_weapons_datum_index::battle_rifle,
	e_weapons_datum_index::beam_rifle,
	e_weapons_datum_index::sniper_rifle,
	e_weapons_datum_index::rocket_launcher
};

static std::unordered_map<uint64, int> gungamePlayers;

GunGame::GunGame()
{
}

void GunGame::ResetPlayerLevels() {
	gungamePlayers.clear();
}

void GunGame::Initialize()
{
	event(_event_status, "h2mod:gungame: Peer host init");
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
		event(_event_verbose, "h2mod:gungame: %s - unknown execTime", __FUNCTION__);
		break;
	}
}

void GunGame::OnPlayerDeath(ExecTime execTime, datum player_index)
{
	switch (execTime)
	{
	case ExecTime::_preEventExec:
		// to note after the original function executes, the controlled unit by this player is set to NONE
		if (!game_is_predicted())
		{
			const player_datum* player = (player_datum*)datum_get(player_data_get(), player_index);

			unit_datum* unit = unit_try_and_get(player->unit_index);
			unit->unit.grenade_counts[_unit_grenade_human_fragmentation] = 0;
			unit->unit.grenade_counts[_unit_grenade_covenant_plasma] = 0;
			simulation_action_object_update(player->unit_index, FLAG(_simulation_action_update_grenade_count_bit));

			unit_delete_all_weapons(player->unit_index);
		}
		break;

	case ExecTime::_postEventExec:
		break;

	case ExecTime::_ExecTimeUnknown:
	default:
		event(_event_verbose, "h2mod:gungame: %s - unknown execTime", __FUNCTION__);
		break;
	}
}

void GunGame::OnPlayerSpawn(ExecTime execTime, datum player_index)
{
	player_datum* player = (player_datum*)datum_get(player_data_get(), player_index);
	const uint16 player_abs_index = DATUM_INDEX_TO_ABSOLUTE_INDEX(player_index);

	switch (execTime)
	{
		// prespawn handler
	case ExecTime::_preEventExec:
		player->configuration.profile_traits.profile.player_character_type = _character_type_spartan;
		break;

		// postspawn handler
	case ExecTime::_postEventExec:
		// host only (dedicated server and client)
		if (!game_is_predicted())
		{
			event(_event_verbose, "h2mod:gungame: %s player index: %d, player name: %ws", __FUNCTION__, player_abs_index, player->configuration.player_name);

			if (biped_try_and_get(player->unit_index))
			{
				int level = 0;
				uint64 id;
				s_player_identifier identifier = NetworkSession::GetPlayerId(player_abs_index);
				csmemcpy(&id, &identifier, sizeof(uint64));

				auto gungamePlayer = gungamePlayers.find(id);
				if (gungamePlayer != gungamePlayers.end())
				{
					level = gungamePlayer->second;
				}
				else
				{
					gungamePlayers.insert(std::make_pair(id, level));
				}

				event(_event_verbose, "h2mod:gungame: %s - player index: %d, player name: %ws - Level: %d", __FUNCTIONW__, player_abs_index, player->configuration.player_name, level);


				if (level == 15)
				{
					event(_event_verbose, "h2mod:gungame: %s - %ws on frag grenade level!", __FUNCTION__, player->configuration.player_name);
					unit_delete_all_weapons(player->unit_index);
					unit_add_grenade_type_to_inventory(player->unit_index, _unit_grenade_human_fragmentation, 99);
				}
				else if (level == 16)
				{
					event(_event_verbose, "h2mod:gungame: %s - %ws on plasma grenade level!", __FUNCTION__, player->configuration.player_name);
					unit_delete_all_weapons(player->unit_index);
					unit_add_grenade_type_to_inventory(player->unit_index, _unit_grenade_covenant_plasma, 99);
				}
				else
				{
					call_give_player_weapon(player_abs_index, (datum)k_level_weapons[level], 1);
				}
			}
		}

		break;

	case ExecTime::_ExecTimeUnknown:
	default:
		event(_event_verbose, "h2mod:gungame: %s - unknown execTime", __FUNCTION__);
		break;
	}
}

bool GunGame::c_game_statborg__adjust_player_stat(ExecTime execTime, c_game_statborg* statborg, datum player_index, e_statborg_entry statistic, short count, int game_results_statistic, bool adjust_team_stat)
{
	const player_datum* player = (player_datum*)datum_get(player_data_get(), player_index);

	uint64 id;
	s_player_identifier identifier = NetworkSession::GetPlayerId(player_index);
	csmemcpy(&id, &identifier, sizeof(uint64));

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
#ifdef EVENTS_ENABLED
			const uint16 player_abs_index = DATUM_INDEX_TO_ABSOLUTE_INDEX(player_index);
#endif
			event(_event_verbose, "h2mod:gungame: %s - player index: %d, player name: %ws", __FUNCTION__, player_abs_index, player->configuration.player_name);

			int32 level = gungamePlayers[id];
			++level;

			if (level > 16)
			{
				level = 0; // reset level, so we dont keep the player without weapons, in case the game doesnt end
			}

			gungamePlayers[id] = level;

			event(_event_verbose, "h2mod:gungame: %s - player index: %d - new level: %d ", __FUNCTION__, player_abs_index, level);

			if (player->unit_index != NONE)
			{
				if (level == 15)
				{
					event(_event_verbose, "h2mod:gungame: %s - %ws Level 15 - Frag Grenades!", __FUNCTION__, player->configuration.player_name);
					unit_delete_all_weapons(player->unit_index);
					unit_add_grenade_type_to_inventory(player->unit_index, _unit_grenade_human_fragmentation, 99);
				}
				else if (level == 16)
				{
					event(_event_verbose, "h2mod:gungame: %s - %ws Level 16 - Plasma Grenades!", __FUNCTION__, player->configuration.player_name);
					unit_delete_all_weapons(player->unit_index);

					unit_datum* unit = unit_try_and_get(player->unit_index);
					unit->unit.grenade_counts[_unit_grenade_human_fragmentation] = 0;
					unit->unit.grenade_counts[_unit_grenade_covenant_plasma] = 0;
					simulation_action_object_update(player->unit_index, FLAG(_simulation_action_update_grenade_count_bit));

					unit_add_grenade_type_to_inventory(player->unit_index, _unit_grenade_covenant_plasma, 99);
				}
				else
				{
					event(_event_verbose, "h2mod:gungame: %s - %ws on level %d giving them weapon...", __FUNCTION__, player->configuration.player_name, level);

					unit_datum* unit = unit_try_and_get(player->unit_index);
					unit->unit.grenade_counts[_unit_grenade_human_fragmentation] = 0;
					unit->unit.grenade_counts[_unit_grenade_covenant_plasma] = 0;
					simulation_action_object_update(player->unit_index, FLAG(_simulation_action_update_grenade_count_bit));

					unit_delete_all_weapons(player->unit_index);
					call_give_player_weapon(player_index, (datum)k_level_weapons[level], 1);
				}
			}
		}

		break;

	case ExecTime::_ExecTimeUnknown:
	default:
		event(_event_verbose, "h2mod:gungame: %s - unknown execTime", __FUNCTION__);
		break;
	}

	return handled;
}
