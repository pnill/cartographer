#include "stdafx.h"

#include "player_profile.h"
#include "saved_game_files.h"
#include "saved_game_files_async_windows.h"
#include "cseries/async.h"
#include "cseries/async_helpers.h"
#include "tag_files/files_windows.h"

// storage for the profile that is being loaded that exists on a global scope awaiting the async file loading to complete;
s_cartographer_player_profile loading_profile{};

typedef bool(__cdecl* t_saved_games_async_helper_write_file)(int32 enumerated_index, wchar_t* new_display_name, void* buffer, uint32 buffer_size, int8* completion);
t_saved_games_async_helper_write_file p_saved_games_async_helper_write_file;

bool __cdecl saved_games_async_helper_write_file(int32 enumerated_index, wchar_t* new_display_name, void* buffer, uint32 buffer_size, int8* completion)
{
	if (buffer_size == sizeof(s_saved_game_player_profile))
	{
		csmemcpy(&loading_profile, buffer, buffer_size);
		return p_saved_games_async_helper_write_file(enumerated_index, new_display_name, &loading_profile, sizeof(s_cartographer_player_profile), completion);
	}

	return p_saved_games_async_helper_write_file(enumerated_index, new_display_name, buffer, buffer_size, completion);
}


bool __cdecl saved_games_async_helper_async_read_create_task(void* unk, void* buffer, uint32 buffer_size, bool unk_flag, int8* in_out_completion)
{
	return INVOKE(0x9b149, 0, saved_games_async_helper_async_read_create_task, unk, buffer, buffer_size, unk_flag, in_out_completion);
}

typedef bool(__cdecl* t_saved_games_async_helper_read_file_internal)(int enumerated_index, void* buffer, unsigned int buffer_size, int8* in_out_completion);
t_saved_games_async_helper_read_file_internal p_saved_games_async_helper_read_file_internal;

bool __cdecl saved_games_async_helper_read_file_internal(int enumerated_index, void* buffer, unsigned int buffer_size, int8* in_out_completion)
{
	if (buffer_size == sizeof(s_saved_game_player_profile))
	{
		csmemset(&loading_profile, 0, sizeof(s_cartographer_player_profile));
		s_saved_game_main_menu_globals* saved_game_globals = saved_game_main_menu_globals_get();

		auto abs_index = (enumerated_index >> 8) & 0x1FFF;
		auto last_index = saved_game_globals->default_save_files.get_count() - 1;

		// 0x200000 in the index flags that the index is a default file
		if ((enumerated_index & 0x200000) != 0 && saved_game_globals)
		{

			// Removed check of buffer_size so it can read the entry from the globals
			// potentially unsafe will have to come up with a way to verify if this was actually needed or not.
			if ((abs_index <= last_index || abs_index == last_index))
			{
				auto test = saved_game_globals->default_save_files[abs_index];
				csmemcpy(buffer, saved_game_globals->default_save_files[abs_index]->buffer, buffer_size);
				in_out_completion[2] = 1;
				*(in_out_completion + 1) = 0;
				*in_out_completion = 1;
				*(in_out_completion + 514) = 0x3F800000;
				return true;
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
						WideCharToMultiByte(0xFDE9u, 0, file_path, -1, flattened_file_path, 256, 0, 0);
						bool result =  saved_games_async_helper_async_read_create_task(flattened_file_path, buffer, buffer_size, 0, in_out_completion);

						return result;
					}
				}
			}
		}
		return false;
	}

	return p_saved_games_async_helper_read_file_internal(enumerated_index, buffer, buffer_size, in_out_completion);
}

void saved_games_async_helper_read_file_success(int8* a1, s_file_reference* file_reference, int8* buffer, uint32 buffer_size)
{
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
}

typedef void(__cdecl* t_saved_games_async_helper_read_file_callback)(int32 a1, int8* context, int32 data_size);
t_saved_games_async_helper_read_file_callback p_saved_games_async_helper_read_file_callback;

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
}

typedef void(__cdecl* t_saved_games_async_helper_read_file)(uint32 enumerated_index, int8* buffer, uint32 buffer_size);
t_saved_games_async_helper_read_file p_saved_games_async_helper_read_file;

bool saved_games_async_helper_read_file(uint32 enumerated_index, int8* buffer, uint32 buffer_size)
{
	s_saved_game_files_globals* saved_game_globals = saved_game_files_globals_get();

	// todo: figure out structure of the completetion object for async work
	int8 in_out_completion[2060];

	if (!saved_games_async_helper_read_file_internal(enumerated_index, buffer, buffer_size, in_out_completion))
		return false;
	async_yield_until_done(in_out_completion, true);

	bool result = in_out_completion[0];
	if (!result)
		saved_game_globals->unk_4 = *(int32*)&in_out_completion[4];
	return result;
}

void saved_games_async_helpers_apply_hooks()
{
	//DETOUR_ATTACH(p_saved_games_async_helper_write_file, Memory::GetAddress<t_saved_games_async_helper_write_file>(0x42CA6), saved_games_async_helper_write_file);
	//DETOUR_ATTACH(p_saved_games_async_helper_read_file_internal, Memory::GetAddress<t_saved_games_async_helper_read_file_internal>(0x4259E), saved_games_async_helper_read_file_internal);
	//DETOUR_ATTACH(p_saved_games_async_helper_read_file_callback, Memory::GetAddress<t_saved_games_async_helper_read_file_callback>(0x9B0D0), saved_games_async_helper_read_file_callback);
	//DETOUR_ATTACH(p_saved_games_async_helper_read_file, Memory::GetAddress<t_saved_games_async_helper_read_file>(0x46450), saved_games_async_helper_read_file);
}
