#pragma once

enum e_pc_file_system_type : uint32
{
	_pc_file_system_type_game_save = 0,
	_pc_file_system_type_2 = 1,
	_pc_file_system_type_3 = 2,
	_pc_file_system_type_4 = 3,
	_pc_file_system_type_custom_map = 4,
	_pc_file_system_type_6 = 5,
	_pc_file_system_type_screen_shot = 6,
	_pc_file_system_type_8 = 7,
	k_pc_file_system_count = 8
};

void file_seek_and_read(FILE* file_handle, uint32 file_offset, uint32 read_size, uint32 read_count, void* out_buffer);