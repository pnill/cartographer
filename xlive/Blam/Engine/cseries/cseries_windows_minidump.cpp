#include "stdafx.h"
#include "cseries_windows_minidump.h"

#include "cseries_windows_minidump_logs.h"
#include "tag_files/files_windows.h"

#include "H2MOD/Modules/OnScreenDebug/OnscreenDebug.h"

#define k_initial_dump_path "C:\\TEMP\\"
#define k_initial_dump_path_wide L"C:\\TEMP\\"
#define k_minidump_folder_name "crash_file_archive\\"
#define k_crash_report_folder_name L"\\halo2_crash_reports\\"
#define k_crash_minidump_file_name "minidump.dmp"
#define k_crash_minidump_file_name_wide L"minidump.dmp"
#define k_crash_file_archive "crash_file_archive.zip"
#define k_crash_file_archive_wide L"crash_file_archive.zip"

// forward declarations

// Returns true if we should include a module's code segment in the dump
bool should_include_module_code_seg(const wchar_t* path);

// Get timestamp and create a string to append to archive name
void dump_timestamp_get(c_static_wchar_string64* timestamp);

// Initialize paths for file dumps
void create_reports_path(c_static_wchar_string260* dump_path, c_static_wchar_string260* minidump_file_path);

// Create crash archive and add crash files to it
void crash_archive_create_and_populate(const char* zip_file_path);

// Create zip report_file and populate it with files we've created
void crash_archive_add_dump_file(zipFile zip_file);

// Add crash report files to crash archive
void crash_archive_add_crash_report_files(zipFile zip_file);


bool should_include_module_code_seg(const wchar_t* path)
{
	bool result = false;

	if (wcsstr(path, L"xlive.dll"))
	{
		result = true;
	}

	return result;
}

void dump_timestamp_get(c_static_wchar_string64* timestamp)
{
	time_t timer = time(NULL);
	tm* tm_info = localtime(&timer);
	wcsftime(timestamp->get_buffer(), timestamp->max_length(), L"%Y%m%d-%H%M%S_", tm_info);
	return;
}

void create_reports_path(c_static_wchar_string260* dump_path, c_static_wchar_string260* minidump_file_path)
{
	dump_path->set(k_initial_dump_path_wide);			// Create the folder initially in the C:\Temp folder and then move to our own folder
	CreateDirectoryW(dump_path->get_string(), NULL);	// Make sure Temp exists

	dump_path->append(L"crash_file_archive\\");			// Set path to our own folder for the report
	CreateDirectoryW(dump_path->get_string(), NULL);	// Make sure our specific report path exists

	// Make sure the reports path that will have our text files exists
	c_static_wchar_string260 reports_path;
	reports_path.set(dump_path->get_string());
	reports_path.append(k_reports_path);
	CreateDirectoryW(reports_path.get_string(), NULL);

	minidump_file_path->set(dump_path->get_string());
	minidump_file_path->append(k_crash_minidump_file_name_wide);
	return;
}

BOOL WINAPI minidump_info_cb(
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

void crash_archive_create_and_populate(const char* zip_file_path)
{
	zipFile zip_file = zipOpen(zip_file_path, 0);

	crash_archive_add_dump_file(zip_file);
	crash_archive_add_crash_report_files(zip_file);

	if (zipClose(zip_file, NULL) != Z_OK)
	{
		LOG_ERROR_FUNC("Failed to close {}", zip_file_path);
	}
	
	return;
}

void crash_archive_move_to_crash_report_folder(const char* initial_zip_file_path, c_static_wchar_string260* archive_path)
{
	// get timestamp
	c_static_wchar_string64 timestamp;
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
void crash_archive_add_dump_file(zipFile zip_file)
{
	c_static_string260 minidump_path;
	minidump_path.set(k_initial_dump_path);
	minidump_path.append(k_minidump_folder_name);
	minidump_path.append(k_crash_minidump_file_name);

	s_file_reference minidump_file;
	file_reference_create_from_path(&minidump_file, minidump_path.get_string(), 0);

	if (!compress_file_to_zip(zip_file, &minidump_file, k_crash_minidump_file_name))
	{
		LOG_ERROR_FUNC("Failed to add {} to archive", minidump_path.get_string());
	}

	return;
}

void crash_archive_add_crash_report_files(zipFile zip_file)
{
	for (uint32 i = 0; i < k_report_text_file_type_count; i++)
	{
		const wchar_t* path = get_crash_info_text_file_path((e_report_file_type)i);
		utf8 utf8_path[MAX_PATH];
		wchar_string_to_utf8_string(path, utf8_path, MAX_PATH);

		s_file_reference report_file;
		file_reference_create_from_path(&report_file, utf8_path, 0);

		c_static_wchar_string260 path_in_archive;
		path_in_archive.set(k_reports_path);
		path_in_archive.append(k_report_text_file_names[i]);

		// Reuse utf8 path from before for the path in the archive
		wchar_string_to_utf8_string(path_in_archive.get_string(), utf8_path, MAX_PATH);

		if (!compress_file_to_zip(zip_file, &report_file, utf8_path))
		{
			LOG_ERROR_FUNCW("Failed to add {} to archive", k_report_text_file_names[i]);
		}
	}

	return;
}

void write_crash_dump_files(_EXCEPTION_POINTERS* ExceptionInfo, c_static_wchar_string260* report_path, c_static_wchar_string260* archive_path)
{
	c_static_wchar_string260 minidump_path;
	create_reports_path(report_path, &minidump_path);

	HANDLE dump_file = CreateFileW(minidump_path.get_string(),
		GENERIC_WRITE,
		0,
		NULL,
		CREATE_ALWAYS,
		FILE_ATTRIBUTE_NORMAL,
		NULL);

	DWORD error = GetLastError();
	if (error != 0)
	{
		LOG_ERROR_FUNCW(L"CreateFileW returned error: {} {}", error, minidump_path.get_string());
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
		DWORD error = GetLastError();
		if (error != 0)
		{
			LOG_ERROR_FUNCW(L"MiniDumpWriteDump returned error: {}", error);
		}
	}
	else
	{
		LOG_TRACE_FUNCW(L"Halo 2 has crashed and a dump file has been saved to \"{}\".", minidump_path.get_string());
	}

	crash_info_text_files_create(report_path->get_string(), &minidump_info);

	c_static_string260 initial_zip_file_path;
	initial_zip_file_path.set(k_initial_dump_path);
	initial_zip_file_path.append(k_crash_file_archive);

	crash_archive_create_and_populate(initial_zip_file_path.get_string());
	crash_archive_move_to_crash_report_folder(initial_zip_file_path.get_string(), archive_path);
	return;
}