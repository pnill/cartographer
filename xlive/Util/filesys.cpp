#include "stdafx.h"
#include "filesys.h"

static wchar_t exe_directory[MAX_PATH];
static char exe_directory_narrow[MAX_PATH];

void set_exe_directory()
{
	GetModuleFileNameW(NULL, exe_directory, MAX_PATH);
	
	// Remove the final (\\) from exe_directory
	wchar_t* last_part = wcsrchr(exe_directory, L'\\');
	if (last_part)
	{
		*last_part = L'\0';
	}

	// Conver the path to a narrow filepath
	WideCharToMultiByte(CP_ACP, 0, get_exe_directory(), -1, exe_directory_narrow, MAX_PATH, NULL, NULL);
}

wchar_t* get_exe_directory()
{
	return exe_directory;
}

char* get_exe_directory_narrow()
{
	return exe_directory_narrow;
}