#pragma once

/* public code */

int current_game_engine();
void game_engine_check_for_round_winner();
void game_engine_end_round_with_winner(int player_datum_or_team_index, bool go_to_next_round);
bool game_engine_has_teams();

// ### TODO: function name
bool __cdecl sub_4701B6(datum player_index);
