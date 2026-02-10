#include "stdafx.h"
#include "files.h"

#include "files_windows.h"

/* public code */

s_file_reference_info* file_reference_get_info(s_file_reference* info)
{
	ASSERT(info);
	ASSERT(info->signature == FILE_REFERENCE_SIGNATURE);
	ASSERT(VALID_BITS(info->flags, NUMBER_OF_REFERENCE_INFO_FLAGS));
	ASSERT(info->location >= NONE && info->location < NUMBER_OF_FILE_REFERENCE_LOCATIONS);
	return info;
}

s_file_reference* file_reference_add_directory(
	s_file_reference* reference,
	const char* directory)
{
	s_file_reference_info* info= file_reference_get_info(reference);
	
	ASSERT(directory);
	ASSERT(!TEST_BIT(info->flags, _has_filename_bit));
	file_path_add_name(info->path, directory);

	return reference;
}

char* __cdecl file_reference_get_name(
	const s_file_reference* reference,
	uint8 flags,
	char* name)
{
	char* directory;
	char* filename;
	char* parent_directory;
	char* extension;

	s_file_reference_info* info = file_reference_get_info((s_file_reference*)reference);
	char path[k_maximum_filename_length] = {};

	ASSERT(name);
	ASSERT(VALID_BITS(info->flags, NUMBER_OF_NAME_FLAGS));
	ASSERT(flags);
	ASSERT(flags!=(FLAG(_name_directory_bit)|FLAG(_name_extension_bit)));
	ASSERT(!TEST_BIT(flags, _name_directory_bit) || !TEST_BIT(flags, _name_parent_directory_bit));

	file_location_get_full_path(info->location, info->path, path);
	file_path_split(path, &directory, &parent_directory, &filename, &extension, TEST_BIT(info->flags, _name_directory_bit));
	name[0] = '\0';

	if (TEST_BIT(flags, _name_directory_bit))
	{
		file_path_add_name(name, directory);
	}
	if (TEST_BIT(flags, _name_parent_directory_bit))
	{
		file_path_add_name(name, parent_directory);
	}
	if (TEST_BIT(flags, _name_filename_bit))
	{
		file_path_add_name(name, filename);
	}
	if (TEST_BIT(flags, _name_extension_bit))
	{
		file_path_add_extension(name, extension);
	}

	return name;
}

void file_create_parent_directories_if_not_present(
	s_file_reference* file_reference)
{
	char directory[k_maximum_filename_length];
	char name[k_maximum_filename_length];

	s_file_reference directory_reference;
	csmemset(&directory_reference, 0, sizeof(directory_reference));
	directory_reference.signature = FILE_REFERENCE_SIGNATURE;
	directory_reference.location = NONE;

	file_reference_add_directory(&directory_reference, "");
	file_reference_get_name(file_reference, FLAG(_name_directory_bit), name);

	csstrtok_data data;
	for (char* i = csstrtok(name, "\\", true, &data);
		i && i[0];
		i = csstrtok(NULL, "\\", true, &data))
	{
		csstrncpy(directory, i, NUMBEROF(directory));
		csstrncat(directory, "\\", NUMBEROF(directory));
		file_reference_add_directory(&directory_reference, directory);
		if (!file_exists(&directory_reference))
		{
			file_create(&directory_reference);
		}
	}

	return;
}

void file_trim(s_file_reference* reference, size_t max_size)
{
	bool opened_file = false;

	void* buffer = CSERIES_MALLOC(max_size);
	if (buffer)
	{
		e_file_open_error error;
		e_file_open_flags flags = (e_file_open_flags)(_permission_read_bit | _permission_write_bit);
		opened_file = file_open(reference, flags, &error);
		if (opened_file)
		{
			const size_t eof_val = file_get_eof(reference);
			if (eof_val > max_size)
			{
				if (file_set_position(reference, eof_val - max_size, 0))
				{
					if (file_read(reference, max_size, 0, buffer))
					{
						if (file_set_position(reference, 0, 0))
						{
							if (file_write(reference, max_size, buffer))
							{
								file_set_eof(reference, max_size);
							}
						}
					}
				}
			}
		}
	}

	if (buffer)
	{
		CSERIES_FREE(buffer);
	}

	if (reference)
	{
		if (opened_file)
		{
			file_close(reference);
		}
	}
	return;
}
