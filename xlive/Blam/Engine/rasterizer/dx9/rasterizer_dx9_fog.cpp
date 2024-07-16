#include "stdafx.h"
#include "rasterizer_dx9_fog.h"

static real32 fog_wind_x_axis_velocity_factor1 = 30.0f;

void __declspec(naked) rasterizer_fog_constant_velocity_update()
{
#define k_delta_time_stack_location (-10h)
	__asm
	{
		pop eax

		// esi - atmospheric fog ptr, [esp+1Ch + k_delta_time_stack_location] - dt
		// edi - tag definition data
		// xmm0 to this point dt as well
		movss xmm1, dword ptr[edi+44h]
		mulss xmm1, fog_wind_x_axis_velocity_factor1
		movss xmm2, dword ptr[esi]
		mulss xmm2, dword ptr[esi + 10h]
		addss xmm1, xmm2
		mulss xmm1, xmm0 // multiply by dt
		movss dword ptr[ebp], xmm1

		// repeat the code block above 1 more time
		movss xmm1, dword ptr[edi + 44h + 4h]
		mulss xmm1, fog_wind_x_axis_velocity_factor1
		movss xmm2, dword ptr[esi + 4]
		mulss xmm2, dword ptr[esi + 10h]
		addss xmm1, xmm2
		mulss xmm1, xmm0 // multiply by dt
		movss dword ptr[ebp + 4], xmm1

		// this block without addss
		movss xmm1, dword ptr[edi + 44h + 8h]
		mulss xmm1, fog_wind_x_axis_velocity_factor1
		mulss xmm1, xmm0 // multiply by dt
		movss dword ptr[ebp + 8], xmm1

		pop edi
		pop esi

		// push return back
		push eax
		retn
	}
#undef k_delta_time_stack_location
}

void rasterizer_fog_apply_patches()
{
	Codecave(Memory::GetAddress(0x2773B5), rasterizer_fog_constant_velocity_update, 42);
	// xorps xmm0, xmm0, 2 nops
	BYTE xmm0_clear[] = { 0x0F, 0x57, 0xC0, 0x90, 0x90 };
	WriteBytes(Memory::GetAddress(0x2776D4), xmm0_clear, sizeof(xmm0_clear));
	WriteBytes(Memory::GetAddress(0x2776E8), xmm0_clear, sizeof(xmm0_clear));
	WriteBytes(Memory::GetAddress(0x277700), xmm0_clear, sizeof(xmm0_clear));

	return;
}