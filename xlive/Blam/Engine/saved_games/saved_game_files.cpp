#include "stdafx.h"
#include "saved_game_files.h"

s_saved_game_main_menu_globals* saved_game_main_menu_globals_get()
{
	return *Memory::GetAddress<s_saved_game_main_menu_globals**>(0x482300);
}

s_saved_game_files_globals* saved_game_files_globals_get()
{
	return Memory::GetAddress<s_saved_game_files_globals*>(0x482424);
}

bool saved_games_get_file_info(s_saved_game_main_menu_globals_save_file_info* out_info, uint32 enumerated_index)
{
	s_saved_game_main_menu_globals* saved_game_main_menu_globals = saved_game_main_menu_globals_get();
	s_saved_game_files_globals* saved_game_files_globals = saved_game_files_globals_get();

	if (saved_game_main_menu_globals)
	{
		// file is not a default save
		if ((enumerated_index & 0x200000) == 0)
		{
			auto abs_index = (enumerated_index >> 8) & 0x1FFF;
			auto last_index = saved_game_main_menu_globals->save_files.get_count() - 1;
			if ((abs_index <= last_index || abs_index == last_index))
			{
				auto asdf = saved_game_main_menu_globals->save_files[abs_index];
				csmemcpy(out_info, saved_game_main_menu_globals->save_files[abs_index], saved_game_main_menu_globals->save_files.get_type_size());
				return true;
			}
		}
	}
	else if (saved_game_files_globals->cache_files_exist)
	{
		if (saved_game_files_globals->cached_save_files.get_count() > 0)
		{
			for (int32 i = 0; i < saved_game_files_globals->cached_save_files.get_count(); i++)
			{
				if (enumerated_index == saved_game_files_globals->cached_save_files[i]->enumerated_index)
				{
					csmemcpy(out_info, &saved_game_files_globals->cached_save_files[i]->file_info, saved_game_files_globals->cached_save_files.get_type_size());
					return true;
				}
			}
		}
	}

	return false;
}

const wchar_t* k_saved_game_file_type_strings[k_number_of_saved_game_file_types]
{
	L"profile",
	L"slayer",
	L"koth",
	L"race",
	L"oddball",
	L"juggernaut",
	L"headhunter",
	L"ctf",
	L"assault",
	L"territories",
};

const wchar_t* saved_games_get_file_type_as_string(e_saved_game_file_type file_type)
{
	return file_type < k_number_of_saved_game_file_types ? k_saved_game_file_type_strings[file_type] : L"unknown";
}

bool saved_games_append_file_type_to_path(wchar_t* in_path, e_saved_game_file_type file_type, wchar_t* out_path)
{
	wcsncpy(out_path, in_path, 256);
	wchar_t* cat_path = wcscat(out_path, saved_games_get_file_type_as_string(file_type));
	cat_path[255] = '\0';
	wcsncpy(out_path, cat_path, 256);
	return true;
}