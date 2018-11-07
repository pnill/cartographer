
#include "..\Input.h"

#include "H2MOD.h"
#include <Windows.h>


typedef struct DIMOUSESTATE {
	LONG lX;
	LONG lY;
	LONG lZ;
	BYTE rgbButtons[4];
} DIMOUSESTATE, *LPDIMOUSESTATE;

DWORD base;
DWORD rtnAddr = 0;
DIMOUSESTATE* ms;
float* dx;
float* dy;

__declspec(naked) void Input_Calculate_cave(void)
{
	__asm
	{
		POP rtnAddr
		PUSHAD
		PUSHFD
	}

	*dx = -1 * (float)ms->lX * 0.00075;
	*dy = -1 * (float)ms->lY * 0.00075;

	__asm
	{
		POPFD
		POPAD

		FSTP dword ptr [esp+0x20]
		ADD esp,0xC
		MOVSS xmm0,[esp+0x14]

		PUSH rtnAddr
		ret
	}
}

void Input::Tweaks::Init::RawMouseInput()
{
	if (h2mod->Server)
		return;

	ms = (DIMOUSESTATE*)(h2mod->GetBase() + 0x47A570);
	BYTE assmNop[8] = { 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90 };

	DWORD dwBack;

	dx = (float*)(h2mod->GetBase() + 0x4AE610);
	VirtualProtect(dx, 4, PAGE_EXECUTE_READWRITE, &dwBack);

	dy = (float*)(h2mod->GetBase() + 0x4AE614);
	VirtualProtect(dy, 4, PAGE_EXECUTE_READWRITE, &dwBack);

	auto setDx = (h2mod->GetBase() + 0x627CC);
	VirtualProtect((LPVOID)setDx, 8, PAGE_EXECUTE_READWRITE, &dwBack);
	WriteBytes(base + 0x627CC, assmNop, 8);

	auto setDy = (h2mod->GetBase() + 0x62802);
	VirtualProtect((LPVOID)setDy, 8, PAGE_EXECUTE_READWRITE, &dwBack);
	WriteBytes(base + 0x62802, assmNop, 8);

	auto setDx2 = (h2mod->GetBase() + 0x627E7);
	VirtualProtect((LPVOID)setDx2, 8, PAGE_EXECUTE_READWRITE, &dwBack);
	WriteBytes(h2mod->GetBase() + 0x627E7, assmNop, 8);

	Codecave(h2mod->GetBase() + 0x622AA, Input_Calculate_cave, 0x8);
}

void Input::Tweaks::SetSens(Input::Type input_type, int sens)
{
	if (h2mod->Server)
		return;

	if (sens < 0)
		return;

	int absSensIndex = sens - 1;

	if (input_type == CONTROLLER) 
	{
		*reinterpret_cast<float*>(h2mod->GetBase() + 0x4A89BC) = 40.0f + 10.0f * static_cast<float>(absSensIndex); //y-axis
		*reinterpret_cast<float*>(h2mod->GetBase() + 0x4A89B8) = 80.0f + 20.0f * static_cast<float>(absSensIndex); //x-axis
	}
	else if (input_type == MOUSE) 
	{
		*reinterpret_cast<float*>(h2mod->GetBase() + 0x4A89B4) = 25.0f + 10.0f * static_cast<float>(absSensIndex); //y-axis
		*reinterpret_cast<float*>(h2mod->GetBase() + 0x4A89B0) = 50.0f + 20.0f * static_cast<float>(absSensIndex); //x-axis
	}
}