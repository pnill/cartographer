#pragma once
#include "interface/user_interface_widget_window.h"

class c_screen_network_squad_browser : c_screen_with_menu
{
	char gap_A60[0x14A0];
	bool m_live_list;
	char gap_1F04[11];

	void build_players_list_fix(c_player_widget_representation* representations, int32 player_count);
public:
	static void apply_patches();
};
ASSERT_STRUCT_SIZE(c_screen_network_squad_browser, 0x1F0C);