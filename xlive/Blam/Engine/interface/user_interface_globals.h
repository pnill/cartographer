#pragma once
#include "user_interface_widget.h"

#include "tag_files/tag_block.h"
#include "tag_files/tag_reference.h"
#include "saved_games/game_variant.h"
#include "saved_games/player_profile.h"

#include "dialog_channel.h"
#include "game_error_dialog_channel.h"
#include "gameshell_background_channel.h"
#include "gameshell_channel.h"
#include "hardware_error_dialog_channel.h"
#include "user_interface_channel.h"
#include "user_interface_main_menu_music.h"
#include "user_interface_text_chat_receiver.h"
#include "user_interface_text_chat_sender.h"

/* forward declarations */

enum e_scenario_type : int16;


/* enums */

enum
{
	k_maximum_number_of_active_screens = 36,
};



/* structures */

struct s_user_interface_tag_globals
{
	// Explaination("Shared Globals", "This is a reference to the ui shared globals tag")
	tag_reference shared_globals;	// wigl
	
	// Explaination("Screen Widgets", "These are the screen widgets")
	tag_block<s_user_interface_widget_reference> screen_widgets;

	// Explaination("Multiplayer Variant Settings Interface", "This blob defines the ui for setting multiplayer game variant parameters")
	tag_reference mp_variant_settings_ui;	// goof
	
	// Explaination("Game Hopper Localization Strings", "This is for the loc game hopper strings")
	tag_reference game_hopper_descriptions;	// unic
};
ASSERT_STRUCT_SIZE(s_user_interface_tag_globals, 32);

#pragma pack(push,1)
struct s_user_interface_globals
{
	int32 field_0;
	bool game_shell_active;
	bool render_title_safe_bounds;
	bool render_screen_tag_path;
	bool render_element_bounds;
	bool build_is_beta;
	int8 gap_9[3];
	int32 field_C;
	/*e_scenario_type*/int8 map_type;
	int8 gap_11[3];
	float m_near_clip_distance;
	float m_projection_plane_distance;
	float m_far_clip_distance;
	int32 field_20;
	int32 field_24;
	int32 field_28;
	c_gameshell_background_channel gameshell_background_channel;
	c_gameshell_channel gameshell_channel[k_number_of_render_windows];
	c_user_interface_channel dialog_channel[k_number_of_render_windows];
	c_game_error_dialog_channel game_error_channel[k_number_of_render_windows];
	c_dialog_channel dialog_history_channel[k_number_of_render_windows];
	c_hardware_error_dialog_channel hardware_errror_channel;
	c_user_interface_channel virtual_keyboard_channel;
	c_screen_widget* screen_collection[k_maximum_number_of_active_screens];
	int8 field_6A8[104];
	int32 online_task_datum;
	int32 field_714;
	int8 field_718[1694];
	int8 field_DB6[142];
	int32 field_E44;
	int32 field_E48;
	int8 gap_E4C[8];
	int32 edit_saved_game_variant_index;
	s_game_variant edit_saved_game_variant;
	int32 m_controller_index;
	int32 edit_player_profile_index;
	s_saved_game_player_profile m_player_profile;
	int8 field_2198;
	int8 gap_2199[3];
	int32 field_219C;
	int32 m_game_campaign_id;
	int32 m_game_map_id;
	int32 m_game_difficulty;
	bool load_from_persistent_storage;
	int8 field_21AD;
	int32 current_year;
	int32 field_21B2;
	int32 time_in_hours;
	int32 field_21BA;
	int8 gap_21BE[2];
	c_user_interface_main_menu_music main_menu_music;
	int32 field_21D8;
	bool xbox_live_active;
	int8 gap_21DD[3];
	c_user_interface_text_chat_sender text_chat_sender;
	c_user_interface_text_chat_receiver text_chat_receiver;
};
#pragma pack(pop)
ASSERT_STRUCT_SIZE(s_user_interface_globals, 0x3FED);

/* prototypes */

/* gets the current loaded user_interface_globals tag aka wgtz depending on the scenario type */
static s_user_interface_tag_globals* _cdecl user_interface_get_tag_globals(void)
{
	// dives into globals/globals tag and gets the block "Interface Tags"
	// from there checks current scnr type and itself returns a pointer for the correct one
	return INVOKE(0x20BB1D, 0x1F2C5A, user_interface_get_tag_globals);
}

bool __cdecl user_interface_globals_is_beta_build();
int32 __cdecl user_interface_globals_get_game_difficulty();
int32 __cdecl user_interface_globals_get_edit_player_profile_index();
struct s_saved_game_player_profile* user_interface_globals_get_edit_player_profile();
e_scenario_type __cdecl user_interface_globals_get_map_type();
void __cdecl user_interface_globals_set_game_difficulty_real(int32 difficulty);
void __cdecl user_interface_globals_set_loading_from_persistent_storage(bool a1);
void __cdecl user_interface_globals_commit_edit_profile_changes();
void __cdecl user_interface_globals_save_profile_changes_to_disk();
void __cdecl user_interface_globals_finish_saving_profile_changes();
void __cdecl user_interface_globals_set_edit_player_profile(e_controller_index controller_index, uint32 profile_index, struct s_saved_game_player_profile* profile);
void __cdecl user_interface_globals_save_edit_profile_to_disk();
s_user_interface_globals* user_interface_globals_get(void);