#pragma once
#include "Blam/Cache/DataTypes/BlamDataTypes.h"
#include "Blam/Cache/TagGroups.hpp"
#include "Blam/Engine/math/color_math.h"


#include "Blam/Engine/interface/new_hud_definitions.h"

enum e_scaling_flags : short
{
	scaling_flag_dont_scale_offset = FLAG(0),
	scaling_flag_dont_scale_size = FLAG(1),
};

enum e_button_icon_flags : byte
{
	button_icon_flag_use_text_from_string_list_instead = FLAG(0),
	button_icon_flag_override_default_color = FLAG(1),
	button_icon_flag_width_offset_is_absolute_icon_width = FLAG(2),
};

#define NUMBER_OF_GAMEPAD_BUTTONS 16
#define NUMBER_OF_GAMEPAD_STICKS 2
struct icon_hud_element_definition
{
	short sequence_index;
	short width_offset;
	point2d offset_from_reference_corner;
	pixel32 override_icon_color;
	byte frame_rate;

	e_button_icon_flags flags;
	short text_index;
};
TAG_BLOCK_SIZE_ASSERT(icon_hud_element_definition, 16);

enum e_flash_flags : short
{
	flash_flag_reverse_default_flashing_colors = FLAG(0),
};

enum e_waypoint_flags : int
{
	waypoint_flag_dont_rotate_when_pointing_offscreen = FLAG(0),
};


#define MAXIMUM_NUMBER_OF_WAYPOINTS 16
struct s_waypoint_arrows_block
{
	static_string32 name;
	int pad_0[2];
	pixel32 color;
	float opacity;
	float translucency;
	short on_screen_sequence_index;
	short off_screen_sequence_index;
	short occluded_sequence_index;
	short pad_1;
	int pad_2[4];

	e_waypoint_flags flags;
	int pad3[6];
};
TAG_BLOCK_SIZE_ASSERT(s_waypoint_arrows_block, 104);

struct hud_globals_definition : TagGroup<'hudg'>
{
	// Explaination("Messaging parameters", "")
	e_hud_anchor anchor;
	short pad_2;
	int pad_4[8];

	point2d anchor_offset;
	float width_scale;
	float height_scale;

	e_scaling_flags scaling_flags;
	short pad_32;
	int pad_34[5];

	// 2 below fields are unused since font packages are used in halo 2 and up
	tag_reference fullscreen_font;		// bitm
	tag_reference splitscreen_font;		// bitm

	float up_time;
	float fade_time;

	real_argb_color icon_color;
	real_argb_color text_color;
	float text_spacing;

	tag_reference item_message_text;	// unic
	tag_reference icon_bitmap;			// bitm
	tag_reference alternate_icon_text;	// unic

	tag_block<icon_hud_element_definition> button_icons;

	// Explaination("HUD HELP TEXT COLOR", "")
	pixel32 help_text_default_color;
	pixel32 help_text_flashing_color;
	float help_text_flash_period;
	float help_text_flash_delay;			// time between flashes
	short help_text_number_of_flashes;

	e_flash_flags help_text_flash_flags;
	float help_text_flash_length;			// time of each flash

	pixel32 help_text_disabled_color;
	int pad_C0;

	// Explaination("Other hud messaging data", "")

	// This is unused
	tag_reference hud_messages;	// hmt


	// Explaination("Objective colors", "")
	pixel32 hud_messaging_data_default_color;
	pixel32 hud_messaging_data_flashing_color;
	float hud_messaging_data_flash_period;
	float hud_messaging_data_flash_delay;			// time between flashes
	short hud_messaging_data_number_of_flashes;

	e_flash_flags hud_messaging_data_flash_flags;
	float hud_messaging_data_flash_length;			// time of each flash
	pixel32 hud_messaging_disabled_color;
	short hud_messaging_data_uptime_ticks;
	short hud_messaging_data_fade_ticks;

	// Explaination("Waypoint parameters", "The offset values are how much the waypoint rectangle border is offset from the safe camera bounds")
	float waypoint_top_offset;
	float waypoint_bottom_offset;
	float waypoint_left_offset;
	float waypoint_right_offset;
	int pad_FC[8];

	tag_reference arrow_bitmap;
	tag_block<s_waypoint_arrows_block> waypoint_arrows;

	int pad_12C[20];
	float hud_scale_in_multiplayer;
	int pad_180[64];

	// Explaination("Hud globals", "")
	int pad_280[4];
	float motion_sensor_range;
	float motion_sensor_velocity_sensitivity;		// how fast something moves to show up on the motion sensor
	float motion_sensor_scale;						// DONTTOUCHEVER
	rectangle2d default_chapter_title_bounds;

	int pad_2A4[11];

	// Explaination("Hud damage indicators", "")
	short top_offset;
	short bottom_offset;
	short left_offset;
	short right_offset;

	int pad_2D8[8];

	// bitm
	tag_reference damage_indicator_bitmap;
	short damage_indicator_sequence_index;
	short damage_indicator_multiplayer_sequence_index;
	pixel32 hud_damage_color;
	int pad_308[4];

	// Explaination("Hud timer definitions", "")

	// Explaination("Not much time left flash color", "")
	pixel32 not_much_time_left_default_color;
	pixel32 not_much_time_left_flashing_color;
	float not_much_time_left_flash_period;
	float not_much_time_left_flash_delay;			// time between flashes
	short not_much_time_left_number_of_flashes;
	e_flash_flags not_much_time_left_flash_flags;
	float not_much_time_left_flash_length;			// time of each flash
	pixel32 not_much_time_left_disabled_color;
	int pad_334;

	// Explaination("Time out flash color", "")
	pixel32 time_out_default_color;
	pixel32 time_out_flashing_color;
	float time_out_flash_period;
	float time_out_flash_delay;						// time between flashes
	short time_out_number_of_flashes;
	e_flash_flags time_out_flash_flags;
	float time_out_flash_length;					// time of each flash
	pixel32 time_out_disabled_color;
	int padding_354[11];
	tag_reference carnage_report_bitmap;	// bitm

	// Explaination("Hud crap that wouldn't fit anywhere else", "")
	short loading_begin_text;
	short loading_end_text;
	short checkpoint_begin_text;
	short checkpoint_end_text;
	tag_reference checkpoint_sound;			// snd!

	int pad_398[24];

	s_new_hud_globals new_globals;
};
TAG_GROUP_SIZE_ASSERT(hud_globals_definition, 1160);

// gets the current loaded hud globals tag
hud_globals_definition* get_hud_globals();
