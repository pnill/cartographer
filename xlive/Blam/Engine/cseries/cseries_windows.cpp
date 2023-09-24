#include "stdafx.h"
#include "cseries_windows.h"

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
