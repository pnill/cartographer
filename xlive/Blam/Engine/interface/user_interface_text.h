#pragma once
#include "math/color_math.h"

/* macro defines */

#define K_PRIVATE_USE_CHARACTER_SPACE_START 0xE000u
#define K_PRIVATE_USE_CHARACTER_SPACE_END 0xF8FFu
#define TO_PRIVATE_CHAR(id) ((K_PRIVATE_USE_CHARACTER_SPACE_START + (id)))

/* enums */

enum e_private_use_characters
{
	_private_use_character_a_button = TO_PRIVATE_CHAR(0x100),
	_private_use_character_b_button = TO_PRIVATE_CHAR(0x101),
	_private_use_character_x_button = TO_PRIVATE_CHAR(0x102),
	_private_use_character_y_button = TO_PRIVATE_CHAR(0x103),
	_private_use_character_black_button = TO_PRIVATE_CHAR(0x104),
	_private_use_character_white_button = TO_PRIVATE_CHAR(0x105),
	_private_use_character_left_trigger = TO_PRIVATE_CHAR(0x106),
	_private_use_character_right_trigger = TO_PRIVATE_CHAR(0x107),
	_private_use_character_dpad_up = TO_PRIVATE_CHAR(0x108),
	_private_use_character_dpad_down = TO_PRIVATE_CHAR(0x109),
	_private_use_character_dpad_left = TO_PRIVATE_CHAR(0x10A),
	_private_use_character_dpad_right = TO_PRIVATE_CHAR(0x10B),
	_private_use_character_start_button = TO_PRIVATE_CHAR(0x10C),
	_private_use_character_back_button = TO_PRIVATE_CHAR(0x10D),
	_private_use_character_left_thumb = TO_PRIVATE_CHAR(0x10E),
	_private_use_character_right_thumb = TO_PRIVATE_CHAR(0x10F),
	_private_use_character_left_stick = TO_PRIVATE_CHAR(0x110),
	_private_use_character_right_stick = TO_PRIVATE_CHAR(0x111),
	_private_use_character_mapname = TO_PRIVATE_CHAR(0x408),
	_private_use_character_gametype = TO_PRIVATE_CHAR(0x409),
	_private_use_character_variant = TO_PRIVATE_CHAR(0x40A),
	_private_use_character_hopper_name = TO_PRIVATE_CHAR(0x43A),
	_private_use_character_hopper_description = TO_PRIVATE_CHAR(0x43B),
	_private_use_character_motion_sensor_enabled = TO_PRIVATE_CHAR(0x40B),
	_private_use_character_teams_enabled = TO_PRIVATE_CHAR(0x40C),
	_private_use_character_vehicle_set = TO_PRIVATE_CHAR(0x40D),
	_private_use_character_weapon_set = TO_PRIVATE_CHAR(0x40E),
	_private_use_character_player0_profile_name = TO_PRIVATE_CHAR(0x40F),
	_private_use_character_player1_profile_name = TO_PRIVATE_CHAR(0x410),
	_private_use_character_player2_profile_name = TO_PRIVATE_CHAR(0x411),
	_private_use_character_player3_profile_name = TO_PRIVATE_CHAR(0x412),
	_private_use_character_player0_gamertag = TO_PRIVATE_CHAR(0x429),
	_private_use_character_player1_gamertag = TO_PRIVATE_CHAR(0x42A),
	_private_use_character_player2_gamertag = TO_PRIVATE_CHAR(0x42B),
	_private_use_character_player3_gamertag = TO_PRIVATE_CHAR(0x42C),
	_private_use_character_countdown = TO_PRIVATE_CHAR(0x413),
	_private_use_character_name_of_last_person_to_delay_countdown = TO_PRIVATE_CHAR(0x414),
	_private_use_character_build_number = TO_PRIVATE_CHAR(0x415),
	_private_use_character_leader = TO_PRIVATE_CHAR(0x416),
	_private_use_character_qprs = TO_PRIVATE_CHAR(0x417),
	_private_use_character_qprr = TO_PRIVATE_CHAR(0x418),
	_private_use_character_qpimi = TO_PRIVATE_CHAR(0x419),
	_private_use_character_qpime = TO_PRIVATE_CHAR(0x41A),
	_private_use_character_qbwd = TO_PRIVATE_CHAR(0x41B),
	_private_use_character_qbwu = TO_PRIVATE_CHAR(0x41C),
	_private_use_character_qmaxm = TO_PRIVATE_CHAR(0x41D),
	_private_use_character_qebw = TO_PRIVATE_CHAR(0x432),
	_private_use_character_qnat = TO_PRIVATE_CHAR(0x41E),
	_private_use_character_active_multiplayer_protocol = TO_PRIVATE_CHAR(0x41F),
	_private_use_character_live_ui_driver_gamertag = TO_PRIVATE_CHAR(0x420),
	_private_use_character_battle_rifle = TO_PRIVATE_CHAR(0x112),
	_private_use_character_bomb = TO_PRIVATE_CHAR(0x113),
	_private_use_character_brute_shot = TO_PRIVATE_CHAR(0x114),
	_private_use_character_carbine = TO_PRIVATE_CHAR(0x115),
	_private_use_character_flag = TO_PRIVATE_CHAR(0x116),
	_private_use_character_flak_cannon = TO_PRIVATE_CHAR(0x117),
	_private_use_character_flamethrower = TO_PRIVATE_CHAR(0x118),
	_private_use_character_fuel_rod = TO_PRIVATE_CHAR(0x119),
	_private_use_character_magnum = TO_PRIVATE_CHAR(0x11A),
	_private_use_character_needler = TO_PRIVATE_CHAR(0x11B),
	_private_use_character_oddball = TO_PRIVATE_CHAR(0x11C),
	_private_use_character_plasma_pistol = TO_PRIVATE_CHAR(0x11D),
	_private_use_character_plasma_rifle = TO_PRIVATE_CHAR(0x11E),
	_private_use_character_plasma_sword = TO_PRIVATE_CHAR(0x11F),
	_private_use_character_rocket_launcher = TO_PRIVATE_CHAR(0x120),
	_private_use_character_shotgun = TO_PRIVATE_CHAR(0x121),
	_private_use_character_smg = TO_PRIVATE_CHAR(0x122),
	_private_use_character_silenced_smg = TO_PRIVATE_CHAR(0x12B),
	_private_use_character_sniper = TO_PRIVATE_CHAR(0x123),
	_private_use_character_beam_rifle = TO_PRIVATE_CHAR(0x124),
	_private_use_character_disintegrator = TO_PRIVATE_CHAR(0x125),
	_private_use_character_sentinel_beam_weapon = TO_PRIVATE_CHAR(0x126),
	_private_use_character_sentinel_needle_weapon = TO_PRIVATE_CHAR(0x127),
	_private_use_character_sentinel_grenade_weapon = TO_PRIVATE_CHAR(0x128),
	_private_use_character_sentinel_charge_weapon = TO_PRIVATE_CHAR(0x129),
	_private_use_character_brute_plasma_rifle = TO_PRIVATE_CHAR(0x12A),
	_private_use_character_hud_quantity = TO_PRIVATE_CHAR(0x422),
	_private_use_character_hud_betraying_player = TO_PRIVATE_CHAR(0x475),
	_private_use_character_ge_round_time_left = TO_PRIVATE_CHAR(0x423),
	_private_use_character_target_player_gamertag = TO_PRIVATE_CHAR(0x424),
	_private_use_character_game_player_count = TO_PRIVATE_CHAR(0x425),
	_private_use_character_coop_level_name = TO_PRIVATE_CHAR(0x426),
	_private_use_character_coop_level_description = TO_PRIVATE_CHAR(0x427),
	_private_use_character_coop_difficulty = TO_PRIVATE_CHAR(0x428),
	_private_use_character_hud_scoreboard_player_score = TO_PRIVATE_CHAR(0x42D),
	_private_use_character_hud_scoreboard_other_player_score = TO_PRIVATE_CHAR(0x42E),
	_private_use_character_hud_scoreboard_variant_name = TO_PRIVATE_CHAR(0x42F),
	_private_use_character_hud_scoreboard_time_left = TO_PRIVATE_CHAR(0x430),
	_private_use_character_edit_player_profile_name = TO_PRIVATE_CHAR(0x431),
	_private_use_character_player_profile_blocks = TO_PRIVATE_CHAR(0x433),
	_private_use_character_game_variant_blocks = TO_PRIVATE_CHAR(0x434),
	_private_use_character_playlist_blocks = TO_PRIVATE_CHAR(0x435),
	_private_use_character_saved_film_blocks = TO_PRIVATE_CHAR(0x436),
	_private_use_character_target_player_clan_name = TO_PRIVATE_CHAR(0x437),
	_private_use_character_hour_12 = TO_PRIVATE_CHAR(0x43E),
	_private_use_character_hour_24 = TO_PRIVATE_CHAR(0x43F),
	_private_use_character_minute = TO_PRIVATE_CHAR(0x440),
	_private_use_character_day = TO_PRIVATE_CHAR(0x441),
	_private_use_character_month = TO_PRIVATE_CHAR(0x442),
	_private_use_character_year = TO_PRIVATE_CHAR(0x443),
	_private_use_character_saved_game_size = TO_PRIVATE_CHAR(0x444),
	_private_use_character_total_minimal_storage_blocks = TO_PRIVATE_CHAR(0x445),
	_private_use_character_button_jump = TO_PRIVATE_CHAR(0x446),
	_private_use_character_button_switch_grenade = TO_PRIVATE_CHAR(0x447),
	_private_use_character_button_action_reload = TO_PRIVATE_CHAR(0x448),
	_private_use_character_button_switch_weapon = TO_PRIVATE_CHAR(0x449),
	_private_use_character_button_pickup_weapon = TO_PRIVATE_CHAR(0x45A),
	_private_use_character_button_take_weapon = TO_PRIVATE_CHAR(0x45B),
	_private_use_character_button_melee_attack = TO_PRIVATE_CHAR(0x44A),
	_private_use_character_button_flashlight = TO_PRIVATE_CHAR(0x44B),
	_private_use_character_button_throw_grenade = TO_PRIVATE_CHAR(0x44C),
	_private_use_character_button_fire = TO_PRIVATE_CHAR(0x44D),
	_private_use_character_button_start = TO_PRIVATE_CHAR(0x44E),
	_private_use_character_button_back = TO_PRIVATE_CHAR(0x44F),
	_private_use_character_button_stealth = TO_PRIVATE_CHAR(0x45F),
	_private_use_character_button_chat = TO_PRIVATE_CHAR(0x460),
	_private_use_character_button_crouch = TO_PRIVATE_CHAR(0x450),
	_private_use_character_button_scope_zoom_in = TO_PRIVATE_CHAR(0x452),
	_private_use_character_button_scope_zoom_out = TO_PRIVATE_CHAR(0x453),
	_private_use_character_button_scope_zoom = TO_PRIVATE_CHAR(0x451),
	_private_use_character_button_lean_left = TO_PRIVATE_CHAR(0x454),
	_private_use_character_button_lean_right = TO_PRIVATE_CHAR(0x455),
	_private_use_character_button_touch_device = TO_PRIVATE_CHAR(0x461),
	_private_use_character_button_banshee_bomb = TO_PRIVATE_CHAR(0x45E),
	_private_use_character_button_trick = TO_PRIVATE_CHAR(0x458),
	_private_use_character_button_e_brake = TO_PRIVATE_CHAR(0x45D),
	_private_use_character_button_brake = TO_PRIVATE_CHAR(0x459),
	_private_use_character_button_speed_boost = TO_PRIVATE_CHAR(0x45C),
	_private_use_character_button_accept = TO_PRIVATE_CHAR(0x456),
	_private_use_character_button_cancel = TO_PRIVATE_CHAR(0x457),
	_private_use_character_button_roster = TO_PRIVATE_CHAR(0x493),
	_private_use_character_button_verb1 = TO_PRIVATE_CHAR(0x47C),
	_private_use_character_button_verb2 = TO_PRIVATE_CHAR(0x47D),
	_private_use_character_button_verb3 = TO_PRIVATE_CHAR(0x47E),
	_private_use_character_button_verb4 = TO_PRIVATE_CHAR(0x47F),
	_private_use_character_stick_move = TO_PRIVATE_CHAR(0x473),
	_private_use_character_stick_look = TO_PRIVATE_CHAR(0x474),
	_private_use_character_animating_left_thumbstick = TO_PRIVATE_CHAR(0x476),
	_private_use_character_animating_right_thumbstick = TO_PRIVATE_CHAR(0x477),
	_private_use_character_matchmaking_countdown = TO_PRIVATE_CHAR(0x478),
	_private_use_character_new_content = TO_PRIVATE_CHAR(0x479),
	_private_use_character_map_load_percent = TO_PRIVATE_CHAR(0x47A),
	_private_use_character_button_dual_fire = TO_PRIVATE_CHAR(0x47B),
	_private_use_character_button_flip_vehicle = TO_PRIVATE_CHAR(0x466),
	_private_use_character_button_enter_vehicle = TO_PRIVATE_CHAR(0x467),
	_private_use_character_button_evict_from_vehicle = TO_PRIVATE_CHAR(0x469),
	_private_use_character_button_board_vehicle = TO_PRIVATE_CHAR(0x468),
	_private_use_character_button_pickup_dual_wield = TO_PRIVATE_CHAR(0x46A),
	_private_use_character_training_looklong_grouping = TO_PRIVATE_CHAR(0x480),
	_private_use_character_training_looklongH_grouping = TO_PRIVATE_CHAR(0x481),
	_private_use_character_training_move_grouping = TO_PRIVATE_CHAR(0x482),
	_private_use_character_training_moveURL_grouping = TO_PRIVATE_CHAR(0x483),
	_private_use_character_button_move_forward = TO_PRIVATE_CHAR(0x46B),
	_private_use_character_button_move_backward = TO_PRIVATE_CHAR(0x46C),
	_private_use_character_button_strafe_left = TO_PRIVATE_CHAR(0x46D),
	_private_use_character_button_strafe_right = TO_PRIVATE_CHAR(0x46E),
	_private_use_character_button_yaw_left = TO_PRIVATE_CHAR(0x46F),
	_private_use_character_button_yaw_right = TO_PRIVATE_CHAR(0x470),
	_private_use_character_button_pitch_forward = TO_PRIVATE_CHAR(0x471),
	_private_use_character_button_pitch_backward = TO_PRIVATE_CHAR(0x472),
	_private_use_character_grouping_EOS = TO_PRIVATE_CHAR(0x486),
	_private_use_character_looklong_grouping_nested = TO_PRIVATE_CHAR(0x487),
	_private_use_character_move_grouping_nested = TO_PRIVATE_CHAR(0x488),
	_private_use_character_keyboard_button_background0 = TO_PRIVATE_CHAR(0x132),
	_private_use_character_keyboard_button_background1 = TO_PRIVATE_CHAR(0x133),
	_private_use_character_keyboard_button_background2 = TO_PRIVATE_CHAR(0x134),
	_private_use_character_keyboard_button_background3 = TO_PRIVATE_CHAR(0x135),
	_private_use_character_footer_button_bg0_normal = TO_PRIVATE_CHAR(0x136),
	_private_use_character_footer_button_bg1_normal = TO_PRIVATE_CHAR(0x137),
	_private_use_character_footer_button_bg2_normal = TO_PRIVATE_CHAR(0x138),
	_private_use_character_footer_button_bg0_hovered = TO_PRIVATE_CHAR(0x139),
	_private_use_character_footer_button_bg1_hovered = TO_PRIVATE_CHAR(0x13A),
	_private_use_character_footer_button_bg2_hovered = TO_PRIVATE_CHAR(0x13B),
	_private_use_character_gold_only = TO_PRIVATE_CHAR(0x13C),
	_private_use_character_map_download_percent = TO_PRIVATE_CHAR(0x489),
	_private_use_character_proposer = TO_PRIVATE_CHAR(0x48A),
	_private_use_character_yes_votes = TO_PRIVATE_CHAR(0x48B),
	_private_use_character_no_votes = TO_PRIVATE_CHAR(0x48C),
	_private_use_character_vote_booted = TO_PRIVATE_CHAR(0x48D),
	_private_use_character_vote_timer = TO_PRIVATE_CHAR(0x48E),
	_private_use_character_vote_variant = TO_PRIVATE_CHAR(0x48F),
	_private_use_character_vote_mapname = TO_PRIVATE_CHAR(0x490),
	_private_use_character_pending_votes = TO_PRIVATE_CHAR(0x491),
	_private_use_character_resolution_change_seconds_remaining = TO_PRIVATE_CHAR(0x492),
};


/* classes */

class c_user_interface_text
{
protected:
	//void* __vtable;
	int32 m_custom_font_type;
	real_rgb_color m_text_color;
	int16 m_text_is_pulsating;
	int16 field_16;
	datum field_18;
	int32 m_text_justification;
	real32 field_20;
	datum field_24;
	uint8 gap_28[16];
	int32 field_38;
	int16 field_3C;
	int16 text_length;
	int32 m_ui_start_time;

public:
	c_user_interface_text();

	const real_rgb_color* get_color();
	const int32 get_font();

	void set_font(int32 font_type);
	void set_pulsating(bool pulsating);
	void set_color(real_rgb_color* color);
	void set_color(const real_rgb_color* color);

	static bool is_private_use_character(wchar_t character);

	// c_user_interface_text virtual functions

	virtual c_user_interface_text* destructor(uint32 flags);
	virtual void  set_text(const wchar_t* raw_text) = 0;
	virtual void  append_text(const wchar_t* Source) = 0;
	virtual const wchar_t* get_raw_string() = 0;
};
ASSERT_STRUCT_SIZE(c_user_interface_text, 0x44);


/* public methods */

float get_ui_text_label_scale();
void set_ui_text_label_scale(float scale);
bool __cdecl user_interface_parse_string(wchar_t* string, size_t max_length, char a3);

void user_interface_text_apply_hooks();