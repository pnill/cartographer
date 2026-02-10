#include "stdafx.h"
#include "files_windows.h"

#include "files.h"

#ifdef PROFILE_ENABLED
#include "cseries/profile.h"
#endif

/* constants */

enum
{
	DRIVE_NAME_LENGTH = 4,
};

/* enums */

enum e_file_timer_type
{
	_file_timer_type_file_create = 0,
	_file_timer_type_file_set_writeable,
	_file_timer_type_file_set_hidden,
	_file_timer_type_file_delete,
	_file_timer_type_file_exists,
	_file_timer_type_file_move_to,
	_file_timer_type_file_copy_to,
	_file_timer_type_file_open,
	_file_timer_type_file_close,
	_file_timer_type_file_set_position,
	_file_timer_type_file_get_eof,
	_file_timer_type_file_set_eof,
	_file_timer_type_file_read,
	_file_timer_type_unknown,			// This enum value is never used
	_file_timer_type_file_write,
	_file_timer_type_file_get_last_modification_date,
	_file_timer_type_file_get_size,
	_file_timer_type_file_read_only,
	k_number_of_file_timer_types,
};

/* classes */

#ifdef PROFILE_ENABLED
class c_file_timer
{
public:
	c_file_timer(e_file_timer_type type);
	~c_file_timer(void);

private:
	e_file_timer_type m_type;
	c_stop_watch m_stop_watch;
};
#endif

/* prototypes */

#ifdef ERRORS_ENABLED
static void file_error(const char* function_name, const s_file_reference* file);
#endif

static bool __cdecl file_write_encrypted_hook(s_file_reference* file_ptr, DWORD nNumberOfBytesToWrite, LPVOID lpBuffer);

/* globals */

#ifdef PROFILE_ENABLED
static int64 g_file_timers[18];
#endif

/* public code */

#ifdef PROFILE_ENABLED

c_file_timer::c_file_timer(e_file_timer_type type) :m_type(type), m_stop_watch() {};

c_file_timer::~c_file_timer(void)
{
	const int64 current_time = m_stop_watch.stop();
	g_file_timers[m_type] += current_time;
	return;
}

#endif

void files_windows_apply_patches(void)
{
	PatchCall(Memory::GetAddress(0x9B09F, 0x85F73), file_write_encrypted_hook);
	return;
}

s_file_reference* __cdecl file_reference_create_from_path(s_file_reference* file_reference, const char* path, bool path_is_directory)
{
	return INVOKE(0x8C409, 0x86D37, file_reference_create_from_path, file_reference, path, path_is_directory);
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

bool file_exists(const s_file_reference* file)
{
#ifdef PROFILE_ENABLED
	c_file_timer timer(_file_timer_type_file_exists);
#endif

	bool result = false;
	const s_file_reference_info* info = file_reference_get_info((s_file_reference*)file);

	char full_path[k_maximum_filename_length] = {};
	wchar_t wide_full_path[k_maximum_filename_length] = {};

	file_location_get_full_path(info->location, info->path, full_path);

	if (!MultiByteToWideChar(CP_UTF8, 0, full_path, -1, wide_full_path, NUMBEROF(wide_full_path)))
	{
		ASSERT(false);
	}

	if (GetFileAttributes(wide_full_path) == -1)
	{
#ifdef ERRORS_ENABLED
		if (GetLastError() != ERROR_FILE_NOT_FOUND && GetLastError() != ERROR_PATH_NOT_FOUND)
		{
			file_error(__FUNCTION__, file);
		}
#endif
	}
	else
	{
		result = true;
	}

	return result;
}

bool file_set_position(s_file_reference* file, size_t position, bool silent)
{
#ifdef PROFILE_ENABLED
	c_file_timer timer(_file_timer_type_file_set_position);
#endif

	bool result = false;
	s_file_reference_info* info = file_reference_get_info(file);

	// If the position is equal to the current position then we don't need to do anything
	if ((size_t)info->api_result == position)	
	{
		result = true;
	}
	else
	{
		const DWORD set_result = SetFilePointer(info->file_handle, position, 0, FILE_BEGIN);
		result = set_result != INVALID_SET_FILE_POINTER;

		info->api_result = set_result;
		if (set_result == INVALID_SET_FILE_POINTER && silent == result)
		{
#ifdef ERRORS_ENABLED
			file_error(__FUNCTION__, file);
#endif
		}
	}
	return result;
}

uint32 file_get_eof(const s_file_reference* file)
{
#ifdef PROFILE_ENABLED
	c_file_timer timer(_file_timer_type_file_get_eof);
#endif

	const s_file_reference_info* info = file_reference_get_info((s_file_reference*)file);
	const uint32 size = GetFileSize(info->file_handle, NULL);
	if (size == INVALID_FILE_SIZE)
	{
#ifdef ERRORS_ENABLED
		file_error(__FUNCTION__, file);
#endif
	}

	return size;
}

bool file_set_eof(s_file_reference* file, size_t position)
{
#ifdef PROFILE_ENABLED
	c_file_timer timer(_file_timer_type_file_set_eof);
#endif

	bool result = false;
	const s_file_reference_info* info = file_reference_get_info((s_file_reference*)file);

	if (file_set_position(file, position, 0) && SetEndOfFile(info->file_handle))
	{
		result = true;
	}
	else
	{
#ifdef ERRORS_ENABLED
		file_error(__FUNCTION__, file);
#endif
	}

	return result;
}


bool __cdecl file_read(s_file_reference* file_reference, uint32 bytes_to_read, bool suppress_errors, void* data_buffer)
{
	return INVOKE(0x63C60, 0x65F3C, file_read, file_reference, bytes_to_read, suppress_errors, data_buffer);
}

bool __cdecl file_write(s_file_reference* file_reference, uint32 data_size, const void* data)
{
	return INVOKE(0x63CBC, 0x65F98, file_write, file_reference, data_size, data);
}

bool __cdecl file_get_size(s_file_reference* file_reference, uint32* size)
{
	return INVOKE(0x63E10, 0x660EC, file_get_size, file_reference, size);
}


bool file_change_size(s_file_reference* file_reference, int32 new_size)
{
	if (file_reference->file_handle)
	{
		if (SetFilePointer(file_reference->file_handle, new_size, NULL, FILE_BEGIN) != INVALID_SET_FILE_POINTER)
		{
			return SetEndOfFile(file_reference->file_handle);
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

void file_location_get_full_path(int16 location, char const* path, char(&full_path)[k_maximum_filename_length])
{
	ASSERT(path && full_path);
	full_path[0] = '\0';
	if (location <= 0)
	{
		csstrncpy(full_path, path, NUMBEROF(full_path));
	}
	else
	{
		ASSERT(DRIVE_NAME_LENGTH + cstrlen(path) < k_maximum_filename_length);
		csstrncpy(full_path, "\\:?", NUMBEROF(full_path));
		csstrncat(full_path, path, NUMBEROF(full_path));
	}
	return;
}

void file_path_add_name(
	char* out_path,
	char const* name)
{
	if (name[0])
	{
		size_t dwPathLen = cstrlen(out_path);
		ASSERT(dwPathLen+1+strlen(name)<k_maximum_filename_length);
		
		char* out_path_insertion_point = &out_path[dwPathLen];
		if (out_path_insertion_point!=out_path &&
			*(out_path_insertion_point-1)!='\\')
		{
			*out_path_insertion_point++ = '\\';
			*out_path_insertion_point = '\0';
			++dwPathLen;
		}
		strncpy_s(
			out_path_insertion_point,
			k_maximum_filename_length-dwPathLen,
			name,
			k_maximum_filename_length-dwPathLen
		);
		out_path[k_maximum_filename_length-1] = '\0';
	}

	return;
}

void file_path_add_extension(
	char* out_path,
	char const* extension)
{
	if (extension[0])
	{
		size_t dwPathLen = cstrlen(out_path);
		ASSERT(dwPathLen+1+strlen(extension)<k_maximum_filename_length);

		char* out_path_insertion_point = &out_path[dwPathLen];
		if (*out_path_insertion_point!='.')
		{
			*out_path_insertion_point++ = '.';
			*out_path_insertion_point = '\0';
			++dwPathLen;
		}
		strncpy_s(
			out_path_insertion_point,
			k_maximum_filename_length-dwPathLen,
			extension,
			k_maximum_filename_length-dwPathLen
		);
		out_path[k_maximum_filename_length - 1] = '\0';
	}

	return;
}

void file_path_split(
	char* path,
	char** directory,
	char** parent_directory,
	char** filename,
	char** extension,
	bool has_filename)
{
	int16 length = (int16)cstrlen(path);
	*directory = &path[length];
	*parent_directory = &path[length];
	*filename = &path[length];
	*extension = &path[length];
	
	while (length>0)
	{
		if (path[length]=='.')
		{
			if (has_filename && !**filename && !**extension)
			{
				path[length] = '\0';
				*extension = &path[length+1];
			}
		}
		else if (path[length]=='\\')
		{
			if (!has_filename || **filename)
			{
				if (!**parent_directory)
				{
					*parent_directory = &path[length+1];
				}
			}
			else
			{
				path[length] = '\0';
				*filename = &path[length+1];
			}
		}
		--length;
	}

	if (has_filename && !**filename)
	{
		*filename = path;
	}
	
	if (*filename != path)
	{
		*directory = path;
	}

	return;
}

/* private code */

#ifdef ERRORS_ENABLED
static void file_error(const char* function_name, const s_file_reference* file)
{
	const s_file_reference_info* validated_file = file_reference_get_info((s_file_reference*)file);
	
	const DWORD error_val = GetLastError();
	char message[2048];
	
	FormatMessageA(
		FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS | FORMAT_MESSAGE_MAX_WIDTH_MASK,
		0,
		error_val,
		0,
		message,
		NUMBEROF(message),
		0
	);

	error(_error_delayed, "%s('%s') error 0x%08x '%s'", function_name, validated_file->path, error_val, message);
	SetLastError(0);	// Reset error back to zero
	return;
}
#endif

static bool __cdecl file_write_encrypted_hook(s_file_reference* file_ptr, DWORD nNumberOfBytesToWrite, LPVOID lpBuffer)
{
	DWORD file_size = GetFileSize(file_ptr->file_handle, NULL);

	if (file_size > nNumberOfBytesToWrite) // clear the file as unencrypted data is shorter then encrypted data.
		file_change_size(file_ptr, 0);
	return file_write(file_ptr, nNumberOfBytesToWrite, lpBuffer);
}


