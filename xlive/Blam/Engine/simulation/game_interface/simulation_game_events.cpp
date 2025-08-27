#include "stdafx.h"
#include "simulation_game_events.h"

#include "game/game_engine_util.h"
#include "game/players.h"

typedef void(__cdecl* t_game_engine_event_play)(s_game_engine_event* event);
t_game_engine_event_play p_game_engine_event_play;

void game_engine_event_new(e_multiplayer_event_response_game_type game_type, e_multiplayer_event_response_event event_type, s_game_engine_event* event)
{
	event->game_type = game_type;
	event->pad = 0;
	event->event_type = event_type;
	event->player_index = UINT_MAX;
	event->causing_player_index = NONE;
	event->causing_player_team = NONE;
	event->effect_player_index = NONE;
	event->effect_player_team = NONE;
	event->field_1C = 0;
	event->field_20 = 0xFFFF;
}

void __cdecl game_engine_send_event(s_game_engine_event* event)
{
	INVOKE(0x5DC77, 0x3F8D7, game_engine_send_event, event);
}

void game_engine_event_evaluate(s_game_engine_event* event, int32 player_index)
{
	player_datum* player = (player_datum*)datum_get(player_data_get(), player_index);
	s_multiplayer_event_response_definition* event_response = nullptr;

	if (player_index == event->causing_player_index) {
		event_response = game_engine_find_event_response(event, player_index, _multiplayer_event_response_audience_cause_player);
	}
	if (!event_response && player_index == event->effect_player_index) {
		event_response = game_engine_find_event_response(event, player_index, _multiplayer_event_response_audience_effect_player);
	}
	if (!event_response && player->properties[0].team_index == event->causing_player_team) {
		event_response = game_engine_find_event_response(event, player_index, _multiplayer_event_response_audience_cause_team);
	}
	if (!event_response && player->properties[0].team_index == event->effect_player_team) {
		event_response = game_engine_find_event_response(event, player_index, _multiplayer_event_response_audience_effect_team);
	}
	if (!event_response) {
		event_response = game_engine_find_event_response(event, player_index, _multiplayer_event_response_audience_all);
	}

	if (event_response) {
		game_engine_event_execute(event_response, event, player_index);
	}
}

void game_engine_event_play(s_game_engine_event* event)
{
	if (event->player_index == NONE)
	{
		c_player_in_game_iterator it;
		while (it.next())
		{
			if (!(TEST_BIT(it.get_datum()->flags, _player_left_game_bit)))
				game_engine_event_evaluate(event, it.get_index());
		}
	}
	else
	{
		game_engine_event_evaluate(event, event->player_index);
	}
}

void simulation_game_events_apply_patches()
{
	DETOUR_ATTACH(p_game_engine_event_play, Memory::GetAddress<t_game_engine_event_play>(0x5DBF0, 0x3F850), game_engine_event_play);
}