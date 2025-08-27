#include "stdafx.h"
#include "pc_file_system.h"

/* typedefs */

typedef int(__cdecl* t_pc_file_system_create_directory_hierarchy)(e_pc_file_system_type type, wchar_t* buffer, wchar_t* buffer_2, bool return_folder_path);
t_pc_file_system_create_directory_hierarchy p_pc_file_system_create_directory_hierarchy;

/* public code */

s_pc_file_type_save_path_info* pc_file_system_get_save_path_info(e_pc_file_system_type type)
{
	return &Memory::GetAddress<s_pc_file_type_save_path_info*>(0x413868, 0)[type];
}


void file_seek_and_read(FILE* file_handle, uint32 file_offset, uint32 read_size, uint32 read_count, void* out_buffer)
{
#ifdef ASSERTS_ENABLED
	const bool seek_result = fseek(file_handle, file_offset, SEEK_SET) == 0;
	ASSERT(seek_result);
#else
	fseek(file_handle, file_offset, SEEK_SET);
#endif

#ifdef ASSERTS_ENABLED
	const bool read_result = fread(out_buffer, read_size, read_count, file_handle);
	ASSERT(read_result);
#else
	fread(out_buffer, read_size, read_count, file_handle);
#endif
	return;
}

int __cdecl pc_file_system_create_directory_hierarchy(e_pc_file_system_type type, wchar_t* buffer, wchar_t* buffer_2, bool is_folder)
{
	ASSERT(type < k_pc_file_system_count);


	int result = p_pc_file_system_create_directory_hierarchy(type, buffer, buffer_2, is_folder);
	return result;
}

bool __cdecl pc_file_system_get_drive_letter_for_type(e_pc_file_system_type type, wchar_t* buffer)
{
	return INVOKE(0x8F1FE, 0, pc_file_system_get_drive_letter_for_type, type, buffer);
}

char* pc_file_system_get_saved_games_location(char* out_path, int16 buffer_size)
{
	// invoking by type to remove unused parameter
	return INVOKE_TYPE(0x3E43E, 0, char* (__cdecl*)(int32, char*, int16), 0, out_path, buffer_size);
}

int32 __cdecl pc_file_system_delete_save_directory(CHAR* flat_file_path, wchar_t* save_file_name)
{
	return INVOKE(0x9CB52, 0, pc_file_system_delete_save_directory, flat_file_path, save_file_name);
}

bool __cdecl pc_file_system_check_disk_free_space(uint32 size)
{
	return INVOKE(0x3E705, 0, pc_file_system_check_disk_free_space, size);
}

void pc_file_system_apply_hooks()
{
	//DETOUR_ATTACH(p_pc_file_system_create_directory_hierarchy, Memory::GetAddress<t_pc_file_system_create_directory_hierarchy>(0x8EF9E), pc_file_system_create_directory_hierarchy);
}
