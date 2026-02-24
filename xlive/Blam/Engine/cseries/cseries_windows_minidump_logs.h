#pragma once
#include <minidumpapiset.h>

/* enums */

enum e_report_file_type : uint8
{
	_report_file_type_exception = 0,
	_report_file_type_cpu = 1,
	_report_file_type_game_options = 2,
	_report_file_type_game_globals = 3,
	_report_file_type_rasterizer = 4,

	k_report_text_file_type_count
};

/* constants */

extern const wchar_t k_crash_message_header_break[];
extern const wchar_t k_reports_path[];
extern const wchar_t *const k_report_text_file_names[k_report_text_file_type_count];

/* globals */

extern c_static_wchar_string<MAX_PATH>* g_report_text_file_paths;

/* prototypes */

template<typename t_array_type>
void print_array_to_file(FILE* file, const t_array_type array, size_t length)
{
	fwprintf(file, L"{ ");
	for (size_t i = 0; i < length; i++)
	{
		fwprintf(file, L"%u", array[i]);

		// Print commas inbetween each element in the array
		if (i != length - 1)
		{
			fwprintf(file, L", ");
		}
	}
	fwprintf(file, L" }\n");
}

// Create text files with crash info
void crash_info_text_files_create(const wchar_t* reports_path, const MINIDUMP_EXCEPTION_INFORMATION* minidump_info);
