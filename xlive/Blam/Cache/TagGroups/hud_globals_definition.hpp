#pragma once
#include "Blam\Cache\DataTypes\BlamDataTypes.h"
#include "Blam\Cache\TagGroups.hpp"
#include "Blam\Math\BlamMath.h"

/*********************************************************************
* name: hud_globals
* group_tag : hudg
* header size : 1160
* *********************************************************************/

#pragma pack(push,1)
struct s_hud_globals_group_definition :TagGroup<'hudg'>
{
	struct
	{
		enum class e_anchor : __int16
		{
			top_left = 0,
			top_right = 1,
			bottom_left = 2,
			bottom_right = 3,
			center = 4,
			crosshair = 5,
		};
		e_anchor anchor;//0x0
		PAD(0x22);//0x2
		__int16 anchor_offset_x;//0x24
		__int16 anchor_offset_y;//0x26
		float width_scale;//0x28
		float height_scale;//0x2C
		enum class e_scaling_flags : __int16
		{
			dont_scale_offset = FLAG(0),
			dont_scale_size = FLAG(1),
		};
		e_scaling_flags scaling_flags;//0x30
		PAD(0x16);//0x32
		tag_reference obsolete1;//0x48
		tag_reference obsolete2;//0x50
		float up_time;//0x58
		float fade_time;//0x5C
		real_color_argb icon_color;//0x60
		real_color_argb text_color;//0x70
		float text_spacing;//0x80
		tag_reference item_message_text;//0x84
		tag_reference icon_bitmap;//0x8C
		tag_reference alternate_icon_text;//0x94
	}messaging_parameters;

	struct s_button_icons_block
	{
		__int16 sequence_index;//0x0
		__int16 width_offset;//0x2
		__int16 offset_from_reference_corner_x;//0x4
		__int16 offset_from_reference_corner_y;//0x6
		byte_color_argb override_icon_color;//0x8
		__int8 frame_rate;//0xC
		enum class e_flags : __int8
		{
			use_text_from_string_list_instead = FLAG(0),
			override_default_color = FLAG(1),
			width_offset_is_absolute_icon_width = FLAG(2),
		};
		e_flags flags;//0xD
		__int16 text_index;//0xE
	};
	TAG_BLOCK_SIZE_ASSERT(s_button_icons_block, 0x10);
	tag_block<s_button_icons_block> button_icons;//0x9C

	struct
	{
		byte_color_argb default_color;//0xA4
		byte_color_argb flashing_color;//0xA8
		float flash_period;//0xAC
		float flash_delay;//0xB0
		__int16 number_of_flashes;//0xB4
		enum class e_flash_flags : __int16
		{
			reverse_defaultflashing_colors = FLAG(0),
		};
		e_flash_flags flash_flags;//0xB6
		float flash_length;//0xB8
		byte_color_argb disabled_color;//0xBC
	}hud_help_text_color;

	PAD(0x4);//0xC0
	tag_reference hud_messages;//0xC4

	struct
	{
		byte_color_argb default_color;//0xCC
		byte_color_argb flashing_color;//0xD0
		float flash_period;//0xD4
		float flash_delay;//0xD8
		__int16 number_of_flashes;//0xDC
		enum class e_flash_flags : __int16
		{
			reverse_defaultflashing_colors = FLAG(0),
		};
		e_flash_flags flash_flags;//0xDE
		float flash_length;//0xE0
		byte_color_argb disabled_color;//0xE4
		__int16 uptime_ticks;//0xE8
		__int16 fade_ticks;//0xEA
	}objective_colors;

	struct
	{
		float top_offset;//0xEC
		float bottom_offset;//0xF0
		float left_offset;//0xF4
		float right_offset;//0xF8
		PAD(0x20);//0xFC
		tag_reference arrow_bitmap;//0x11C
		struct s_waypoint_arrows_block
		{
			tag_string32 name;//0x0
			PAD(0xC);//0x20
			float opacity;//0x2C
			float translucency;//0x30
			__int16 on_screen_sequence_index;//0x34
			__int16 off_screen_sequence_index;//0x36
			__int16 occluded_sequence_index;//0x38
			PAD(0x12);//0x3A
			enum class e_flags : __int32
			{
				dont_rotate_when_pointing_offscreen = FLAG(0),
			};
			e_flags flags;//0x4C
			PAD(0x18);//0x50
		};
		TAG_BLOCK_SIZE_ASSERT(s_waypoint_arrows_block, 0x68);
		tag_block<s_waypoint_arrows_block> waypoint_arrows;//0x124
	}way_point_parameters;


	PAD(0x50);//0x12C
	float hud_scale_in_multiplayer;//0x17C
	PAD(0x110);//0x180
	struct
	{
		float motion_sensor_range;//0x290
		float motion_sensor_velocity_sensitivity;//0x294
		float motion_sensor_scale;//0x298
		rect2d default_chapter_title_boundstop;//0x29C
	}hud_globals;

	PAD(0x2C);//0x2A4

	struct
	{
		__int16 top_offset;//0x2D0
		__int16 bottom_offset;//0x2D2
		__int16 left_offset;//0x2D4
		__int16 right_offset;//0x2D6
		PAD(0x20);//0x2D8
		tag_reference indicator_bitmap;//0x2F8
		__int16 sequence_index;//0x300
		__int16 multiplayer_sequence_index;//0x302
		byte_color_argb color;//0x304

	}hud_damage_inidicators;

	PAD(0x10);//0x308
	//hud timer definitions
	struct
	{
		byte_color_argb default_color;//0x318
		byte_color_argb flashing_color;//0x31C
		float flash_period;//0x320
		float flash_delay;//0x324
		__int16 number_of_flashes;//0x328
		enum class e_flash_flags : __int16
		{
			reverse_defaultflashing_colors = FLAG(0),
		};
		e_flash_flags flash_flags;//0x32A
		float flash_length;//0x32C
		byte_color_argb disabled_color;//0x330
	}not_much_time_left;

	PAD(0x4);//0x334
	struct
	{
		byte_color_argb default_color;//0x338
		byte_color_argb flashing_color;//0x33C
		float flash_period;//0x340
		float flash_delay;//0x344
		__int16 number_of_flashes;//0x348
		enum class e_flash_flags : __int16
		{
			reverse_defaultflashing_colors = FLAG(0),
		};
		e_flash_flags flash_flags;//0x34A
		float flash_length;//0x34C
		byte_color_argb disabled_color;//0x350
		PAD(0x2C);//0x354
		tag_reference carnage_report_bitmap;//0x380
	}time_out_flash_color;

	//hud crap that wont fit anywhere else
	__int16 loading_begin_text;//0x388
	__int16 loading_end_text;//0x38A
	__int16 checkpoint_begin_text;//0x38C
	__int16 checkpoint_end_text;//0x38E
	tag_reference checkpoint_sound;//0x390
	PAD(0x60);//0x398
	tag_reference hud_text;//0x3F8


	struct s_dashlights_block
	{
		tag_reference bitmap;//0x0
		tag_reference shader;//0x8
		__int16 sequence_index;//0x10
		enum class e_flags : __int16
		{
			dont_scale_when_pulsing = FLAG(0),
		};
		e_flags flags;//0x12
		tag_reference sound;//0x14
	};
	TAG_BLOCK_SIZE_ASSERT(s_dashlights_block, 0x1C);
	tag_block<s_dashlights_block> dashlights;//0x400
	struct s_waypoint_arrows_block
	{
		tag_reference bitmap;//0x0
		tag_reference shader;//0x8
		__int16 sequence_index;//0x10
		PAD(0x2);//0x12
		float smallest_size;//0x14
		float smallest_distance;//0x18
		tag_reference border_bitmap;//0x1C
	};
	TAG_BLOCK_SIZE_ASSERT(s_waypoint_arrows_block, 0x24);
	tag_block<s_waypoint_arrows_block> waypoint_arrows;//0x408
	struct s_waypoints_block
	{
		tag_reference bitmap;//0x0
		tag_reference shader;//0x8
		__int16 onscreen_sequence_index;//0x10
		__int16 occluded_sequence_index;//0x12
		__int16 offscreen_sequence_index;//0x14
		PAD(0x2);//0x16
	};
	TAG_BLOCK_SIZE_ASSERT(s_waypoints_block, 0x18);
	tag_block<s_waypoints_block> waypoints;//0x410
	struct s_hud_sounds_block
	{
		tag_reference chief_sound;//0x0
		enum class e_latched_to : __int32
		{
			shield_recharging = FLAG(0),
			shield_damaged = FLAG(1),
			shield_low = FLAG(2),
			shield_empty = FLAG(3),
			health_low = FLAG(4),
			health_empty = FLAG(5),
			health_minor_damage = FLAG(6),
			health_major_damage = FLAG(7),
			rocket_locking = FLAG(8),
			rocket_locked = FLAG(9),
		};
		e_latched_to latched_to;//0x8
		float scale;//0xC
		tag_reference dervish_sound;//0x10
	};
	TAG_BLOCK_SIZE_ASSERT(s_hud_sounds_block, 0x18);
	tag_block<s_hud_sounds_block> hud_sounds;//0x418
	struct s_player_training_data_block
	{
		string_id display_string;//0x0
		string_id display_string2;//0x4
		string_id display_string3;//0x8
		__int16 max_display_time;//0xC
		__int16 display_count;//0xE
		__int16 dissapear_delay;//0x10
		__int16 redisplay_delay;//0x12
		float display_delay_s;//0x14
		enum class e_flags : __int16
		{
			not_in_multiplayer = FLAG(0),
		};
		e_flags flags;//0x18
		PAD(0x2);//0x1A
	};
	TAG_BLOCK_SIZE_ASSERT(s_player_training_data_block, 0x1C);
	tag_block<s_player_training_data_block> player_training_data;//0x420


	tag_reference primary_message_sound;//0x428
	tag_reference secondary_message_sound;//0x430
	string_id boot_griefer_string;//0x438
	string_id cannot_boot_griefer_string;//0x43C
	tag_reference training_shader;//0x440
	tag_reference human_training_top_right;//0x448
	tag_reference human_training_top_center;//0x450
	tag_reference human_training_top_left;//0x458
	tag_reference human_training_middle;//0x460
	tag_reference elite_training_top_right;//0x468
	tag_reference elite_training_top_center;//0x470
	tag_reference elite_training_top_left;//0x478
	tag_reference elite_training_middle;//0x480
};
TAG_GROUP_SIZE_ASSERT(s_hud_globals_group_definition, 0x488);
#pragma pack(pop)

/* gets the current loaded hud globals tag aka hudg */
s_hud_globals_group_definition *get_hud_globals_ptr()
{
	return Memory::GetAddress<s_hud_globals_group_definition*>(0x9765C8, 0x99FBB0);
}