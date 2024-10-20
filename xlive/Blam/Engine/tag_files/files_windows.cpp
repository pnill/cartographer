#include "stdafx.h"
#include "files_windows.h"

// TODO create an extended version of s_file_reference that supports MAX_PATH and wide strings

s_file_reference* __cdecl file_reference_create_from_path(s_file_reference* file_reference, const utf8* path, bool path_is_directory)
{
	return INVOKE(0x8C409, 0x86D37, file_reference_create_from_path, file_reference, path, path_is_directory);
}

void __cdecl file_create_parent_directories_if_not_present(s_file_reference* file_reference)
{
	INVOKE(0x8C531, 0x0, file_create_parent_directories_if_not_present, file_reference);
	return;
}

bool __cdecl file_open(s_file_reference* file_reference, e_file_open_flags flags, e_file_open_error* out_error_code)
{
	return INVOKE(0x638BF, 0x65BBF, file_open, file_reference, flags, out_error_code);
}

bool __cdecl file_close(s_file_reference* file_reference)
{
	return INVOKE(0x63B66, 0x65E66, file_close, file_reference);
}

bool __cdecl file_create(s_file_reference* file_reference)
{
	return INVOKE(0x632F1, 0x655F1, file_close, file_reference);
}

bool __cdecl file_delete(s_file_reference* file_reference)
{
	return INVOKE(0x63667, 0x65967, file_delete, file_reference);
}

bool __cdecl file_read(s_file_reference* file_reference, uint32 bytes_to_read, bool suppress_errors, void* data_buffer)
{
	return INVOKE(0x63C60, 0x65F3C, file_read, file_reference, bytes_to_read, suppress_errors, data_buffer);
}

bool __cdecl file_write(s_file_reference* file_reference, uint32 data_size, void* data)
{
	return INVOKE(0x63CBC, 0x65F98, file_write, file_reference, data_size, data);
}

bool __cdecl file_get_size(s_file_reference* file_reference, uint32* size)
{
	return INVOKE(0x63E10, 0x660EC, file_read_only, file_reference, size);
}

bool file_set_eof(s_file_reference* file_reference)
{
	if (file_reference->handle)
	{
		if (SetEndOfFile(file_reference->handle))
			return true;

		return false;
	}

	return false;
}

bool file_change_size(s_file_reference* file_reference, int32 new_size)
{
	if (file_reference->handle)
	{
		if (SetFilePointer(file_reference->handle, new_size, NULL, FILE_BEGIN) != INVALID_SET_FILE_POINTER)
		{
			return SetEndOfFile(file_reference->handle);
		}
	}
	return false;
}

bool __cdecl file_read_only(s_file_reference* file_reference, bool read_only)
{
	return INVOKE(0x6341D, 0x6571D, file_read_only, file_reference, read_only);
}

bool __cdecl file_set_hidden(s_file_reference* file_reference, bool hidden)
{
	return INVOKE(0x63545, 0x65845, file_set_hidden, file_reference, hidden);
}

bool compress_file_to_zip(zipFile zip_file, s_file_reference* file_to_add, const char* path_in_zip)
{
	bool result = true;

	if (zip_file)
	{
		e_file_open_flags flags = _permission_read_bit;
		e_file_open_error error;

		if (file_open(file_to_add, flags, &error))
		{
			uint32 file_size;
			if (file_get_size(file_to_add, &file_size))
			{
				// Add the file to the zip
				if (zipOpenNewFileInZip(zip_file, path_in_zip, NULL, NULL, 0, NULL, 0, NULL, Z_DEFLATED, Z_BEST_COMPRESSION) == Z_OK)
				{
					void* file_data = malloc(file_size);
					if (file_read(file_to_add, file_size, false, file_data))
					{
						// Write the data to the file in the zip
						if(zipWriteInFileInZip(zip_file, file_data, file_size) != Z_OK)
						{
							result = false;
						}
					}
					else
					{
						result = false;
					}

					free(file_data);
					// Close the zip
					if (zipCloseFileInZip(zip_file) != Z_OK)
					{
						result = false;
					}
				}
				else
				{
					result = false;
				}
			}
			else
			{
				result = false;
			}

			// Close the file reference to the zip
			if (!file_close(file_to_add))
			{
				result = false;
			}
		}
		else
		{
			result = false;
		}
	}
	else
	{
		result = false;
	}

	return result;
}