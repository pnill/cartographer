#pragma once

#include "Blam\Common\Common.h"

struct filo
{
	unsigned long			signature;
	unsigned short      	flags;
	signed short     		location;
	char                    path[256];
	HANDLE		            handle;
	HRESULT		            api_result;
};
static_assert(sizeof(filo) == 0x110, "Invalid 'filo' struct size");

enum FILO_PATH_FLAGS : __int16
{
	CONTAINING_FILENAME = FLAG(0), // base
	CONTAINING_DIRECTORY_NAME = FLAG(1), // base
	FILE_NAME = FLAG(2), // base
	FILE_EXTENSION = FLAG(3), // base
};

enum FILO_FILE_OPEN_FLAGS : __int16
{
	FILO_READ_FILE = FLAG(0),
	FILO_WRITE_FILE = FLAG(1),
	FILO_FILE_POINTER_SET_AT_END = FLAG(1),
	FILO_SHARE_READ = FLAG(3),

	FILO_FILE_ATTRIBUTE_TEMP = FLAG(5),
	FILO_FILE_DELETE_ON_CLOSE = FLAG(6),
	FILO_FILE_RANDOM_ACCESS = FLAG(7),
	FILO_FILE_SEQUENTIAL_SCAN = FLAG(8),

	FILO_READ_WRITE = (FILO_READ_FILE | FILO_WRITE_FILE),
};

enum FILO_FILE_OPEN_ERROR : DWORD
{
	FILO_OPEN_FILE_ERROR_SUCCESS = 0,
	FILO_OPEN_FILE_ERROR_NOT_FOUND = 1,
	FILO_OPEN_FILE_ERROR_ACCESS_DENIED = 2,
	FILO_OPEN_FILE_ERROR_PATH_NOT_FOUND = 3,
	FILO_OPEN_FILE_ERROR_INVALID_DRIVE = 4,
	FILO_OPEN_FILE_ERROR_SHARING_VIOLATION = 5,
	FILO_OPEN_FILE_ERROR_UNKONWN = 6,
};

namespace FiloInterface
{
	void filo_init(filo* filo_ptr, const std::string& path, bool path_is_directory);

	bool create_file(filo* filo_ptr);

	/* Returns success */
	bool open(filo* filo_ptr, __int16 mode, DWORD* error_code);

	/* Returns success */
	bool close(filo* filo_ptr);

	/* Deletes the file or directory pointed to by the filo, returns success */
	bool delete_existing(filo* filo_ptr);

	/* Returns true if the path exists and we can access it */
	bool create_file_or_directory(filo* filo_ptr);

	/*
	On success the data read is written to data_buffer and the function returns true
	On failure, if hide_errors_from_user is set to false an error is displayed to the user and false is returned, if the number of bytes read doesn't match the requested amount ERROR_HANDLE_EOF is set
	*/
	bool read(filo* filo_ptr, LPVOID data_buffer, DWORD nNumberOfBytesToRead, bool hide_errors_from_user);

	/* Returns success */
	bool write(filo* filo_ptr, LPVOID data, size_t data_size);

	/* */
	bool get_low_file_size(filo* filo_ptr, DWORD* out_low_size);

	/* */
	bool set_end_of_file(filo* filo_ptr);

	/* Can be used to truncate or extend an open file, returns success */
	bool change_size(filo* filo_ptr, LONG new_size);

	/* */
	bool set_file_attribute_readonly(filo* filo_ptr, bool read_only);

	/* */
	bool set_file_attribute_hidden(filo* filo_ptr, bool hidden);
};
