#include "stdafx.h"

#include "FiloInterface.h"

namespace FiloInterface
{
	void filo_init(filo* filo_ptr, const std::string& path, bool path_is_directory)
	{
		auto filo__init_filo_impl = Memory::GetAddress<filo* (__cdecl*)(filo*, const char* path, char mode)>(0x8C409, 0x0); // TODO DEDI OFFSET
		filo__init_filo_impl(filo_ptr, path.c_str(), path_is_directory);
	}

	bool create_file(filo* filo_ptr)
	{
		typedef bool(__cdecl* filo__create_file)(filo*);

		DWORD func_offset = Memory::GetAddress(0x632F1, 0x0); // TODO DEDI OFFSET
		auto filo__create_file_impl = reinterpret_cast<filo__create_file>(func_offset);

		return filo__create_file_impl(filo_ptr);
	}

	bool open(filo* filo_ptr, __int16 mode, DWORD* error_code)
	{
		typedef bool(__cdecl* filo__open)(filo*, __int16 mode, DWORD* error_code);
		DWORD func_offset = Memory::GetAddress(0x638BF, 0x65BBF);
		auto filo__open_impl = reinterpret_cast<filo__open>(func_offset);

		return filo__open_impl(filo_ptr, mode, error_code);
	}

	bool close(filo* filo_ptr)
	{
		typedef bool(__cdecl* filo__close)(filo*);
		DWORD func_offset = Memory::GetAddress(0x63B66, 0x65E66);
		auto filo__close_impl = reinterpret_cast<filo__close>(func_offset);

		return filo__close_impl(filo_ptr);
	}

	bool create_file_or_directory(filo* filo_ptr)
	{
		typedef bool(__cdecl* filo__check_access)(filo*);
		DWORD func_offset = Memory::GetAddress(0x632F1, 0x0); // TODO DEDI OFFSET
		auto filo__create_file_or_directory_impl = reinterpret_cast<filo__check_access>(func_offset);

		return filo__create_file_or_directory_impl(filo_ptr);
	}

	bool delete_existing(filo* filo_ptr)
	{
		typedef bool(__cdecl* filo__delete)(filo*);
		DWORD func_offset = Memory::GetAddress(0x63667, 0x65967);
		auto filo__delete_impl = reinterpret_cast<filo__delete>(func_offset);

		return filo__delete_impl(filo_ptr);
	}

	bool read(filo* filo_ptr, LPVOID data_buffer, DWORD nNumberOfBytesToRead, bool hide_errors_from_user)
	{
		typedef char(__cdecl* filo__read)(filo*, DWORD nNumberOfBytesToRead, char hide_errors_from_user, LPVOID lpBuffer);
		DWORD func_offset = Memory::GetAddress(0x63C60, 0x65F3C);
		auto filo__read_impl = reinterpret_cast<filo__read>(func_offset);

		return filo__read_impl(filo_ptr, nNumberOfBytesToRead, hide_errors_from_user, data_buffer);
	}

	bool write(filo* filo_ptr, LPVOID data, size_t data_size)
	{
		typedef bool(__cdecl* filo__write)(filo*, DWORD nNumberOfBytesToWrite, LPVOID lpBuffer);
		DWORD func_offset = Memory::GetAddress(0x63CBC, 0x65F98);
		auto filo__write_impl = reinterpret_cast<filo__write>(func_offset);

		return filo__write_impl(filo_ptr, data_size, data);
	}

	bool get_low_file_size(filo* filo_ptr, DWORD* out_low_size)
	{
		typedef bool(__cdecl* filo__get_low_file_size)(filo*, DWORD*);
		DWORD func_offset = Memory::GetAddress(0x63E10, 0x0); // TODO DEDI OFFSET
		auto filo__get_low_file_size_impl = reinterpret_cast<filo__get_low_file_size>(func_offset);

		return filo__get_low_file_size_impl(filo_ptr, out_low_size);
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
		typedef bool(__cdecl* filo__set_file_attribute_readonly)(filo* filo_ptr, bool read_only);

		DWORD func_offset = Memory::GetAddress(0x6341D, 0x0); // TODO DEDI OFFSET
		auto filo__set_file_attribute_readonly_impl = reinterpret_cast<filo__set_file_attribute_readonly>(func_offset);

		return filo__set_file_attribute_readonly_impl(filo_ptr, read_only);
	}

	bool set_file_attribute_hidden(filo* filo_ptr, bool hidden)
	{
		typedef bool(__cdecl* filo__set_file_attribute_hidden)(filo* filo_ptr, bool hidden);

		DWORD func_offset = Memory::GetAddress(0x63545, 0x0); // TODO DEDI OFFSET
		auto filo__set_file_attribute_hidden_impl = reinterpret_cast<filo__set_file_attribute_hidden>(func_offset);

		return filo__set_file_attribute_hidden_impl(filo_ptr, hidden);
	}

}