#include "stdafx.h"
#include "Memory.h"

/* structures */

struct ProcessInfo
{
	HMODULE base;
	e_h2_type process_type;
};

/* constants */

static const wchar_t k_supported_h2_game_version[] = L"1.00.00.11122";
static const wchar_t k_supported_h2_tool_version[] = L"1, 0, 0, 1";

/* globals */

uintptr_t Memory::baseAddress;
bool Memory::g_memory_is_dedicated_server;

ProcessInfo game_info;

/* prototypes */

static e_h2_type detect_process_type(void);

/* public code */

// TODO maybe this is not the best place to keep this
e_h2_type Memory::Initialize(void)
{
	game_info.base = GetModuleHandle(NULL);
	game_info.process_type = detect_process_type();

	static const char* unsupported_game_version_error =
		"Project Cartographer loaded into unsupported game version, will now exit!";

	static const char* unsupported_game_version_error_help =
		"Download latest official patch at \"halo2pc.com/downloads\", Latest Official Update.zip";

	switch (game_info.process_type)
	{
	case _h2_type_none:
		MessageBoxA(NULL, "Project Cartographer loaded into unsupported process, will now exit!", "ERROR!", MB_ICONERROR);
		TerminateProcess(GetCurrentProcess(), 1);
		break;
	case _h2_type_unsupported:
		MessageBoxA(NULL, unsupported_game_version_error, "ERROR!", MB_ICONERROR);
		MessageBoxA(NULL, unsupported_game_version_error_help, "", MB_ICONINFORMATION);
		TerminateProcess(GetCurrentProcess(), 1);
		break;
	case _h2_type_game:
		Memory::SetBaseAddress((DWORD)game_info.base, false);
		break;
	case _h2_type_server:
		Memory::SetBaseAddress((DWORD)game_info.base, true);
		break;
	case _h2_type_ek_sapien:
	case _h2_type_ek_tool:
	case _h2_type_ek_guerilla:
		Memory::SetBaseAddress((DWORD)game_info.base, false);
		break;
	default:
		unreachable();
		break;
	}

	return game_info.process_type;
}

uintptr_t Memory::GetAddress(
	uintptr_t client)
{
	ASSERT(client != 0);
	return GetBaseAddress() + client;
}

uintptr_t Memory::GetAddress(
	uintptr_t client,
	uintptr_t server)
{
	ASSERT(g_memory_is_dedicated_server || client != 0);
	ASSERT(!g_memory_is_dedicated_server || server != 0);

	const uintptr_t address = g_memory_is_dedicated_server ? server : client;
	return GetBaseAddress() + address;
}

static e_h2_type detect_process_type(void)
{
	e_h2_type result = _h2_type_none;

	// fallback to checking file information in case the file was renamed.
	wchar_t exe_file_path[_MAX_PATH + 1];
	int filename_result = GetModuleFileNameW(NULL, exe_file_path, ARRAYSIZE(exe_file_path));

	if (filename_result <= _MAX_PATH && GetLastError() != ERROR_INSUFFICIENT_BUFFER)
	{
		DWORD version_info_size = GetFileVersionInfoSizeW(exe_file_path, NULL);
		if (version_info_size != 0)
		{
			BYTE* version_info = (BYTE*)malloc(version_info_size);
			if (version_info != NULL && GetFileVersionInfoW(exe_file_path, NULL, version_info_size, version_info))
			{
				const wchar_t* orginal_filename;
				const wchar_t* original_product_version;
				size_t filename_len;
				size_t product_version_len;

				VerQueryValueW(version_info, L"\\StringFileInfo\\040904b0\\OriginalFilename", (LPVOID*)&orginal_filename, &filename_len);
				VerQueryValueW(version_info, L"\\StringFileInfo\\040904b0\\ProductVersion", (LPVOID*)&original_product_version, &product_version_len);

				// Check filename
				if (!ustrnicmp(orginal_filename, L"Halo2.exe", filename_len))
				{
					result = _h2_type_game;
				}
				else if (!ustrnicmp(orginal_filename, L"h2server.exe", filename_len))
				{
					result = _h2_type_server;
				}
				else if (!ustrnicmp(orginal_filename, L"treelistview.exe", filename_len))
				{
					result = _h2_type_ek_sapien;
				}
				else if (!ustrnicmp(orginal_filename, L"tool.exe", filename_len))
				{
					result = _h2_type_ek_tool;
				}
				else if (!ustrnicmp(orginal_filename, L"guerilla.exe", filename_len))
				{
					result = _h2_type_ek_guerilla;
				}


				// Check version
				if (
					ustrnicmp(original_product_version, k_supported_h2_game_version, product_version_len) &&
					ustrnicmp(original_product_version, k_supported_h2_tool_version, product_version_len))
				{
					result = _h2_type_unsupported;
				}

				free(version_info);
			}
		}
		// If there's no original version info
		else
		{
			if (ustrstr(exe_file_path, L"tool.exe"))
			{
				result = _h2_type_ek_tool;
			}
		}
	}
	return result;
}