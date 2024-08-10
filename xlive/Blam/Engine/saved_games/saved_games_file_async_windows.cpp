#include "stdafx.h"

#include "player_profile.h"
#include "saved_game_files.h"
#include "saved_game_files_async_windows.h"
#include "cseries/async_helpers.h"

typedef bool(__cdecl* t_saved_games_async_helper_write_file)(int32 enumerated_index, wchar_t* new_display_name, int8* buffer, uint32 buffer_size, int8* completion);
t_saved_games_async_helper_write_file p_saved_games_async_helper_write_file;

int8 new_buffer[0x1208 * 2] = {};

bool __cdecl saved_games_async_helper_write_file(int32 enumerated_index, wchar_t* new_display_name, int8* buffer, uint32 buffer_size, int8* completion)
{
	if (buffer_size == sizeof(s_saved_game_player_profile))
	{
		csmemcpy(new_buffer, buffer, buffer_size);
		return p_saved_games_async_helper_write_file(enumerated_index, new_display_name, new_buffer, buffer_size * 2, completion);
	}

	return p_saved_games_async_helper_write_file(enumerated_index, new_display_name, buffer, buffer_size, completion);
}

typedef bool(__cdecl* t_saved_games_async_helper_read_file)(int enumerated_index, void* buffer, unsigned int buffer_size, int8* in_out_completion);
t_saved_games_async_helper_read_file p_saved_games_async_helper_read_file;

bool __cdecl saved_games_async_helper_read_file(int enumerated_index, void* buffer, unsigned int buffer_size, int8* in_out_completion)
{
	if (buffer_size == sizeof(s_saved_game_player_profile))
	{
		s_saved_game_main_menu_globals* saved_game_globals = saved_game_main_menu_globals_get();

		// 0x200000 in the index flags that the index exists inside the globals already
		if ((enumerated_index & 0x200000) && saved_game_globals)
		{
			auto abs_index = (enumerated_index >> 8) & 0x1FFF;
			auto last_index = saved_game_globals->m_count - 1;

			// Removed check of buffer_size so it can read the entry from the globals
			// potentially unsafe will have to come up with a way to verify if this was actually needed or not.
			if ((abs_index <= last_index || abs_index == last_index))
			{
				csmemcpy(buffer, &saved_game_globals->gap1[0x120C * abs_index + 4], buffer_size);
				in_out_completion[2] = 1;
				*(in_out_completion + 1) = 0;
				*in_out_completion = 1;
				*(in_out_completion + 514) = 0x3F800000;
				return true;
			}
		}

		bool result = p_saved_games_async_helper_read_file(enumerated_index, new_buffer, buffer_size * 2, in_out_completion);

		csmemcpy(buffer, new_buffer, buffer_size);

		return result;
	}

	return p_saved_games_async_helper_read_file(enumerated_index, buffer, buffer_size, in_out_completion);
}

void saved_games_async_helpers_apply_hooks()
{
	DETOUR_ATTACH(p_saved_games_async_helper_write_file, Memory::GetAddress<t_saved_games_async_helper_write_file>(0x42CA6), saved_games_async_helper_write_file);
	DETOUR_ATTACH(p_saved_games_async_helper_read_file, Memory::GetAddress<t_saved_games_async_helper_read_file>(0x4259E), saved_games_async_helper_read_file);
}
