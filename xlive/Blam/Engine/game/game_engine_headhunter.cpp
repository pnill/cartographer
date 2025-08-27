#include "stdafx.h"
#include "game_engine_headhunter.h"

#include "game.h"
#include "game_engine.h"
#include "game_engine_util.h"
#include "game_time.h"
#include "camera/camera.h"
#include "interface/hud_definitions.h"
#include "interface/new_hud.h"
#include "interface/new_hud_definitions.h"
#include "interface/user_interface_text.h"
#include "items/weapons.h"
#include "math/random_math.h"
#include "objects/objects.h"
#include "objects/object_placement.h"
#include "rasterizer/rasterizer_text.h"
#include "simulation/game_interface/simulation_game_action.h"
#include "simulation/game_interface/simulation_game_engine_headhunter.h"
#include "simulation/game_interface/simulation_game_entities.h"
#include "simulation/game_interface/simulation_game_events.h"
#include "tag_files/tag_loader/tag_injection.h"
#include "text/draw_string.h"
#include "units/bipeds.h"

static c_headhunter_engine g_headhunter_engine;
static c_headhunter_engine_globals* g_headhunter_engine_globals{};
static datum g_ball_datum = NONE;

e_game_engine_type c_headhunter_engine::get_type()
{
	return _game_engine_type_headhunter;
}

bool c_headhunter_engine::setup()
{
	c_king_engine::setup();

	s_game_engine_globals* engine_globals = game_engine_globals_get();

	g_headhunter_engine_globals = (c_headhunter_engine_globals*)engine_globals->game_engine_globals;

	memset(g_headhunter_engine_globals->m_player_skull_count, 0, sizeof(g_headhunter_engine_globals->m_player_skull_count));

	g_ball_datum = tag_loaded(_tag_group_weapon, "objects\\weapons\\multiplayer\\ball\\ball");

	if (g_ball_datum == NONE)
		return false;

	s_multiplayer_globals_definition* m_globals = get_multiplayer_globals();

	s_multiplayer_runtime_globals_definition* runtime = m_globals->runtime[0];

	uint32 data_offset = 0;

	s_multiplayer_event_response_definition* head_hunter_events =
		(s_multiplayer_event_response_definition*)tag_injection_reserve_cache_memory(
			sizeof(s_multiplayer_event_response_definition) * (runtime->king_events.count + 1),
			&data_offset);

	csmemcpy(head_hunter_events, runtime->king_events[0], sizeof(s_multiplayer_event_response_definition) * runtime->king_events.count);

	runtime->headhunter_events.count = runtime->king_events.count + 1;
	runtime->headhunter_events.data = data_offset;

	for (int32 i = 0; i < runtime->headhunter_events.count; ++i)
	{
		s_multiplayer_event_response_definition* event = &head_hunter_events[i];
		event->type = _multiplayer_event_response_game_type_headhunter;
	}

	runtime->headhunter_events[0]->display_string = 0x110028AB;
	csmemset(&runtime->headhunter_events[0]->primary_sound, 0, sizeof(s_multiplayer_event_sound_response_definition));
	// todo: replace sound with injected sound when complete.

	s_multiplayer_event_response_definition* skull_pickup_event = runtime->headhunter_events[runtime->king_events.count];

	skull_pickup_event->event = _multiplayer_event_response_headhunter_head_grabbed;

	skull_pickup_event->audience = _multiplayer_event_response_audience_cause_player;

	skull_pickup_event->primary_sound.sound_flags.set(_multiplayer_event_response_sound_announcer_sound, false);

	datum ping_sound = tag_loaded(_tag_group_sound, "sound\\game_sfx\\multiplayer\\target_point_collected");

	for (uint32 language = 0; language < k_language_count; ++language)
	{
		skull_pickup_event->primary_sound.sound[language].group.group = _tag_group_sound;
		skull_pickup_event->primary_sound.sound[language].index = ping_sound;
	}

	// swap the indicator for the hill from crown to skull
	datum hud_globals_datum = tag_loaded(_tag_group_hud_globals, "ui\\hud\\default");

	if (hud_globals_datum != NONE)
	{
		hud_globals_definition* hud_globals = (hud_globals_definition*)tag_get_fast(hud_globals_datum);

		hud_globals->new_globals.waypoints[6]->onscreen_sequence_index = 4;
		hud_globals->new_globals.waypoints[6]->offscreen_sequence_index = 4;
		hud_globals->new_globals.waypoints[6]->occluded_sequence_index = 4;
	}
	//tag_injection_set_active_map(L"elongation");
	//if(tag_injection_active_map_verified())
	//{
	//	// effect doesn't actually work?
	//	datum small_fire = tag_injection_load(_tag_group_scenery, "effects\\scenery\\fires\\fire_basic_small", true);
	//	if(small_fire)
	//	{
	//		tag_injection_inject();

	//		weapon_definition* ball_weapon = (weapon_definition*)tag_get_fast(g_ball_datum);
	//		s_model_definition* ball_model = (s_model_definition*)tag_get_fast(ball_weapon->object.model.index);
	//		s_model_variant* ball_variant = (s_model_variant*)tag_injection_extend_block(&ball_model->variants, sizeof(s_model_variant), 1);

	//		memset(ball_variant->runtime_model_region_index, -1, sizeof(ball_variant->runtime_model_region_index));

	//		s_model_variant_object* ball_variant_object = (s_model_variant_object*)tag_injection_extend_block(&ball_variant->objects, sizeof(s_model_variant_object), 1);

	//		ball_variant_object->parent_marker = _string_id_ground_point;
	//		ball_variant_object->child_object.group.group = _tag_group_scenery;
	//		ball_variant_object->child_object.index = small_fire;

	//	}
	//}

	return true;
}

bool c_headhunter_engine::function_4()
{
	s_game_variant* variant = get_game_variant();

	// instead of a hill being spawned at the start of a round, an intentional delay has been set so people can't speed run the point.
	g_headhunter_engine_globals->m_hill_id = -1;
	g_headhunter_engine_globals->m_ticks_till_hill_move = time_globals::seconds_to_ticks_round(15);

	return true;
}

void c_headhunter_engine::send_game_start_event(datum player_index)
{
	s_game_engine_event event{};
	game_engine_event_new(_multiplayer_event_response_game_type_headhunter, _multiplayer_event_response_headhunter_game_start, &event);
	event.player_index = player_index;
	game_engine_send_event(&event);
}

void c_headhunter_engine::swap_player_indices(uint32 old_index, uint32 new_index)
{
	g_headhunter_engine_globals->m_player_skull_count[new_index] = g_headhunter_engine_globals->m_player_skull_count[old_index];
	g_headhunter_engine_globals->m_player_skull_count[old_index] = 0;

	c_king_engine::swap_player_indices(old_index, new_index);
}

void c_headhunter_engine::render_game_engine_hud_elements(uint32 user_index)
{
	draw_skulls_carried_string(user_index);
	c_king_engine::render_game_engine_hud_elements(user_index);
}

real32 c_headhunter_engine::get_player_speed_modifier(datum player_index)
{
	if (g_headhunter_engine_globals->m_player_skull_count[DATUM_INDEX_TO_ABSOLUTE_INDEX(player_index)])
	{
		const s_game_variant* variant = get_game_variant();
		switch (variant->game_engine_variant.head_hunter.speed_with_heads)
		{
		case _ctf_engine_player_speed_slow:
			return 0.75f;
		case _ctf_engine_player_speed_fast:
			return 1.25f;
		default:
			return 1.f;
		}
	}
	return 1.f;
}

void c_headhunter_engine::player_killed(datum killing_player, datum killed_player, bool suicide, int32 unk_index)
{
	player_datum* player = (player_datum*)datum_get(player_data_get(), killing_player);

	const biped_datum* biped_unit = (biped_datum*)object_try_and_get_and_verify_type(player->unit_index, _object_mask_biped);

	uint16 player_index = DATUM_INDEX_TO_ABSOLUTE_INDEX(killed_player);

	if (biped_unit != NULL && IN_RANGE(player_index, 0, k_maximum_players))
	{
		LOG_TRACE_GAME(L"[{}] {} Died dropping {} skulls", __FUNCTIONW__, player->properties->player_name, g_headhunter_engine_globals->m_player_skull_count[player_index] + 1);

		object_placement_data placement_data;

		object_placement_data_new(&placement_data, g_ball_datum, -1, 0);

		placement_data.position = biped_unit->object.position;

		for (int32 i = 0; i < g_headhunter_engine_globals->m_player_skull_count[player_index] + 1; ++i)
		{
			uint32* r_seed = get_local_random_seed_address();

			constexpr real32 scale = 1.f;

			placement_data.translational_velocity.i = _real_random_range(r_seed, -scale, scale);
			placement_data.translational_velocity.j = _real_random_range(r_seed, -scale, scale);
			placement_data.translational_velocity.k = 2.f;

			placement_data.angular_velocity.i = _real_random_range(r_seed, -scale, scale);
			placement_data.angular_velocity.j = _real_random_range(r_seed, -scale, scale);
			placement_data.angular_velocity.k = 4.f;

			simulation_action_object_create(object_new(&placement_data));
		}

		g_headhunter_engine_globals->m_player_skull_count[player_index] = 0;
	}
}

bool c_headhunter_engine::player_can_interact_with_weapon(datum player_index, datum weapon_index)
{
	object_header_datum* weapon_header = (object_header_datum*)datum_get(object_header_data_get(), weapon_index);

	weapon_datum* weapon = (weapon_datum*)weapon_header->datum;

	if (weapon && weapon->definition_index == g_ball_datum)
	{
		if (game_is_authoritative() && !weapon_header->flags.test(_object_header_being_deleted_bit))
		{
			player_datum* player = (player_datum*)datum_get(player_data_get(), player_index);

			const biped_datum* player_biped = (biped_datum*)object_try_and_get_and_verify_type(player->unit_index, _object_mask_biped);

			if (player_biped && !player_biped->object.object_damage_flags.test(_object_is_dead_bit))
			{
				constexpr real32 k_distance_from_skull_auto_pickup = 0.3f;

				real32 distance = distance3d(&weapon->object.position, &player_biped->object.position);

				if (distance < k_distance_from_skull_auto_pickup)
				{
					if (g_headhunter_engine_globals->m_player_skull_count[DATUM_INDEX_TO_ABSOLUTE_INDEX(player_index)] < variant_get_max_heads_carried())
					{
						object_delete(weapon_index);
						++g_headhunter_engine_globals->m_player_skull_count[DATUM_INDEX_TO_ABSOLUTE_INDEX(player_index)];

						s_game_engine_event event;
						game_engine_event_new(_multiplayer_event_response_game_type_headhunter, _multiplayer_event_response_headhunter_head_grabbed, &event);
						event.player_index = player_index;
						event.causing_player_index = player_index;

						game_engine_send_event(&event);

						LOG_TRACE_GAME(L"[{}] {} picked up a head", __FUNCTIONW__, player->properties->player_name);
					}
				}
			}
		}

		return false;
	}

	return c_king_engine::player_can_interact_with_weapon(player_index, weapon_index);
}

bool c_headhunter_engine::function_34(datum player_index, void* unk)
{
	// reworks behavior of spawn zones based on if the game engine has teams
	// due to a default case behavior because the headhunter game engine isn't in a switch table
	// the default behaviour for a game engine is to operate as if teams are enabled
	// causing every player in a FFA match to spawn on top of each other in red team spawn zone
	return game_engine_has_teams();
}

void c_headhunter_engine::update()
{
	if (!game_is_finished() && !game_is_predicted())
	{
		uint16 players_mask = 0;
		c_player_in_game_iterator it;
		while (it.next())
		{
			if (it.get_datum()->player_on_hill_time)
				players_mask |= FLAG(it.get_absolute_index());
		}

		if (players_mask != g_headhunter_engine_globals->m_players_in_hill)
		{
			c_headhunter_engine_globals::alert_players_of_players_in_hill(g_headhunter_engine_globals->m_players_in_hill, players_mask);
			g_headhunter_engine_globals->m_players_in_hill = players_mask;
			simulation_action_game_engine_globals_update(64);
		}

		c_headhunter_engine::update_scores();

		s_game_variant* variant = get_game_variant();

		if (variant->game_engine_variant.king.hill_move_time || g_headhunter_engine_globals->m_ticks_till_hill_move != 0)
		{
			--g_headhunter_engine_globals->m_ticks_till_hill_move;

			if (g_headhunter_engine_globals->m_ticks_till_hill_move <= 0)
			{
				g_headhunter_engine_globals->m_ticks_till_hill_move = time_globals::seconds_to_ticks_round(variant->game_engine_variant.king.hill_move_time);

				int32 next_hill_index = g_headhunter_engine_globals->get_next_hill_index();
				if (next_hill_index != NONE)
				{
					while (true)
					{
						g_headhunter_engine_globals->setup_points(next_hill_index);
						if (g_headhunter_engine_globals->m_even_count >= 4 && (g_headhunter_engine_globals->m_even_count & 1) == 0)
							break;

						next_hill_index = g_headhunter_engine_globals->get_next_hill_index();
						if (next_hill_index == NONE)
							break;
					}

					if (g_headhunter_engine_globals->m_hill_id != next_hill_index && next_hill_index != NONE)
					{
						s_game_engine_event event;

						game_engine_event_new(
							_multiplayer_event_response_game_type_king_of_the_hill,
							_multiplayer_event_response_king_hill_move,
							&event);


						game_engine_send_event(&event);
						g_headhunter_engine_globals->m_hill_id = next_hill_index;
					}
				}
				simulation_action_game_engine_globals_update(32);
			}
		}
	}
}

void c_headhunter_engine::set_simulation_baseline_data(int32 unused, void* state_data)
{
	s_headhunter_engine_state_data* game_state_data = (s_headhunter_engine_state_data*)state_data;

	c_king_engine::set_simulation_baseline_data(unused, state_data);

	memset(game_state_data->player_skull_count, 0, sizeof(game_state_data->player_skull_count));

	c_headhunter_engine::function_42(state_data);
}

void c_headhunter_engine::build_simulation_update(uint32* update_mask, int32 unused, void* state_data)
{
	s_headhunter_engine_state_data* game_state_data = (s_headhunter_engine_state_data*)state_data;

	uint32 out_mask = 0;

	if (*update_mask & k_game_engine_state_data_flags_mask)
	{
		c_king_engine::function_43(*update_mask & k_game_engine_state_data_flags_mask, &out_mask, state_data);
	}
	if (TEST_BIT(*update_mask, _king_engine_state_data_flag_hill_id_exists) && game_state_data->hill_id != g_headhunter_engine_globals->m_hill_id)
	{
		out_mask |= FLAG(_king_engine_state_data_flag_hill_id_exists);
		game_state_data->hill_id = (uint16)g_headhunter_engine_globals->m_hill_id;
	}

	if (TEST_BIT(*update_mask, _king_engine_state_data_flag_players_in_hill_exists))
	{
		if (game_state_data->players_in_hill != g_headhunter_engine_globals->m_players_in_hill)
		{
			game_state_data->players_in_hill = g_headhunter_engine_globals->m_players_in_hill;
			out_mask |= FLAG(_king_engine_state_data_flag_players_in_hill_exists);
		}
	}

	memcpy(game_state_data->player_skull_count, g_headhunter_engine_globals->m_player_skull_count, sizeof(game_state_data->player_skull_count));

	out_mask |= FLAG(_headhunter_engine_state_flag_player_skull_count);

	*update_mask = out_mask;
}

bool c_headhunter_engine::apply_simulation_update(uint32 update_mask, int32 unused, void* state_data)
{
	s_headhunter_engine_state_data* game_state_data = (s_headhunter_engine_state_data*)state_data;

	if (!c_king_engine::apply_simulation_update(update_mask, unused, state_data))
	{
		return false;
	}

	if (TEST_BIT(update_mask, _headhunter_engine_state_flag_player_skull_count))
	{
		memcpy(g_headhunter_engine_globals->m_player_skull_count, game_state_data->player_skull_count, sizeof(game_state_data->player_skull_count));
		return true;
	}

	return true;
}

e_simulation_entity_type c_headhunter_engine::get_game_engine_entity_type()
{
	return _simulation_entity_type_headhunter_engine_globals;
}

void c_headhunter_engine::draw_skulls_carried_string(uint32 user_index)
{
	datum player_index = player_index_from_user_index(user_index);

	wchar_t buffer[256];

	swprintf(buffer, NUMBEROF(buffer), L"%c x %d", _private_use_character_oddball, g_headhunter_engine_globals->m_player_skull_count[DATUM_INDEX_TO_ABSOLUTE_INDEX(player_index)]);

	if (g_headhunter_engine_globals->m_player_skull_count[DATUM_INDEX_TO_ABSOLUTE_INDEX(player_index)] == variant_get_max_heads_carried())
	{
		swprintf(buffer, NUMBEROF(buffer), L"%s (max)", buffer);
	}

	render_camera* camera = get_global_camera();

	rectangle2d bounds{};

	bounds.top = camera->window_bounds.bottom - camera->viewport_bounds.top + 10;
	bounds.bottom = bounds.top + 80;
	bounds.left = camera->window_bounds.left - camera->viewport_bounds.left;
	bounds.right = camera->window_bounds.right - camera->viewport_bounds.left;

	real32 safe_area = *Memory::GetAddress<real32*>(0x9770F0);

	bounds.top -= (int16)(safe_area * 2.f);
	bounds.bottom -= (int16)(safe_area * 2.f);

	real_argb_color text_color;

	text_color.alpha = 1.f;

	*((real_rgb_color*)(&text_color.red)) = *get_local_user_hud_color(user_index);

	draw_string_set_draw_mode(6, -1, 2, 0, &text_color, global_real_argb_black, false);

	if (draw_string_set_string(buffer))
		rasterizer_draw_unicode_string(&bounds, buffer);
}

uint8 c_headhunter_engine::variant_get_max_heads_carried()
{
	s_game_variant* variant = get_game_variant();

	switch (variant->game_engine_variant.head_hunter.max_heads_carried)
	{
	case _headhunter_max_heads_carried_none:
		return UINT8_MAX;
	case _headhunter_max_heads_carried_one:
		return 1;
	case _headhunter_max_heads_carried_five:
		return 5;
	case _headhunter_max_heads_carried_ten:
		return 10;
	default:
	{
		DISPLAY_ASSERT("INVALID VARIANT SETTING FOR HEAD HUNTER HEADS CARRIED");
		return UINT8_MAX;
	}
	}
}

void c_headhunter_engine::update_scores()
{
	uint32 players_in_hill_mask = 0;
	uint32 teams_in_hill_mask = 0;

	c_player_in_game_iterator mask_it;
	while (mask_it.next())
	{
		if (mask_it.get_datum()->player_on_hill_time)
		{
			players_in_hill_mask |= FLAG(mask_it.get_absolute_index());

			if (mask_it.get_datum()->properties->team_index != NONE)
				teams_in_hill_mask |= FLAG(mask_it.get_datum()->properties->team_index);
		}
	}

	s_game_variant* variant = get_game_variant();

	if (variant->game_engine_variant.king.flags.test(_king_engine_uncontested_hill_to_score_bit))
	{
		if (game_engine_has_teams())
		{
			if (!teams_in_hill_mask || ((teams_in_hill_mask - 1) & teams_in_hill_mask) != 0)
				return;
		}
		else if (!players_in_hill_mask || ((players_in_hill_mask - 1) & players_in_hill_mask) != 0)
		{
			return;
		}
	}

	if (game_engine_in_round())
	{
		if (game_engine_has_teams())
		{
			for (int16 i = 0; i < k_game_multiplayer_team_count; ++i)
			{
				if ((teams_in_hill_mask & FLAG(i)) != 0)
					++g_headhunter_engine_globals->m_team_control_accumulator[i];
				else
					g_headhunter_engine_globals->m_team_control_accumulator[i] = 0;
			}

			for (int16 i = 0; i < k_game_multiplayer_team_count; ++i)
			{
				if (TEST_BIT(teams_in_hill_mask, i))
				{
					if (!(g_headhunter_engine_globals->m_team_control_accumulator[i] % time_globals::get()->ticks_per_second))
					{
						c_player_in_game_iterator it;
						while (it.next())
						{
							player_datum* player = it.get_datum();

							if (g_headhunter_engine_globals->m_player_skull_count[it.get_absolute_index()] > 0)
							{
								if (player->player_on_hill_time && !(player->player_on_hill_time % time_globals::get()->ticks_per_second))
								{
									c_game_statborg* statborg = game_engine_get_statborg();

									int32 current_score = statborg->get_score(it.get_absolute_index());

									// todo: have to rewrite the game results functions to add a new carry type for skulls pickedup
									// game_results_add_carry_event();

									game_engine_adjust_score(it.get_absolute_index(), g_headhunter_engine_globals->m_player_skull_count[it.get_absolute_index()]);

									g_headhunter_engine_globals->m_player_skull_count[it.get_absolute_index()] = 0;

									// todo: add c_slayer_engine::update_scores function that plays a sound when N kills to win
								}
							}
						}
					}
				}
			}
		}
		else
		{
			c_player_in_game_iterator it;
			while (it.next())
			{
				player_datum* player = it.get_datum();

				if (g_headhunter_engine_globals->m_player_skull_count[it.get_absolute_index()] > 0)
				{
					if (player->player_on_hill_time && !(player->player_on_hill_time % time_globals::get()->ticks_per_second))
					{
						c_game_statborg* statborg = game_engine_get_statborg();

						int32 current_score = statborg->get_score(it.get_absolute_index());

						// todo: have to rewrite the game results functions to add a new carry type for skulls pickedup
						// game_results_add_carry_event();

						game_engine_adjust_score(it.get_absolute_index(), g_headhunter_engine_globals->m_player_skull_count[it.get_absolute_index()]);

						g_headhunter_engine_globals->m_player_skull_count[it.get_absolute_index()] = 0;

						// todo: add c_slayer_engine::update_scores function that plays a sound when N kills to win
					}
				}
			}
		}
	}
}

c_headhunter_engine* get_global_headhunter_engine_instance()
{
	return &g_headhunter_engine;
}