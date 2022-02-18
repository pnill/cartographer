#include "stdafx.h"

#include "c_screen_with_menu.h"
#include "CustomMenuGlobals.h"

#include "Util\Hooks\Hook.h"

int __fastcall c_screen_with_menu_title_desc(int a1, DWORD _EDX, char a2)
{
	c_screen_with_menu *screen_with_menu = *(c_screen_with_menu**)(a1);

	return sub_2111ab_CMLTD_((int)a1, a2, screen_with_menu->menu_id, screen_with_menu->label_id_title, screen_with_menu->label_id_description);
}

int __fastcall c_screen_with_menu_buttonpress(c_screen_with_menu* a1, DWORD _EDX)
{
	return sub_20F790_CM_((int)a1, 0);
}

int __fastcall c_screen_with_menu_keyhandler_outer(c_screen_with_menu* a1, DWORD _EDX, int a2)
{
	DWORD engine_state = *(DWORD*)((int)*(int*)((DWORD)H2BaseAddr + 0x482D3C) + 0x8);
	DWORD keyhandler_offset = 0;

	if (engine_state == 3)
		keyhandler_offset = 0x20EEBE; // main menu
	else
		keyhandler_offset = 0x23D8AE; // in-game pause menu function

	int(__thiscall* keyhandler_cm_func)(int, int) = (int(__thiscall*)(int, int))((char*)H2BaseAddr + keyhandler_offset);

	return keyhandler_cm_func((int)a1, a2);
}


int __fastcall c_screen_with_menu_keyhandler(c_screen_with_menu* a1, DWORD _EDX, int a2)
{
	DWORD engine_state = *(DWORD*)((int)*(int*)((DWORD)H2BaseAddr + 0x482D3C) + 0x8);
	DWORD keyhandler_offset = 0;
	
	if (engine_state == 3) 
		keyhandler_offset = 0x20EEBE; // normal menu
	else
		keyhandler_offset = 0x24DC0D; // in-game pause menu function

	int(__thiscall* keyhandler_cm_func)(int,int) = (int(__thiscall*)(int,int))((char*)H2BaseAddr + keyhandler_offset);
	
	return keyhandler_cm_func((int)a1, a2);
}

#pragma region Live list fix for broken labels
void __thiscall c_screen_with_menu::build_player_list(void* a1, int player_count)
{
	auto p_build_player_list = Memory::GetAddressRelative<void(__thiscall*)(void* thisx, void* a1, int player_count)>(0x611CD0);

	// we only hook calls done to c_screen_with_menu::build_player_list in c_screen_network_squad_browser

	BYTE* thisx = (BYTE*)this;

	bool network_squad_browser_live_browser = *(bool*)(thisx + 7936);

	if (network_squad_browser_live_browser)
		return; // don't list the player list, since we are in the live list, and we don't use the details pane located in the lower right part where players are listed

	return p_build_player_list(this, a1, player_count);
}

__declspec(naked) void jmp_build_player_list() { __asm jmp c_screen_with_menu::build_player_list }

#pragma endregion

void c_screen_with_menu::applyPatches()
{
	if (Memory::isDedicatedServer()) return;

	PatchCall(Memory::GetAddressRelative(0x619650), jmp_build_player_list);
}
