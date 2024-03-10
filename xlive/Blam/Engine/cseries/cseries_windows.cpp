#include "stdafx.h"
#include "cseries_windows.h"

bool g_debugger_flag_0 = false;
bool g_debugger_flag_1 = false;
bool g_debugger_flag_2 = false;

bool is_debugger_present(void)
{
    bool result = false;

    bool present = g_debugger_flag_0 || IsDebuggerPresent();
    return !g_debugger_flag_1 && !g_debugger_flag_2 && present;
}

void center_window(HWND hwnd)
{
    int screen_w = GetSystemMetrics(SM_CXSCREEN);
    int screen_h = GetSystemMetrics(SM_CYSCREEN);

    RECT rect;
    if (GetWindowRect(hwnd, &rect))
    {
        uint32 width = rect.right - rect.left;
        uint32 height = rect.bottom - rect.top;

        // Centering is done here
        SetWindowPos(hwnd, HWND_TOP, (screen_w - width) / 2, (screen_h - height) / 2, 0, 0, SWP_NOSIZE);
    }

    return;
}
