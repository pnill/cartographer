#pragma once

#include "Blam/Common/Common.h"

struct s_file_reference
{
	unsigned long			signature;
	unsigned short      	flags;
	signed short     		location;
	char                    path[256];
	HANDLE		            handle;
	HRESULT		            api_result;
};
static_assert(sizeof(s_file_reference) == 0x110, "Invalid 's_file_reference' struct size");

enum FILE_REFERENCE_PATH_FLAGS : __int16
{
	CONTAINING_FILENAME = FLAG(0), // base
	CONTAINING_DIRECTORY_NAME = FLAG(1), // base
	FILE_NAME = FLAG(2), // base
	FILE_EXTENSION = FLAG(3), // base
};

enum FILE_REFERENCE_FILE_OPEN_FLAGS : __int16
{
	FILE_REFERENCE_READ_FILE = FLAG(0),
	FILE_REFERENCE_WRITE_FILE = FLAG(1),
	FILE_REFERENCE_FILE_POINTER_SET_AT_END = FLAG(2),
	FILE_REFERENCE_SHARE_READ = FLAG(3),

	FILE_REFERENCE_FILE_ATTRIBUTE_TEMP = FLAG(5),
	FILE_REFERENCE_FILE_DELETE_ON_CLOSE = FLAG(6),
	FILE_REFERENCE_FILE_RANDOM_ACCESS = FLAG(7),
	FILE_REFERENCE_FILE_SEQUENTIAL_SCAN = FLAG(8),

	FILE_REFERENCE_READ_WRITE = (FILE_REFERENCE_READ_FILE | FILE_REFERENCE_WRITE_FILE),
};

enum FILE_REFERENCE_FILE_OPEN_ERROR : DWORD
{
	FILE_REFERENCE_OPEN_FILE_ERROR_SUCCESS = 0,
	FILE_REFERENCE_OPEN_FILE_ERROR_NOT_FOUND = 1,
	FILE_REFERENCE_OPEN_FILE_ERROR_ACCESS_DENIED = 2,
	FILE_REFERENCE_OPEN_FILE_ERROR_PATH_NOT_FOUND = 3,
	FILE_REFERENCE_OPEN_FILE_ERROR_INVALID_DRIVE = 4,
	FILE_REFERENCE_OPEN_FILE_ERROR_SHARING_VIOLATION = 5,
	FILE_REFERENCE_OPEN_FILE_ERROR_UNKONWN = 6,
};


s_file_reference* file_reference_create_from_path(s_file_reference* filo_ptr, const std::string& path, bool path_is_directory);

/* Returns success */
bool file_open(s_file_reference* filo_ptr, __int16 mode, DWORD* out_error_code);

/* Returns success */
bool file_close(s_file_reference* filo_ptr);

/* Deletes the file or directory pointed to by the s_file_reference, returns success */
bool file_delete(s_file_reference* filo_ptr);

/* Returns true if the path exists and we can access it */
bool file_create(s_file_reference* filo_ptr);

/*
On success the data read is written to data_buffer and the function returns true
On failure, if hide_errors_from_user is set to false an error is displayed to the user and false is returned, if the number of bytes read doesn't match the requested amount ERROR_HANDLE_EOF is set
*/
bool file_read(s_file_reference* filo_ptr, LPVOID data_buffer, DWORD nNumberOfBytesToRead, bool suprress_errors);

/* Returns success */
bool file_write(s_file_reference* filo_ptr, LPVOID data, size_t data_size);

/* */
bool file_get_size_low(s_file_reference* filo_ptr, DWORD* out_low_size);

/* */
bool file_set_eof(s_file_reference* filo_ptr);

/* Can be used to truncate or extend an open file, returns success */
bool file_change_size(s_file_reference* filo_ptr, LONG new_size);

/* */
bool file_read_only(s_file_reference* filo_ptr, bool read_only);

/* */
bool file_set_hidden(s_file_reference* filo_ptr, bool hidden);

