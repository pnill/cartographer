#include "stdafx.h"
#include "filesys.h"

#include <codecvt>

std::wstring GetExeDirectoryWide()
{
	wchar_t main_exe_dir[MAX_PATH];
	GetModuleFileNameW(NULL, main_exe_dir, sizeof(main_exe_dir));
	size_t path_len = wcsnlen_s(main_exe_dir, sizeof(main_exe_dir));
	if (main_exe_dir[path_len - 1] == L'\\')
		main_exe_dir[path_len - 1] = NULL;
	wchar_t *last_part = wcsrchr(main_exe_dir, L'\\');
	if (last_part)
		*last_part = NULL;
	return main_exe_dir;
}

static std::wstring_convert<std::codecvt_utf8<wchar_t>> wstring_to_string;
std::string GetExeDirectoryNarrow()
{
	return wstring_to_string.to_bytes(GetExeDirectoryWide());
}