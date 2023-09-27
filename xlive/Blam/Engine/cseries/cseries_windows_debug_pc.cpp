#include "stdafx.h"
#include "cseries_windows_debug_pc.h"

#include "cseries_strings.h"
#include "cseries_windows_minidump.h"

#include "Blam/Engine/shell/windows/crash_report_window/crash_report_window.h"
#include "Util/Hooks/Hook.h"

LPTOP_LEVEL_EXCEPTION_FILTER pfn_SecondaryExceptionFilter = NULL;

LONG WINAPI debug_unhandled_exception_cb(_In_ struct _EXCEPTION_POINTERS* exception_info)
{
	c_static_wchar_string260 reports_path;
	c_static_wchar_string260 zip_file_path;
	write_crash_dump_files(exception_info, &reports_path, &zip_file_path);

	// don't attempt to display a GUI error box if running on a server.
	if (!Memory::IsDedicatedServer()) 
	{
		crash_window_create(reports_path.get_string(), zip_file_path.get_string());
	}
	else
	{
		// Delete temp report path after we read the data
		SHFILEOPSTRUCT file_op = {
				NULL,
				FO_DELETE,
				reports_path.get_string(),
				L"",
				FOF_NOCONFIRMATION |
				FOF_NOERRORUI |
				FOF_SILENT,
				false,
				0,
				L"" };
		int32 result = SHFileOperationW(&file_op);
		if (result)
		{
			LOG_ERROR_FUNCW("SHFileOperationW failed to delete folder {} with error {}", reports_path.get_string(), result);
		}
	}

	// pass through error to game/server code.
	if (pfn_SecondaryExceptionFilter)
		return pfn_SecondaryExceptionFilter(exception_info);
	else
		return EXCEPTION_CONTINUE_SEARCH;
}

PIMAGE_NT_HEADERS get_nt_headers(void* pe_module)
{
	PIMAGE_DOS_HEADER dos_header = reinterpret_cast<PIMAGE_DOS_HEADER>(pe_module);
	PIMAGE_NT_HEADERS nt_headers = reinterpret_cast<PIMAGE_NT_HEADERS>(reinterpret_cast<uint8_t*>(pe_module) + dos_header->e_lfanew);
	return nt_headers;
}

uintptr_t devirtualize_address(void* pe_module, uintptr_t virtual_address, uint32 base_address)
{
	PIMAGE_NT_HEADERS nt_headers = get_nt_headers(pe_module);
	PIMAGE_SECTION_HEADER sections = IMAGE_FIRST_SECTION(nt_headers);

	LOG_INFO_FUNCW(L"Base address: {:x}", base_address);
	LOG_INFO_FUNCW(L"Virtual address: {:x}", virtual_address);
	
	virtual_address -= base_address;
	LOG_INFO_FUNCW(L"Relative virtual address: {:x}", virtual_address);

	for (size_t i = 0; i < nt_headers->FileHeader.NumberOfSections; i++)
	{
		if (virtual_address >= sections[i].VirtualAddress && virtual_address < sections[i].SizeOfRawData)
		{
			uintptr_t file_offset = sections[i].PointerToRawData + (virtual_address - sections[i].VirtualAddress);
			LOG_INFO_FUNCW(L"File offset: {:x}", file_offset);
			return file_offset;
		}
	}

	return 0;
}

static_assert(std::is_same<decltype(&debug_unhandled_exception_cb), LPTOP_LEVEL_EXCEPTION_FILTER>::value, "invalid exception handler declaration");

LPTOP_LEVEL_EXCEPTION_FILTER(WINAPI* pfn_SetUnhandledExceptionFilter)(LPTOP_LEVEL_EXCEPTION_FILTER);
LPTOP_LEVEL_EXCEPTION_FILTER WINAPI SetUnhandledExceptionFilterHook(LPTOP_LEVEL_EXCEPTION_FILTER pfnFilter)
{
	LPTOP_LEVEL_EXCEPTION_FILTER pfn_OldExceptionFilter = pfn_SecondaryExceptionFilter;
	pfn_SecondaryExceptionFilter = pfnFilter;
	return pfn_OldExceptionFilter;
}
static_assert(std::is_same_v<decltype(&SetUnhandledExceptionFilterHook), decltype(&SetUnhandledExceptionFilter)>, "invalid type of RedirectedSetUnhandledExceptionFilter");

void cseries_debug_initialize()
{
	pfn_SecondaryExceptionFilter = SetUnhandledExceptionFilter(debug_unhandled_exception_cb);

	// Credits: multitheftauto/mtasa-blue
	// https://github.com/multitheftauto/mtasa-blue/blob/6c1f3184764aca0655b5b64fe88ca0a73b2b69c8/Client/core/CrashHandler.cpp#L102
	// Stop the OS from turning off our handler
	// Ref: http://www.codeproject.com/Articles/154686/SetUnhandledExceptionFilter-and-the-C-C-Runtime-Li
	DETOUR_BEGIN();
	DETOUR_ATTACH(pfn_SetUnhandledExceptionFilter, decltype(pfn_SetUnhandledExceptionFilter)(DetourFindFunction("kernel32.dll", "SetUnhandledExceptionFilter")), SetUnhandledExceptionFilterHook);
	DETOUR_COMMIT();
}