#pragma once

bool __cdecl saved_games_async_helper_write_profile_to_file(int32 a1, int32* a2);

bool __cdecl saved_games_async_helper_save_profile_to_file(int a1, int8* buffer, int size, char a4, int8* completion);

bool saved_games_async_helper_read_file(uint32 enumerated_index, int8* buffer, uint32 buffer_size);

void saved_games_async_helpers_apply_hooks();