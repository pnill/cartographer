#pragma once

#define FILE_REFERENCE_SIGNATURE "filo"

struct s_file_reference
{
	unsigned long			signature;
	unsigned short      	flags;
	signed short     		location;
	utf8                    path[256];
	HANDLE		            handle;
	HRESULT		            api_result;
};
static_assert(sizeof(s_file_reference) == 272, "Invalid 's_file_reference' struct size");

enum FILE_REFERENCE_PATH_FLAGS : __int16
{
	CONTAINING_FILENAME = FLAG(0), // base
	CONTAINING_DIRECTORY_NAME = FLAG(1), // base
	FILE_NAME = FLAG(2), // base
	FILE_EXTENSION = FLAG(3), // base
};

enum e_file_open_flags : DWORD
{
	_permission_read_bit = FLAG(0),
	_permission_write_bit = FLAG(1),
	_permission_write_append_bit = FLAG(2),
	_permission_write_allow_read_bit = FLAG(3),

	_file_open_file_attribute_temp = FLAG(5),
	_file_open_delete_on_close = FLAG(6),
	_file_open_random_access_bit = FLAG(7),
	_file_open_sequential_scan_bit = FLAG(8),
	k_number_of_file_open_flags = FLAG(9)		// originally called NUMBER_OF_FILE_OPEN_FLAGS but we change it to maintain "some" code consistency
};

enum e_file_open_error : DWORD
{
	_file_open_error_success = 0,
	_file_open_error_not_found = 1,
	_file_open_error_access_denied = 2,
	_file_open_error_path_not_found = 3,
	_file_open_error_invalid_drive = 4,
	_file_open_error_sharing_violation = 5,
	_file_open_error_unknown = 6,
};

s_file_reference* file_reference_create_from_path(s_file_reference* file_reference, const utf8* path, bool path_is_directory);

/* Returns success */
bool file_open(s_file_reference* file_reference, e_file_open_flags flags, e_file_open_error* out_error_code);

/* Returns success */
bool file_close(s_file_reference* file_reference);

/* Deletes the file or directory pointed to by the s_file_reference, returns success */
bool file_create(s_file_reference* file_reference);

/* Returns true if the path exists and we can access it */
bool file_delete(s_file_reference* file_reference);

/*
On success the data read is written to data_buffer and the function returns true
On failure, if hide_errors_from_user is set to false an error is displayed to the user and false is returned, if the number of bytes read doesn't match the requested amount ERROR_HANDLE_EOF is set
*/
bool file_read(s_file_reference* file_reference, size_t bytes_to_read, bool suppress_errors, LPVOID data_buffer);

/* Returns success */
bool file_write(s_file_reference* file_reference, size_t data_size, LPVOID data);

/* */
bool file_get_size(s_file_reference* file_reference, size_t* size);

/* */
bool file_set_eof(s_file_reference* file_reference);

/* Can be used to truncate or extend an open file, returns success */
bool file_change_size(s_file_reference* file_reference, LONG new_size);

/* */
bool file_read_only(s_file_reference* file_reference, bool read_only);

/* */
bool file_set_hidden(s_file_reference* file_reference, bool hidden);
