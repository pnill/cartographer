#include "Mouseinput.h"

#include "..\Util\Hooks\Hook.h"
#include "H2MOD/Modules/Config/Config.h"

#include "Blam/Engine/Game/GameTimeGlobals.h"

typedef struct DIMOUSESTATE {
	LONG lX;
	LONG lY;
	LONG lZ;
	BYTE rgbButtons[4];
} DIMOUSESTATE, *LPDIMOUSESTATE;

bool b_raw_init = false;
DWORD base;
DWORD rtnAddr = 0;
DIMOUSESTATE* ms;
float* dx;
float* dy;
BYTE assmNop[8] = { 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90 };
DWORD dwBack;

BYTE o_SetDX[8];
BYTE o_SetDY[8];
BYTE o_SetDX2[8];

typedef char(__cdecl p_mouse_input)(int local_player_index, void *data, int a4, float *a5, float *a6, void *a7);
p_mouse_input* c_mouse_input;

char __cdecl mouse_input(int local_player_index, void *data, int a4, float *a5, float *a6, void *a7)
{
	time_globals* time = time_globals::get();
	if(H2Config_raw_input)
	{
		if (!b_raw_init) {
			MouseInput::SetSensitivity(1);
			WriteBytes(base + 0x627CC, assmNop, 8);
			WriteBytes(base + 0x62802, assmNop, 8);
			WriteBytes(base + 0x627E7, assmNop, 8);
			b_raw_init = true;
		}
		*dx = time->seconds_per_tick * (float)ms->lX * -(H2Config_raw_mouse_scale / 100);
		*dy = time->seconds_per_tick * (float)ms->lY * -(H2Config_raw_mouse_scale / 100);
	} 
	else
	{
		if(b_raw_init)
		{
			MouseInput::SetSensitivity(H2Config_mouse_sens);
			WriteBytes(base + 0x627CC, o_SetDX, 8);
			WriteBytes(base + 0x62802, o_SetDY, 8);
			WriteBytes(base + 0x627E7, o_SetDX2, 8);
			b_raw_init = false;
		}
	}
	return c_mouse_input(local_player_index, data, a4, a5, a6, a7);
}

char* MouseInput::GetMouseState()
{
	return Memory::GetAddress<char*>(0x47a570);
}

void MouseInput::SetSensitivity(float value)
{
	
	if (value == 0)
		return;
	float t_value = value;
	if (H2Config_raw_input)
		t_value = 1;
	*Memory::GetAddress<float*>(0x4A89B0) = 50.0f + 20.0f * t_value; //x-axis
	if(!H2Config_mouse_uniform)
		*Memory::GetAddress<float*>(0x4A89B4) = 25.0f + 10.0f * t_value; //y-axis
	else
		*Memory::GetAddress<float*>(0x4A89B4) = 50.0f + 20.0f * t_value; //y-axis
}

void MouseInput::Initialize()
{
	base = Memory::GetAddress(0x0);
	ms = (DIMOUSESTATE*)(base + 0x47A570);
	dx = (float*)(base + 0x4AE610);
	//VirtualProtect(dx, 4, PAGE_EXECUTE_READWRITE, &dwBack);
	dy = (float*)(base + 0x4AE614);
	//VirtualProtect(dy, 4, PAGE_EXECUTE_READWRITE, &dwBack);
	ReadBytesProtected(base + 0x627CC, o_SetDX, 8);
	ReadBytesProtected(base + 0x62802, o_SetDY, 8);
	ReadBytesProtected(base + 0x627E7, o_SetDX2, 8);
	auto setDx = (base + 0x627CC);
	//VirtualProtect((LPVOID)setDx, 8, PAGE_EXECUTE_READWRITE, &dwBack);
	auto setDy = (base + 0x62802);
	//VirtualProtect((LPVOID)setDy, 8, PAGE_EXECUTE_READWRITE, &dwBack);
	auto setDx2 = (base + 0x627E7);
	//VirtualProtect((LPVOID)setDx2, 8, PAGE_EXECUTE_READWRITE, &dwBack);
	c_mouse_input = Memory::GetAddress<p_mouse_input*>(0x61ea2);
	PatchCall(Memory::GetAddress(0x62f65), mouse_input);
	SetSensitivity(H2Config_mouse_sens);
}
