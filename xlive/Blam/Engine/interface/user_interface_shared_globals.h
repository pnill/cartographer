#pragma once
#include "user_interface.h"

#include "math/color_math.h"
#include "math/integer_math.h"
#include "tag_files/tag_block.h"
#include "tag_files/tag_reference.h"

/* constants */

enum
{
	k_maximum_error_category_count = 100,
	k_ui_maximum_error_per_category_count = 100,
	k_maximum_number_of_widget_animations_per_screen = 64,
	k_maximum_number_of_animation_keyframe_blocks = 64,
	k_maximum_number_of_shapes_per_group = 64,
	k_maximum_number_of_shape_blocks = 32,
	k_maximum_points_per_shape = 16,
	k_maximum_object_scenes_per_screen = 32,
	k_maximum_objects_per_ui_scene = 32,
	k_maximum_lights_per_ui_scene = 8,
	k_maximum_number_of_bitmap_blocks = 64,
	k_maximum_number_of_persisant_animations = 100,
	k_maximum_number_of_list_item_skins = 32,
};

/* enums */

enum e_user_interface_global_sound
{
	_user_interface_global_sound_cursor,
	_user_interface_global_sound_selection,
	_user_interface_global_sound_error,
	_user_interface_global_sound_advancing,
	_user_interface_global_sound_retreating,
	_user_interface_global_sound_initial_login,
	_user_interface_global_sound_vkbd_cursor,
	_user_interface_global_sound_vkbd_character_insertion,
	_user_interface_global_sound_online_notification,
	_user_interface_global_sound_tabbed_view_pane_tabbing,
	_user_interface_global_sound_pregame_countdown_timer,
	_user_interface_global_sound_unused0,
	_user_interface_global_sound_matchmaking_advance,
	_user_interface_global_sound_unused1,
	_user_interface_global_sound_unused2,
	_user_interface_global_sound_unused3,
	k_user_interface_global_sound_count
};

enum e_ui_error_category_flags : int16
{
	_ui_error_category_use_large_dialog_bit = 0
};

enum e_ui_error_default_button : uint8
{
	_ui_error_default_button_no_default,
	_ui_error_default_button_default_ok,
	_ui_error_default_button_default_cancel,
};

enum e_header_text_type
{
	_header_text_type_fullscreen,
	_header_text_type_large,
	_header_text_type_half,
	_header_text_type_quarter,
	k_header_text_type_count
};

enum e_header_text_font : int16
{
	_header_text_font_terminal = 0,
	_header_text_font_body_text = 1,
	_header_text_font_title = 2,
	_header_text_font_super_large = 3,
	_header_text_font_large_body_text = 4,
	_header_text_font_split_screen_hud_message = 5,
	_header_text_font_full_screen_hud_message = 6,
	_header_text_font_english_body_text = 7,
	_header_text_font_hud_number_text = 8,
	_header_text_font_subtitle_font = 9,
	_header_text_font_main_menu = 10,
	_header_text_font_text_chat = 11
};

enum e_ui_animation_index : int16
{
	_ui_animation_index_none = 0,
	_ui_animation_index_00 = 1,
	_ui_animation_index_01 = 2,
	_ui_animation_index_02 = 3,
	_ui_animation_index_03 = 4,
	_ui_animation_index_04 = 5,
	_ui_animation_index_05 = 6,
	_ui_animation_index_06 = 7,
	_ui_animation_index_07 = 8,
	_ui_animation_index_08 = 9,
	_ui_animation_index_09 = 10,
	_ui_animation_index_10 = 11,
	_ui_animation_index_11 = 12,
	_ui_animation_index_12 = 13,
	_ui_animation_index_13 = 14,
	_ui_animation_index_14 = 15,
	_ui_animation_index_15 = 16,
	_ui_animation_index_16 = 17,
	_ui_animation_index_17 = 18,
	_ui_animation_index_18 = 19,
	_ui_animation_index_19 = 20,
	_ui_animation_index_20 = 21,
	_ui_animation_index_21 = 22,
	_ui_animation_index_22 = 23,
	_ui_animation_index_23 = 24,
	_ui_animation_index_24 = 25,
	_ui_animation_index_25 = 26,
	_ui_animation_index_26 = 27,
	_ui_animation_index_27 = 28,
	_ui_animation_index_28 = 29,
	_ui_animation_index_29 = 30,
	_ui_animation_index_30 = 31,
	_ui_animation_index_31 = 32,
	_ui_animation_index_32 = 33,
	_ui_animation_index_33 = 34,
	_ui_animation_index_34 = 35,
	_ui_animation_index_35 = 36,
	_ui_animation_index_36 = 37,
	_ui_animation_index_37 = 38,
	_ui_animation_index_38 = 39,
	_ui_animation_index_39 = 40,
	_ui_animation_index_40 = 41,
	_ui_animation_index_41 = 42,
	_ui_animation_index_42 = 43,
	_ui_animation_index_43 = 44,
	_ui_animation_index_44 = 45,
	_ui_animation_index_45 = 46,
	_ui_animation_index_46 = 47,
	_ui_animation_index_47 = 48,
	_ui_animation_index_48 = 49,
	_ui_animation_index_49 = 50,
	_ui_animation_index_50 = 51,
	_ui_animation_index_51 = 52,
	_ui_animation_index_52 = 53,
	_ui_animation_index_53 = 54,
	_ui_animation_index_54 = 55,
	_ui_animation_index_55 = 56,
	_ui_animation_index_56 = 57,
	_ui_animation_index_57 = 58,
	_ui_animation_index_58 = 59,
	_ui_animation_index_59 = 60,
	_ui_animation_index_60 = 61,
	_ui_animation_index_61 = 62,
	_ui_animation_index_62 = 63,
	_ui_animation_index_63 = 64
};

enum e_animation_reference_flags : uint32
{
	_animation_reference_bit_0	// unused
};

enum e_animation_looping_style : int16
{
	_animation_looping_style_none = 0,
	_animation_looping_style_reverse_loop = 1,
	_animation_looping_style_loop = 2,
	_animation_looping_style_dont_loop = 3
};

enum e_shape_block_reference_flags : uint32
{
	_shape_block_reference_bit_0	// unused
};

enum e_ui_model_scene_reference_flags : uint32
{
	_ui_model_scene_reference_bit_0	// unused
};

enum e_bitmap_block_reference_flags : uint32
{
	_bitmap_block_reference_bit_ignore_for_list_skin_size_calculation_bit,
	_bitmap_block_reference_bit_swap_on_relative_list_position_bit,
	_bitmap_block_reference_bit_render_as_progress_bar_bit,
};

enum e_bitmap_blend_method : int16
{
	_bitmap_blend_method_standard = 0,
	_bitmap_blend_method_multiply = 1,
	_bitmap_blend_method_unused = 2
};

/* structures */


struct s_ui_error_information
{
	e_ui_error_types error;
	e_ui_error_category_flags flags;
	e_ui_error_default_button default_button;
	int8 pad;
};

struct s_ui_error_string_data
{
	string_id title;
	string_id message;
	string_id ok;
	string_id cancel;
};

// max count: k_ui_maximum_error_per_category_count
struct s_ui_error
{
	s_ui_error_information info;
	s_ui_error_string_data strings;
};
ASSERT_STRUCT_SIZE(s_ui_error, 24);

// max count: k_maximum_error_category_count
struct s_ui_error_category
{
	s_ui_error_information info;
	tag_reference string_tag;	// unic
	s_ui_error_string_data default_strings;

	tag_block<s_ui_error> error_block;
};
ASSERT_STRUCT_SIZE(s_ui_error_category, 40);

// max count: k_maximum_number_of_animation_keyframe_blocks
struct s_animation_keyframe_reference
{
	int32 pad;
	real32 alpha;
	real_point3d position;
};
ASSERT_STRUCT_SIZE(s_animation_keyframe_reference, 20);

// max count: k_maximum_number_of_widget_animations_per_screen
struct s_animation_reference
{
	e_animation_reference_flags flags;

	// Explaination("Primary Intro Transition", "Defines the primary intro transitional animation")
	
	int32 intro_animation_period;	// Milliseconds
	tag_block<s_animation_keyframe_reference> keyframes;

	// Explaination("Primary Outro Transition", "Defines the primary outro transitional animation")
	
	int32 outro_animation_period;	// Milliseconds
	tag_block<s_animation_keyframe_reference> outro_keyframes;

	// Explaination("Ambient Animation", "Defines the ambient animation")
	
	int32 ambient_animation_period;	// Milliseconds

	e_animation_looping_style ambient_animation_looping_style;
	int16 pad;
	tag_block<s_animation_keyframe_reference> ambient_keyframes;
};
ASSERT_STRUCT_SIZE(s_animation_reference, 44);

// max count: k_maximum_points_per_shape
struct s_point_block_reference
{
	point2d coordinates;
};
ASSERT_STRUCT_SIZE(s_point_block_reference, 4);

// max count: k_maximum_number_of_shape_blocks
struct s_shape_block_reference
{
	e_shape_block_reference_flags flags;
	e_ui_animation_index animation_index;
	int16 intro_animation_delay;	// Milliseconds
	real_argb_color color;
	tag_block<s_point_block_reference> points;

	int16 render_depth_bias;
	int8 pad[14];
};
ASSERT_STRUCT_SIZE(s_shape_block_reference, 48);

// max count: k_maximum_objects_per_ui_scene
struct s_ui_object_reference
{
	char name[k_tag_string_length];
};
ASSERT_STRUCT_SIZE(s_ui_object_reference, 32);

// max count: k_maximum_lights_per_ui_scene
struct s_ui_light_reference
{
	char name[k_tag_string_length];
};
ASSERT_STRUCT_SIZE(s_ui_light_reference, 32);

// max count: k_maximum_object_scenes_per_screen
struct s_ui_model_scene_reference
{
	/* Explaination("NOTE on coordinate systems", "Halo y-axis=ui z-axis, and Halo z-axis=ui y-axis.
	As a convention, let's always place objects in the ui scenario such that
	they are facing in the '-y' direction, and the camera such that is is
	facing the '+y' direction.This way the ui animation for models(which
	gets applied to the camera) will always be consisitent.") */

	e_ui_model_scene_reference_flags flags;

	e_ui_animation_index animation_index;
	int16 intro_animation_delay;	// Milliseconds
	int16 render_depth_bias;
	int16 pad;
	
	tag_block<s_ui_object_reference> objects;
	tag_block<s_ui_light_reference> lights;

	real_vector3d animation_scale_factor;
	real_point3d camera_position;
	real32 fov;
	rectangle2d ui_viewport;
	string_id intro_anim;	// unused
	string_id outro_anim;	// unused
	string_id ambient_anim;	// unused
};
ASSERT_STRUCT_SIZE(s_ui_model_scene_reference, 76);

// max count: k_maximum_number_of_bitmap_blocks
struct s_bitmap_block_reference
{
	e_bitmap_block_reference_flags flags;
	e_ui_animation_index animation_index;
	int16 intro_animation_delay;	// Milliseconds

	e_bitmap_blend_method bitmap_blend_method;
	int16 initial_sprite_frame;
	point2d topleft;
	real32 horiz_texture_wrapssecond;
	real32 vert_texture_wraps_second;
	
	tag_reference bitmap_tag;	// bitm
	
	int16 render_depth_bias;
	int16 pad;
	real32 sprite_animation_speed_fps;
	point2d progress_bottomleft;
	string_id string_identifier;
	real_vector2d progress_scale;
};
ASSERT_STRUCT_SIZE(s_bitmap_block_reference, 56);

// max count: k_maximum_number_of_shapes_per_group
struct s_shape_group_reference
{
	// Explaination("Unused Debug Geometry Shapes", "This is the old way")

	tag_block<s_shape_block_reference> shapes;

	// Explaination("Model-Light Groups", "Specify commonly used model/light groups here")
	
	tag_block<s_ui_model_scene_reference> model_scene_blocks;

	// Explaination("Bitmaps", "Specify more flavor bitmaps here")
	
	tag_block<s_bitmap_block_reference> bitmap_blocks;
};
ASSERT_STRUCT_SIZE(s_shape_group_reference, 24);

// max count: k_maximum_number_of_persisant_animations
struct s_persistant_animation_reference
{
	int32 pad;
	int32 animation_period;	// Milliseconds
	tag_block<s_animation_keyframe_reference> interpolatedKeyframes;
};
ASSERT_STRUCT_SIZE(s_persistant_animation_reference, 16);

// max count: k_maximum_number_of_list_item_skins
struct s_user_interface_list_skin_reference
{
	tag_reference list_item_skins;	// skin
};
ASSERT_STRUCT_SIZE(s_user_interface_list_skin_reference, 8);

// max count: UNSIGNED_SHORT_MAX
struct s_skill_to_rank_mapping
{
	short_bounds skill_bounds;
};
ASSERT_STRUCT_SIZE(s_skill_to_rank_mapping, 4);

struct s_user_interface_dialog_bounds
{
	rectangle2d dialog_header_text_bounds;
	rectangle2d dialog_button_key_text_bounds;
};

struct s_user_interface_shared_globals
{
	real32 gap0;
	int16 auto_typing_text_time;
	int16 auto_typing_text_words_per_line;
	real32 auto_typing_text_size;
	real_argb_color auto_typing_text_color;
	real32 gap1;
	real_vector2d blur_blending_speed;
	real_argb_color blur_color;
	real32 gap2[3];

	// Explaination("UI Rendering Globals", "miscellaneous rendering globals, more below...")
	
	real32 overlayed_screen_alpha_mod;
	int16 inc_text_update_period_milliseconds;
	int16 inc_text_block_character;				// ASCII Code
	real32 callout_text_scale;
	real_argb_color progress_bar_color;
	real32 near_clip_plane_distance;	// ObjectsCloserThanThisAreNotDrawn
	real32 projection_plane_distance;	// DistanceAtWhichObjectsAreRenderedWhenZ0normalSize
	real32 far_clip_plane_distance;		// ObjectsFartherThanThisAreNotDrawn
	
	// Explaination("Overlayed UI Color", "This is the color of the overlayed ui effect; the alpha component is the maximum opacity")
	
	real_argb_color overlayed_interface_color;
	int8 pad0[12];

	// Explaination("Displayed Errors", "For each error condition displayed in the UI, set the title and description string ids here")
	
	tag_block<s_ui_error_category> errors;

	tag_reference sounds[k_user_interface_global_sound_count]; // snd!

	// Explaination("Global Bitmaps", "Sprite sequences for global ui bitmaps, as follows: 1) vkeyboard cursor")
	
	tag_reference global_bitmaps_tag; // bitm

	// Explaination("Global Text Strings", "Global UI Text goes here")
	
	tag_reference unicode_string_list_tag;	// unic

	// Explaination("Screen Animations", "Animations used by screen definitions for transitions and ambient animating")
	
	tag_block<s_animation_reference> screen_animations;

	// Explaination("Polygonal Shape Groups", "Define the various groups of shape-objects for use on any ui screens here")
	
	tag_block<s_shape_group_reference> shape_gsroups;

	// Explaination("Persistant Background Animations", "These are the animations used by elements that live in the persistant background")
	
	tag_block<s_persistant_animation_reference> animations;

	/* Explaination("List Skins", "These define the visual appearances (skins) available for UI lists
	They are expected to be entered in the following order :
		0) default
		1) squad lobby player list
		2) settings list
		3) playlist entry list
		4) variants list
		5) game browser list
		6) online player menu
		7) game setup menu
		8) playlist contents display
		9) profile picker
		10) mp map list
		11) main menu
		")
	*/

	tag_block<s_user_interface_list_skin_reference> list_item_skins;

	// Explaination("Additional UI Strings", "These are for specific purposes as noted")

	tag_reference button_key_type_strings;	// unic
	tag_reference gametype_strings;	// unic
	tag_reference unknown_tag_reference;

	// Explaination("Skill to rank mapping table", "")
	
	tag_block<s_skill_to_rank_mapping> skill_mappings;

	// Explaination("WINDOW PARAMETERS", "Various settings for different sized UI windows")

	e_header_text_font header_text_font[k_header_text_type_count];
	
	real_argb_color default_text_color;
	
	s_user_interface_dialog_bounds dialog_bounds[k_header_text_type_count];
	
	// Explaination("Main menu music", "Looping sound that plays while the main menu is active")

	tag_reference main_menu_music;	// lsnd
	int32 music_fade_time_milliseconds;
};
ASSERT_STRUCT_SIZE(s_user_interface_shared_globals, 452);

/* prototypes */

s_user_interface_shared_globals* __cdecl user_interface_shared_globals_get(void);
