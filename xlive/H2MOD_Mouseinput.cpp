#include "H2MOD_Mouseinput.h"
#include <Windows.h>
#include "H2MOD.h"
#include "xliveless.h"
#include "Hook.h"

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

__declspec(naked) void CC_Fug(void)
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

void Mouseinput::Initialize()
{
	base = h2mod->GetBase();
	ms = (DIMOUSESTATE*)(base + 0x47A570);
	BYTE assmNop[8] = { 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90 };

	DWORD dwBack;

	dx = (float*)(base + 0x4AE610);
	VirtualProtect(dx, 4, PAGE_EXECUTE_READWRITE, &dwBack);

	dy = (float*)(base + 0x4AE614);
	VirtualProtect(dy, 4, PAGE_EXECUTE_READWRITE, &dwBack);

	auto setDx = (base + 0x627CC);
	VirtualProtect((LPVOID)setDx, 8, PAGE_EXECUTE_READWRITE, &dwBack);
	WriteBytes(base + 0x627CC, assmNop, 8);

	auto setDy = (base + 0x62802);
	VirtualProtect((LPVOID)setDy, 8, PAGE_EXECUTE_READWRITE, &dwBack);
	WriteBytes(base + 0x62802, assmNop, 8);

	auto setDx2 = (base + 0x627E7);
	VirtualProtect((LPVOID)setDx2, 8, PAGE_EXECUTE_READWRITE, &dwBack);
	WriteBytes(base + 0x627E7, assmNop, 8);

	Codecave(base + 0x622AA, CC_Fug, 0x8);
}