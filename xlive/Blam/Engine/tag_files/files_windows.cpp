#include "stdafx.h"

#include "files_windows.h"


s_file_reference* file_reference_create_from_path(s_file_reference* filo_ptr, const std::string& path, bool path_is_directory)
{
	typedef s_file_reference* (__cdecl* file_reference_create_from_path_t)(s_file_reference*, const char*, bool);
	auto p_init_filo = Memory::GetAddress<file_reference_create_from_path_t>(0x8C409, 0x86D37);
	return p_init_filo(filo_ptr, path.c_str(), path_is_directory);
}

bool file_open(s_file_reference* filo_ptr, __int16 mode, DWORD* out_error_code)
{
	typedef bool(__cdecl* filo_open_t)(s_file_reference*, __int16, DWORD*);
	auto p_filo_open = Memory::GetAddress<filo_open_t>(0x638BF, 0x65BBF);
	return p_filo_open(filo_ptr, mode, out_error_code);
}

bool file_close(s_file_reference* filo_ptr)
{
	typedef bool(__cdecl* filo_close_t)(s_file_reference*);
	auto p_filo_close = Memory::GetAddress<filo_close_t>(0x63B66, 0x65E66);
	return p_filo_close(filo_ptr);
}

bool file_create(s_file_reference* filo_ptr)
{
	typedef bool(__cdecl* file_create_t)(s_file_reference*);
	auto p_file_create = Memory::GetAddress<file_create_t>(0x632F1, 0x655F1);
	return p_file_create(filo_ptr);
}

bool file_delete(s_file_reference* filo_ptr)
{
	typedef bool(__cdecl* file_delete_t)(s_file_reference*);
	auto p_file_delete = Memory::GetAddress<file_delete_t>(0x63667, 0x65967);
	return p_file_delete(filo_ptr);
}

bool file_read(s_file_reference* filo_ptr, LPVOID data_buffer, DWORD nNumberOfBytesToRead, bool suppress_errors)
{
	typedef char(__cdecl* file_read_t)(s_file_reference*, DWORD, bool, LPVOID);
	auto p_file_read = Memory::GetAddress<file_read_t>(0x63C60, 0x65F3C);
	return p_file_read(filo_ptr, nNumberOfBytesToRead, suppress_errors, data_buffer);
}

bool file_write(s_file_reference* filo_ptr, LPVOID data, size_t data_size)
{
	typedef bool(__cdecl* file_write_t)(s_file_reference*, DWORD, LPVOID);
	auto p_file_write = Memory::GetAddress<file_write_t>(0x63CBC, 0x65F98);
	return p_file_write(filo_ptr, data_size, data);
}

bool file_get_size_low(s_file_reference* filo_ptr, DWORD* out_low_size)
{
	typedef bool(__cdecl* filo_get_low_file_size_t)(s_file_reference*, DWORD*);
	auto p_filo_get_low_file_size = Memory::GetAddress<filo_get_low_file_size_t>(0x63E10, 0x660EC);
	return p_filo_get_low_file_size(filo_ptr, out_low_size);
}

bool file_set_eof(s_file_reference* filo_ptr)
{
	if (filo_ptr->handle)
	{
		if (SetEndOfFile(filo_ptr->handle))
			return true;

		return false;
	}

	return false;
}

bool file_change_size(s_file_reference* filo_ptr, LONG new_size)
{
	if (filo_ptr->handle)
	{
		if (SetFilePointer(filo_ptr->handle, new_size, NULL, FILE_BEGIN) != INVALID_SET_FILE_POINTER)
		{
			return SetEndOfFile(filo_ptr->handle);
		}
	}
	return false;
}

bool file_read_only(s_file_reference* filo_ptr, bool read_only)
{
	typedef bool(__cdecl* filo_set_file_attribute_readonly_t)(s_file_reference*, bool);
	auto p_filo_set_file_attribute_readonly = Memory::GetAddress<filo_set_file_attribute_readonly_t>(0x6341D, 0x6571D);
	return p_filo_set_file_attribute_readonly(filo_ptr, read_only);
}

bool file_set_hidden(s_file_reference* filo_ptr, bool hidden)
{
	typedef bool(__cdecl* filo_set_file_attribute_hidden_t)(s_file_reference*, bool);
	auto p_filo_set_file_attribute_hidden = Memory::GetAddress<filo_set_file_attribute_hidden_t>(0x63545, 0x65845);
	return p_filo_set_file_attribute_hidden(filo_ptr, hidden);
}
