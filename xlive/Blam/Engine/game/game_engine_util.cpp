#include "stdafx.h"
#include "game_engine_util.h"

#include "game_time.h"
#include "game/game.h"
#include "game/game_engine.h"
#include "interface/hud_messaging.h"
#include "main/game_preferences.h"
#include "math/random_math.h"
#include "sound/game_sound.h"
#include "text/text_group.h"

/* public code */

bool game_engine_in_round()
{
	return current_game_engine() != NULL
		&& game_engine_globals_get()->current_state == 1
		&& (game_is_predicted() || game_engine_globals_get()->field_C44 == 1);
}

void game_engine_check_for_round_winner()
{
	typedef void(__cdecl game_engine_check_for_round_winner_t)();
	auto p_game_engine_check_for_round_winner = Memory::GetAddress<game_engine_check_for_round_winner_t*>(0x70F49, 0x6FA4A);
	p_game_engine_check_for_round_winner();
}

void game_engine_end_round_with_winner(int player_datum_or_team_index, bool go_to_next_round)
{
	typedef void(__cdecl game_engine_end_round_with_winner_t)(int player_datum_or_team_index, bool go_to_next_round);
	auto p_game_engine_end_round_with_winner = Memory::GetAddress<game_engine_end_round_with_winner_t*>(0x70A6F, 0x6F570);
	p_game_engine_end_round_with_winner(player_datum_or_team_index, go_to_next_round);
}

bool game_engine_has_teams()
{
	if (current_game_engine())
	{
		return current_game_variant()->game_engine_flags.test(_game_engine_teams_bit);
	}

	return false;
}

void game_engine_event_play_sound(datum sound, int32 delay_time, s_game_engine_event* event, bool is_announcer)
{
	if (event->event_type == _multiplayer_event_response_headhunter_head_grabbed)
	{
		unspatialized_impulse_sound_new(sound, 1.0f);
	}
	else
	{
		INVOKE(0xD0F72, 0, game_engine_event_play_sound, sound, delay_time, event, is_announcer);
	}
}

void game_engine_build_event_string(wchar_t* input, s_game_engine_event* event, int32 buffer_size, wchar_t* buffer)
{
	INVOKE(0x5D7C8, 0, game_engine_build_event_string, input, event, buffer_size, buffer);
}

s_multiplayer_event_response_definition* game_engine_find_event_response(s_game_engine_event* game_event, int32 player_index, e_multiplayer_event_response_audience filter)
{
	s_multiplayer_runtime_globals_definition* runtime = get_multiplayer_globals()->runtime[0];

	tag_block<s_multiplayer_event_response_definition>* events = nullptr;

	player_datum* player = (player_datum*)datum_get(player_data_get(), player_index);

	switch (game_event->game_type)
	{
	case _multiplayer_event_response_game_type_general:
		events = &runtime->general_events;
		break;
	case _multiplayer_event_response_game_type_flavor:
		events = &runtime->flavor_events;
		break;
	case _multiplayer_event_response_game_type_slayer:
		events = &runtime->slayer_events;
		break;
	case _multiplayer_event_response_game_type_capture_the_flag:
		events = &runtime->ctf_events;
		break;
	case _multiplayer_event_response_game_type_oddball:
		events = &runtime->oddball_events;
		break;
	case _multiplayer_event_response_game_type_headhunter:
		events = &runtime->headhunter_events;
		break;
	case _multiplayer_event_response_game_type_king_of_the_hill:
		events = &runtime->king_events;
		break;
	case _multiplayer_event_response_game_type_juggernaut:
		events = &runtime->juggernaut_events;
		break;
	case _multiplayer_event_response_game_type_assault:
		events = &runtime->assault_events;
		break;
	case _multiplayer_event_response_game_type_unused_7:
	case _multiplayer_event_response_game_type_unused_10:
	case _multiplayer_event_response_game_type_unused_11:
	case _multiplayer_event_response_game_type_unused_12:
	case _multiplayer_event_response_game_type_unused_13:
		events = nullptr;
		break;
	}
	if (!events)
		return nullptr;

	if (events->count <= 0)
		return nullptr;

	for (int32 i = 0; i < events->count; ++i)
	{
		s_multiplayer_event_response_definition* event = (*events)[i];

		if (event->event == game_event->event_type && event->audience == filter)
		{
			bool valid = true;
			switch (event->required_field)
			{
			case _multiplayer_event_response_audience_filter_cause_player:
				valid = (game_event->causing_player_index != NONE);
				break;
			case _multiplayer_event_response_audience_filter_cause_team:
				valid = (game_event->causing_player_team != NONE);
				break;
			case _multiplayer_event_response_audience_filter_effect_player:
				valid = (game_event->effect_player_index != NONE);
				break;
			case _multiplayer_event_response_audience_filter_effect_team:
				valid = (game_event->effect_player_team != NONE);
				break;
			default:
				break;
			}

			if (!valid)
				continue;

			bool excluded = false;
			switch (event->excluded_audience)
			{
			case _multiplayer_event_response_audience_filter_cause_player:
				excluded = (game_event->causing_player_index == player_index);
				break;
			case _multiplayer_event_response_audience_filter_cause_team:
				excluded = (game_event->causing_player_team == player->properties->team_index);
				break;
			case _multiplayer_event_response_audience_filter_effect_player:
				excluded = (game_event->effect_player_index == player_index);
				break;
			case _multiplayer_event_response_audience_filter_effect_team:
				excluded = (game_event->effect_player_team == player->properties->team_index);
				break;
			default:
				break;
			}

			if (!excluded)
				return event;
		}
	}

	return nullptr;
}

void game_engine_event_execute_sound(s_multiplayer_event_response_definition* event_response, s_game_engine_event* event)
{
	bool is_announcer = false;
	int32 language_index = 0;

	if (event_response->primary_sound.sound_flags.test(_multiplayer_event_response_sound_announcer_sound))
	{
		is_announcer = true;
		language_index = get_current_language();
	}

	datum sound_datum = event_response->primary_sound.sound[language_index].index;

	int32 sound_delay = 0;

	if (event_response->sound_delay != 0.0f)
		sound_delay = time_globals::seconds_to_ticks_round(event_response->sound_delay);

	if (event_response->sound_permutations.count > 0)
	{
		real32 random_probability = (real32)_random_integer(get_local_random_seed_address());
		constexpr real32 k_event_sound_permutation_modifier = 0.000015259022f;
		real32 probability_accum = 0.f;

		for (int32 i = 0; i < event_response->sound_permutations.count; ++i)
		{
			s_multiplayer_event_response_sound* permu = event_response->sound_permutations[i];
			probability_accum += permu->probability;
			if (random_probability * k_event_sound_permutation_modifier > probability_accum)
			{
				sound_datum = permu->sound[language_index].index;
				is_announcer = permu->sound_flags.test(_multiplayer_event_response_sound_announcer_sound);
				break;
			}
		}
	}

	if (sound_datum != NONE)
		game_engine_event_play_sound(sound_datum, sound_delay, event, is_announcer);
}

void game_engine_event_execute(s_multiplayer_event_response_definition* event_response, s_game_engine_event* event, int32 player_index)
{
	s_multiplayer_runtime_globals_definition* runtime = get_multiplayer_globals()->runtime[0];

	player_datum* player = (player_datum*)datum_get(player_data_get(), player_index);

	if (player->user_index != NONE &&
		(!game_is_finished() || event->game_type == _multiplayer_event_response_game_type_general && event->event_type == _multiplayer_event_response_general_game_over))
	{
		bool do_draw_text = false;
		if (get_players_globals()->local_player_count < 2)
		{
			do_draw_text = true;
		}
		else
		{
			switch (event->game_type)
			{
			case _multiplayer_event_response_game_type_general:
			{
				switch (event->event_type)
				{
				case _multiplayer_event_response_general_kill:
				case _multiplayer_event_response_general_kill_collision:
				case _multiplayer_event_response_general_kill_melee:
				case _multiplayer_event_response_general_kill_sticky_grenade:
				case _multiplayer_event_response_general_kill_sniper:
				case _multiplayer_event_response_general_kill_stealth_melee:
				{
					do_draw_text = (event_response->audience != _multiplayer_event_response_audience_cause_team &&
						event_response->audience != _multiplayer_event_response_audience_effect_team);
					break;
				}
				case _multiplayer_event_response_general_one_minute_to_win:
				case _multiplayer_event_response_general_team_one_minute_to_win:
				case _multiplayer_event_response_general_thirty_seconds_to_win:
				case _multiplayer_event_response_general_team_thirty_seconds_to_win:
				case _multiplayer_event_response_general_thirty_minutes_left:
				case _multiplayer_event_response_general_fifteen_minutes_left:
				case _multiplayer_event_response_general_five_minutes_left:
				case _multiplayer_event_response_general_one_minute_left:
				case _multiplayer_event_response_general_gained_lead:
				case _multiplayer_event_response_general_gained_team_lead:
				case _multiplayer_event_response_general_lost_lead:
				case _multiplayer_event_response_general_lost_team_lead:
				case _multiplayer_event_response_general_tied_leader:
				case _multiplayer_event_response_general_tied_team_leader:
				case _multiplayer_event_response_general_thirty_seconds_left:
				case _multiplayer_event_response_general_ten_seconds_left:
				case _multiplayer_event_response_general_boarded_vehicle:
				case _multiplayer_event_response_general_ten_seconds_to_win:
				case _multiplayer_event_response_general_team_ten_seconds_to_win:
				{
					do_draw_text = false;
					break;
				}
				default:
				{
					do_draw_text = true;
					break;
				}
				}
				break;
			}
			case _multiplayer_event_response_game_type_oddball:
			{
				do_draw_text = (event->event_type != _multiplayer_event_response_oddball_ball_picked_up);
				break;
			}
			case _multiplayer_event_response_game_type_headhunter:
			case _multiplayer_event_response_game_type_king_of_the_hill:
			{
				switch (event->event_type)
				{
				case _multiplayer_event_response_king_hill_controlled:
				case _multiplayer_event_response_king_hill_contested:
				case _multiplayer_event_response_king_hill_controlled_team:
				case _multiplayer_event_response_king_hill_contested_team:
				{
					do_draw_text = false;
					break;
				}
				default:
					do_draw_text = true;
					break;
				}
			}
			case _multiplayer_event_response_game_type_juggernaut:
			{
				do_draw_text = (event->event_type != _multiplayer_event_response_juggernaut_new_juggernaut);
				break;
			}
			}
		}

		if (do_draw_text)
		{
			if (event_response->display_string != NONE && event_response->display_string)
			{
				c_maximum_interface_text display_string;
				wchar_t formatted_string[512];
				string_list_get_normal_string(runtime->in_game_text.index, event_response->display_string, &display_string);
				game_engine_build_event_string(display_string.get_buffer(), event, 256, formatted_string);

				if (event_response->flags.test(_multiplayer_event_response_quantity_message))
				{
					c_maximum_interface_text plural_string;
					wchar_t formatted_plural_string[512];
					string_list_get_normal_string(runtime->in_game_text.index, event_response->plural_display_string, &plural_string);
					game_engine_build_event_string(plural_string.get_buffer(), event, 256, formatted_plural_string);
					hud_messaging_add_message_plural(player->user_index, formatted_string, formatted_plural_string, event->field_1C);
				}
				else
				{
					hud_messaging_add_message(player->user_index, formatted_string);
				}
			}

			if (event_response->primary_string != NONE && event_response->primary_string)
			{
				c_maximum_interface_text primary_string;
				wchar_t formatted_string[512];
				string_list_get_normal_string(runtime->in_game_text.index, event_response->primary_string, &primary_string);
				game_engine_build_event_string(primary_string.get_buffer(), event, 256, formatted_string);

				int32 display_time = time_globals::seconds_to_ticks_round((real32)event_response->primary_string_duration);
				hud_messaging_display_primary_text(player->user_index, formatted_string, display_time);
			}
		}

		game_engine_event_execute_sound(event_response, event);
	}
}


bool __cdecl sub_4701B6(datum player_index)
{
	return INVOKE(0x701B6, 0x0, sub_4701B6, player_index);
}

e_game_variant_description_index game_engine_type_get_variant_description_index(e_game_engine_type type)
{
	switch (type)
	{
	case _game_engine_type_ctf:
		return _game_variant_description_ctf;
	case _game_engine_type_slayer:
		return _game_variant_description_slayer;
	case _game_engine_type_oddball:
		return _game_variant_description_oddball;
	case _game_engine_type_koth:
		return _game_variant_description_king;
	case _game_engine_type_race:
		return k_game_variant_description_invalid;
	case _game_engine_type_headhunter:
		return _game_variant_description_headhunter;
	case _game_engine_type_juggernaut:
		return _game_variant_description_juggernaut;
	case _game_engine_type_territories:
		return _game_variant_description_territories;
	case _game_engine_type_assault:
		return _game_variant_description_invasion;
	}

	return k_game_variant_description_invalid;
}