#pragma once
#include "player_profile.h"
#include "memory/static_arrays.h"

/* constants */

#define k_maximum_enumerated_saved_game_files_any_type_per_memory_unit 4096
#define k_maximum_enumerated_default_save_game_files 101
#define k_maximum_enumerated_cached_save_game_files 4

/* enums */

enum e_saved_game_file_type : uint32
{
	_saved_game_file_type_profile = 0,
	_saved_game_file_type_game_variant_slayer = 0x1,
	_saved_game_file_type_game_variant_koth = 0x2,
	_saved_game_file_type_game_variant_race = 0x3,
	_saved_game_file_type_game_variant_oddball = 0x4,
	_saved_game_file_type_game_variant_juggernaut = 0x5,
	_saved_game_file_type_game_variant_headhunter = 0x6,
	_saved_game_file_type_game_variant_ctf = 0x7,
	_saved_game_file_type_game_variant_assault = 0x8,
	_saved_game_file_type_game_variant_territories = 0x9,
	k_number_of_saved_game_file_types = 0xA,
};

/* structures */

struct s_saved_game_file_loading_information
{
	int8 data[591];
	wchar_t file_path[MAX_PATH];
	wchar_t meta_information[128];
};
ASSERT_STRUCT_SIZE(s_saved_game_file_loading_information, 1368);

struct s_saved_game_main_menu_globals_default_save_file
{
	e_saved_game_file_type type;

	// buffer is the size of the largest possible save file type
	int8 buffer[sizeof(s_saved_game_player_profile)];
};
ASSERT_STRUCT_SIZE(s_saved_game_main_menu_globals_default_save_file, 0x120C);

struct s_saved_game_main_menu_globals_save_file_info
{
	wchar_t file_path[256];
	uint32 unk_1;
	int32 unk_2;
	wchar_t display_name[16];
	uint32 unk_3;
	e_saved_game_file_type type;
	uint32 unk_5;
};
ASSERT_STRUCT_SIZE(s_saved_game_main_menu_globals_save_file_info, 0x234);


struct s_saved_game_files_globals_cached_game_save
{
	uint32 enumerated_index;
	s_saved_game_main_menu_globals_save_file_info file_info;
};

struct s_saved_game_main_menu_globals
{
	c_static_array_tracked<s_saved_game_main_menu_globals_default_save_file, k_maximum_enumerated_default_save_game_files> default_save_files;
	c_static_array_tracked<s_saved_game_main_menu_globals_save_file_info, k_maximum_enumerated_saved_game_files_any_type_per_memory_unit> save_files;
	datum saved_game_file_index_salt;
	int8 gap2[131072];
};
ASSERT_STRUCT_SIZE(s_saved_game_main_menu_globals, 2907848);

struct s_saved_game_files_globals
{
	bool async_io_complete;
	bool unk_1;
	bool memory_initialized_for_game;
	bool cache_files_exist;
	int32 unk_3;
	int32 unk_4;
	int32 unk_5;
	int32 unk_6;
	c_static_array_tracked<s_saved_game_files_globals_cached_game_save, k_maximum_enumerated_cached_save_game_files> cached_save_files;
	bool meta_data_lock_done;
	int32 unk_7;
	int32 locked_thread_id;
	int32 unk_8;
};
ASSERT_STRUCT_SIZE(s_saved_game_files_globals, 2312);

/* prototypes */

s_saved_game_main_menu_globals* saved_game_main_menu_globals_get();

s_saved_game_files_globals* saved_game_files_globals_get();

bool saved_games_get_file_info(s_saved_game_main_menu_globals_save_file_info* out_info, uint32 enumerated_index);

const wchar_t* saved_games_get_file_type_as_string(e_saved_game_file_type file_type);

bool saved_games_append_file_type_to_path(wchar_t* in_path, e_saved_game_file_type file_type, wchar_t* out_path);

void saved_games_get_display_name(uint32 enumerated_index, wchar_t* display_name);

void saved_game_files_apply_hooks();