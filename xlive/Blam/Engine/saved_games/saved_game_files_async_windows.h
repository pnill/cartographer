#pragma once

bool saved_games_async_helper_read_file(uint32 enumerated_index, int8* buffer, uint32 buffer_size);

bool saved_games_async_helper_write_saved_game_bin(const wchar_t* binary_name, uint32 enumerated_file_index, int8* buffer, uint32 buffer_size);

bool saved_games_async_helper_read_saved_game_bin(const wchar_t* binary_name, uint32 enumerated_file_index, int8* buffer, uint32 buffer_size);

void saved_games_async_helpers_apply_hooks();