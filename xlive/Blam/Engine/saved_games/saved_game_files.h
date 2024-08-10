#pragma once

#define k_maximum_enumerated_saved_game_files_any_type_per_memory_unit 4096

struct s_saved_game_main_menu_globals_save_file
{
	int16 initialized;
	int8 gap4[518];
	int16 unk_204;
	int8 gap2[42];
};

struct s_saved_game_main_menu_globals
{
	int32 m_count;
	int8 gap1[466620];
	uint32 save_files_count;
	s_saved_game_main_menu_globals_save_file save_files[4096];
	datum saved_game_file_index_salt;
	int8 gap2[131072];
};


s_saved_game_main_menu_globals* saved_game_main_menu_globals_get();