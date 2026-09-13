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
#include "screen_error_dialog.h"
#include "screen_button_settings.h"
#include "screen_controller_settings_menu.h"
#include "screen_multiplayer_video_settings_menu.h"
#include "screen_video_settings.h"
#include "screen_resolution.h"
#include "screen_pause_game.h"

/* public code */

void screens_apply_patches_on_map_load(void)
{
	c_screen_xbox_live_task_progress_dialog::apply_patches_on_map_load();
	c_screen_4way_signin::apply_patches_on_map_load();
	c_screen_multiplayer_pregame_lobby::apply_patches_on_map_load();
	c_screen_squad_settings::apply_patches_on_map_load();
	c_screen_settings::apply_patches_on_map_load();
	c_screen_button_settings_menu::apply_patches_on_ui_map_load();
	c_screen_video_settings::apply_patches_on_map_load();
	return;
}

void screens_apply_patches_on_mp_map_load(void)
{
	c_screen_button_settings_menu::apply_patches_on_mp_map_load();
	return;
}

void screens_apply_patches(void)
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
	c_screen_error_dialog_ok::apply_patches();
	c_screen_error_dialog_ok_cancel::apply_patches();
	c_screen_button_settings_menu::apply_instance_patches();
	c_controller_settings_edit_list::apply_instance_patches();
	c_screen_multiplayer_video_settings::apply_instance_patches();
	c_screen_resolution_menu::apply_instance_patches();
	screen_pause_game_apply_patches();
	return;
}
