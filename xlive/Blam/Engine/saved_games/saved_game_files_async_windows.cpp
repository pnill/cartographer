#include "stdafx.h"
#include "saved_game_files_async_windows.h"

#include "saved_game_files.h"

#include "cseries/async.h"
#include "tag_files/files_windows.h"

/* typedefs */

typedef bool(__cdecl* t_saved_games_async_helper_write_file)(int32 enumerated_index, wchar_t* new_display_name, void* buffer, uint32 buffer_size, s_async_completion* completion);
t_saved_games_async_helper_write_file p_saved_games_async_helper_write_file;

typedef bool(__cdecl* t_saved_games_async_helper_read_file_internal)(int enumerated_index, void* buffer, unsigned int buffer_size, s_async_completion* in_out_completion);
t_saved_games_async_helper_read_file_internal p_saved_games_async_helper_read_file_internal;

typedef void(__cdecl* t_saved_games_async_helper_read_file_callback)(int32 a1, int8* context, int32 data_size);
t_saved_games_async_helper_read_file_callback p_saved_games_async_helper_read_file_callback;

typedef void(__cdecl* t_saved_games_async_helper_read_file)(uint32 enumerated_index, int8* buffer, uint32 buffer_size);
t_saved_games_async_helper_read_file p_saved_games_async_helper_read_file;

/* prototypes */

bool __cdecl saved_games_async_helper_write_file(int32 enumerated_index, wchar_t* new_display_name, void* buffer, uint32 buffer_size, s_async_completion* completion);
bool __cdecl saved_games_async_helper_async_read_create_task(void* unk, void* buffer, uint32 buffer_size, bool unk_flag, s_async_completion* in_out_completion);
bool __cdecl saved_games_async_helper_read_file_internal(int enumerated_index, void* buffer, unsigned int buffer_size, s_async_completion* in_out_completion);
void saved_games_async_helper_read_file_success(int8* a1, s_file_reference* file_reference, int8* buffer, uint32 buffer_size);
void __cdecl saved_games_async_helper_read_file_callback(int32 a1, int8* context, int32 data_size);
bool saved_games_async_helper_read_file(uint32 enumerated_index, int8* buffer, uint32 buffer_size);
bool __cdecl saved_games_async_helper_create_task_write_bin_file(wchar_t* full_path, int8* buffer, uint32 header_size, int8* data, uint32 data_size, bool unk, s_async_completion* completion);
bool __cdecl saved_games_async_helper_create_task_read_bin_file(wchar_t* full_path, int8* buffer, uint32 header_size, int8* data, uint32 data_size, bool unk, s_async_completion* completion);

/* public code */

void saved_games_async_helpers_apply_patches(void)
{
	//DETOUR_ATTACH(p_saved_games_async_helper_write_file, Memory::GetAddress<t_saved_games_async_helper_write_file>(0x42CA6), saved_games_async_helper_write_file);
	//DETOUR_ATTACH(p_saved_games_async_helper_read_file_internal, Memory::GetAddress<t_saved_games_async_helper_read_file_internal>(0x4259E), saved_games_async_helper_read_file_internal);
	//DETOUR_ATTACH(p_saved_games_async_helper_read_file_callback, Memory::GetAddress<t_saved_games_async_helper_read_file_callback>(0x9B0D0), saved_games_async_helper_read_file_callback);
	//DETOUR_ATTACH(p_saved_games_async_helper_read_file, Memory::GetAddress<t_saved_games_async_helper_read_file>(0x46450), saved_games_async_helper_read_file);
	return;
}

void saved_games_async_helper_get_saved_game_bin_path(uint32 enumerated_file_index, const wchar_t* binary_name, wchar_t* out_path)
{
	ASSERT(out_path);
	ASSERT(binary_name);

	s_saved_game_main_menu_globals_save_file_info file_info{};
	saved_games_get_file_info(&file_info, enumerated_file_index);

	ustrnzcpy(out_path, file_info.file_path, NUMBEROF(file_info.file_path));
	ustrnzcat(out_path, binary_name, ustrnlen(binary_name, NUMBEROF(file_info.file_path)));
	ustrnzcat(out_path, L".bin", 4);
	return;
}

bool saved_games_async_helper_check_saved_game_bin(const wchar_t* binary_name, uint32 enumerated_file_index)
{
	bool result = false;

	wchar_t bin_path[MAX_PATH]{};

	saved_games_async_helper_get_saved_game_bin_path(enumerated_file_index, binary_name, bin_path);

	utf8 utf8_path[MAX_PATH];
	wchar_string_to_utf8_string(bin_path, utf8_path, MAX_PATH);

	s_file_reference file_reference;
	if (file_reference_create_from_path(&file_reference, utf8_path, false))
	{
		e_file_open_error t_error;
		if (file_open(&file_reference, _permission_read_bit, &t_error))
		{
			file_close(&file_reference);
			result = true;
		}
	}
	return result;
}

bool saved_games_async_helper_write_saved_game_bin(const wchar_t* binary_name, uint32 enumerated_file_index, int8* buffer, uint32 buffer_size)
{
	s_async_completion in_out_completion{};
	wchar_t bin_path[MAX_PATH]{};

	saved_games_async_helper_get_saved_game_bin_path(enumerated_file_index, binary_name, bin_path);

	bool result = saved_games_async_helper_create_task_write_bin_file(bin_path, (int8*)buffer, 0, (int8*)buffer, buffer_size, false, &in_out_completion);

	async_yield_until_done(&in_out_completion, true);

	return result;
}

bool saved_games_async_helper_read_saved_game_bin(const wchar_t* binary_name, uint32 enumerated_file_index, int8* buffer, uint32 buffer_size)
{
	bool result = false;
	if (saved_games_async_helper_check_saved_game_bin(binary_name, enumerated_file_index))
	{
		s_async_completion in_out_completion{};
		wchar_t bin_path[MAX_PATH]{};

		saved_games_async_helper_get_saved_game_bin_path(enumerated_file_index, binary_name, bin_path);

		result = saved_games_async_helper_create_task_read_bin_file(bin_path, buffer, 0, buffer, buffer_size, false, &in_out_completion);

		async_yield_until_done(&in_out_completion, true);
	}
	return result;
}

/* private code */

bool __cdecl saved_games_async_helper_write_file(int32 enumerated_index, wchar_t* new_display_name, void* buffer, uint32 buffer_size, s_async_completion* completion)
{
	return p_saved_games_async_helper_write_file(enumerated_index, new_display_name, buffer, buffer_size, completion);
}

bool __cdecl saved_games_async_helper_async_read_create_task(void* unk, void* buffer, uint32 buffer_size, bool unk_flag, s_async_completion* in_out_completion)
{
	return INVOKE(0x9b149, 0, saved_games_async_helper_async_read_create_task, unk, buffer, buffer_size, unk_flag, in_out_completion);
}

bool __cdecl saved_games_async_helper_read_file_internal(int enumerated_index, void* buffer, unsigned int buffer_size, s_async_completion* in_out_completion)
{
	bool result = false;

	s_saved_game_main_menu_globals* saved_game_globals = saved_game_main_menu_globals_get();

	// 0x200000 in the index flags that the index is a default file
	if (ENUMERATED_INDEX_IS_DEFAULT_SAVE(enumerated_index) && saved_game_globals)
	{
		uint32 abs_index = (enumerated_index >> 8) & 0x1FFF;
		uint32 last_index = saved_game_globals->default_save_files.get_count() - 1;

		// checking the size of the buffer is not specifically for player_profile it is whatever the
		// largest structure that can be stored in the default save files static_array is.
		if (abs_index <= last_index && buffer_size <= sizeof(s_saved_game_player_profile))
		{
			csmemcpy(buffer, saved_game_globals->default_save_files[abs_index]->buffer, buffer_size);
			in_out_completion->unk_2 = true;
			in_out_completion->unk_4 = 0;
			in_out_completion->completed = true;
			in_out_completion->unk_808 = 1.f;
			result = true;
		}
	}
	else
	{
		s_saved_game_main_menu_globals_save_file_info file_info {};
		wchar_t file_path[256];
		char flattened_file_path[256];
		if(saved_games_get_file_info(&file_info, enumerated_index))
		{
			if(file_info.unk_5 != NONE)
			{
				if(saved_games_append_file_type_to_path(file_info.file_path, file_info.type, file_path))
				{
					WideCharToMultiByte(65001, 0, file_path, -1, flattened_file_path, NUMBEROF(flattened_file_path), 0, 0);
					result = saved_games_async_helper_async_read_create_task(flattened_file_path, buffer, buffer_size, 0, in_out_completion);
				}
			}
		}
	}
	return result;
}

void saved_games_async_helper_read_file_success(int8* a1, s_file_reference* file_reference, int8* buffer, uint32 buffer_size)
{
	// todo: figure out structure of context for async callbacks
	int32* unk_1 = ((int32*)a1 + 1);
	int8* unk_2 = &a1[1];

	e_file_open_error error_result;
	if(file_open(file_reference, (e_file_open_flags)(_file_open_file_attribute_temp | _permission_read_bit), &error_result))
	{
		*unk_1 = 3;

		uint32 file_size = 0;
		if(file_get_size(file_reference, &file_size))
		{
			_CRYPTOAPI_BLOB out_blob;
			_CRYPTOAPI_BLOB in_blob;

			out_blob.cbData = 0;
			out_blob.pbData = nullptr;
			in_blob.cbData = file_size;

			BYTE* data_block = (BYTE*)system_heap_alloc(file_size);
			in_blob.pbData = data_block;
			if(data_block)
			{
				if(file_read(file_reference, file_size, true, data_block))
				{
					if(CryptUnprotectData(&in_blob, 0, 0, 0, 0, CRYPTPROTECT_UI_FORBIDDEN, &out_blob))
					{
						BYTE* data = out_blob.pbData;
						if(out_blob.cbData == buffer_size)
						{
							csmemcpy(buffer, data, buffer_size);
							a1[2] = 1;
							*unk_1 = 0;
						}
						LocalFree(data);
					}
					if(!a1[2] && file_size == buffer_size)
					{
						csmemcpy(buffer, in_blob.pbData, buffer_size);
						a1[2] = 1;
						*unk_1 = 0;
					}
					// new if statements allows loading of files that aren't upgraded.
					else if(!a1[2] && buffer_size > file_size)
					{
						csmemcpy(buffer, in_blob.pbData, file_size);
						a1[2] = 1;
						*unk_1 = 0;
					}
					system_heap_free(data_block);
				}
			}
		}
		file_close(file_reference);
	}
	return;
}

void __cdecl saved_games_async_helper_read_file_callback(int32 a1, int8* context, int32 data_size)
{
	// todo: figure out structure of context for async callbacks
	*(int8*)(*((int32*)context + 3) + 2) = 0;
	*(int32*)(*((int32*)context + 3) + 4) = 4;

	s_file_reference file_reference;
	if(file_reference_create_from_path(&file_reference, (char*)(*((int32*)context + 3) + 8), false))
	{
		saved_games_async_helper_read_file_success(*((int8**)context + 3), &file_reference, *(int8**)context, *((uint32*)context + 1));
	}
	return;
}

bool saved_games_async_helper_read_file(uint32 enumerated_index, int8* buffer, uint32 buffer_size)
{
	s_saved_game_files_globals* saved_game_globals = saved_game_files_globals_get();

	// todo: figure out structure of the completetion object for async work
	s_async_completion in_out_completion{};

	if (!saved_games_async_helper_read_file_internal(enumerated_index, buffer, buffer_size, &in_out_completion))
		return false;
	async_yield_until_done(&in_out_completion, true);

	bool result = in_out_completion.completed;
	if (!result)
		saved_game_globals->unk_4 = in_out_completion.unk_4;
	return result;
}

bool __cdecl saved_games_async_helper_create_task_write_bin_file(wchar_t* full_path, int8* buffer, uint32 header_size, int8* data, uint32 data_size, bool unk, s_async_completion* completion)
{
	return INVOKE(0x9AE4C, 0, saved_games_async_helper_create_task_write_bin_file, full_path, buffer, header_size, data, data_size, unk, completion);
}

bool __cdecl saved_games_async_helper_create_task_read_bin_file(wchar_t* full_path, int8* buffer, uint32 header_size, int8* data, uint32 data_size, bool unk, s_async_completion* completion)
{
	return INVOKE(0x9B67D, 0, saved_games_async_helper_create_task_read_bin_file, full_path, buffer, header_size, data, data_size, unk, completion);
}
