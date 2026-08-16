#pragma once
#include "bitmaps/bitmap_types.h"
#include "cseries/language.h"
#include "sound/sound_definitions.h"
#include "text/text_group.h"

/* constants */

enum
{
	NUMBER_OF_GLOBAL_SOUNDS = 2,
	k_maximum_material_types = 256,
	k_game_globals_maximum_multiplayer_colors = 32
};

enum
{
	_game_difficulty_enemy_damage_scale = 0,
	_game_difficulty_enemy_vitality_scale,
	_game_difficulty_enemy_shield_scale,
	_game_difficulty_enemy_recharge_scale,
	_game_difficulty_friend_damage_scale,
	_game_difficulty_friend_vitality_scale,
	_game_difficulty_friend_shield_scale,
	_game_difficulty_friend_recharge_scale,
	_game_difficulty_infection_form_toughness,
	_game_difficulty_health_unused6,
	_game_difficulty_rate_of_fire_scale,
	_game_difficulty_fire_projectile_error_scale,
	_game_difficulty_burst_error_scale,
	_game_difficulty_new_target_delay_scale,
	_game_difficulty_burst_separation_delay_scale,
	_game_difficulty_target_tracking_bonus,
	_game_difficulty_target_leading_bonus,
	_game_difficulty_overcharge_chance_scale,
	_game_difficulty_special_fire_delay_scale,
	_game_difficulty_projectile_guidance_vs_player_scale,
	_game_difficulty_melee_delay_bonus,
	_game_difficulty_melee_delay_scale,
	_game_difficulty_fire_unused6,
	_game_difficulty_grenade_chance_scale,
	_game_difficulty_grenade_timer_scale,
	_game_difficulty_grenade_unused1,
	_game_difficulty_grenade_unused2,
	_game_difficulty_grenade_unused3,
	_game_difficulty_major_normal_placement,
	_game_difficulty_major_few_placement,
	_game_difficulty_major_many_placement,
	_game_difficulty_ram_player_chance,
	_game_difficulty_unused2,
	_game_difficulty_unused3,
	_game_difficulty_unused4,
	NUMBER_OF_GAME_DIFFICULTY_VALUES,
};

enum
{
	_interface_bitmap_spinner = 0,
	_interface_bitmap_obsolete,
	_interface_color_table_screen,
	_interface_color_table_hud,
	_interface_color_table_editor,
	_interface_color_table_dialog,
	_interface_hud_globals,
	_interface_bitmap_motion_sweep,
	_interface_bitmap_motion_sweep_mask,
	_interface_bitmap_multiplayer_hud,
	_interface_unknown_field,
	_interface_hud_digits_definition,
	_interface_bitmap_motion_blip,
	_interface_bitmap_iface_map1,
	_interface_bitmap_iface_map2,
	_interface_bitmap_iface_map3,
	NUMBER_OF_INTERFACE_TAGS
};

/* enums */

/*
 *	New Player Representations Read Me
 *
 *	adding a new representation to the project requires adding a new value to the enum below
 *  once you have done that add a new function in game_globals.cpp to prepare the s_game_globals_custom_representation_result
 *  the function should load/create all necessary data required to create the custom representation (fp, body, biped, variant, etc)
 *	then add the function into the k_game_globals_custom_representation_function_table constant
 */
enum e_character_type
{
	_character_type_masterchief = 0,
	_character_type_dervish,
	_character_type_spartan,
	_character_type_elite,

	k_player_character_type_count_original,	// Original count of characters that shipped with the game

	// cartographer added characters
	_character_type_skeleton = 4,
	_character_type_flood,
	_character_type_lmao,
	k_player_character_type_count,

	k_cartographer_custom_representation_count = k_player_character_type_count - k_player_character_type_count_original
};

enum e_player_color_index
{
	_player_color_white = 0,
	_player_color_steel,
	_player_color_red,
	_player_color_orange,
	_player_color_gold,
	_player_color_olive,
	_player_color_green,
	_player_color_sage,
	_player_color_cyan,
	_player_color_teal,
	_player_color_colbat,
	_player_color_blue,
	_player_color_violet,
	_player_color_purple,
	_player_color_pink,
	_player_color_crimson,
	_player_color_brown,
	_player_color_tan,
	k_player_color_count,

	_player_color_none = NONE,
	_player_color_index_min_value = NONE,
};

enum e_campaign_difficulty_level
{
	_campaign_difficulty_level_easy = 0,
	_campaign_difficulty_level_normal,
	_campaign_difficulty_level_hard,
	_campaign_difficulty_level_impossible,
	k_campaign_difficulty_levels_count,

	k_campaign_difficulty_level_bits = 2,
	k_campaign_difficulty_level_none = NONE,
};

enum e_game_globals_rasterizer_flags 
{
	_game_globals_rasterizer_flag_tint_edge_density_bit = 0,
	k_game_globals_rasterizer_flag_count
};

/* structures */

// max count: 1
struct s_game_globals_havok_cleanup_resources
{
	tag_reference object_cleanup_effect;
};
ASSERT_STRUCT_SIZE(s_game_globals_havok_cleanup_resources, 8);

// max count: 1
struct s_game_globals_collision_damage
{
	tag_reference collision_damage; // jpt!
	real32 min_game_acc_default;    // 0-oo
	real32 max_game_acc_default;    // 0-oo
	real32 min_game_scale_default;  // 0-1
	real32 max_game_scale_default;  // 0-1
	real32 min_abs_acc_default;     // 0-oo
	real32 max_abs_acc_default;     // 0-oo
	real32 min_abs_scale_default;   // 0-1
	real32 max_abs_scale_default;   // 0-1

	int32 pad[8];
};
ASSERT_STRUCT_SIZE(s_game_globals_collision_damage, 72);

// max count: 1
struct s_ai_globals_gravemind_definition
{
	real32 min_retreat_time_secs;
	real32 ideal_retreat_time_secs;
	real32 max_retreat_time_Secs;
};
ASSERT_STRUCT_SIZE(s_ai_globals_gravemind_definition, 12);

// max count: 1
struct s_ai_globals_definition
{
	real32 danger_broadly_facing;
	int32 pad;
	real32 danger_shooting_near;
	int32 pad1;
	real32 danger_shooting_at;
	int32 pad2;
	real32 danger_extremely_close;
	int32 pad3;
	real32 danger_shield_damage;
	real32 danger_extended_shield_damage;
	real32 danger_body_damage;
	real32 danger_extended_body_damage;
	int32 pad4[12];
	tag_reference global_dialogue_tag;                  // adlg
	string_id default_mission_dialogue_sound_effect;
	int32 pad5[5];
	real32 jump_down_wu_tick;
	real32 jump_step_wu_tick;
	real32 jump_crouch_wu_tick;
	real32 jump_stand_wu_tick;
	real32 jump_storey_wu_tick;
	real32 jump_tower_wu_tick;
	real32 max_jump_down_height_down_wu;
	real32 max_jump_down_height_step_wu;
	real32 max_jump_down_height_crouch_wu;
	real32 max_jump_down_height_stand_wu;
	real32 max_jump_down_height_storey_wu;
	real32 max_jump_down_height_tower_wu;
	real_vector2d hoist_step_wus;
	real_vector2d hoist_crouch_wus;
	real_vector2d hoist_stand_wus;
	int32 pad6[6];
	real_vector2d vault_step_wus;
	real_vector2d vault_crouch_wus;
	int32 pad7[12];
	s_tag_block gravemind_properties;	// s_ai_globals_gravemind_definition

	int32 pad8[12];
	real32 scary_target_threhold;       // A target of this scariness is offically considered scary (by combat dialogue, etc.)
	real32 scary_weapon_threhold;       // A weapon of this scariness is offically considered scary (by combat dialogue, etc.)
	real32 player_scariness;
	real32 berserking_actor_scariness;
};
ASSERT_STRUCT_SIZE(s_ai_globals_definition, 360);

// max count: 1
struct s_damage_globals_definition
{
	s_tag_block damage_groups;	// struct: s_damage_group_definition
};
ASSERT_STRUCT_SIZE(s_damage_globals_definition, 8);

// max count: 1
struct s_game_globals_camera
{
	tag_reference default_unit_camera_track;
	real32 default_change_pause;
	real32 first_person_change_pause;
	real32 following_camera_change_pause;
};
ASSERT_STRUCT_SIZE(s_game_globals_camera, 20);

// max count: 1
struct s_game_globals_player_control
{
	real32 magnetism_friction;              // how much the crosshair slows over enemies
	real32 magnetism_adhesion;              // how much the crosshair sticks to enemies
	real32 inconsequential_target_scale;    // scales magnetism level for inconsequential targets like infection forms

	int32 pad[3];
	
	// Explaination("crosshair", "")
	// -1..1, 0 is middle of the screen
	real_point2d crosshair_location;

	// Explaination("sprinting", "")

	real32 seconds_to_start;         // how long you must be pegged before you start sprinting
	real32 seconds_to_full_speed;    // how long you must sprint before you reach top speed
	real32 decay_rate;               // how fast being unpegged decays the timer (seconds per second)
	real32 full_speed_multiplier;    // how much faster we actually go when at full sprint
	real32 pegged_magnitude;         // how far the stick needs to be pressed before being considered pegged
	real32 pegged_angular_threshold; // how far off straight up (in degrees) we consider pegged

	int32 pad1[2];
	// Explaination("looking", "")

	real32 look_default_pitch_rate_degrees;
	real32 look_default_yaw_rate_degrees;

	real32 look_peg_threshold;                      // magnitude of yaw for pegged acceleration to kick in
	real32 look_yaw_acceleration_time_seconds;      // time for a pegged look to reach maximum effect
	real32 look_yaw_acceleration_scale;             // maximum effect of a pegged look (scales last value in the look function below)
	real32 look_pitch_acceleration_time_seconds;    // time for a pegged look to reach maximum effect
	real32 look_pitch_acceleration_scale;           // maximum effect of a pegged look (scales last value in the look function below)
	real32 look_autolevelling_scale;                // 1 is fast, 0 is none, >1 will probably be really fast
	int32 pad2[2];
	real32 gravity_scale;
	int16 pad3;
	int16 minimum_autolevelling_ticks;              // amount of time player needs to move and not look up or down for autolevelling to kick in
	real32 minimum_angle_for_vehicle_flipping;      // 0 means the vehicle's up vector is along the ground, 90 means the up vector is pointing straight up:degrees

	// max count: 16
	s_tag_block look_function;						// real32
	
	real32 minimum_action_hold_time_seconds;        // time that player needs to press ACTION to register as a HOLD
};
ASSERT_STRUCT_SIZE(s_game_globals_player_control, 128);

// max count: 1
struct s_game_globals_difficulty_information
{
	real32 game_difficulty_values[NUMBER_OF_GAME_DIFFICULTY_VALUES][k_campaign_difficulty_levels_count];
	uint32 unused[21];
};
ASSERT_STRUCT_SIZE(s_game_globals_difficulty_information, 644);

// max count: k_unit_grenade_types_count
struct s_game_globals_grenade
{
	int16 maximum_count;
	uint16 pad;
	tag_reference throwing_effect;  // effe
	int32 unused[4];
	tag_reference item;				// item
	tag_reference projectile;       // proj
};
ASSERT_STRUCT_SIZE(s_game_globals_grenade, 44);

// max count: k_global_vertex_shader_count
struct s_vertex_shader_reference
{
	tag_reference vertex_shader;  // vrtx
};
ASSERT_STRUCT_SIZE(s_vertex_shader_reference, 8);

// max count: 1
struct s_game_globals_rasterizer_data
{
	// Explaination("function textures", "Used internally by the rasterizer. (Do not change unless you know what you're doing!)")
	tag_reference distance_attenuation;     // bitm
	tag_reference vector_normalization;     // bitm
	tag_reference gradients;                // bitm
	tag_reference loading_screen;           // bitm
	tag_reference loading_screen_sweep;     // bitm
	tag_reference loading_spinners;         // bitm
	tag_reference glow;                     // bitm
	tag_reference loading_screen_logos;     // bitm
	tag_reference loading_screen_tickers;   // bitm

	int32 pad[4];
	s_tag_block global_vertex_shaders;		// s_vertex_shader_reference

	// Explaination("default textures", "Used internally by the rasterizer - additive, multiplicative, detail, vector. (Do not change ever, period.)")
	tag_reference default_textures[k_bitmap_type_count];    // bitm

	// Explaination("experimental textures", "Used internally by the rasterizer. (Used by Bernie's experimental shaders.)")
	tag_reference experimental_bitmap_0;    // bitm
	tag_reference experimental_bitmap_1;    // bitm
	tag_reference experimental_bitmap_2;    // bitm
	tag_reference experimental_bitmap_3;    // bitm

	// Explaination("video effect textures", "Used in cinematics.")
	tag_reference video_effect_bitmap_0;    // bitm
	tag_reference video_effect_bitmap_1;    // bitm

	int32 pad1[9];
	
	// Explaination("global shader", "Used for layers that need to do something for other layers to work correctly if the layer is disabled, also used for active-camo, etc.")
	tag_reference global_shader;    // shad

	// Explaination("active camouflage", "")

	int16 flags;					// e_game_globals_rasterizer_flags 
	int16 pad2;
	real32 refraction_amount_pixels;
	real32 distance_falloff;
	real_rgb_color tint_color;
	real32 hyper_stealth_refraction_pixels;
	real32 hyper_stealth_distance_falloff;
	real_rgb_color hyper_stealth_tint_color;

	// Explaination("PC textures", "The PC can't use 3D textures, so we use this instead.")
	tag_reference pc_3d_bitmap;     // bitm
};
ASSERT_STRUCT_SIZE(s_game_globals_rasterizer_data, 264);

// max count: 1
struct s_game_globals_interface_tag_references
{
	tag_reference interface_tag_references[NUMBER_OF_INTERFACE_TAGS];
	tag_reference mainmenu_ui_globals;          // wgtz
	tag_reference singleplayer_ui_globals;      // wgtz
	tag_reference multiplayer_ui_globals;       // wgtz
};
ASSERT_STRUCT_SIZE(s_game_globals_interface_tag_references, 152);

struct s_game_globals_tag_reference
{
	tag_reference reference;
};
ASSERT_STRUCT_SIZE(s_game_globals_tag_reference, 8);

// max count: 1
struct s_game_globals_player_information
{
	tag_reference unused;           // unit
	int32 pad[7];
	real32 walking_speed;           // world units per second
	int32 pad1;
	real32 run_forward;             // world units per second
	real32 run_backward;            // world units per second
	real32 run_sideways;            // world units per second
	real32 run_acceleration;        // world units per second
	real32 sneak_forward;           // world units per second
	real32 sneak_backward;          // world units per second
	real32 sneak_sideways;          // world units per second
	real32 sneak_acceleration;      // world units per second
	real32 airborne_acceleration;   // world units per second
	int32 pad2[4];
	real_point3d grenade_origin;
	int32 pad3[3];
	real32 stun_movement_penalty;       // 1.0 prevents moving while stunned
	real32 stun_turning_penalty;        // 1.0 prevents turning while stunned
	real32 stun_jumping;                // 1.0 prevents jumping while stunned
	real32 minimum_stun_time_seconds;   // all stunning damage will last for at least this long
	real32 maximum_stun_time_seconds;   // no stunning damage will last for longer than this
	int32 pad4[2];
	real_vector2d first_person_idle_time_seconds;
	real32 first_person_skip_fraction;
	int32 pad5[4];
	tag_reference coop_respawn_effect;          // effe
	int32 binoculars_zoom_count;
	real_vector2d binoculars_zoom_range;
	tag_reference binoculars_zoom_in_sound;     // snd!
	tag_reference binoculars_zoom_out_sound;    // snd!
	int32 pad6[4];
	tag_reference active_camouflage_on;     // snd!
	tag_reference active_camouflage_off;    // snd!
	tag_reference active_camouflage_error;  // snd!
	tag_reference active_camouflage_ready;  // snd!
	tag_reference flashlight_on;    // snd!
	tag_reference flashlight_off;   // snd!
	tag_reference ice_cream;        // snd!
};
ASSERT_STRUCT_SIZE(s_game_globals_player_information, 284);

// max count: k_player_character_type_count
struct s_game_globals_player_representation
{
	tag_reference first_person_hands;   // mode
	tag_reference first_person_body;    // mode
	int32 pad[10];
	int32 pad1[30];
	tag_reference third_person_unit;    // unit
	string_id third_person_variant;
};
ASSERT_STRUCT_SIZE(s_game_globals_player_representation, 188);

// max count: 1
struct s_game_globals_falling_damage
{
	int32 pad[2];
	real_vector2d harmful_falling_distance; // world units
	tag_reference falling_damage;           // jpt!
	int32 pad1[2];
	real32 maximum_falling_distance;         // world units
	tag_reference distance_damage;          // jpt!
	tag_reference vehicle_environment_collision_damage_effect;  // jpt!
	tag_reference vehicle_killed_unit_damage_effect;            // jpt!
	tag_reference vehicle_collision_damage; // jpt!
	tag_reference flaming_death_damage;     // jpt!
	int32 pad2[4];
	real32 field_5C;
	real32 field_60;
	real32 unk_gravity_64;
};
ASSERT_STRUCT_SIZE(s_game_globals_falling_damage, 104);

// max count: 1
struct s_game_globals_multiplayer_ui
{
	tag_reference random_player_names;		// unic

	// max count: k_game_globals_maximum_multiplayer_colors
	s_tag_block obsolete_profile_colors;	// real_rgb_color  

	// max count: k_game_globals_maximum_multiplayer_colors
	s_tag_block team_colors;				// real_rgb_color              

	tag_reference team_names;				// unic
};
ASSERT_STRUCT_SIZE(s_game_globals_multiplayer_ui, 32);

struct s_game_globals
{
	int32 unused0[43];
	e_language language;

	s_tag_block havok_cleanup_resources;	// s_game_globals_havok_cleanup_resources
	s_tag_block collision_damage;			// s_game_globals_collision_damage
	s_tag_block sound_globals;				// s_sound_globals_definition
	s_tag_block ai_globals;					// s_ai_globals_definition
	s_tag_block damage_table;				// s_damage_globals_definition
	
	s_tag_block unused1;                  
	
	// According to guerilla this is unused (max count: NUMBER_OF_GLOBAL_SOUNDS) 
	s_tag_block sounds;						// tag_reference

	s_tag_block camera;						// s_game_globals_camera
	s_tag_block player_control;				// s_game_globals_player_control
	s_tag_block difficulty;					// s_game_globals_difficulty_information
	s_tag_block grenades;					// s_game_globals_grenade
	s_tag_block rasterizer_data;			// s_game_globals_rasterizer_data
	s_tag_block interface_tags;				// s_game_globals_interface_tag_references
	
	// max count: 20
	s_tag_block weapon_list;				// s_game_globals_tag_reference

	// max count: 20
	s_tag_block cheat_powerups;				// s_game_globals_tag_reference
	s_tag_block multiplayer_information;	// s_game_globals_multiplayer_information
	s_tag_block player_information;			// s_game_globals_player_information
	s_tag_block player_representation;		// s_game_globals_player_representation
	s_tag_block falling_damage;				// s_game_globals_falling_damage
	s_tag_block old_materials;				// struct: material_definition
	s_tag_block materials;					// struct: s_global_material_definition
	s_tag_block multiplayer_ui;				// s_game_globals_multiplayer_ui
	
	// max count: k_game_globals_maximum_multiplayer_colors
	s_tag_block profile_colors;             // real_rgb_color

	tag_reference multiplayer_globals;      // mulg

	s_tag_block runtime_level_data;	// struct: s_runtime_levels_definition
	s_tag_block ui_level_data;		// struct: s_ui_levels_definition

	// Explaination("Default global lighting", "")
	tag_reference default_global_lighting;  // gldf

	// Populated during game startup
	c_language_pack language_pack[k_language_count];
};
ASSERT_STRUCT_SIZE(s_game_globals, 644);

/* prototypes */

void game_globals_apply_tag_patches(void);

s_game_globals* scenario_get_game_globals(void);

struct s_camera_track_definition* game_globals_get_default_camera_track(void);

void scenario_set_game_globals(s_game_globals* globals);

e_language get_map_language(void);

struct s_ui_levels_definition* game_globals_get_ui_levels(void);

s_game_globals_player_representation* game_globals_get_representation(e_character_type type);

/* public code */

inline s_game_globals_rasterizer_data* rasterizer_globals_get_data(void)
{
	return *Memory::GetAddress<s_game_globals_rasterizer_data**>(0xA3DA3C);
}

inline tag_reference game_globals_get_interface_tag_reference(
	int32 interface_tag_index)
{
	return TAG_BLOCK_GET_ELEMENT(&scenario_get_game_globals()->interface_tags, 0, s_game_globals_interface_tag_references)->interface_tag_references[interface_tag_index];
}

inline struct s_sound_globals_definition* game_globals_get_sound_globals(
	s_game_globals* gg,
	uintptr_t offset,
	int32 index)
{
	s_sound_globals_definition* result = NULL;
	s_tag_block const* block = (s_tag_block*)((uint8*)gg + offset);

	ASSERT(gg);

	if (block != NULL)
	{
		result = TAG_BLOCK_GET_ELEMENT(block, index, s_sound_globals_definition);
	}

	return result;
}
