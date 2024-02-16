#include "stdafx.h"
#include "filesys.h"

#include "text/unicode.h"

// ### TODO Remove

std::wstring GetExeDirectoryWide()
{
	wchar_t main_exe_dir[MAX_PATH];
	GetModuleFileNameW(NULL, main_exe_dir, sizeof(main_exe_dir));
	size_t path_len = wcsnlen_s(main_exe_dir, sizeof(main_exe_dir));
	if (path_len > 0)
	{
		if (main_exe_dir[path_len - 1] == L'\\')
			main_exe_dir[path_len - 1] = NULL;
		wchar_t *last_part = wcsrchr(main_exe_dir, L'\\');
		if (last_part)
			*last_part = NULL;
		return main_exe_dir;
	}
	else
	{
		return L"";
	}
}

std::string GetExeDirectoryNarrow()
{
	utf8 string[MAX_PATH * 2];
	wchar_string_to_utf8_string(GetExeDirectoryWide().c_str(), string, MAX_PATH * 2);
	return std::string(string);
}