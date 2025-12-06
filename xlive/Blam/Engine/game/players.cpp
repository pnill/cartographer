#include "stdafx.h"
#include "players.h"

#include "game/game.h"
#include "game/game_engine.h"
#include "game/game_globals.h"
#include "interface/user_interface_controller.h"
#include "saved_games/game_variant.h"
#include "scenario/scenario.h"
#include "scenario/scenario_definitions.h"
#include "shell/shell.h"
#include "simulation/simulation.h"
#include "simulation/simulation_queue_global_events.h"
#include "units/bipeds.h"
#include "objects/objects.h"

#include "H2MOD/Modules/Shell/Config.h"
#include "H2MOD/Modules/SpecialEvents/SpecialEvents.h"


/* globals */

// k_number_of_users, we have 4 bits left to spare in this value
uint8 g_user_weapon_interactions_mask = UINT8_MAX;


/*
	- NOTES:
	- This gets the player data from the game state, thus it is available only during a match or gameplay (game life cycle is in_game or after map has been loaded)
	- If you need to do something in the pregame lobby, use the functions available in Network Session (Blam/networking/session)
*/

data_array* player_data_get()
{
	return *Memory::GetAddress<data_array**>(0x4A8260, 0x4D64C4);
}

c_player_in_game_iterator::c_player_in_game_iterator(void)
{
	iterator_new(&m_data_iterator, player_data_get());
	return;
}

c_player_with_unit_iterator::c_player_with_unit_iterator(void)
{
	iterator_new(&m_data_iterator, player_data_get());
	return;
}

bool c_player_in_game_iterator::next(void)
{
	m_current_player = (player_datum*)iterator_next(&m_data_iterator);

	while (m_current_player)
	{
		if (!TEST_BIT(m_current_player->flags, _player_left_game_bit))
		{
			break;
		}

		m_current_player = (player_datum*)iterator_next(&m_data_iterator);
	}

	return m_current_player != nullptr;
}

player_datum* c_player_in_game_iterator::get_datum(void) const
{
	return m_current_player;
}

datum c_player_in_game_iterator::get_index(void) const
{
	return m_data_iterator.index;
}

int32 c_player_in_game_iterator::get_absolute_index(void) const
{
	return m_data_iterator.absolute_index;
}

bool c_player_with_unit_iterator::next(void)
{
	m_current_player = (player_datum*)iterator_next(&m_data_iterator);

	while (m_current_player)
	{
		if (m_current_player->unit_index != NONE)
		{
			break;
		}

		m_current_player = (player_datum*)iterator_next(&m_data_iterator);
	}

	return m_current_player != nullptr;
}

player_datum* c_player_with_unit_iterator::get_datum(void) const
{
	return m_current_player;
}

datum c_player_with_unit_iterator::get_index(void) const
{
	return m_data_iterator.index;
}

int32 c_player_with_unit_iterator::get_absolute_index(void) const
{
	return m_data_iterator.absolute_index;
}

s_players_globals* get_players_globals(void)
{
	return *Memory::GetAddress<s_players_globals**>(0x4A825C, 0x4D64C0);
}

datum __cdecl player_index_from_user_index(int32 user_index)
{
	return INVOKE(0x5141D, 0x5992B, player_index_from_user_index, user_index);
}

int32 players_first_active_user(void)
{
	int32 player_num = 0;
	const datum* player_user_mapping = get_players_globals()->player_user_mapping;
	
	for (; player_user_mapping[player_num] == NONE; ++player_num)
	{
		if (player_num >= k_number_of_users)
		{
			player_num = NONE;
			break;
		}
	}
	return player_num;
}

e_controller_index players_get_controller_index_from_user_index(int32 user_index)
{
	e_controller_index result = k_no_controller;

	s_players_globals* player_globals = get_players_globals();

	if (user_index < player_globals->local_player_count)
	{
		datum user_datum = player_globals->player_user_mapping[user_index];

		if (user_datum != NONE)
		{
			for (uint32 index = 0; index < k_number_of_controllers; index++)
			{
				if (player_globals->player_controller_mapping[index] == user_datum)
					result = (e_controller_index)index;
			}
		}
	}

	return result;
}

bool __cdecl player_user_is_elite_or_dervish(int32 user_index)
{
	return INVOKE(0x2220DC, 0, player_user_is_elite_or_dervish, user_index);
}

bool __cdecl players_user_is_active(int32 user_index)
{
	return INVOKE(0x5139B, 0x598BE, players_user_is_active, user_index);
}

datum __cdecl player_index_from_absolute_player_index(uint16 abs_player_index)
{
	return INVOKE(0x513F3, 0x59916, player_index_from_absolute_player_index, abs_player_index);
}

void __cdecl players_set_machines(uint32 new_machine_valid_mask, const s_machine_identifier* new_machine_identifiers)
{
	INVOKE(0x56549, 0x5EA41, players_set_machines, new_machine_valid_mask, new_machine_identifiers);
	return;
}

uint32 player_appearance_required_bits()
{
	return 39;
}

void __cdecl player_configuration_validate_character_type(s_player_configuration* configuration_data)
{
	// Campaign verification
	if (game_is_campaign())
	{
		scenario* scnr = global_scenario_get();
		uint32 block_size = scnr->player_starting_locations.count;
		if (block_size > 0)
		{
			// Go through every player starting location and check if there's a campaign player type set
			// Set the appropriate data in the player to the values of the tagblock
			bool found = false;
			for (uint32 i = 0; i < block_size; ++i)
			{
				const scenario_player* player_starting_location = TAG_BLOCK_GET_ELEMENT(&scnr->player_starting_locations, i, scenario_player);
				if (player_starting_location->campaign_player_type != NONE)
				{
					configuration_data->team_index = _game_team_player;
					configuration_data->profile_traits.profile.player_character_type = (e_character_type)player_starting_location->campaign_player_type;
					found = true;
					break;
				 }
			}

			// If a campaign_player_type type wasn't found in any of the starting locations set default values
			if (!found)
			{
				configuration_data->team_index = _game_team_player;
				configuration_data->profile_traits.profile.player_character_type = _character_type_masterchief;
			}
		}
		else
		{
			configuration_data->team_index = _game_team_player;
			configuration_data->profile_traits.profile.player_character_type = _character_type_masterchief;
		}
	}
	// Multiplayer verification
	else if (game_is_multiplayer())
	{
		// If the character is mastechief set him to a spartan in multiplayer
		if (configuration_data->profile_traits.profile.player_character_type == _character_type_masterchief)
		{
			configuration_data->profile_traits.profile.player_character_type = _character_type_spartan;
		}

		// Don't allow dervish since he's not loaded properly in shared
		if (configuration_data->profile_traits.profile.player_character_type == _character_type_dervish)
		{
			configuration_data->profile_traits.profile.player_character_type = _character_type_elite;
		}

		// Force skeletons in mp during the halloween event
		// Carto addition
		if (e_character_type character = configuration_data->profile_traits.profile.player_character_type;
			character != _character_type_flood && H2Config_spooky_boy && get_current_special_event() == _special_event_halloween)
		{
			configuration_data->profile_traits.profile.player_character_type = _character_type_skeleton;
			if (!shell_is_dedicated_server())
			{
				for (uint32 i = 0; i < k_number_of_users; i++)
				{
					network_session_interface_set_local_user_character_type(i, _character_type_skeleton);
					//user_interface_controller_update_network_properties((e_controller_index)i);
				}
			}
		}
	}
}

void __cdecl player_validate_configuration(datum player_index, s_player_configuration* configuration_data)
{
	player_configuration_validate_character_type(configuration_data);
	
	// General character verification
	e_character_type character = configuration_data->profile_traits.profile.player_character_type;
	if (character != NONE)
	{
		if (character >= _character_type_masterchief)
		{
			s_game_globals* globals = scenario_get_game_globals();
			if (character > (e_character_type)globals->player_representation.count - 1)
			{
				character = (e_character_type)(globals->player_representation.count - 1);
			}
		}
		else
		{
			character = _character_type_masterchief;
		}
		configuration_data->profile_traits.profile.player_character_type = character;
	}

	// Skill verification
	int8 player_displayed_skill = configuration_data->player_displayed_skill;
	if (player_displayed_skill != NONE)
	{
		if (player_displayed_skill < 0)
			player_displayed_skill = 0;
		configuration_data->player_displayed_skill = player_displayed_skill;
	}
	int8 player_overall_skill = configuration_data->player_overall_skill;
	if (player_overall_skill != NONE)
	{
		if (player_overall_skill < 0)
		{
			player_overall_skill = 0;
		}
		configuration_data->player_overall_skill = player_overall_skill;
	}

	// Handicap verification
	configuration_data->player_handicap_level = PIN(configuration_data->player_handicap_level, _user_interface_controller_handicap_none, _user_interface_controller_handicap_severe);
	
	// User role verification
	int8 bungie_user_role = configuration_data->bungie_user_role;
	if (bungie_user_role < 0 || bungie_user_role > 7)
	{
		configuration_data->bungie_user_role = 0;
	}

	if (current_game_engine())
	{
		if (TEST_BIT(get_game_variant()->game_engine_flags, _game_engine_teams_bit))
		{
			if (configuration_data->team_index != _game_team_observer && !TEST_BIT(game_engine_globals_get()->team_bitmask, configuration_data->team_index))
			{
				configuration_data->team_index = _game_team_observer;
			}
		}
	}

	return;
}

void __cdecl players_update_activation(void)
{
	if (!game_is_predicted())
	{
		data_iterator iterator;
		iterator_new(&iterator, player_data_get());

		player_datum* player = (player_datum*)iterator_next(&iterator);
		while (player)
		{
			if (!TEST_BIT(player->flags, _player_left_game_bit))
			{
				bool machine_active_in_game = TEST_BIT(player->flags, _player_active_in_game_bit);
				bool insert_event = false;
				if (game_is_distributed())
				{
					if (!game_is_playback())
					{
						machine_active_in_game = simulation_get_machine_active_in_game(&player->machine_identifier);
						insert_event = true;
					}
				}
				else
				{
					machine_active_in_game = true;
				}

				if (TEST_BIT(player->flags, _player_active_in_game_bit) != machine_active_in_game)
				{
					if (insert_event)
					{
						s_simulation_queue_player_event_data event_data{ machine_active_in_game };
						simulation_queue_player_event_insert(_simulation_queue_player_event_update, iterator.index, &event_data);
					}
					else
					{
						SET_BIT(player->flags, _player_active_in_game_bit, machine_active_in_game);

						if (machine_active_in_game)
						{
							game_engine_player_activated(iterator.index);
						}
					}
				}
			}

			player = (player_datum*)iterator_next(&iterator);
		}
	}
	return;
}

void __cdecl player_examine_nearby_biped(datum biped_datum, datum player_datum, s_player_interaction_context* out_action_context)
{
	void* player_examine_nearby_biped_usercall = Memory::GetAddress<void*>(0x53E43);
	__asm
	{
		push out_action_context
		push player_datum
		mov ebx, biped_datum

		call player_examine_nearby_biped_usercall

		add esp, 2 * 4
	}
}

void __cdecl player_examine_nearby_vehicle(datum vehicle_datum, datum player_datum, s_player_interaction_context* out_action_context)
{
	void* player_examine_nearby_vehicle_usercall = Memory::GetAddress<void*>(0x53CC7);
	__asm
	{
		push out_action_context
		push player_datum
		mov eax, vehicle_datum

		call player_examine_nearby_vehicle_usercall

		add esp, 2 * 4
	}
}

void player_user_weapon_interaction_set(int32 user_index, bool enabled)
{
	ASSERT(VALID_INDEX(user_index, k_number_of_users));

	SET_BIT(g_user_weapon_interactions_mask, user_index, enabled);
	return;
}

void player_user_weapon_interaction_reset(void)
{
	// Set all bits to 1
	g_user_weapon_interactions_mask = UINT8_MAX;
	return;
}

void player_examine_nearby_weapon(datum weapon_datum, datum player_index, s_player_interaction_context* out_action_context)
{
	const player_datum* player = (const player_datum*)datum_get(player_data_get(), player_index);

	if (player->user_index != NONE)
		if (!TEST_BIT(g_user_weapon_interactions_mask, player->user_index))
			return;

	void* player_examine_nearby_weapon_usercall = Memory::GetAddress<void*>(0x53F17);
	__asm
	{
		push weapon_datum
		push player_index
		mov eax, out_action_context

		call player_examine_nearby_weapon_usercall

		add esp, 2 * 4
	}
}

void __cdecl player_examine_nearby_control(datum control_datum, datum player_datum, s_player_interaction_context* out_action_context)
{
	void* player_examine_nearby_control_usercall = Memory::GetAddress<void*>(0x51FB2);
	__asm
	{
		push out_action_context
		push player_datum
		mov ebx, control_datum

		call player_examine_nearby_control_usercall

		add esp, 2 * 4
	}
}

void __cdecl player_find_action_context(datum player_index, s_player_interaction_context* out_action_context)
{
	const player_datum* player = (const player_datum*)datum_get(player_data_get(), player_index);

	ASSERT(out_action_context);

	out_action_context->target_datum = 0;
	out_action_context->field_4 = NONE;
	out_action_context->field_8 = NONE;

	const datum unit_index = player->unit_index;
	if (unit_index != NONE)
	{
		biped_datum* player_unit = biped_get(unit_index);

		if (player_unit->object.parent_object_index == NONE)
		{
			// Search 1 includes weapons
			// Search 2 excludes weapons
			const int32 search_types[2]
			{
				_object_mask_weapon,
				~_object_mask_weapon
			};
			// We have a shorter search radius for weapons than other objects
			const real32 search_radius_types[2]
			{
				player_unit->object.radius + 0.4f,
				player_unit->object.radius + 3.4f
			};

			// Perform the two searches
			for (uint8 search_num = 0; search_num < NUMBEROF(search_types); ++search_num)
			{
				const int32 search_type = search_types[search_num];
				const real32 search_radius = search_radius_types[search_num];

				const int32 objects_to_ignore = _object_mask_garbage | _object_mask_projectile | _object_mask_sound_scenery | _object_mask_creature;

				datum nearby_objects[64];

				const int16 number_of_initial_objects = (int16)object_search_for_objects_in_radius(
					0,
					(search_type & ~objects_to_ignore),
					&player_unit->object.location,
					&player_unit->object.center,
					search_radius,
					nearby_objects,
					NUMBEROF(nearby_objects));

				// This is used later on in this function for the default condition in the examine switch case
				int16 nearby_object_num = number_of_initial_objects;

				for (int16 object_num = 0; object_num < nearby_object_num; object_num++)
				{
					const datum nearby_object_index = nearby_objects[object_num];
					const object_datum* object = object_get(nearby_object_index);
					const e_object_type type = object->object.object_identifier.get_type();
					
					// If our object is not one of the objects we want to ignore
					if (!TEST_BIT(objects_to_ignore, type))
					{
						real_vector3d delta;
						vector_from_points3d(&player_unit->object.center, &object->object.center, &delta);
						const real32 magnitude = magnitude_squared3d(&delta);
						const real32 combined_radius = object->object.radius + search_radius;

						const uint32 object_type_requires_radius_check = _object_mask_weapon | _object_mask_machine | _object_mask_control;

						// If the object is not of the following types then we have to run the following check:
						// Compare the magnitude of the vector between the player and object and make sure we aren't going outside the combined radius squared
						if (!TEST_BIT(object_type_requires_radius_check, type) || magnitude <= combined_radius * combined_radius)
						{
							switch (type)
							{
							case _object_type_biped:
								player_examine_nearby_biped(nearby_object_index, player_index, out_action_context);
								break;
							case _object_type_vehicle:
								player_examine_nearby_vehicle(nearby_object_index, player_index, out_action_context);
								break;
							case _object_type_weapon:
								player_examine_nearby_weapon(nearby_object_index, player_index, out_action_context);
								break;
							case _object_type_control:
								player_examine_nearby_control(nearby_object_index, player_index, out_action_context);
								break;
							default:
							{
								// Search through the children of the object and add them to the nearby objects array
								for (datum current_weapon_datum = object->object.current_weapon_datum; current_weapon_datum != NONE; ++nearby_object_num)
								{
									if (nearby_object_num > NUMBEROF(nearby_objects))
									{
										break;
									}

									const object_datum* weapon_object = object_get(current_weapon_datum);
									nearby_objects[nearby_object_num] = current_weapon_datum;
									current_weapon_datum = weapon_object->object.next_object_index;
								}
								break;
							}
							}
						}
					}
				}
			}
		}
	}
}


int16 local_player_count(void)
{
	return get_players_globals()->local_player_count;
}

void __cdecl players_update_before_game(const struct simulation_update* update)
{
	INVOKE(0x5815E, 0x60656, players_update_before_game, update);
	return;
}

void __cdecl players_update_after_game(const struct simulation_update* update)
{
	INVOKE(0x58C22, 0x6111A, players_update_after_game, update);
	return;
}

void players_update_for_checkpoint(void)
{
	players_joined_in_progress_allow_spawn();
	return;
}

void players_joined_in_progress_allow_spawn(void)
{
	// TODO: implement
	return;
}

void players_apply_patches(void)
{
	// Change the validation for player_appearance_valid to use the updated k_player_character_type_count constant
	WriteValue<BYTE>(Memory::GetAddress(0x54fb2, 0x5D4AA) + 1, k_player_character_type_count);

	// Replace the player profile validation function with our own
	PatchCall(Memory::GetAddress(0x5509E, 0x5D596), player_validate_configuration);

	// Replace update activation to insert events into the simulation queue
	PatchCall(Memory::GetAddress(0x58182, 0x6067A), players_update_activation);

	PatchCall(Memory::GetAddress(0x936F2, 0x4B9F2), player_find_action_context);
	return;
}

