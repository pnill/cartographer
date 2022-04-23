#include "stdafx.h"

#include "c_screen_with_menu.h"

#include "CustomMenuGlobals.h"

#include "Util\Hooks\Hook.h"

#pragma region Live list fix for disappearing labels
// derived from c_screen_with_menu, but doesn't really matter
void __thiscall c_screen_with_menu::build_player_list(void* a1, int player_count)
{
	auto p_build_player_list = Memory::GetAddressRelative<void(__thiscall*)(void*, void*, int)>(0x611CD0);

	// we only hook calls done to c_screen_with_menu::build_player_list in c_screen_network_squad_browser

	BYTE* thisx = (BYTE*)this;

	bool network_squad_browser_live_browser = *(bool*)(thisx + 7936);

	if (network_squad_browser_live_browser)
		return; // don't load the player list from data, since we are in the live list, and we don't use the details pane located in the lower right part where players are listed

	return p_build_player_list(this, a1, player_count);
}

__declspec(naked) void jmp_build_player_list() { __asm jmp c_screen_with_menu::build_player_list }

#pragma endregion

void c_screen_with_menu::applyPatches()
{
	if (Memory::IsDedicatedServer()) return;

	PatchCall(Memory::GetAddressRelative(0x619650), jmp_build_player_list);
}
