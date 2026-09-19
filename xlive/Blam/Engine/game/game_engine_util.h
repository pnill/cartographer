#pragma once
#include "saved_games/game_variant.h"

/* prototypes */

bool game_engine_in_round(void);

void __cdecl game_engine_check_for_round_winner(void);
void __cdecl game_engine_end_round_with_winner(int32 player_datum_or_team_index, bool go_to_next_round);
bool game_engine_has_teams(void);

enum e_network_game_simulation_protocol game_engine_get_simulation_protocol(struct s_game_variant const* variant);

// ### TODO: function name
bool __cdecl sub_4701B6(datum player_index);

e_game_variant_description_index game_engine_type_get_variant_description_index(e_game_engine_type type);