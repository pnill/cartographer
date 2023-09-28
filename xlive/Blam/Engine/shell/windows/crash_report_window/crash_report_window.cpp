#include "stdafx.h"
#include "crash_report_window.h"

#include "Blam/Engine/cseries/cseries_errors.h"
#include "Blam/Engine/cseries/cseries_strings.h"
#include "Blam/Engine/cseries/cseries_windows.h"
#include "Blam/Engine/text/unicode.h"
#include "Blam/Engine/tag_files/files_windows.h"

#include "resources/resource.h"

#include "Util/filesys.h"

#define k_id_edit 1
#define k_id_button 2

#define k_crash_window_width 640
#define k_crash_window_height 480

#define k_crash_window_text_box_size_x 600
#define k_crash_window_text_box_size_y 200

#define k_crash_window_name L"Crash Report for \"Halo 2: Project Cartographer\""
#define k_crash_header_text L"Halo 2 has encountered a fatal error and needs to exit."
#define k_crash_header_text_1 L"A crash dump file has been created with information on what caused the fatal error."
#define k_crash_header_text_2 L"This file may be needed for debugging purposes."
#define k_crash_text_1 L"File Location:\r\n"

// forward declarations

void crash_window_message_create(const wchar_t* reports_path, const wchar_t* archive_path);

// Returns the total length for all of the text files generated by the crash report
size_t crash_report_get_text_total_size(const wchar_t* reports_path);

// Reads contents from crash report text files and append them to text_string
void crash_window_set_text(const wchar_t* reports_path, wchar_t* crash_message);

void crash_report_window_create_widgets(HWND hwnd, int x, int y);
void crash_window_create_header_bitmap(PAINTSTRUCT* ps, HDC hdc);
void crash_window_create_text(PAINTSTRUCT* ps, HDC hdc);
void crash_window_wm_create(HWND hwnd);
void crash_window_wm_paint(HWND hwnd);
void crash_window_wm_command(LPARAM lParam);
void crash_window_wm_destroy();
LRESULT CALLBACK crash_window_wnd_proc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
void crash_report_text_add_windows_newline(wchar_t* crash_message);

wchar_t* g_crash_message = NULL;

utf8* g_report_text_file_utf8_strings[k_report_text_file_type_count] = {NULL};
size_t g_report_text_file_string_lengths[k_report_text_file_type_count] = {0};

HWND g_crash_report_window;
HWND g_crash_message_textbox;
HWND g_crash_ok_button;
HBITMAP g_crash_hbitmap;


void crash_window_create(const wchar_t* report_path, const wchar_t* archive_path)
{
    crash_window_message_create(report_path, archive_path);

    MSG msg;
    WNDCLASSW wc = { 0 };

    wc.style = CS_HREDRAW | CS_VREDRAW;
    wc.lpszClassName = k_crash_window_name;
    wc.hbrBackground = GetSysColorBrush(COLOR_3DFACE);
    wc.lpfnWndProc = crash_window_wnd_proc;
    wc.hCursor = LoadCursor(0, IDC_ARROW);

    RegisterClassW(&wc);
    g_crash_report_window = CreateWindow(wc.lpszClassName,
        k_crash_window_name,
        WS_VISIBLE | WS_OVERLAPPED | WS_MINIMIZEBOX | WS_SYSMENU,   // Window style
        0, 0,                                                       // Position
        k_crash_window_width, k_crash_window_height,                // Window string_length
        NULL,
        NULL,
        NULL,
        NULL);

    SetForegroundWindow(g_crash_report_window);

    while (GetMessage(&msg, NULL, 0, 0)) 
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    return;
}

void crash_window_message_create(const wchar_t* reports_path, const wchar_t* archive_path)
{
    const wchar_t double_return[] = L"\r\n\r\n";
    const size_t double_return_length = NUMBEROF(double_return) - 1; // - 1 since we don't include null terminator
    const size_t archive_path_length = ustrnlen(archive_path, MAX_PATH);

    size_t string_allocation_length = NUMBEROF(k_crash_text_1) - 1;  // - 1 since we don't include null terminator
    string_allocation_length += archive_path_length;
    string_allocation_length += double_return_length;
    string_allocation_length += crash_report_get_text_total_size(reports_path);

    string_allocation_length += ((k_report_text_file_type_count - 1) * 2);  // Insert a \r\n between each text file [(file count - 1) * characters added]
    string_allocation_length += 1;                                          // Insert a null terminator

    g_crash_message = (wchar_t*)calloc(string_allocation_length, sizeof(wchar_t));

    if (g_crash_message != NULL)
    {
        ustrnzcat(g_crash_message, k_crash_text_1, NUMBEROF(k_crash_text_1));
        ustrnzcat(g_crash_message, archive_path, archive_path_length);
        ustrnzcat(g_crash_message, double_return, double_return_length);
        crash_window_set_text(reports_path, g_crash_message);
    }

    // Delete temp report path after we read the data
    SHFILEOPSTRUCT file_op = {
            NULL,
            FO_DELETE,
            reports_path,
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
        LOG_ERROR_FUNCW("SHFileOperationW failed to delete folder {} with error {}", reports_path, result);
    }
    return;
}

size_t crash_report_get_text_total_size(const wchar_t* reports_path)
{
    size_t string_length = 0;
    c_static_wchar_string260 file_path;

    for (size_t i = 0; i < k_report_text_file_type_count; i++)
    {
        file_path.clear();
        file_path.set(reports_path);
        file_path.append(k_reports_path);
        file_path.append(k_report_text_file_names[i]);

        // MAX_PATH * 2 since we're converting a 2 byte MAX_PATH string into a multibyte string
        utf8 utf8_path[MAX_PATH * 2];
        wchar_string_to_utf8_string(file_path.get_string(), utf8_path, MAX_PATH * 2);

        s_file_reference file;
        e_file_open_error error;
        e_file_open_flags flags = _permission_read_bit;

        file_reference_create_from_path(&file, utf8_path, false);
        bool file_open_result = file_open(&file, flags, &error);

        if (!error && file_open_result)
        {
            DWORD file_size;
            file_get_size(&file, &file_size);
            g_report_text_file_utf8_strings[i] = (utf8*)calloc(file_size + 1, sizeof(utf8));
            bool result = file_read(&file, file_size, true, g_report_text_file_utf8_strings[i]);
            g_report_text_file_string_lengths[i] += utf8_string_length(g_report_text_file_utf8_strings[i], file_size);
            string_length += g_report_text_file_string_lengths[i];

            file_close(&file);
        }
    }

    return string_length;
}

void crash_window_set_text(const wchar_t* reports_path, wchar_t* crash_message)
{
    c_static_wchar_string260 file_path;

    for (size_t report_file_index = 0; report_file_index < k_report_text_file_type_count; report_file_index++)
    {
        file_path.clear();
        file_path.set(reports_path);
        file_path.append(k_reports_path);
        file_path.append(k_report_text_file_names[report_file_index]);

        s_file_reference file;

        // MAX_PATH * 2 since we're converting a 2 byte MAX_PATH string into a multibyte string
        utf8 utf8_path[MAX_PATH * 2];
        wchar_string_to_utf8_string(file_path.get_string(), utf8_path, MAX_PATH * 2);
        file_reference_create_from_path(&file, utf8_path, false);

        // Convert UTF-8 String to wide string and append it to the crash message
        if (g_report_text_file_utf8_strings[report_file_index] != NULL)
        {
            wchar_t* wide_string = (wchar_t*)calloc(g_report_text_file_string_lengths[report_file_index] + 1, sizeof(wchar_t));
            utf8_string_to_wchar_string(g_report_text_file_utf8_strings[report_file_index], wide_string, g_report_text_file_string_lengths[report_file_index] + 1);
            ustrnzcat(crash_message, wide_string, g_report_text_file_string_lengths[report_file_index]);

            // Cleanup
            free(wide_string);
            free(g_report_text_file_utf8_strings[report_file_index]);
            g_report_text_file_utf8_strings[report_file_index] = NULL;
            g_report_text_file_string_lengths[report_file_index] = 0;
        }

        // Append \r\n to string to separate different text files
        // Don't append after the last text file
        if (report_file_index + 1 != k_report_text_file_type_count)
        {
            crash_report_text_add_windows_newline(crash_message);
        }
    }
    
    return;
}

void crash_report_window_create_widgets(HWND hwnd, int x, int y)
{
    int text_box_padding_x = (k_crash_window_width - k_crash_window_text_box_size_x) / 2;
    g_crash_message_textbox = CreateWindowExW(
        0,									// Optional window styles.
        L"Edit",							// Window class
        L"",                                // Temp Window text
        // Window style
        WS_CHILD | WS_VISIBLE | WS_VSCROLL | WS_BORDER |
        ES_LEFT | ES_MULTILINE | ES_AUTOVSCROLL | ES_READONLY,
        text_box_padding_x, 180,                // Position
        k_crash_window_text_box_size_x, k_crash_window_text_box_size_y,   // Window string_length
        hwnd,			                        // Parent window    
        (HMENU)k_id_edit,	                    // Menu
        NULL,			                        // Instance handle
        NULL			                        // Additional application data
    );

    SetWindowTextW(g_crash_message_textbox, g_crash_message);

    HFONT textbox_font = CreateFontW(14, 0, 0, 0, FW_MEDIUM, FALSE, FALSE, FALSE, 0, 0, CLEARTYPE_QUALITY, ANTIALIASED_QUALITY, 0, TEXT("Arial"));
    SendMessage(g_crash_message_textbox, WM_SETFONT, (WPARAM)textbox_font, TRUE);


    g_crash_ok_button = CreateWindowEx(
        0,						// Optional window styles.
        L"button",				// Window class
        L"Ok",				    // Window text
        WS_CHILD | WS_VISIBLE,  // Window style
        x - 90, y - 80,         // Position
        60, 30,                 // Window string_length
        hwnd,				    // Parent window    
        (HMENU)k_id_button,	    // Menu
        NULL,				    // Instance handle
        NULL				    // Additional application data
    );

    HFONT ok_font = CreateFontW(20, 0, 0, 0, FW_MEDIUM, FALSE, FALSE, FALSE, 0, 0, CLEARTYPE_QUALITY, ANTIALIASED_QUALITY, 0, TEXT("Arial"));
    SendMessage(g_crash_ok_button, WM_SETFONT, (WPARAM)ok_font, TRUE);

    return;
}

void crash_window_create_header_bitmap(PAINTSTRUCT* ps, HDC hdc)
{
    HDC hdc_mem = CreateCompatibleDC(hdc);
    HGDIOBJ old_bitmap = SelectObject(hdc_mem, g_crash_hbitmap);
    BITMAP bitmap;
    GetObjectW(g_crash_hbitmap, sizeof(bitmap), &bitmap);
    SetStretchBltMode(hdc, HALFTONE);
    StretchBlt(hdc, 0, 10, k_crash_window_width, k_crash_window_height / 6, hdc_mem, 0, 0, bitmap.bmWidth, bitmap.bmHeight, SRCCOPY);
    SelectObject(hdc_mem, old_bitmap);
    DeleteDC(hdc_mem);

    return;
}

void crash_window_create_text(PAINTSTRUCT* ps, HDC hdc)
{
    DWORD color = GetSysColor(COLOR_BTNFACE);
    SetBkColor(hdc, color);

    HFONT header_font = CreateFont(24, 0, 0, 0, FW_BLACK, FALSE, FALSE, FALSE, 0, 0, CLEARTYPE_QUALITY, ANTIALIASED_QUALITY, 0, TEXT("Arial"));
    HFONT reg_font = CreateFont(16, 0, 0, 0, FW_MEDIUM, FALSE, FALSE, FALSE, 0, 0, CLEARTYPE_QUALITY, ANTIALIASED_QUALITY, 0, TEXT("Arial"));
    HFONT h_font_original = (HFONT)SelectObject(hdc, header_font);

    TextOutW(hdc, 20, 100, k_crash_header_text, NUMBEROF(k_crash_header_text));

    SelectObject(hdc, reg_font);
    TextOutW(hdc, 20, 130, k_crash_header_text_1, NUMBEROF(k_crash_header_text_1));
    TextOutW(hdc, 20, 150, k_crash_header_text_2, NUMBEROF(k_crash_header_text_2));

    SelectObject(hdc, h_font_original);
    DeleteObject(header_font);
    DeleteObject(reg_font);

    return;
}

void crash_window_wm_create(HWND hwnd)
{
    center_window(hwnd);
    crash_report_window_create_widgets(hwnd, k_crash_window_width, k_crash_window_height);

    // Load bitmap from windows resource
    g_crash_hbitmap = LoadBitmapW(GetModuleHandleW(L"xlive.dll"), MAKEINTRESOURCEW(IDB_CRASH_WINDOW_HEADER_BITMAP));

    if (g_crash_hbitmap == NULL)
    {
        LOG_ERROR_FUNCW("Failed to load header image");
    }

    return;
}

void crash_window_wm_paint(HWND hwnd)
{
    PAINTSTRUCT ps;
    HDC hdc = BeginPaint(hwnd, &ps);
    crash_window_create_text(&ps, hdc);
    crash_window_create_header_bitmap(&ps, hdc);
    EndPaint(hwnd, &ps);

    return;
}


void crash_window_wm_command(LPARAM lParam)
{
    if ((HWND)lParam == g_crash_ok_button)
    {
        DestroyWindow(g_crash_report_window);
    }
    
    return;
}

void crash_window_wm_destroy()
{
    DeleteObject(g_crash_hbitmap);
    DeleteObject(g_crash_ok_button);
    DeleteObject(g_crash_message_textbox);
    PostQuitMessage(0);
    free(g_crash_message);
    
    return;
}

LRESULT CALLBACK crash_window_wnd_proc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    switch (msg) 
    {
    case WM_CREATE:
        crash_window_wm_create(hwnd);
        break;
    case WM_PAINT:
        crash_window_wm_paint(hwnd);
        break;
    case WM_COMMAND:
        crash_window_wm_command(lParam);
        break;
    case WM_DESTROY:
        crash_window_wm_destroy();
        return 0;
    }

    return DefWindowProcW(hwnd, msg, wParam, lParam);
}

void crash_report_text_add_windows_newline(wchar_t* crash_message)
{
    crash_message = ustrnzcat(crash_message, L"\r\n", 2);
    return;
}