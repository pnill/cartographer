#include "stdafx.h"
#include "Infection.h"

#include "cache/cache_files.h"
#include "game/game.h"
#include "game/game_engine_util.h"
#include "game/game_time.h"
#include "game/players.h"
#include "interface/user_interface_controller.h"
#include "items/item_definitions.h"
#include "networking/logic/life_cycle_manager.h"
#include "networking/logic/network_life_cycle.h"
#include "networking/logic/network_session_interface.h"
#include "networking/session/network_session.h"
#include "networking/network_event.h"
#include "networking/network_game_definitions.h"
#include "scenario/scenario.h"
#include "scenario/scenario_definitions.h"
#include "shell/shell.h"
#include "units/bipeds.h"

#include "H2MOD.h"
#include "game/game_engine.h"
#include "H2MOD/Modules/SpecialEvents/SpecialEvents.h"
#include "H2MOD/Modules/Shell/Config.h"
#include "H2MOD/Modules/EventHandler/EventHandler.hpp"
#include "interface/new_hud_draw.h"

static std::vector<uint64> zombieIdentifiers;

/* constants */

enum
{
	k_use_flood_zombies = true,
};

#define k_humans_team _game_team_red
#define k_zombie_team _game_team_green

#define k_human_unit_speed 1.f
#define k_zombie_unit_speed 1.1f

/* globals */

bool initialSpawn;
bool infectedPlayed[k_number_of_users]{};
int32 zombiePlayerIndex = NONE;
int32 last_time_at_game_should_not_end = 0;
const wchar_t *const infectionSoundTable[k_language_count][e_infection_sounds::_infection_end]
{
	{SND_INFECTION_EN, SND_INFECTED_EN, SND_NEW_ZOMBIE_EN },
	{SND_INFECTION_JP, SND_INFECTED_JP, SND_NEW_ZOMBIE_JP },
	{SND_INFECTION_GE, SND_INFECTED_GE, SND_NEW_ZOMBIE_GE },
	{SND_INFECTION_FR, SND_INFECTED_FR, SND_NEW_ZOMBIE_FR },
	{SND_INFECTION_ES, SND_INFECTED_ES, SND_NEW_ZOMBIE_ES },
	{SND_INFECTION_IT, SND_INFECTED_IT, SND_NEW_ZOMBIE_IT },
	{SND_INFECTION_KO, SND_INFECTED_KO, SND_NEW_ZOMBIE_KO },
	{SND_INFECTION_CH, SND_INFECTED_CH, SND_NEW_ZOMBIE_CH },
};

/* prototypes */

static e_character_type infection_human_get_player_type(void);

static e_character_type infection_zombie_get_character_type(void);

static void reset_zombie_player_status(void);

static void set_zombie_player_status(uint64 identifier);

/* public code */

int Infection::calculateZombiePlayerIndex()
{
	c_network_session* session = NULL;
	network_life_cycle_in_squad_session(&session);

	if (session->get_player_count() > 0)
	{
		std::mt19937 mt_rand(rd());
		std::vector<int32> activePlayersIndices = NetworkSession::GetActivePlayerIndicesList();
		std::uniform_int_distribution<int> dist(0, session->get_player_count() - 1);
	
		if (activePlayersIndices.empty())
			return NONE;

		int32 infectedPlayerIndex = activePlayersIndices[dist(mt_rand)];
		event(_event_verbose, "h2mod:infection: random infection player index: %d, with name: %ws", infectedPlayerIndex, session->get_player_name(infectedPlayerIndex));

		return infectedPlayerIndex;
	}

	return NONE;
}

void Infection::sendTeamChange()
{
	c_network_session* session = NULL;
	if (network_life_cycle_in_squad_session(&session))
	{
		if (session->is_host())
		{
			int32 player_count = session->get_player_count();

			if (player_count > 0)
			{
				int32 player_array_index = 0;
				datum player_indexes[k_maximum_players] = {};
				e_game_team player_teams[k_maximum_players] = {};

				for (int32 i = 0; i < k_maximum_players; i++)
				{
					if (session->is_session_player_active(i))
					{
						const bool is_current_player_zombie = zombiePlayerIndex == i;
						const e_game_team team = is_current_player_zombie ? k_zombie_team : k_humans_team;

						player_indexes[player_array_index] = i;
						player_teams[player_array_index++] = team;

						event(_event_verbose, "h2mod:infection: sent team change packet to player index: %d, with name: %ws, infected?: %d",
							i,
							session->get_player_name(i),
							is_current_player_zombie
						);
					}
				}

				session->switch_players_to_teams(player_indexes, player_array_index, player_teams);
			}
		}
	}
}

void Infection::triggerSound(e_infection_sounds sound, int sleep)
{
	const int language_id = *Memory::GetAddress<int*>(0x412818);

	if (infectionSoundTable[language_id][sound] != nullptr)
	{
		event(_event_verbose, "h2mod:infection: Triggering sound %ws", infectionSoundTable[language_id][sound]);
		H2MOD::custom_sound_play(infectionSoundTable[language_id][sound], sleep);
	}
}

void Infection::InitClient()
{
	event(_event_status, "h2mod:infection: Disabling slayer sounds");
	H2MOD::disable_score_announcer_sounds(FLAG(_sound_type_slayer) | ALL_SOUNDS_NO_SLAYER);

	for (int16 i = 0; i < k_number_of_users; i++)
		infectedPlayed[i] = false;

	initialSpawn = true;

	//Change Local Player's Team to Human if Not in Green
	//(In case player wants to start as Alpha Zombie leave him green)

	for(int32 i = 0; i < k_number_of_users; i++)
	{
		e_controller_index user_controller = players_get_controller_index_from_user_index(i);
		if(user_controller != k_no_controller)
		{
			e_game_team team = (e_game_team)network_session_interface_get_team_index(i);
			if (team != k_zombie_team) {
				user_interface_controller_set_desired_team_index(user_controller, k_humans_team);
				user_interface_controller_update_network_properties(user_controller);
			}
		}
	}
}

void Infection::InitHost() {
	event(_event_status, "h2mod:infection: Host init setting unit speed patch");
	//Applying SpeedCheck fix
	g_game_engine_override_player_speed_update = true;

	event(_event_status, "h2mod:infection: Host init resetting zombie player data status");
	reset_zombie_player_status();
}

bool Infection::shouldEndGame()
{
	int humanCount, zombieCount, playerCount;
	humanCount = zombieCount = playerCount = 0;
	c_player_in_game_iterator player_it;
	while (player_it.next())
	{
		uint64 id;
		csmemcpy(&id, &player_it.get_datum()->player_identifier, sizeof(uint64));

		bool isZombie = std::find(zombieIdentifiers.begin(), zombieIdentifiers.end(), id) != zombieIdentifiers.end();

		if (isZombie)
			zombieCount++;
		else
			humanCount++;

		playerCount++;
	}

	return playerCount > 1 && (zombieCount == 0 || humanCount == 0);
}

void Infection::resetWeaponInteractionAndEmblems() {
	event(_event_verbose, "h2mod:infection: Resetting weapons interactions and emblem visibility");
	player_user_weapon_interaction_reset();
	hud_player_indicators_draw_reset();
}

void Infection::preSpawnServerSetup() {

	/*
		Game state players should be initialized when we are about to spawn a player
	*/

	c_network_session* session = NULL;
	network_life_cycle_in_squad_session(&session);
	
	c_player_in_game_iterator player_it;
	while (player_it.next())
	{
		player_datum* player = player_it.get_datum();
		const int32 current_player_index = player_it.get_absolute_index();

		uint64 id;
		csmemcpy(&id, &player->player_identifier, sizeof(uint64));

		bool isZombie = std::find(zombieIdentifiers.begin(), zombieIdentifiers.end(), id) != zombieIdentifiers.end();
		
		const bool zombie_team_status_human = isZombie == false && player->configuration.team_index == k_zombie_team;
		if (zombie_team_status_human)
		{

			// if the player just joined the and he doesn't have zombie status, and his team is green, add him in the array
			set_zombie_player_status(id);
			isZombie = true;
		}

		event(_event_verbose, "h2mod:infection: Zombie pre spawn index = %d, isZombie = %d, playerIdentifier = %llu, playerName:%ws", current_player_index, isZombie, id, player->configuration.name);
		if (isZombie) 
		{
			player->configuration.appearance.player_character_type = infection_zombie_get_character_type();
			if (player->configuration.team_index != k_zombie_team)
			{
				// prevent the fucks from switching to humans in the pre-game lobby after joining
				session->switch_player_team(current_player_index, k_zombie_team);
			}
		}
		else 
		{
			player->configuration.appearance.player_character_type = infection_human_get_player_type();
		}
	}
}

void Infection::setPlayerAsHuman(int32 player_index)
{
	player_datum* player = player_get(player_index);

	player->configuration.appearance.player_character_type = infection_human_get_player_type();
	player->unit_speed = k_human_unit_speed;
	g_game_engine_override_player_speed_changed.set(DATUM_INDEX_TO_ABSOLUTE_INDEX(player_index), true);
}

void Infection::setPlayerAsZombie(int32 player_index)
{
	player_datum* player = player_get(player_index);

	player->configuration.appearance.player_character_type = infection_zombie_get_character_type();
	player->unit_speed = k_zombie_unit_speed;
	g_game_engine_override_player_speed_changed.set(DATUM_INDEX_TO_ABSOLUTE_INDEX(player_index), true);
	call_give_player_weapon(player_index, e_weapons_datum_index::energy_blade, 1);
	
	return;
}

void Infection::onGameTick()
{
	if (network_life_cycle_get_state() == _life_cycle_state_in_game && NetworkSession::LocalPeerIsSessionHost())
	{
		if (game_time_get() > 0)
		{
			bool should_end_game = shouldEndGame();

			// check if the current game should be ended
			if (!should_end_game)
			{
				// and update the time if it shouldn't be
				last_time_at_game_should_not_end = game_time_get();
			}

			// check the difference between game time now
			// if the time wasn't updated for more than 5 seconds, end the game
			if (game_ticks_to_seconds((real32)(game_time_get() - last_time_at_game_should_not_end)) > 5.f)
			{
				NetworkSession::EndGame();
			}
		}
	}
}

void Infection::removeUnwantedItems()
{
	const datum shotgun_ammo_equip_datum = tag_loaded(_tag_group_equipment, "objects\\powerups\\shotgun_ammo\\shotgun_ammo");

	tag_iterator iterator;
	tag_iterator_new(&iterator, _tag_group_item_collection);

	while (tag_iterator_next(&iterator) != NONE)
	{
		char const* tag_name = tag_get_name(iterator.current_tag_index);

		if (strstr(tag_name, "multiplayer\\powerups") ||
			strncmp(tag_name, "multiplayer\\single_weapons\\frag_grenades", 256) == 0 ||
			strncmp(tag_name, "multiplayer\\single_weapons\\plasma_grenades", 256) == 0)
		{
			item_collection_definition const* item_collection = item_collection_definition_get(iterator.current_tag_index);

			for (int32 i = 0; i < item_collection->item_permutations.count; i++)
			{
				struct item_permutation_definition* item_permutation_definition = TAG_BLOCK_GET_ELEMENT(
					&item_collection->item_permutations,
					i,
					struct item_permutation_definition
				);

				item_permutation_definition->item.group = _tag_group_equipment;
				item_permutation_definition->item.index = shotgun_ammo_equip_datum;
			}
		}
	}

	//Replace vehicles with shotgun ammo
	scenario const* scenario_definition = global_scenario_get();

	for (int32 i = 0; i < scenario_definition->netgame_equipment.count; i++)
	{
		scenario_netgame_equipment* netgame_equipment = TAG_BLOCK_GET_ELEMENT(&scenario_definition->netgame_equipment, i, scenario_netgame_equipment);
		if (netgame_equipment->item_vehicle_collection.group == _tag_group_vehicle_collection)
		{
			netgame_equipment->classification = netgame_item_classification_powerup;
			netgame_equipment->item_vehicle_collection.group = _tag_group_item_collection;
			netgame_equipment->item_vehicle_collection.index = NONE;
		}
	}

	return;
}

void Infection::Initialize()
{
	event(_event_status, "h2mod:infection: infection initialization!");
	removeUnwantedItems();

	if (!shell_is_dedicated_server())
	{
		Infection::InitClient();
	}

	if (NetworkSession::LocalPeerIsSessionHost())
	{
		Infection::InitHost();

		last_time_at_game_should_not_end = 0;
		zombiePlayerIndex = Infection::calculateZombiePlayerIndex();
		EventHandler::register_callback(onGameTick, EventType::game_loop, EventExecutionType::execute_after);

		event(_event_verbose, "h2mod:infection: Peer host calculated zombie index %d", zombiePlayerIndex);
		if (zombiePlayerIndex == NONE)
		{
			event(_event_verbose, "h2mod:infection: Failed selecting a zombie!");
		}
		else 
		{
			event(_event_verbose, "h2mod:infection:  Peer host setting player as human");
			//send out the team change packets to peers
			Infection::sendTeamChange();

			uint64 id;
			const s_player_identifier identifier = NetworkSession::GetPlayerId(zombiePlayerIndex);
			csmemcpy(&id, &identifier, sizeof(uint64));
			set_zombie_player_status(id);
		}
	}
}

void Infection::Dispose()
{
	event(_event_status, "h2mod:infection: infection dispose!");

	if(NetworkSession::LocalPeerIsSessionHost())
	{
		EventHandler::remove_callback(onGameTick, EventType::game_loop, EventExecutionType::execute_after);
	}

	Infection::resetWeaponInteractionAndEmblems();
	if (!game_is_predicted()) {
		g_game_engine_override_player_speed_update = false;
	}

	return;
}

CustomVariantId Infection::GetVariantId()
{
	return CustomVariantId::_id_infection;
}

void Infection::OnMapLoad(ExecTime execTime, e_game_mode game_mode)
{
	switch (execTime)
	{
	case ExecTime::_preEventExec:
		break;

	case ExecTime::_postEventExec:
		switch (game_mode)
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
		event(_event_verbose, "h2mod:infection: %s - unknown execTime", __FUNCTION__);
		break;
	}
}

void Infection::OnPlayerDeath(ExecTime execTime, datum player_index)
{
	player_datum* player = player_get(player_index);

	switch (execTime)
	{
	case ExecTime::_preEventExec:
		// to note after the original function executes, the controlled unit by this player is set to NONE
		if (game_engine_in_round())
		{
			if (!shell_is_dedicated_server())
			{
				if (player->configuration.team_index != k_zombie_team)
				{
					if (player->user_index != NONE)
					{
						event(_event_verbose, "h2mod:infection: Infected local player, Name=%ws, identifier=%llu", player->configuration.name, player->player_identifier);
						user_interface_controller_set_desired_team_index(player->controller_index, k_zombie_team);
						user_interface_controller_update_network_properties(player->controller_index);
						player->configuration.appearance.player_character_type = infection_zombie_get_character_type();
					}
					else
					{
						//if not, then this is a new zombie
						event(_event_verbose, "h2mod:infection: Player died, name=%ws, identifer=%llu", player->configuration.name, player->player_identifier);
						Infection::triggerSound(_snd_new_zombie, 1000);
					}
				}
			}

			// host code
			if (!game_is_predicted())
			{
				if (biped_try_and_get(player->unit_index))
				{
					if (unit_get_team_index(player->unit_index) != k_zombie_team)
					{
						uint64 id;
						csmemcpy(&id, &player->player_identifier, sizeof(uint64));
						set_zombie_player_status(id);
					}
					else
					{
						// take away zombie's weapons
						unit_delete_all_weapons(player->unit_index);
					}
				}
			}
		}
		break;

	case ExecTime::_postEventExec:
		break;

	case ExecTime::_ExecTimeUnknown:
	default:
		event(_event_verbose, "h2mod:infection: %s - unknown execTime", __FUNCTION__);
		break;
	}
}

void Infection::OnPlayerSpawn(ExecTime execTime, datum player_index)
{
	const uint16 player_abs_index = DATUM_INDEX_TO_ABSOLUTE_INDEX(player_index);
	player_datum* player = player_get(player_index);

	switch (execTime)
	{
		// prespawn handler
	case ExecTime::_preEventExec:
		// to note after the original function executes, the controlled unit by this player is set to NONE

		if (!shell_is_dedicated_server())
		{
			event(_event_verbose, "h2mod:infection: Client pre spawn, playerIndex=%d, playerIdentifier=%llu", player_abs_index, player->player_identifier);

			if(player->user_index != NONE)
			{
				e_game_team team = (e_game_team)network_session_interface_get_team_index(player->user_index);

				// change biped if user is a zombie

				if(team == k_zombie_team)
				{
					event(_event_verbose, "h2mod:infection: Client is infected! switching bipeds: %d", player_abs_index);
					player->configuration.appearance.player_character_type = infection_zombie_get_character_type();
				}
			}
		}

		if (!game_is_predicted())
			Infection::preSpawnServerSetup();
		break;

		// postspawn handler
	case ExecTime::_postEventExec:

		// client only
		if (!shell_is_dedicated_server())
		{
			if(player->user_index != NONE)
			{
				if(initialSpawn)
				{
					triggerSound(e_infection_sounds::_snd_infection, 1000);
					initialSpawn = false;
				}

				e_game_team team = (e_game_team)network_session_interface_get_team_index(player->user_index);

				if(team == k_zombie_team && !infectedPlayed[player->user_index])
				{
					triggerSound(_snd_infected, 500);
					infectedPlayed[player->user_index] = true;
				}
				if(team == k_humans_team)
				{
					player_user_weapon_interaction_set(player->user_index, true);
					hud_player_indicators_draw_enabled_set(player->user_index, false);
				}
				else if (team == k_zombie_team)
				{
					player->configuration.appearance.player_character_type = infection_zombie_get_character_type();
					player_user_weapon_interaction_set(player->user_index, false);
					hud_player_indicators_draw_enabled_set(player->user_index, true);
				}
			}
		}

		// host only (both client/dedicated server)
		if (!game_is_predicted())
		{
			event(_event_verbose, "h2mod:infection: Spawn player server index=%d", player_abs_index);
			if (biped_try_and_get(player->unit_index))
			{
				//if the unit_object data pointer is not nullptr, the spawned object is "alive"
				e_game_team team = unit_get_team_index(player->unit_index);
				event(_event_verbose, "h2mod:infection: Spawn player server index=%d, unit team index=%d", player_abs_index, (int16)team);
				if (team == k_humans_team)
				{
					Infection::setPlayerAsHuman(player_index);
				}
				else if (team == k_zombie_team)
				{
					Infection::setPlayerAsZombie(player_index);
				}
			}
		}

		break;

	case ExecTime::_ExecTimeUnknown:
	default:
		event(_event_verbose, "h2mod:infection: %s - unknown execTime", __FUNCTION__);
		break;
	}
}

/* private code */

static e_character_type infection_human_get_player_type(void)
{
	const bool human_should_be_skeleton = get_current_special_event() == _special_event_halloween && H2Config_spooky_boy;
	return human_should_be_skeleton ? _character_type_skeleton : _character_type_spartan;
}

static e_character_type infection_zombie_get_character_type(void)
{
	return k_use_flood_zombies ? _character_type_flood : _character_type_elite;
}

static void reset_zombie_player_status(void)
{
	zombieIdentifiers.clear();
}

static void set_zombie_player_status(uint64 identifier)
{
	zombieIdentifiers.push_back(identifier);
}
