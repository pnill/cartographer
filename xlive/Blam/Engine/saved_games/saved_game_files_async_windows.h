#pragma once
#include "player_profile.h"

bool saved_games_async_helper_read_file(uint32 enumerated_index, int8* buffer, uint32 buffer_size);

bool saved_games_async_helper_write_cartographer_bin(uint32 enumerated_file_index, s_saved_game_cartographer_player_profile_v1* settings);

void saved_games_async_helper_read_cartographer_bin(uint32 enumerated_file_index, s_saved_game_cartographer_player_profile_v1* settings);

void saved_games_async_helpers_apply_hooks();