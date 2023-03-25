#include "stdafx.h"

#include "Mouseinput.h"
#include "Blam/Engine/game/GameTimeGlobals.h"
#include "H2MOD\Modules\Shell\Config.h"
#include "Util\Hooks\Hook.h"

bool b_raw_init = false;
DWORD base;
DIMOUSESTATE* ms;
float* dx;
float* dy;
BYTE assmNop[8] = { 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90 };

BYTE o_SetDX[8];
BYTE o_SetDY[8];
BYTE o_SetDX2[8];

typedef char(__cdecl mouse_input_t)(int local_player_index, void *data, int a4, float *a5, float *a6, void *a7);
mouse_input_t* p_mouse_input;

char __cdecl mouse_input(int local_player_index, void *data, int a4, float *a5, float *a6, void *a7)
{
	time_globals* time = time_globals::get();
	if (H2Config_raw_input)
	{
		if (!b_raw_init) {
			MouseInput::SetSensitivity(1.0f);
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
		if (b_raw_init)
		{
			MouseInput::SetSensitivity(H2Config_mouse_sens);
			WriteBytes(base + 0x627CC, o_SetDX, 8);
			WriteBytes(base + 0x62802, o_SetDY, 8);
			WriteBytes(base + 0x627E7, o_SetDX2, 8);
			b_raw_init = false;
		}
	}
	return p_mouse_input(local_player_index, data, a4, a5, a6, a7);
}

char* MouseInput::GetMouseState()
{
	return Memory::GetAddress<char*>(0x47a570);
}

void MouseInput::SetSensitivity(float value)
{
	if (value == 0.0f)
		return;
	if (H2Config_raw_input)
		value = 1.0f;

	value = blam_max(value - 1.0f, 0.0f);

	*Memory::GetAddress<float*>(0x4A89B0) = (80.0f + 20.0f * value) - 30.0f; //x-axis

	//y-axis
	if (H2Config_mouse_uniform)
		*Memory::GetAddress<float*>(0x4A89B4) = (80.0f + 20.0f * value) - 30.0f;
	else
		*Memory::GetAddress<float*>(0x4A89B4) = (40.0f + 10.0f * value) - 15.0f;
}

void MouseInput::Initialize()
{
	// DWORD dwBack;
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
	p_mouse_input = Memory::GetAddress<mouse_input_t*>(0x61ea2);
	PatchCall(Memory::GetAddress(0x62f65), mouse_input);
	SetSensitivity(H2Config_mouse_sens);
}
