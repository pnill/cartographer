#pragma once
#include "stdafx.h"

void InitH2Startup();
void InitH2Startup2();
void DeinitH2Startup();
int H2GetInstanceId();

extern bool H2IsDediServer;
extern DWORD H2BaseAddr;
extern wchar_t* H2ProcessFilePath;
extern wchar_t* H2AppDataLocal;
extern wchar_t* FlagFilePathConfig;
extern HWND H2hWnd;
