#pragma once
#include "multiplayer_globals_definition.h"
#include "saved_games/game_variant.h"
#include "simulation/game_interface/simulation_game_events.h"
/* public code */

bool game_engine_in_round();

void game_engine_check_for_round_winner();
void game_engine_end_round_with_winner(int player_datum_or_team_index, bool go_to_next_round);
bool game_engine_has_teams();

void __cdecl game_engine_build_event_string(wchar_t* input, s_game_engine_event* event, int32 buffer_size, wchar_t* buffer);
s_multiplayer_event_response_definition* game_engine_find_event_response(s_game_engine_event* event, int32 player_index, e_multiplayer_event_response_audience filter);
void game_engine_event_execute(s_multiplayer_event_response_definition* event_response, s_game_engine_event* event, int32 player_index);

// ### TODO: function name
bool __cdecl sub_4701B6(datum player_index);

e_game_variant_description_index game_engine_type_get_variant_description_index(e_game_engine_type type);