#include "stdafx.h"
#include "screens_patches.h"

#include "screen_esrb_warning.h"
#include "screen_xbox_live_task_progress_dialog.h"
#include "screen_4way_signin.h"
#include "screen_main_menu.h"
#include "screen_multiplayer_pregame_lobby.h"
#include "screen_press_start_introduction.h"
#include "screen_squad_settings.h"
#include "screen_settings.h"
#include "screen_network_squad_browser.h"
#include "screen_single_player_profile_select.h"
#include "screen_virtual_keyboard.h"

void screens_apply_patches_on_map_load()
{
	c_screen_xbox_live_task_progress_dialog::apply_patches_on_map_load();
	c_screen_4way_signin::apply_patches_on_map_load();
	c_screen_multiplayer_pregame_lobby::apply_patches_on_map_load();
	c_screen_squad_settings::apply_patches_on_map_load();
	c_screen_settings::apply_patches_on_map_load();
}

void screens_apply_patches()
{
	c_screen_esrb_warning::apply_patches();
	c_screen_4way_signin::apply_instance_patches();
	c_main_menu_list::apply_instance_patches();
	c_screen_multiplayer_pregame_lobby::apply_instance_patches();
	c_screen_squad_settings::apply_instance_patches();
	c_screen_network_squad_browser::apply_patches();
	c_screen_press_start_introduction::apply_instance_patches();
	c_player_profile_list::apply_instance_patches();
	c_screen_virtual_keyboard::apply_patches();
}
