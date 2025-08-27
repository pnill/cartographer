#pragma once
#include "player_profile.h"
#include "main/game_preferences.h"
#include "tag_files/global_string_ids.h"

/* constants */

enum
{
	k_maximum_enumerated_saved_game_files_any_type_per_memory_unit = 4096,
	k_maximum_enumerated_default_save_game_files = 101,
	k_maximum_enumerated_cached_save_game_files = 4,
	k_enumerated_salt_count = 512,
	k_maximum_enumerated_total_save_game_files =(k_maximum_enumerated_saved_game_files_any_type_per_memory_unit + k_maximum_enumerated_default_save_game_files)
};

/* enums */

enum e_saved_game_file_type : int32
{
	_saved_game_file_type_profile = 0,
	_saved_game_file_type_game_variant_slayer,
	_saved_game_file_type_game_variant_koth,
	_saved_game_file_type_game_variant_race,
	_saved_game_file_type_game_variant_oddball,
	_saved_game_file_type_game_variant_juggernaut,
	_saved_game_file_type_game_variant_headhunter,
	_saved_game_file_type_game_variant_ctf,
	_saved_game_file_type_game_variant_assault,
	_saved_game_file_type_game_variant_territories,
	k_number_of_saved_game_file_types,

	_saved_game_file_type_invalid = NONE,
};

enum e_saved_game_disk_result : int32
{
	_saved_gave_disk_result_success = 0,
	_saved_game_disk_result_no_free_slots = 1,
	_saved_game_disk_result_no_free_space = 2,
	_saved_games_disk_result_memory_unit_error = 4
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
	byte pad_230;
	byte language_id;
	byte pad_232[2];
};
ASSERT_STRUCT_SIZE(s_saved_game_main_menu_globals_save_file_info, 0x234);


struct s_saved_game_files_globals_cached_game_save
{
	enumerated_file_index enumerated_index;
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
	e_saved_game_disk_result saved_file_creation_result;
	int32 unk_5;
	int32 unk_6;
	c_static_array_tracked<s_saved_game_files_globals_cached_game_save, k_maximum_enumerated_cached_save_game_files> cached_save_files;
	bool meta_data_lock_done;
	int32 unk_7;
	int32 locked_thread_id;
	e_language language_id;
};
ASSERT_STRUCT_SIZE(s_saved_game_files_globals, 2312);

/* prototypes */

e_global_string_id saved_game_get_type_string_id(e_saved_game_file_type type);

bool saved_game_get_file_info(s_saved_game_main_menu_globals_save_file_info* out_info, enumerated_file_index enumerated_index);

void saved_game_files_search_by_type(e_controller_index controller_index, e_saved_game_file_type save_type, uint32* number_of_profiles, enumerated_file_index* saved_game_file_indicies, bool search_default_profiles);

uint32 saved_game_get_file_size_kb_for_type(e_saved_game_file_type type);

const wchar_t* saved_game_get_file_type_as_string(e_saved_game_file_type file_type);

bool saved_game_new_main_menu_globals_save_file(s_saved_game_main_menu_globals_save_file_info* new_save, e_saved_game_file_type file_type, wchar_t* out_path);

bool __cdecl saved_game_add_save_to_cache(s_saved_game_main_menu_globals_save_file_info* new_save, uint32* out_save_index);

void __fastcall saved_game_remove_save_from_cache(enumerated_file_index enumerated_file_index);

void saved_game_get_display_name(enumerated_file_index enumerated_index, wchar_t* display_name);

bool __cdecl saved_game_get_display_name_for_type(wchar_t* display_name, e_saved_game_file_type saved_game_type, e_language language, wchar_t* full_display_name);

bool __cdecl saved_game_create_save_game_directory(e_saved_game_file_type type, wchar_t* out_string);

e_game_variant_description_index saved_game_type_to_variant_description(e_saved_game_file_type type);

e_saved_game_file_type saved_game_get_variant_file_type(const s_game_variant* variant);

enumerated_file_index saved_game_create_new_game_variant(e_controller_index origin_controller, e_saved_game_file_type type, wchar_t* name);

enumerated_file_index saved_game_create_file(e_saved_game_file_type type, e_controller_index originating_controller_index, wchar_t* new_file_name);

s_saved_game_main_menu_globals* saved_game_main_menu_globals_get();

s_saved_game_files_globals* saved_game_files_globals_get();

bool saved_game_append_file_type_to_path(const wchar_t* in_path, e_saved_game_file_type file_type, wchar_t* out_path);

bool saved_game_load_game_variant(enumerated_file_index enumerated_index, s_game_variant* out_variant);

void saved_game_new_failure_cleanup(e_saved_game_file_type type, enumerated_file_index enumerated_file_index);

enumerated_file_index saved_game_enumerated_index_new(e_saved_game_file_type save_type, uint32 enumerated_index, bool is_default_save);

void saved_game_files_apply_hooks();