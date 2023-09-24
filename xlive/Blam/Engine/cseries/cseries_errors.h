#pragma once

enum e_report_file_type : uint8
{
    _report_file_type_exception = 0,
    _report_file_type_cpu = 1,
    _report_file_type_game_options = 2,
    _report_file_type_game_globals = 3,

    k_report_text_file_type_count
};


#define k_reports_path L"reports\\"
extern const wchar_t* k_report_text_file_names[k_report_text_file_type_count];


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

// Get c-string of the path a report text files that can be dumped
const wchar_t* get_crash_info_text_file_path(e_report_file_type type);

// Create text files with crash info
void crash_info_text_files_create(const wchar_t* reports_path, const MINIDUMP_EXCEPTION_INFORMATION* minidump_info);
