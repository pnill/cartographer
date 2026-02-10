#pragma once
#include "files_windows.h"

/* constants */

enum
{
	FILE_REFERENCE_SIGNATURE = 'filo'
};

enum
{
	_has_filename_bit = 0,
	NUMBER_OF_REFERENCE_INFO_FLAGS
};

enum
{
	_file_reference_application_relative = 0,
	_file_reference_cd_relative,
	NUMBER_OF_FILE_REFERENCE_LOCATIONS,
};

enum
{
	_name_directory_bit= 0,
	_name_parent_directory_bit,
	_name_filename_bit,
	_name_extension_bit,
	NUMBER_OF_NAME_FLAGS,
	_name_full_mask = FLAG(_name_directory_bit) | FLAG(_name_filename_bit) | FLAG(_name_extension_bit),
};

/* structures */

struct s_file_reference_info
{
	uint32 signature;
	uint16 flags;
	int16 location;
	char path[k_maximum_filename_length];
	HANDLE file_handle;
	HRESULT api_result;
};

struct s_file_reference : s_file_reference_info
{
};
ASSERT_STRUCT_SIZE(s_file_reference, 272);

/* prototypes */

s_file_reference_info* file_reference_get_info(s_file_reference* info);

s_file_reference* file_reference_add_directory(s_file_reference* reference, const char* directory);

char* __cdecl file_reference_get_name(const s_file_reference* reference, uint8 flags, char* out_name);

void file_create_parent_directories_if_not_present(struct s_file_reference* file_reference);

void file_trim(s_file_reference* reference, size_t max_size);
