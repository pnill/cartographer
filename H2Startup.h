#pragma once
#include "stdafx.h"

void ProcessH2Startup();
int getPlayerNumber();

extern bool H2IsDediServer;
extern DWORD H2BaseAddr;
extern wchar_t* processFilePath;
extern HWND H2hWnd;
extern int custom_resolution_x;
extern int custom_resolution_y;

extern wchar_t dedi_server_name[];
extern wchar_t dedi_server_playlist[];
