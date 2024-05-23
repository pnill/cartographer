#include "stdafx.h"
#include "hud_messaging.h"

#include "interface/hud.h"

/* globals */

// Pointer to the crosshair and text scale for the below hook
float* p_text_scale_factor;

__declspec(naked) void ui_hud_left_messaging_top_scale()
{
	__asm
	{
		// sp: 1938h
		// mov     dl, [esp + 27h]
		// add     esp, 12

		fild dword ptr [esp + 18h]
		push eax
		mov eax, [p_text_scale_factor]
		fmul dword ptr [eax]
		pop eax
		fistp dword ptr [esp + 18h]

		// original code
		mov     ecx, ebx
		imul    ecx, 4E0h
		retn
	}
}


/* public code */

void hud_messaging_apply_hooks(void)
{
	p_text_scale_factor = get_secondary_hud_scale();
	Codecave(Memory::GetAddress(0x22D29E), ui_hud_left_messaging_top_scale, 3);
	return;
}

void hud_messaging_update(int32 user_index)
{
	INVOKE(0x22D1BD, 0x0, hud_messaging_update, user_index);
	return;
}