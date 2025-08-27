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
	// remove checks preventing pick-up messaging from displaying
	// in splitscreen mode
	NopFill(Memory::GetAddress(0x2217BE), 10);
	NopFill(Memory::GetAddress(0x221879), 10);
	NopFill(Memory::GetAddress(0x220DF7), 10);
	NopFill(Memory::GetAddress(0x220DAC), 10);

	// nop call + cmp
	NopFill(Memory::GetAddress(0x5D928), 8);
	// force jmp
	WriteValue(Memory::GetAddress(0x5D930), (uint8)0xEB);

	p_text_scale_factor = get_secondary_hud_scale();
	Codecave(Memory::GetAddress(0x22D29E), ui_hud_left_messaging_top_scale, 3);
	return;
}

void __cdecl hud_messaging_update(int32 user_index)
{
	INVOKE(0x22D1BD, 0x0, hud_messaging_update, user_index);
	return;
}

void hud_messaging_add_message_plural(int32 user_index, wchar_t* display_string, wchar_t* plural_string, int32 a4)
{
	INVOKE(0x22DD6F, 0, hud_messaging_add_message_plural, user_index, display_string, plural_string, a4);
}

void hud_messaging_add_message(int32 user_index, wchar_t* display_string)
{
	INVOKE(0x22DE21, 0, hud_messaging_add_message, user_index, display_string);
}

void hud_messaging_display_primary_text(int32 user_index, wchar_t* primary_string, int32 display_ticks)
{
	INVOKE(0x22CD3C, 0, hud_messaging_display_primary_text, user_index, primary_string, display_ticks);
}

void __cdecl hud_messaging_clear(void)
{
	INVOKE(0x22CE83, 0x206863, hud_messaging_clear);
	return;
}

void __cdecl hud_messaging_post(int32 user_index, string_id string_id)
{
	INVOKE(0x22DEA4, 0x206BB7, hud_messaging_post, user_index, string_id);
	return;
}
