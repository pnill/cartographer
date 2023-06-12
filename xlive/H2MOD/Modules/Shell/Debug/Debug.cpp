#include "stdafx.h"

#include "Debug.h"

#include <DbgHelp.h>
#include <ShlObj.h>
#include <Shlwapi.h>
#include <string>
#include <time.h>

#include "../Shell.h"
#include "Util/Hooks/Hook.h"

#define CRASH_REPORTS_PATH "\\halo2_crash_reports\\"

using namespace Debug;

LPTOP_LEVEL_EXCEPTION_FILTER pfn_PreviousExceptionFilter = NULL;

LONG WINAPI Debug::UnhandledExceptionCb(_In_ struct _EXCEPTION_POINTERS* ExceptionInfo)
{
	// get documents path.
	char documents_path[MAX_PATH];
	SHGetFolderPathA(NULL, CSIDL_PERSONAL, NULL, SHGFP_TYPE_CURRENT, documents_path);

	// get reports path.
	char reports_path[MAX_PATH];
	_fullpath(reports_path, documents_path, sizeof(reports_path));
	PathAppendA(reports_path, CRASH_REPORTS_PATH);

	// get exe name.
	CHAR exe_path_buffer[MAX_PATH];
	GetModuleFileNameA(NULL, exe_path_buffer, sizeof(exe_path_buffer));
	std::string exe_name = exe_path_buffer;
	exe_name = exe_name.substr(exe_name.find_last_of('\\') + 1);

	// make sure the reports path exists.
	MakeSureDirectoryPathExists(reports_path);

	// get current timestamp.
	time_t timer;
	char timestamp[128];
	struct tm* tm_info;

	time(&timer);
	tm_info = localtime(&timer);

	strftime(timestamp, sizeof(timestamp), "_%Y%m%d-%H%M%S", tm_info);

	// do the actual dumping.

	std::string dump_file_name = reports_path + exe_name + timestamp + ".dmp";

	HANDLE dump_file = CreateFileA(dump_file_name.c_str(),
		GENERIC_WRITE,
		0,
		NULL,
		CREATE_ALWAYS,
		FILE_ATTRIBUTE_NORMAL,
		NULL);

	MINIDUMP_EXCEPTION_INFORMATION aMiniDumpInfo;
	aMiniDumpInfo.ThreadId = GetCurrentThreadId();
	aMiniDumpInfo.ExceptionPointers = ExceptionInfo;
	aMiniDumpInfo.ClientPointers = TRUE;

	MiniDumpWriteDump(GetCurrentProcess(),
		GetCurrentProcessId(),
		dump_file,
		(MINIDUMP_TYPE)(MiniDumpWithDataSegs | MiniDumpWithHandleData | MiniDumpWithUnloadedModules |
			MiniDumpWithProcessThreadData | MiniDumpWithIndirectlyReferencedMemory
			),
		&aMiniDumpInfo,
		NULL,
		NULL);

	CloseHandle(dump_file);

	// don't attempt to display a GUI error box if running on a server.
	if (!Memory::IsDedicatedServer()) {
		std::string message = "Halo 2 has encountered a fatal error and needs to exit,\n"
			" a crash dump has been saved to '" + dump_file_name + "',\n"
			"please note the path if you want to report the issue, as the file may be necessary.";

		_Shell::OpenMessageBox(NULL, MB_ICONERROR, "Crash!", message.c_str());
	}

	LOG_TRACE_GAME("Halo 2 has crashed and a dump file has been saved to \"{}\".", dump_file_name.c_str());

	// pass through error to game/server code.
	if (pfn_PreviousExceptionFilter)
		return pfn_PreviousExceptionFilter(ExceptionInfo);
	else
		return EXCEPTION_CONTINUE_SEARCH;
}
static_assert(std::is_same<decltype(&Debug::UnhandledExceptionCb), LPTOP_LEVEL_EXCEPTION_FILTER>::value, "invalid exception handler declaration");

LPTOP_LEVEL_EXCEPTION_FILTER(WINAPI* pfn_SetUnhandledExceptionFilter)(LPTOP_LEVEL_EXCEPTION_FILTER);
LPTOP_LEVEL_EXCEPTION_FILTER WINAPI SetUnhandledExceptionFilterHook(LPTOP_LEVEL_EXCEPTION_FILTER pfnFilter)
{
	return 0;
}
static_assert(std::is_same_v<decltype(&SetUnhandledExceptionFilterHook), decltype(&SetUnhandledExceptionFilter)>,
	"invalid type of RedirectedSetUnhandledExceptionFilter");

void Debug::Initialize()
{
	pfn_PreviousExceptionFilter = SetUnhandledExceptionFilter(Debug::UnhandledExceptionCb);

	// Credits: multitheftauto/mtasa-blue
	// https://github.com/multitheftauto/mtasa-blue/blob/6c1f3184764aca0655b5b64fe88ca0a73b2b69c8/Client/core/CrashHandler.cpp#L102
	// Stop the OS from turning off our handler
	// Ref: http://www.codeproject.com/Articles/154686/SetUnhandledExceptionFilter-and-the-C-C-Runtime-Li
	DETOUR_BEGIN();
	DETOUR_ATTACH(pfn_SetUnhandledExceptionFilter, decltype(pfn_SetUnhandledExceptionFilter)(DetourFindFunction("kernel32.dll", "SetUnhandledExceptionFilter")), SetUnhandledExceptionFilterHook);
	DETOUR_COMMIT();
}