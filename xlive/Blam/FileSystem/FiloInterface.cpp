#include "stdafx.h"

#include "FiloInterface.h"

namespace FiloInterface
{
	filo* filo_init(filo* filo_ptr, const std::string& path, bool path_is_directory)
	{
		typedef filo* (__cdecl* filo_init)(filo*, const char*, bool);
		auto p_init_filo = Memory::GetAddress<filo_init>(0x8C409, 0x86D37);
		return p_init_filo(filo_ptr, path.c_str(), path_is_directory);
	}

	bool open(filo* filo_ptr, __int16 mode, DWORD* out_error_code)
	{
		typedef bool(__cdecl* filo_open_t)(filo*, __int16, DWORD*);
		auto p_filo_open = Memory::GetAddress<filo_open_t>(0x638BF, 0x65BBF);
		return p_filo_open(filo_ptr, mode, out_error_code);
	}

	bool close(filo* filo_ptr)
	{
		typedef bool(__cdecl* filo_close_t)(filo*);
		auto p_filo_close = Memory::GetAddress<filo_close_t>(0x63B66, 0x65E66);
		return p_filo_close(filo_ptr);
	}

	bool create_file_or_directory(filo* filo_ptr)
	{
		typedef bool(__cdecl* filo_create_file_or_directory_t)(filo*);
		auto p_filo_create_file_or_directory = Memory::GetAddress<filo_create_file_or_directory_t>(0x632F1, 0x655F1);
		return p_filo_create_file_or_directory(filo_ptr);
	}

	bool delete_existing(filo* filo_ptr)
	{
		typedef bool(__cdecl* filo_delete_t)(filo*);
		auto p_filo_delete = Memory::GetAddress<filo_delete_t>(0x63667, 0x65967);
		return p_filo_delete(filo_ptr);
	}

	bool read(filo* filo_ptr, LPVOID data_buffer, DWORD nNumberOfBytesToRead, bool suppress_errors)
	{
		typedef char(__cdecl* filo_read_t)(filo*, DWORD, bool, LPVOID);
		auto p_filo_read = Memory::GetAddress<filo_read_t>(0x63C60, 0x65F3C);
		return p_filo_read(filo_ptr, nNumberOfBytesToRead, suppress_errors, data_buffer);
	}

	bool write(filo* filo_ptr, LPVOID data, size_t data_size)
	{
		typedef bool(__cdecl* filo_write_t)(filo*, DWORD, LPVOID);
		auto p_filo_write = Memory::GetAddress<filo_write_t>(0x63CBC, 0x65F98);
		return p_filo_write(filo_ptr, data_size, data);
	}

	bool get_low_file_size(filo* filo_ptr, DWORD* out_low_size)
	{
		typedef bool(__cdecl* filo_get_low_file_size_t)(filo*, DWORD*);
		auto p_filo_get_low_file_size = Memory::GetAddress<filo_get_low_file_size_t>(0x63E10, 0x660EC);
		return p_filo_get_low_file_size(filo_ptr, out_low_size);
	}

	bool set_end_of_file(filo* filo_ptr)
	{
		if (filo_ptr->handle)
		{
			if (SetEndOfFile(filo_ptr->handle))
				return true;

			return false;
		}

		return false;
	}

	bool change_size(filo* filo_ptr, LONG new_size)
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

	bool set_file_attribute_readonly(filo* filo_ptr, bool read_only)
	{
		typedef bool(__cdecl* filo_set_file_attribute_readonly_t)(filo*, bool);
		auto p_filo_set_file_attribute_readonly = Memory::GetAddress<filo_set_file_attribute_readonly_t>(0x6341D, 0x6571D);
		return p_filo_set_file_attribute_readonly(filo_ptr, read_only);
	}

	bool set_file_attribute_hidden(filo* filo_ptr, bool hidden)
	{
		typedef bool(__cdecl* filo_set_file_attribute_hidden_t)(filo*, bool);
		auto p_filo_set_file_attribute_hidden = Memory::GetAddress<filo_set_file_attribute_hidden_t>(0x63545, 0x65845);
		return p_filo_set_file_attribute_hidden(filo_ptr, hidden);
	}
}