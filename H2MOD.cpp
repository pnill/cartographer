#include <windows.h>
#include <iostream>
#include <sstream>
#include "H2MOD.h"
#include "Network.h"
#include "xliveless.h"

H2MOD *h2mod = new H2MOD();




void H2MOD::Initialize()
{

	if (GetModuleHandleW(L"H2Server.exe"))
	{
		this->Base = (DWORD)GetModuleHandleW(L"H2Server.exe");
		this->Server = TRUE;
	}
	else
	{
		this->Base = (DWORD)GetModuleHandleW(L"halo2.exe");
		this->Server = FALSE;
	}

	//TRACE_GAME("H2MOD - Initialized v0.1a");
	//TRACE_GAME("H2MOD - BASE ADDR %08X", this->Base);
	//TRACE_GAME("H2MOD - Initializing H2MOD Network handlers");

	Network::Initialize();
}

DWORD H2MOD::GetBase()
{
	return this->Base;
}