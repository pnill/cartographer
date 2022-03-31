#include "stdafx.h"

#include "Memory.h"

DWORD Memory::baseAddress;
bool Memory::dedicatedServer;

ProcessInfo game_info;

H2Type DetectProcessType()
{
	// try and detect type based on module name.
	if (GetModuleHandleW(L"halo2.exe"))
		return H2Type::H2Game;
	else if (GetModuleHandleW(L"h2server.exe"))
		return H2Type::H2Server;

	// fallback to checking file information in case the file was renamed.
	wchar_t exe_file_path[_MAX_PATH + 1];
	int result = GetModuleFileNameW(NULL, exe_file_path, ARRAYSIZE(exe_file_path));
	if (result <= _MAX_PATH && GetLastError() != ERROR_INSUFFICIENT_BUFFER) {
		DWORD version_info_size = GetFileVersionInfoSizeW(exe_file_path, NULL);
		if (version_info_size != 0) {

			BYTE* version_info = new BYTE[version_info_size];
			if (GetFileVersionInfoW(exe_file_path, NULL, version_info_size, version_info)) {
				wchar_t* orginal_filename;
				size_t filename_len;
				// shouldn't be hardcoded but who cares
				VerQueryValueW(version_info, L"\\StringFileInfo\\040904b0\\OriginalFilename", (LPVOID*)&orginal_filename, &filename_len);

				std::wstring exe_orginal_filename = orginal_filename;
				delete[] version_info;

				if (exe_orginal_filename == L"halo2.exe")
					return H2Type::H2Game;
				else if (exe_orginal_filename == L"h2server.exe")
					return H2Type::H2Server;
			}
		}
	}
	return H2Type::Invalid;
}

// TODO maybe this is not the best place to keep this
void Memory::Initialize()
{
	game_info.base = GetModuleHandle(NULL);
	game_info.process_type = DetectProcessType();

	switch (game_info.process_type)
	{
	case H2Type::Invalid:
		MessageBoxA(NULL, "Project Cartographer loaded into unsupported process, will now exit!", "ERROR!", MB_OK);
		TerminateProcess(GetCurrentProcess(), 1);
		break;
	case H2Type::H2Game:
		Memory::SetBaseAddress((DWORD)game_info.base, false);
		break;
	case H2Type::H2Server:
		Memory::SetBaseAddress((DWORD)game_info.base, true);
		break;
	default:
		assert(0);
		break;
	}
}
