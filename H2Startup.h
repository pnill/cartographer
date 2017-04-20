#pragma once
#include "stdafx.h"

void ProcessH2Startup();
int getPlayerNumber();

extern bool H2IsDediServer;
extern DWORD H2BaseAddr;
extern HWND H2hWnd;

extern wchar_t dedi_server_name[];
