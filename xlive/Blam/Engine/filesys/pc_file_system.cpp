#include "stdafx.h"
#include "pc_file_system.h"

s_pc_file_type_save_path_info* g_pc_file_system_save_path_info;

s_pc_file_type_save_path_info* pc_file_system_get_save_path_info(e_pc_file_system_type type)
{
	return &Memory::GetAddress<s_pc_file_type_save_path_info*>(0x413868, 0)[type];
}


void file_seek_and_read(FILE* file_handle, uint32 file_offset, uint32 read_size, uint32 read_count, void* out_buffer)
{
	bool seek_result = fseek(file_handle, file_offset, SEEK_SET) == 0;
	ASSERT(seek_result);
	bool read_result = fread(out_buffer, read_size, read_count, file_handle);
	ASSERT(read_result);
}

typedef int(__cdecl* t_pc_file_system_create_directory_hierarchy)(e_pc_file_system_type type, wchar_t* buffer, wchar_t* buffer_2, bool return_folder_path);
t_pc_file_system_create_directory_hierarchy p_pc_file_system_create_directory_hierarchy;

int __cdecl pc_file_system_create_directory_hierarchy(e_pc_file_system_type type, wchar_t* buffer, wchar_t* buffer_2, bool is_folder)
{
	ASSERT(type < k_pc_file_system_count);


	int result = p_pc_file_system_create_directory_hierarchy(type, buffer, buffer_2, is_folder);
	return result;
}

void pc_file_system_apply_hooks()
{
	DETOUR_ATTACH(p_pc_file_system_create_directory_hierarchy, Memory::GetAddress<t_pc_file_system_create_directory_hierarchy>(0x8EF9E), pc_file_system_create_directory_hierarchy);
}
