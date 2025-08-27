#pragma once
#include "game_variant.h"

/* prototypes */

void saved_games_async_helpers_apply_patches(void);

bool saved_games_async_helper_read_file(enumerated_file_index enumerated_index, int8* buffer, uint32 buffer_size);

bool saved_games_async_helper_write_saved_game_bin(const wchar_t* binary_name, enumerated_file_index enumerated_file_index, int8* buffer, uint32 buffer_size);

bool saved_games_async_helper_read_saved_game_bin(const wchar_t* binary_name, enumerated_file_index enumerated_file_index, int8* buffer, uint32 buffer_size);

bool saved_games_async_helper_write_variant(enumerated_file_index enumerated_file_index, s_game_variant* variant);