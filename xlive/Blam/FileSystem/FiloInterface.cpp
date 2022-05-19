#include "stdafx.h"

#include "FiloInterface.h"

namespace FiloInterface
{
	void filo_init(filo* filo_ptr, const std::string& path, bool path_is_directory)
	{
		auto p_init_filo = Memory::GetAddress<filo* (__cdecl*)(filo*, const char* path, char mode)>(0x8C409, 0x86D37);
		p_init_filo(filo_ptr, path.c_str(), path_is_directory);
	}

	bool open(filo* filo_ptr, __int16 mode, DWORD* error_code)
	{
		typedef bool(__cdecl* filo_open_t)(filo*, __int16 mode, DWORD* error_code);
		DWORD func_offset = Memory::GetAddress(0x638BF, 0x65BBF);
		auto p_filo_open = reinterpret_cast<filo_open_t>(func_offset);
		return p_filo_open(filo_ptr, mode, error_code);
	}

	bool close(filo* filo_ptr)
	{
		typedef bool(__cdecl* filo_close_t)(filo*);
		DWORD func_offset = Memory::GetAddress(0x63B66, 0x65E66);
		auto p_filo_close = reinterpret_cast<filo_close_t>(func_offset);
		return p_filo_close(filo_ptr);
	}

	bool create_file_or_directory(filo* filo_ptr)
	{
		typedef bool(__cdecl* filo_create_file_or_directory_t)(filo*);
		DWORD func_offset = Memory::GetAddress(0x632F1, 0x655F1);
		auto p_filo_create_file_or_directory = reinterpret_cast<filo_create_file_or_directory_t>(func_offset);
		return p_filo_create_file_or_directory(filo_ptr);
	}

	bool delete_existing(filo* filo_ptr)
	{
		typedef bool(__cdecl* filo_delete_t)(filo*);
		DWORD func_offset = Memory::GetAddress(0x63667, 0x65967);
		auto p_filo_delete = reinterpret_cast<filo_delete_t>(func_offset);

		return p_filo_delete(filo_ptr);
	}

	bool read(filo* filo_ptr, LPVOID data_buffer, DWORD nNumberOfBytesToRead, bool hide_errors_from_user)
	{
		typedef char(__cdecl* filo_read_t)(filo*, DWORD nNumberOfBytesToRead, char hide_errors_from_user, LPVOID lpBuffer);
		DWORD func_offset = Memory::GetAddress(0x63C60, 0x65F3C);
		auto p_filo_read = reinterpret_cast<filo_read_t>(func_offset);

		return p_filo_read(filo_ptr, nNumberOfBytesToRead, hide_errors_from_user, data_buffer);
	}

	bool write(filo* filo_ptr, LPVOID data, size_t data_size)
	{
		typedef bool(__cdecl* filo_write_t)(filo*, DWORD nNumberOfBytesToWrite, LPVOID lpBuffer);
		DWORD func_offset = Memory::GetAddress(0x63CBC, 0x65F98);
		auto p_filo_write = reinterpret_cast<filo_write_t>(func_offset);

		return p_filo_write(filo_ptr, data_size, data);
	}

	bool get_low_file_size(filo* filo_ptr, DWORD* out_low_size)
	{
		typedef bool(__cdecl* filo_get_low_file_size_t)(filo*, DWORD*);
		DWORD func_offset = Memory::GetAddress(0x63E10, 0x660EC);
		auto p_filo_get_low_file_size = reinterpret_cast<filo_get_low_file_size_t>(func_offset);

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
		typedef bool(__cdecl* filo_set_file_attribute_readonly_t)(filo* filo_ptr, bool read_only);

		DWORD func_offset = Memory::GetAddress(0x6341D, 0x6571D);
		auto p_filo_set_file_attribute_readonly = reinterpret_cast<filo_set_file_attribute_readonly_t>(func_offset);

		return p_filo_set_file_attribute_readonly(filo_ptr, read_only);
	}

	bool set_file_attribute_hidden(filo* filo_ptr, bool hidden)
	{
		typedef bool(__cdecl* filo_set_file_attribute_hidden_t)(filo* filo_ptr, bool hidden);

		DWORD func_offset = Memory::GetAddress(0x63545, 0x65845);
		auto p_filo_set_file_attribute_hidden = reinterpret_cast<filo_set_file_attribute_hidden_t>(func_offset);

		return p_filo_set_file_attribute_hidden(filo_ptr, hidden);
	}
}