#include "stdafx.h"
#include "screen_network_squad_browser.h"

#pragma region Live list fix for disappearing labels
void c_screen_network_squad_browser::build_players_list_fix(c_player_widget_representation* representations, int32 player_count)
{
	if (m_live_list)
	{
		// don't load the player list from data, 
		// since we are in the live list, and we don't use the details pane located in the lower right part where players are listed
		return;
	}

	apply_new_representations_to_players(representations, player_count);
}

__declspec(naked) void jmp_build_player_list() { __asm jmp c_screen_network_squad_browser::build_players_list_fix }
#pragma endregion

void* c_screen_network_squad_browser::load(s_screen_parameters* parameters)
{
	return INVOKE(0x21A238, 0x0, c_screen_network_squad_browser::load, parameters);
}

void c_screen_network_squad_browser::apply_patches()
{
	if (Memory::IsDedicatedServer()) return;

	PatchCall(Memory::GetAddressRelative(0x619650), jmp_build_player_list);
}
