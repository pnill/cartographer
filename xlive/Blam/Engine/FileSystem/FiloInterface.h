#pragma once

struct filo
{
	unsigned long			signature;
	unsigned short      	flags;
	signed short     		location;
	char                    path[256];
	HANDLE		            handle;
	HRESULT		            api_result;
};
static_assert(sizeof(filo) == 0x110, "Invalid 'filo' struct length.");

enum PATH_FLAGS : BYTE {
	CONTAINING_DIRECTORY_PATH = 1, // base
	CONTAINING_DIRECTORY_NAME = 2, // base
	FILE_NAME = 4, // base
	FILE_EXTENSION = 8, // base

	FULL_PATH = (FILE_NAME | CONTAINING_DIRECTORY_PATH | FILE_EXTENSION),
};

inline PATH_FLAGS operator|(PATH_FLAGS a, PATH_FLAGS b)
{
	return static_cast<PATH_FLAGS>(static_cast<int>(a) | static_cast<int>(b));
}

namespace FiloInterface
{
	void init_filo(filo *data, std::string path, bool mode);

	/* Deletes the file or directory pointed to by the filo, returns success */
	bool delete_existing(filo *data);

	/* Returns true if the path exists and we can access it */
	bool check_access(filo *data);

	/* Returns success */
	bool open(filo *data, __int16 mode, DWORD *error_code);

	/* Returns success */
	bool close(filo *data);

	/*
	On success the data read is written to data_buffer and the function returns true
	On failure, if hide_errors_from_user is set to false an error is displayed to the user and false is returned, if the number of bytes read doesn't match the requested amount ERROR_HANDLE_EOF is set
	*/
	bool read(filo *filo_ptr, LPVOID data_buffer, DWORD nNumberOfBytesToRead, bool hide_errors_from_user);

	/* Returns success */
	bool write(filo *filo_ptr, LPVOID data, size_t data_size);

	/* Can be used to truncate or extend an open file , returns success */
	bool change_size(filo *filo_ptr, LONG new_size);
};
