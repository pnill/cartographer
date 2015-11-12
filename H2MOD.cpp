#include <windows.h>
#include <iostream>
#include <sstream>
#include "H2MOD.h"
#include "Network.h"
#include "xliveless.h"

H2MOD *h2mod = new H2MOD();

//int(__cdecl *kill_player)(int) = (int(__cdecl*)(int))GetModuleHandle(L"halo2.exe")+0x13B514;
int(__cdecl* set_gravity)(float);
int(__cdecl* engine_unit_kill)(int unit);
int(__cdecl* engine_set_camera_fov)(float fov, short index);
int(__cdecl* engine_set_active_camo)(int unit, bool state, float time);

int H2MOD::SetGravity(float Gravity)
{
	set_gravity = (int(__cdecl *)(float))(this->GetBase() + 0xB3C00);

	
	return set_gravity(Gravity);
}

int H2MOD::get_unit_from_player_index(int pIndex)
{
	int unit = 0;
	DWORD player_table_ptr = *(DWORD*)(this->GetBase() + 0x004A8260);
	player_table_ptr += 0x44;

    unit = (int)*(short*)(*(DWORD*)player_table_ptr + (pIndex * 0x204) + 0x28);



	//DWORD object_ptr = *(DWORD*)(*(DWORD*)(this->GetBase() + 0x004E461C) + 0x44);

//	TRACE("object_ptr: %08X", object_ptr);

	/*for (int i = 0; i < 2048; i++)
	{
		DWORD object_identifier_ptr = *(DWORD*)(object_ptr + (i * 12) + 8);
		TRACE("object_identifier_ptr: %08X", object_identifier_ptr);

		if (object_identifier_ptr > 0)
		{
			TRACE("object_identifier_referenced: %08X", *(DWORD*)object_identifier_ptr);
			TRACE("object_identifier: %08X", object_identifer);

			if (*(DWORD*)(object_identifier_ptr + 0x3F8) == object_identifer)
			{

				unit = i;
				break;
			}
		}*/

	//}

	return unit;
}

int H2MOD::unit_set_active_camo(int unit)
{
	engine_set_active_camo = (int(__cdecl*)(int,bool,float))(this->GetBase() + 0x184555);

	return engine_set_active_camo(unit,true,90000.0f);
}

int H2MOD::unit_kill(int unit)
{
	engine_unit_kill = (int(__cdecl*)(int))(this->GetBase() + 0x13B514);
	

	return engine_unit_kill(unit);
}

int H2MOD::camera_set_fov(float fov, short index)
{
	engine_set_camera_fov = (int(__cdecl *)(float,short))(this->GetBase() + 0x97CC9);

	return engine_set_camera_fov(fov, index);
}
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

	TRACE_GAME("H2MOD - Initialized v0.1a");
	TRACE_GAME("H2MOD - BASE ADDR %08X", this->Base);
	TRACE_GAME("H2MOD - Initializing H2MOD Network handlers");

	Network::Initialize();
}

DWORD H2MOD::GetBase()
{
	return this->Base;
}