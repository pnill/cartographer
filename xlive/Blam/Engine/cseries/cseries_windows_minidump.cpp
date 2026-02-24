#include "stdafx.h"
#include "cseries_windows_minidump.h"

#include "cseries_windows_minidump_logs.h"
#include "tag_files/files.h"

#include "H2MOD/Modules/OnScreenDebug/OnscreenDebug.h"

#include <ShlObj_core.h>	// SHGetFolderPathW

static const char k_initial_dump_path[] = "C:\\TEMP\\";
static const wchar_t k_initial_dump_path_wide[] = L"C:\\TEMP\\";
static const char k_minidump_folder_name[] = "crash_file_archive\\";
static const wchar_t k_crash_report_folder_name[] = L"\\halo2_crash_reports\\";
static const char k_crash_minidump_file_name[] = "minidump.dmp";
static const wchar_t k_crash_minidump_file_name_wide[] = L"minidump.dmp";
static const char k_crash_file_archive[] = "crash_file_archive.zip";
static const wchar_t k_crash_file_archive_wide[] = L"crash_file_archive.zip";

/* prototypes */

// Returns true if we should include a module's code segment in the dump
static bool should_include_module_code_seg(const wchar_t* path);

// Get timestamp and create a string to append to archive name
static void dump_timestamp_get(c_static_wchar_string<64>* timestamp);

// Initialize paths for file dumps
static void create_reports_path(c_static_wchar_string<MAX_PATH>* dump_path, c_static_wchar_string<MAX_PATH>* minidump_file_path);

static BOOL WINAPI minidump_info_cb(
	_Inout_ PVOID CallbackParam,
	_In_    PMINIDUMP_CALLBACK_INPUT CallbackInput,
	_Inout_ PMINIDUMP_CALLBACK_OUTPUT CallbackOutput
);

// Create crash archive and add crash files to it
static void crash_archive_create_and_populate(const char* zip_file_path);

static void crash_archive_move_to_crash_report_folder(const char* initial_zip_file_path, c_static_wchar_string<MAX_PATH>* archive_path);

// Create zip report_file and populate it with files we've created
static void crash_archive_add_dump_file(zipFile zip_file);

// Add crash report files to crash archive
static void crash_archive_add_crash_report_files(zipFile zip_file);

/* public code */

void write_crash_dump_files(_EXCEPTION_POINTERS* ExceptionInfo, c_static_wchar_string<MAX_PATH>* report_path, c_static_wchar_string<MAX_PATH>* archive_path)
{
	c_static_wchar_string<MAX_PATH> minidump_path;
	create_reports_path(report_path, &minidump_path);

	HANDLE dump_file = CreateFileW(minidump_path.get_string(),
		GENERIC_WRITE,
		0,
		NULL,
		CREATE_ALWAYS,
		FILE_ATTRIBUTE_NORMAL,
		NULL);

	DWORD error_val = GetLastError();
	if (error_val != 0)
	{
		error(_error_immediate, "CreateFileW returned error: %d %ws", error_val, minidump_path.get_string());
	}

	MINIDUMP_EXCEPTION_INFORMATION minidump_info{};
	minidump_info.ThreadId = GetCurrentThreadId();
	minidump_info.ExceptionPointers = ExceptionInfo;
	minidump_info.ClientPointers = TRUE;

	MINIDUMP_CALLBACK_INFORMATION minidump_callback_info{};
	minidump_callback_info.CallbackParam = NULL;
	minidump_callback_info.CallbackRoutine = &minidump_info_cb;

	bool result = MiniDumpWriteDump(GetCurrentProcess(),
		GetCurrentProcessId(),
		dump_file,
		(MINIDUMP_TYPE)(MiniDumpWithDataSegs | MiniDumpWithHandleData | MiniDumpWithUnloadedModules |
			MiniDumpWithProcessThreadData | MiniDumpWithIndirectlyReferencedMemory
			),
		&minidump_info,
		NULL,
		&minidump_callback_info);
	CloseHandle(dump_file);

	if (!result)
	{
		error_val = GetLastError();
		if (error_val != 0)
		{
			error(_error_immediate, "MiniDumpWriteDump returned error: %lu", error_val);
		}
	}
	else
	{
		error(_error_immediate, "Halo 2 has crashed and a dump file has been saved to \"%ws\".", minidump_path.get_string());
	}

	crash_info_text_files_create(report_path->get_string(), &minidump_info);

	c_static_string<MAX_PATH> initial_zip_file_path;
	initial_zip_file_path.set(k_initial_dump_path);
	initial_zip_file_path.append(k_crash_file_archive);

	crash_archive_create_and_populate(initial_zip_file_path.get_string());
	crash_archive_move_to_crash_report_folder(initial_zip_file_path.get_string(), archive_path);
	return;
}

/* private code */

static bool should_include_module_code_seg(const wchar_t* path)
{
	bool result = false;

	if (wcsstr(path, L"xlive.dll"))
	{
		result = true;
	}

	return result;
}

static void dump_timestamp_get(c_static_wchar_string<64>* timestamp)
{
	time_t timer = time(NULL);
	tm tm_info;
	const errno_t err = localtime_s(&tm_info, &timer);
	if (err)
	{
		error(_error_immediate, "Error occurred when getting timestamp: %d", err);
	}
	else
	{
		wcsftime(timestamp->get_buffer(), timestamp->max_length(), L"%Y%m%d-%H%M%S_", &tm_info);
	}

	return;
}

static void create_reports_path(c_static_wchar_string<MAX_PATH>* dump_path, c_static_wchar_string<MAX_PATH>* minidump_file_path)
{
	dump_path->set(k_initial_dump_path_wide);			// Create the folder initially in the C:\Temp folder and then move to our own folder
	CreateDirectoryW(dump_path->get_string(), NULL);	// Make sure Temp exists

	dump_path->append(L"crash_file_archive");			// Set path to our own folder for the report
	CreateDirectoryW(dump_path->get_string(), NULL);	// Make sure our specific report path exists

	// Make sure the reports path that will have our text files exists
	c_static_wchar_string<MAX_PATH> reports_path;
	reports_path.set(dump_path->get_string());
	reports_path.append(L"\\");
	reports_path.append(k_reports_path);
	CreateDirectoryW(reports_path.get_string(), NULL);

	minidump_file_path->set(dump_path->get_string());
	minidump_file_path->append(L"\\");
	minidump_file_path->append(k_crash_minidump_file_name_wide);
	return;
}

static BOOL WINAPI minidump_info_cb(
	_Inout_ PVOID CallbackParam,
	_In_    PMINIDUMP_CALLBACK_INPUT CallbackInput,
	_Inout_ PMINIDUMP_CALLBACK_OUTPUT CallbackOutput
)
{
	switch (CallbackInput->CallbackType)
	{
	case ModuleCallback:
		if (should_include_module_code_seg(CallbackInput->Module.FullPath))
		{
			CallbackOutput->ModuleWriteFlags |= (ModuleWriteDataSeg | ModuleWriteCodeSegs);
			addDebugText(L"added code segments and data segments to minidump of module: %ws", CallbackInput->Module.FullPath);
		}
		else
		{
			if ((CallbackOutput->ModuleWriteFlags & ModuleWriteCodeSegs) == 0)
			{
				addDebugText(L"skipped code segment of module: %ws", CallbackInput->Module.FullPath);
			}
		}
		break;
	default:
		break;
	}

	return TRUE;
}

static void crash_archive_create_and_populate(const char* zip_file_path)
{
	zipFile zip_file = zipOpen(zip_file_path, 0);

	crash_archive_add_dump_file(zip_file);
	crash_archive_add_crash_report_files(zip_file);

	if (zipClose(zip_file, NULL) != Z_OK)
	{
		error(_error_immediate, "Failed to close %ws", zip_file_path);
	}
	
	return;
}

static void crash_archive_move_to_crash_report_folder(const char* initial_zip_file_path, c_static_wchar_string<MAX_PATH>* archive_path)
{
	// get timestamp
	c_static_wchar_string<64> timestamp;
	dump_timestamp_get(&timestamp);

	// Construct reports path
	SHGetFolderPathW(NULL, CSIDL_PERSONAL, NULL, SHGFP_TYPE_CURRENT, archive_path->get_buffer());	// Get documents path.
	archive_path->append(k_crash_report_folder_name);
	
	// Make sure crash report folder exists
	CreateDirectoryW(archive_path->get_string(), NULL);

	archive_path->append(timestamp.get_string());
	archive_path->append(k_crash_file_archive_wide);

	// Convert zip path to wide string
	wchar_t zip_file_path_wide[MAX_PATH];
	utf8_string_to_wchar_string(initial_zip_file_path, zip_file_path_wide, MAX_PATH);

	MoveFileW(zip_file_path_wide, archive_path->get_buffer());
	return;
}

// Create zip report_file and populate it with files we've created
static void crash_archive_add_dump_file(zipFile zip_file)
{
	c_static_string<MAX_PATH> minidump_path;
	minidump_path.set(k_initial_dump_path);
	minidump_path.append(k_minidump_folder_name);
	minidump_path.append(k_crash_minidump_file_name);

	s_file_reference minidump_file;
	file_reference_create_from_path(&minidump_file, minidump_path.get_string(), 0);

	if (!compress_file_to_zip(zip_file, &minidump_file, k_crash_minidump_file_name))
	{
		error(_error_immediate, "Failed to add %ws to archive", minidump_path.get_string());
	}

	return;
}

static void crash_archive_add_crash_report_files(zipFile zip_file)
{
	for (uint32 i = 0; i < k_report_text_file_type_count; i++)
	{
		const wchar_t* path = g_report_text_file_paths[i].get_string();
		utf8 utf8_path[MAX_PATH];
		wchar_string_to_utf8_string(path, utf8_path, MAX_PATH);

		s_file_reference report_file;
		file_reference_create_from_path(&report_file, utf8_path, 0);

		c_static_wchar_string<MAX_PATH> path_in_archive;
		path_in_archive.set(k_reports_path);
		path_in_archive.append(k_report_text_file_names[i]);

		// Reuse utf8 path from before for the path in the archive
		wchar_string_to_utf8_string(path_in_archive.get_string(), utf8_path, MAX_PATH);

		if (!compress_file_to_zip(zip_file, &report_file, utf8_path))
		{
			error(_error_immediate, "Failed to add %ws to archive", k_report_text_file_names[i]);
		}
	}

	// Cleanup paths
	delete[] g_report_text_file_paths;
	return;
}
