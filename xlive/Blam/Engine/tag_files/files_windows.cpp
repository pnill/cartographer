#include "stdafx.h"
#include "files_windows.h"

// TODO create an extended version of s_file_reference that supports MAX_PATH and wide strings

s_file_reference* file_reference_create_from_path(s_file_reference* file_reference, const utf8* path, bool path_is_directory)
{
	typedef s_file_reference* (__cdecl* file_reference_create_from_path_t)(s_file_reference*, const utf8*, bool);
	auto p_file_reference_create_from_path = Memory::GetAddress<file_reference_create_from_path_t>(0x8C409, 0x86D37);
	return p_file_reference_create_from_path(file_reference, path, path_is_directory);
}

bool file_open(s_file_reference* file_reference, e_file_open_flags flags, e_file_open_error* out_error_code)
{
	typedef bool(__cdecl* file_open_t)(s_file_reference*, e_file_open_flags, e_file_open_error*);
	auto p_file_open = Memory::GetAddress<file_open_t>(0x638BF, 0x65BBF);
	return p_file_open(file_reference, flags, out_error_code);
}

bool file_close(s_file_reference* file_reference)
{
	typedef bool(__cdecl* file_close_t)(s_file_reference*);
	auto p_file_close = Memory::GetAddress<file_close_t>(0x63B66, 0x65E66);
	return p_file_close(file_reference);
}

bool file_create(s_file_reference* file_reference)
{
	typedef bool(__cdecl* file_create_t)(s_file_reference*);
	auto p_file_create = Memory::GetAddress<file_create_t>(0x632F1, 0x655F1);
	return p_file_create(file_reference);
}

bool file_delete(s_file_reference* file_reference)
{
	typedef bool(__cdecl* file_delete_t)(s_file_reference*);
	auto p_file_delete = Memory::GetAddress<file_delete_t>(0x63667, 0x65967);
	return p_file_delete(file_reference);
}

bool file_read(s_file_reference* file_reference, size_t bytes_to_read, bool suppress_errors, LPVOID data_buffer)
{
	typedef char(__cdecl* file_read_t)(s_file_reference*, size_t, bool, LPVOID);
	auto p_file_read = Memory::GetAddress<file_read_t>(0x63C60, 0x65F3C);
	return p_file_read(file_reference, bytes_to_read, suppress_errors, data_buffer);
}

bool file_write(s_file_reference* file_reference, size_t data_size, LPVOID data)
{
	typedef bool(__cdecl* file_write_t)(s_file_reference*, DWORD, LPVOID);
	auto p_file_write = Memory::GetAddress<file_write_t>(0x63CBC, 0x65F98);
	return p_file_write(file_reference, data_size, data);
}

bool file_get_size(s_file_reference* file_reference, size_t* size)
{
	typedef bool(__cdecl* file_get_size_low_t)(s_file_reference*, size_t*);
	auto p_file_get_size_low = Memory::GetAddress<file_get_size_low_t>(0x63E10, 0x660EC);
	return p_file_get_size_low(file_reference, size);
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

bool file_change_size(s_file_reference* file_reference, LONG new_size)
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

bool file_read_only(s_file_reference* file_reference, bool read_only)
{
	typedef bool(__cdecl* file_read_only_t)(s_file_reference*, bool);
	auto p_file_read_only = Memory::GetAddress<file_read_only_t>(0x6341D, 0x6571D);
	return p_file_read_only(file_reference, read_only);
}

bool file_set_hidden(s_file_reference* file_reference, bool hidden)
{
	typedef bool(__cdecl* file_set_hidden_t)(s_file_reference*, bool);
	auto p_file_set_hidden = Memory::GetAddress<file_set_hidden_t>(0x63545, 0x65845);
	return p_file_set_hidden(file_reference, hidden);
}
