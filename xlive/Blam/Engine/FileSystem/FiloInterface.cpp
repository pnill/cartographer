#include "FiloInterface.h"

namespace FiloInterface
{
	void init_filo(filo *data, std::string path, bool mode)
	{
		auto filo__init_filo_impl = Memory::GetAddress<filo *(__cdecl*)(filo *data_struct, const char *path, char mode)>(0x632F1, 0x655F1);
		filo__init_filo_impl(data, path.c_str(), mode);
	}

	bool delete_existing(filo *data)
	{
		typedef bool(__cdecl *filo__delete)(filo *data_struct);
		DWORD func_offset = Memory::GetAddress(0x63667, 0x65967);
		auto filo__delete_impl = reinterpret_cast<filo__delete>(func_offset);

		return filo__delete_impl(data);
	}

	bool check_access(filo *data)
	{
		typedef bool(__cdecl *filo__check_access)(filo *a1);
		DWORD func_offset = Memory::GetAddress(0x632F1, 0x0);
		auto filo__check_access_impl = reinterpret_cast<filo__check_access>(func_offset);

		return filo__check_access_impl(data);
	}

	bool open(filo *data, __int16 mode, DWORD *error_code)
	{
		typedef bool(__cdecl *filo__open)(filo *filo, __int16 mode, DWORD *error_code);
		DWORD func_offset = Memory::GetAddress(0x638BF, 0x65BBF);
		auto filo__open_impl = reinterpret_cast<filo__open>(func_offset);

		return filo__open_impl(data, mode, error_code);
	}

	bool close(filo *data)
	{
		typedef bool(__cdecl *filo__close)(filo *a1);
		DWORD func_offset = Memory::GetAddress(0x63B66, 0x65E66);
		auto filo__close_impl = reinterpret_cast<filo__close>(func_offset);

		return filo__close_impl(data);
	}

	bool read(filo *filo_ptr, LPVOID data_buffer, DWORD nNumberOfBytesToRead, bool hide_errors_from_user)
	{
		typedef char(__cdecl *filo__read)(filo *filo_ptr, DWORD nNumberOfBytesToRead, char hide_errors_from_user, LPVOID lpBuffer);
		DWORD func_offset = Memory::GetAddress(0x63C60, 0x65F3C);
		auto filo__read_impl = reinterpret_cast<filo__read>(func_offset);

		return filo__read_impl(filo_ptr, nNumberOfBytesToRead, hide_errors_from_user, data_buffer);
	}

	bool write(filo *filo_ptr, LPVOID data, size_t data_size)
	{
		typedef bool(__cdecl *filo__write)(filo *a1, DWORD nNumberOfBytesToWrite, LPVOID lpBuffer);
		DWORD func_offset = Memory::GetAddress(0x63CBC, 0x65F98);
		auto filo__write_impl = reinterpret_cast<filo__write>(func_offset);

		return filo__write_impl(filo_ptr, data_size, data);
	}

	bool change_size(filo *filo_ptr, LONG new_size)
	{
		if (filo_ptr->handle)
		{
			if (SetFilePointer(filo_ptr->handle, new_size, NULL, FILE_BEGIN) != INVALID_SET_FILE_POINTER)
			{
				return SetEndOfFile(filo_ptr->handle);
			}
		}
		else {
			SetLastError(ERROR_INVALID_HANDLE);
		}
		return false;
	}
}