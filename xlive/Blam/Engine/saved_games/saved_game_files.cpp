#include "stdafx.h"
#include "saved_game_files.h"

#include "saved_game_files_async_windows.h"
#include "cache/physical_memory_map.h"
#include "cseries/async.h"
#include "filesys/pc_file_system.h"
#include "filesys/pc_saved_game.h"
#include "input/input_windows.h"
#include "tag_files/files_windows.h"
#include "text/unicode.h"

/* constants  */

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

/* typedef */

typedef void(__cdecl* t_saved_games_load_save_file_information_from_disk)(c_static_array<s_saved_game_main_menu_globals_save_file_info, k_maximum_enumerated_saved_game_files_any_type_per_memory_unit>* save_files_storage);

/* prototypes */

static void saved_game_main_menu_globals_set(s_saved_game_main_menu_globals* new_globals);
static uint32 __cdecl saved_game_loading_allocate_storage(int32 a1, s_saved_game_file_loading_information* loading_information);
static void saved_games_load_save_file_information_from_disk(c_static_array<s_saved_game_main_menu_globals_save_file_info, k_maximum_enumerated_saved_game_files_any_type_per_memory_unit>* save_files_storage);
static void saved_game_main_menu_globals_initialize(void);
static void saved_game_files_memory_initialize(int32 unk);
static void saved_game_file_globals_wait_for_io_to_complete();

/* globals */

t_saved_games_load_save_file_information_from_disk p_saved_games_load_save_file_information_from_disk;

/* public code */

void saved_game_new_failure_cleanup(e_saved_game_file_type type, enumerated_file_index enumerated_file_index)
{
	if (enumerated_file_index != NONE)
	{
		s_saved_game_files_globals* file_globals = saved_game_files_globals_get();

		s_saved_game_main_menu_globals_save_file_info save_file_info{};

		ASSERT(file_globals);

		saved_game_file_globals_wait_for_io_to_complete();

		if (saved_game_get_file_info(&save_file_info, enumerated_file_index))
		{
			if (ENUMERATED_INDEX_IS_DEFAULT_SAVE(enumerated_file_index))
			{
				char flat_directory_path[MAX_PATH]{};

				int8 drive_letter;

				if (input_windows_drive_letter_test(ENUMERATED_INDEX_GET_MEMORY_UNIT(enumerated_file_index), &drive_letter))
				{
					wchar_t full_display_name[save_game_max_name]{};

					saved_game_get_display_name_for_type(
						save_file_info.display_name,
						ENUMERATED_INDEX_GET_TYPE(enumerated_file_index),
						(e_language)save_file_info.language_id,
						full_display_name);

					pc_file_system_get_saved_games_location(flat_directory_path, MAX_PATH);

					pc_file_system_delete_save_directory(flat_directory_path, full_display_name);
				}
			}

			saved_game_remove_save_from_cache(enumerated_file_index);
		}
	}
}

enumerated_file_index saved_game_enumerated_index_new(e_saved_game_file_type save_type, uint32 enumerated_index, bool is_default_save)
{
	s_saved_game_main_menu_globals* saved_game_main_menu_globals = saved_game_main_menu_globals_get();

	ASSERT(saved_game_main_menu_globals);
	ASSERT(_saved_game_file_type_profile <= save_type && save_type < k_number_of_saved_game_file_types);
	ASSERT(0 <= enumerated_index && enumerated_index <= k_maximum_enumerated_saved_game_files_any_type_per_memory_unit);
	ASSERT(IN_RANGE(saved_game_main_menu_globals->saved_game_file_index_salt, 0, k_enumerated_salt_count - 1));

	const enumerated_file_index result = (save_type & 0xF | (0x10 * (0 & 0xF | (0x10 * (enumerated_index & 0x1FFF | ((saved_game_main_menu_globals->saved_game_file_index_salt & 0x1FF) << 0xE))))));

	if (is_default_save)
	{
		return result | 0x200000;
	}
	else
	{
		return result & ~0x200000;
	}
}

void saved_game_files_apply_hooks(void)
{
	//WritePointer(Memory::GetAddress(0x39BD90), saved_game_files_memory_initialize);
	//DETOUR_ATTACH(p_saved_games_load_save_file_information_from_disk, Memory::GetAddress<t_saved_games_load_save_file_information_from_disk>(0x46596), saved_games_load_save_file_information_from_disk);
	return;
}

s_saved_game_main_menu_globals* saved_game_main_menu_globals_get()
{
	return *Memory::GetAddress<s_saved_game_main_menu_globals**>(0x482300);
}

s_saved_game_files_globals* saved_game_files_globals_get()
{
	return Memory::GetAddress<s_saved_game_files_globals*>(0x482424);
}
e_global_string_id saved_game_get_type_string_id(e_saved_game_file_type type)
{
	const static e_global_string_id saved_game_file_type_string_ids[k_number_of_saved_game_file_types]
	{
		_string_id_player_profile_display_name,
		_string_id_slayer_display_name,
		_string_id_koth_display_name,
		_string_id_race_display_name,
		_string_id_oddball_display_name,
		_string_id_juggernaut_display_name,
		_string_id_headhunter_display_name,
		_string_id_ctf_display_name,
		_string_id_assault_display_name,
		_string_id_territories_display_name
	};

	if (VALID_INDEX(type, k_number_of_saved_game_file_types))
		return saved_game_file_type_string_ids[type];

	return saved_game_file_type_string_ids[0];
}

bool saved_game_get_file_info(s_saved_game_main_menu_globals_save_file_info* out_info, enumerated_file_index enumerated_index)
{
	s_saved_game_main_menu_globals* saved_game_main_menu_globals = saved_game_main_menu_globals_get();
	s_saved_game_files_globals* saved_game_files_globals = saved_game_files_globals_get();

	if (saved_game_main_menu_globals)
	{
		// file is not a default save
		if (!ENUMERATED_INDEX_IS_DEFAULT_SAVE(enumerated_index))
		{
			uint32 abs_index = ENUMERATED_INDEX_GET_ABS_INDEX(enumerated_index);
			uint32 last_index = saved_game_main_menu_globals->save_files.get_count() - 1;
			if ((abs_index <= last_index || abs_index == last_index))
			{
				csmemcpy(out_info, saved_game_main_menu_globals->save_files[abs_index], saved_game_main_menu_globals->save_files.get_type_size());
				return true;
			}
		}
	}
	else if (saved_game_files_globals->cache_files_exist)
	{
		for (uint32 i = 0; i < saved_game_files_globals->cached_save_files.get_count(); ++i)
		{
			if (enumerated_index == saved_game_files_globals->cached_save_files[i]->enumerated_index)
			{
				csmemcpy(out_info, &saved_game_files_globals->cached_save_files[i]->file_info, sizeof(s_saved_game_main_menu_globals_save_file_info));
				return true;
			}
		}
	}

	return false;
}

void saved_game_files_search_by_type(e_controller_index controller_index, e_saved_game_file_type save_type,
	uint32* number_of_profiles, enumerated_file_index* saved_game_file_indicies, bool search_default_profiles)
{
	INVOKE(0x3F3BB, 0, saved_game_files_search_by_type, controller_index, save_type, number_of_profiles, saved_game_file_indicies, search_default_profiles);
}

uint32 saved_game_get_file_size_kb_for_type(e_saved_game_file_type type)
{
	if (type == _saved_game_file_type_profile)
		return (sizeof(s_saved_game_player_profile) + 1023) / k_kilo;

	if (type > _saved_game_file_type_profile && type <= _saved_game_file_type_game_variant_territories)
		return (sizeof(s_game_variant) + 1023) / k_kilo;

	return 0;
}

const wchar_t* saved_game_get_file_type_as_string(e_saved_game_file_type file_type)
{
	return VALID_INDEX(file_type, k_number_of_saved_game_file_types) ? k_saved_game_file_type_strings[file_type] : L"unknown";
}

bool saved_game_new_main_menu_globals_save_file(s_saved_game_main_menu_globals_save_file_info* new_save, e_saved_game_file_type file_type, wchar_t* out_path)
{
	wchar_t temp_buffer[256];
	wcsncpy_s(temp_buffer, new_save->file_path, 256);
	wchar_t* cat_path = ustrncat(temp_buffer, saved_game_get_file_type_as_string(file_type), 256);
	cat_path[255] = '\0';
	ustrncpy(out_path, cat_path, 256);
	return true;
}

bool __cdecl saved_game_add_save_to_cache(s_saved_game_main_menu_globals_save_file_info* new_save, uint32* out_save_index)
{
	s_saved_game_main_menu_globals* main_menu_globals = saved_game_main_menu_globals_get();

	ASSERT(new_save);
	ASSERT(main_menu_globals);

	uint32 last_save_index = main_menu_globals->save_files.get_count();

	if (last_save_index != k_maximum_enumerated_saved_game_files_any_type_per_memory_unit)
	{
		s_saved_game_main_menu_globals_save_file_info* save = main_menu_globals->save_files.next();

		memcpy(save, new_save, sizeof(s_saved_game_main_menu_globals_save_file_info));

		*out_save_index = last_save_index;

		return true;
	}

	return false;
	//return INVOKE(0x427A7, 0, saved_game_add_save_to_cache, new_save, out_save_index);
}

void __fastcall saved_game_remove_save_from_cache(enumerated_file_index enumerated_file_index)
{
	INVOKE(0x3EEC3, 0, saved_game_remove_save_from_cache, enumerated_file_index);
}

bool saved_game_append_file_type_to_path(const wchar_t* in_path, e_saved_game_file_type file_type, wchar_t* out_path)
{
	ustrncpy(out_path, in_path, 256);
	wchar_t* cat_path = ustrncat(out_path, saved_game_get_file_type_as_string(file_type), 256);
	cat_path[255] = '\0';
	ustrncpy(out_path, cat_path, 256);
	return true;
}

void saved_game_get_display_name(enumerated_file_index enumerated_index, wchar_t* display_name)
{
	ASSERT(display_name);

	display_name[0] = '\0';
	s_saved_game_main_menu_globals* saved_game_main_menu_globals = saved_game_main_menu_globals_get();

	if (saved_game_main_menu_globals && ENUMERATED_INDEX_IS_DEFAULT_SAVE(enumerated_index))
	{
		uint32 absolute_index = ENUMERATED_INDEX_GET_ABS_INDEX(enumerated_index);
		uint32 last_index = saved_game_main_menu_globals->default_save_files.get_count() - 1;
		if (absolute_index <= last_index || absolute_index == last_index)
		{
			s_saved_game_main_menu_globals_default_save_file* default_save = saved_game_main_menu_globals->default_save_files[absolute_index];
			s_saved_game_player_profile* default_profile = (s_saved_game_player_profile*)default_save->buffer;
			if (IN_RANGE(ENUMERATED_INDEX_GET_TYPE(enumerated_index), _saved_game_file_type_profile, k_number_of_saved_game_file_types))
			{
				csmemcpy(display_name, &default_save->buffer[0xC], 128);
			}
			else
			{
				csmemcpy(display_name, &default_save->buffer[0x8], 128);
			}
		}
	}
	else
	{
		s_saved_game_main_menu_globals_save_file_info file_info = {};
		if (saved_game_get_file_info(&file_info, enumerated_index))
		{
			ustrncpy(display_name, file_info.display_name, NUMBEROF(file_info.display_name));
		}
	}
	return;
}

bool saved_game_get_display_name_for_type(wchar_t* display_name, e_saved_game_file_type saved_game_type, e_language language, wchar_t* full_display_name)
{
	//todo: rewrite and fix broken implementation to return the game engine mode string?

	return INVOKE(0x427F6, 0, saved_game_get_display_name_for_type, display_name, saved_game_type, language, full_display_name);
}

bool __cdecl saved_game_create_save_game_directory(e_saved_game_file_type type, wchar_t* out_string)
{
	ASSERT(out_string);

	s_saved_game_files_globals* saved_game_files_globals = saved_game_files_globals_get();


	int8 test_result;

	if (input_windows_drive_letter_test(0, &test_result))
	{
		wchar_t full_display_name[save_game_max_name]{};
		char flat_path[MAX_PATH]{};
		wchar_t wide_path[MAX_PATH]{};

		pc_file_system_get_saved_games_location(flat_path, NUMBEROF(flat_path));


		int16 current_folder_index = 0;
		do
		{
			usnzprintf(out_string, save_game_max_name, L"Halo%04d", current_folder_index + 1);

			saved_game_get_display_name_for_type(out_string, type, saved_game_files_globals->language_id, full_display_name);

			if (pc_saved_game_get_next_available_save_location(flat_path, full_display_name, 3, 0, wide_path, MAX_PATH))
				break;

			++current_folder_index;

		} while (current_folder_index + 1 < k_maximum_enumerated_saved_game_files_any_type_per_memory_unit);

		if (current_folder_index == 4096)
		{
			saved_game_files_globals->saved_file_creation_result = _saved_game_disk_result_no_free_slots;

			memset(out_string, 0, sizeof(wchar_t) * save_game_max_name);
		}
	}

	return *out_string != 0;

	//return INVOKE(0x4333A, 0, saved_game_create_save_game_directory, type, out_string);
}

e_game_variant_description_index saved_game_type_to_variant_description(const e_saved_game_file_type type)
{
	switch (type)
	{
	case _saved_game_file_type_game_variant_slayer:
		return _game_variant_description_slayer;
	case _saved_game_file_type_game_variant_koth:
		return _game_variant_description_king;
	case _saved_game_file_type_game_variant_race:
		return _game_variant_description_slayer; // todo: add race
	case _saved_game_file_type_game_variant_oddball:
		return _game_variant_description_oddball;
	case _saved_game_file_type_game_variant_juggernaut:
		return _game_variant_description_juggernaut;
	case _saved_game_file_type_game_variant_headhunter:
		return _game_variant_description_headhunter;
	case _saved_game_file_type_game_variant_ctf:
		return _game_variant_description_ctf;
	case _saved_game_file_type_game_variant_assault:
		return _game_variant_description_invasion;
	case _saved_game_file_type_game_variant_territories:
		return _game_variant_description_territories;
	default:
		return _game_variant_description_slayer;
	}
}

e_saved_game_file_type saved_game_get_variant_file_type(const s_game_variant* variant)
{
	switch (variant->variant_game_engine_index)
	{
	case _game_engine_type_ctf:
		return _saved_game_file_type_game_variant_ctf;
	case _game_engine_type_slayer:
		return _saved_game_file_type_game_variant_slayer;
	case _game_engine_type_oddball:
		return _saved_game_file_type_game_variant_oddball;
	case _game_engine_type_koth:
		return _saved_game_file_type_game_variant_koth;
	case _game_engine_type_race:
		return _saved_game_file_type_game_variant_race;
	case _game_engine_type_headhunter:
		return _saved_game_file_type_game_variant_headhunter;
	case _game_engine_type_juggernaut:
		return _saved_game_file_type_game_variant_juggernaut;
	case _game_engine_type_territories:
		return _saved_game_file_type_game_variant_territories;
	case _game_engine_type_assault:
		return _saved_game_file_type_game_variant_assault;
	default:
		return _saved_game_file_type_game_variant_slayer;
	}
}

enumerated_file_index saved_game_create_file(e_saved_game_file_type type, e_controller_index originating_controller_index,
	wchar_t* new_file_name)
{
	s_saved_game_files_globals* saved_game_files_globals = saved_game_files_globals_get();
	s_saved_game_main_menu_globals* saved_game_main_menu_globals = saved_game_main_menu_globals_get();

	ASSERT(new_file_name);
	ASSERT(saved_game_files_globals);
	ASSERT(saved_game_main_menu_globals);

	saved_game_file_globals_wait_for_io_to_complete();

	if (saved_game_main_menu_globals->save_files.get_count() >= k_maximum_enumerated_saved_game_files_any_type_per_memory_unit)
	{
		saved_game_files_globals->saved_file_creation_result = _saved_game_disk_result_no_free_slots;

		return NONE;
	}

	wchar_t full_display_name[save_game_max_name]{};

	if (saved_game_get_display_name_for_type(new_file_name, type, saved_game_files_globals->language_id, full_display_name))
	{
		char flat_games_location[MAX_PATH]{};
		wchar_t wide_save_location[MAX_PATH]{};

		pc_file_system_get_saved_games_location(flat_games_location, MAX_PATH);

		if (!pc_saved_game_get_next_available_save_location(flat_games_location, full_display_name, 1, 0, wide_save_location, MAX_PATH))
		{
			CHAR multi_byte_location[256]{};
			WCHAR saved_game_full_path[256]{};

			s_saved_game_main_menu_globals_save_file_info new_main_menu_save{};

			uint32 new_saved_game_index = 0;

			s_file_reference filo;

			new_main_menu_save.type = type;
			new_main_menu_save.language_id = (byte)saved_game_files_globals->language_id;

			wcsncpy_s(new_main_menu_save.display_name, new_file_name, 17u);
			wcsncpy_s(new_main_menu_save.file_path, wide_save_location, MAX_PATH);


			saved_game_new_main_menu_globals_save_file(&new_main_menu_save, type, saved_game_full_path);

			WideCharToMultiByte(CP_UTF8, 0, saved_game_full_path, NONE, multi_byte_location, 256, 0, 0);

			if (file_reference_create_from_path(&filo, multi_byte_location, 0)
				&& file_create(&filo)
				&& saved_game_add_save_to_cache(&new_main_menu_save, &new_saved_game_index))
			{
				const enumerated_file_index new_file_enumerated_file_index = saved_game_enumerated_index_new(type, new_saved_game_index, false);

				if (new_file_enumerated_file_index != NONE)
				{
					saved_game_files_globals->saved_file_creation_result = _saved_gave_disk_result_success;

					return new_file_enumerated_file_index;
				}
			}
			else
			{
				pc_file_system_delete_save_directory(flat_games_location, full_display_name);
			}
		}

		uint32 save_file_type_size = saved_game_get_file_size_kb_for_type(type);

		saved_game_files_globals->saved_file_creation_result = pc_file_system_check_disk_free_space(save_file_type_size)
			? _saved_games_disk_result_memory_unit_error
			: _saved_game_disk_result_no_free_space;
	}

	return NONE;
}

enumerated_file_index saved_game_create_new_game_variant(e_controller_index origin_controller, e_saved_game_file_type type,
	wchar_t* name)
{
	const enumerated_file_index enumerated_file_index = saved_game_create_file(type, origin_controller, name);

	s_game_variant new_variant{};

	if (enumerated_file_index == NONE)
		return NONE;

	game_variant_create_default_new(&new_variant, saved_game_type_to_variant_description(type));

	new_variant.flags &= ~1u;

	ASSERT(game_variant_cleanup(&new_variant));

	wcsncpy_s(new_variant.variant_name, name, NUMBEROF(new_variant.variant_name));

	if (saved_games_async_helper_write_variant(enumerated_file_index, &new_variant))
	{
		return enumerated_file_index;
	}

	saved_game_new_failure_cleanup(type, enumerated_file_index);
	return NONE;

	//return INVOKE(0x5AB63, 0, saved_game_create_new_game_variant, origin_controller, type, name);
}


bool __cdecl saved_game_load_game_variant(int32 enumerated_index, s_game_variant* out_variant)
{
	if (enumerated_index == NONE)
		return false;

	if (!saved_games_async_helper_read_file(enumerated_index, (int8*)out_variant, sizeof(s_game_variant)))
		return false;

	bool validation_result = game_variant_cleanup(out_variant);

	ASSERT(validation_result);

	if (validation_result)
	{
		if (ENUMERATED_INDEX_IS_DEFAULT_SAVE(enumerated_index))
		{
			out_variant->flags |= 1u;
		}
		else
		{
			out_variant->flags &= 1u;
		}

		return true;
	}

	return false;
	//return INVOKE(0x5A96B, 0, saved_game_load_game_variant, enumerated_index, out_variant);
}

/* private code */

static void saved_game_main_menu_globals_set(s_saved_game_main_menu_globals* new_globals)
{
	*Memory::GetAddress<s_saved_game_main_menu_globals**>(0x482300) = new_globals;
}

static uint32 __cdecl saved_game_loading_allocate_storage(int32 a1, s_saved_game_file_loading_information* loading_information)
{
	return INVOKE(0x9CC67, 0, saved_game_loading_allocate_storage, a1, loading_information);
}

static void saved_game_load_save_file_information_from_disk(c_static_array<s_saved_game_main_menu_globals_save_file_info, k_maximum_enumerated_saved_game_files_any_type_per_memory_unit>* save_files_storage)
{
	return p_saved_games_load_save_file_information_from_disk(save_files_storage);
}

void saved_game_file_globals_wait_for_io_to_complete()
{
	s_saved_game_files_globals* saved_game_file_globals = saved_game_files_globals_get();

	async_yield_until_done((s_async_completion*)saved_game_file_globals, true);
}

static void saved_game_main_menu_globals_initialize(void)
{
	s_saved_game_files_globals* saved_game_files_globals = saved_game_files_globals_get();

	s_saved_game_main_menu_globals* saved_game_main_menu_globals = (s_saved_game_main_menu_globals*)c_physical_memory::allocate(sizeof(s_saved_game_main_menu_globals));

	ASSERT(saved_game_main_menu_globals);

	if(saved_game_main_menu_globals)
	{
		saved_game_main_menu_globals->default_save_files.clear();
		saved_game_main_menu_globals->save_files.clear();
	}
	else
	{
		saved_game_main_menu_globals = nullptr;
	}

	saved_game_main_menu_globals->saved_game_file_index_salt = saved_game_files_globals->unk_6;

	saved_game_files_globals->unk_6 = (saved_game_files_globals->unk_6 + 1) % 200;
	saved_game_files_globals->unk_3 = 3;

	saved_game_main_menu_globals_set(saved_game_main_menu_globals);
	return;
}

static void saved_game_files_memory_initialize(int32 unk)
{
	s_saved_game_files_globals* saved_game_files_globals = saved_game_files_globals_get();

#ifdef ASSERTS_ENABLED
	s_saved_game_main_menu_globals* saved_game_main_menu_globals = saved_game_main_menu_globals_get();
	ASSERT(saved_game_main_menu_globals);
#endif

	ASSERT(saved_game_files_globals->memory_initialized_for_game);
	
	if (unk == 1)
	{
		saved_game_main_menu_globals_initialize();
	}

	saved_game_files_globals->memory_initialized_for_game = true;
	return;
}
