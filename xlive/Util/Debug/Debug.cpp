#include "stdafx.h"
#include <string>
#include <Dbghelp.h>
#include <time.h>
#include <Shlwapi.h>
#include <Shlobj.h>
#include "Debug.h"
#include "../H2MOD.h"

#define crash_reports_path "\\halo2_crash_reports\\"

using namespace Debug;

LPTOP_LEVEL_EXCEPTION_FILTER expection_filter = nullptr;
LONG WINAPI Debug::On_UnhandledException(struct _EXCEPTION_POINTERS* ExceptionInfo)
{
	// get documents path.
	CHAR documents_path[MAX_PATH];
	SHGetFolderPathA(NULL, CSIDL_PERSONAL, NULL, SHGFP_TYPE_CURRENT, documents_path);

	// get reports path.
	char reports_path[400];
	_fullpath(reports_path, documents_path, 400);
	PathAppendA(reports_path, crash_reports_path);

	// get exe name.
	CHAR exe_path_buffer[MAX_PATH + 1];
	GetModuleFileNameA(NULL, exe_path_buffer, sizeof(exe_path_buffer));
	std::string exe_name = exe_path_buffer;
	exe_name = exe_name.substr(exe_name.find_last_of('\\') + 1);

	// make sure the reports path exists.
	MakeSureDirectoryPathExists(reports_path);

	// get current timestamp.
	time_t timer;
	char timestamp[20];
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
	if (!h2mod->Server) {
		std::string message = "Halo 2 has encountered a fatal error and needs to exit,\n"
			" a crash dump has been saved to '" + dump_file_name + "',\n"
			"please note the path if you want to report the issue, as the file may be necessary.";
		MessageBoxA(NULL, message.c_str(), "Crash!", 0);
	}

	TRACE_GAME_N("Halo 2 has crashed and a dump file has been saved to \"%s\".", dump_file_name.c_str());

	// pass through error to game/server code.
	if (expection_filter)
		return expection_filter(ExceptionInfo);
	else
		return EXCEPTION_CONTINUE_SEARCH;
}

void Debug::init()
{
	LPTOP_LEVEL_EXCEPTION_FILTER expection_filter = SetUnhandledExceptionFilter(Debug::On_UnhandledException);
}

void Debug::set_expection_filter(LPTOP_LEVEL_EXCEPTION_FILTER filter)
{
	LPTOP_LEVEL_EXCEPTION_FILTER expection_filter = filter;
}

LPTOP_LEVEL_EXCEPTION_FILTER Debug::get_expection_filter()
{
	return expection_filter;
}