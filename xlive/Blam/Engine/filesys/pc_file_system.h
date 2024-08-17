#pragma once

enum e_pc_file_system_type : uint32
{
	_pc_file_system_type_saved_games = 0,
	_pc_file_system_type_maps = 1,
	_pc_file_system_type_secondary = 2,
	_pc_file_system_type_document_base = 3,
	_pc_file_system_type_custom_map = 4,
	_pc_file_system_type_my_games_base = 5,
	_pc_file_system_type_screen_shot = 6,
	_pc_file_system_type_my_games_map_data_files = 7,
	k_pc_file_system_count = 8
};

struct s_pc_file_type_save_path_info
{
	// KNOWNFOLDERID Reference: https://learn.microsoft.com/en-us/windows/win32/shell/knownfolderid#constants
	KNOWNFOLDERID* known_folder_id;
	// CSIDL Reference: https://github.com/tpn/winsdk-10/blob/master/Include/10.0.10240.0/um/ShlObj.h#L1113
	uint32 cs_idl;
	wchar_t* path;
};

s_pc_file_type_save_path_info* pc_file_system_get_save_path_info(e_pc_file_system_type type);

void file_seek_and_read(FILE* file_handle, uint32 file_offset, uint32 read_size, uint32 read_count, void* out_buffer);

void pc_file_system_apply_hooks();