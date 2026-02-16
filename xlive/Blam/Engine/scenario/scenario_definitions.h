#pragma once
#include "game/game_allegiance.h"
#include "math/periodic_functions.h"
#include "tag_files/data_reference.h"
#include "tag_files/tag_reference.h"

/* constants */

enum
{
	MAXIMUM_CHILD_SCENARIOS_PER_SCENARIO = 16,
	MAXIMUM_FUNCTIONS_PER_SCENARIO = 32,
	MAXIMUM_ENVIRONMENT_OBJECTS_PER_SCENARIO = 4096,
	MAXIMUM_OBJECT_NAMES_PER_SCENARIO = 640,
	MAXIMUM_STRUCTURE_BSPS_PER_SCENARIO = 16,
	MAXIMUM_SCENERY_DATUMS_PER_SCENARIO = 2000,
	MAXIMUM_SCENARIO_OBJECT_PALETTE_ENTRIES_PER_BLOCK = 256,
	MAXIMUM_BIPED_DATUMS_PER_SCENARIO = 128,
	MAXIMUM_VEHICLE_DATUMS_PER_SCENARIO = 256,
	MAXIMUM_EQUIPMENT_DATUMS_PER_SCENARIO = 256,
	MAXIMUM_WEAPON_DATUMS_PER_SCENARIO = 128,
	MAXIMUM_DEVICE_GROUPS_PER_SCENARIO = 128,
	MAXIMUM_MACHINE_DATUMS_PER_SCENARIO = 400,
	MAXIMUM_CONTROL_DATUMS_PER_SCENARIO = 100,
	MAXIMUM_LIGHT_FIXTURE_DATUMS_PER_SCENARIO = 500,
	MAXIMUM_SOUND_SCENERY_DATUMS_PER_SCENARIO = 256,
	MAXIMUM_LIGHTS_PER_SCENARIO = 500,
	MAXIMUM_SCENARIO_PLAYERS_PER_BLOCK = 256,
	MAXIMUM_TRIGGER_VOLUMES_PER_SCENARIO = 256,
	MAXIMUM_RECORDED_ANIMATIONS_PER_MAP = 1024,
	MAXIMUM_SCENARIO_NETPOINTS_PER_SCENARIO = 200,
	MAXIMUM_SCENARIO_NETGAME_EQUIPMENT_PER_SCENARIO = 100,
	MAXIMUM_SCENARIO_STARTING_EQUIPMENT_PER_SCENARIO = 200,
	MAXIMUM_DECALS_PER_SCENARIO = 65536,
	MAXIMUM_DECAL_PALETTES_PER_SCENARIO = 128,
	k_max_scenes_per_scenario = 100,
	k_maximum_hs_scripts_per_scenario = 1024,
	MAXIMUM_CUTSCENE_FLAGS_PER_SCENARIO = 512,
	MAXIMUM_CUTSCENE_CAMERA_POINTS_PER_SCENARIO = 512,
	k_maximum_scenario_cutscene_titles = 128,
	NUMBER_OF_SCENARIO_RESOURCE_TYPES = 16,
	k_number_of_scenario_ai_types = 2,
	k_maximum_hs_syntax_nodes_per_scenario = 36864,
	MAXIMUM_CRATE_DATUMS_PER_SCENARIO = 1024,
	MAXIMUM_CREATURE_DATUMS_PER_SCENARIO = 128,
	k_maximum_decorator_sets_per_scenario = 32,
	k_maximum_structure_bsp_spherical_harmonic_lighting_points = 32768,
	k_maximum_scenario_editor_folders = 32767,
	k_maximum_screen_effect_references_per_scenario = 16,
	k_maximum_simulation_definition_table_elements_per_scenario = 512,
};

/* enums */

enum e_scenario_type : int16
{
	_scenario_type_solo = 0,
	_scenario_type_multiplayer = 1,
	_scenario_type_main_menu = 2,
	_scenario_type_multiplayer_shared = 3,
	_scenario_type_single_player_shared = 4,
	k_scenario_type_count
};

enum e_scenario_flags : int16
{
	_scenario_cortana_hack_bit = 0,										// Sorts Cortana In Front Of Other Transparent Geometry
	_scenario_always_draw_sky_bit = 1,									// Always Draws Sky Even If No sky Polygons Are Visible
	_scenario_dont_strip_pathfinding_bit = 2,							// Always Leaves Pathfinding In Even For Multiplayer Scenario
	_scenario_symmetrical_multiplayer_map_bit = 3,
	_scenario_quick_load_bit = 4,
	_scenario_characters_use_weapons_from_previous_mission_bit = 5,
	_scenario_lightmaps_smooth_palettes_with_neighbor_clusters_bit = 6,
	_scenario_snap_to_white_bit = 7,
};

enum e_scenario_function_flags : int32
{
	_scenario_function_scripted_bit = 0,								// Level Script Will Set This Value Other Settings Here Will Be Ignored
	_scenario_function_invert_function_bit = 1,							// Level Script Will Set This Value Other Settings Here Will Be Ignored
	_scenario_function_additive_bit = 2,
	_scenario_function_does_not_deactivate_below_lower_bound_bit = 3,	// Function Does Not Deactivate When At Or Below Lower Bound
	NUMBER_OF_SCENARIO_FUNCTION_DEFINITION_FLAGS = 4,
};


enum e_bounds_mode : int16
{
	_scenario_function_clip_to_bounds = 0,
	_scenario_function_clip_to_bounds_and_normalize = 1,
	_scenario_function_scale_to_fit_bounds = 2,
	NUMBER_OF_SCENARIO_FUNCTION_BOUNDS_MODES = 3,
};

/* structures */

// max count: MAXIMUM_CHILD_SCENARIOS_PER_SCENARIO 16
struct scenario_child_scenario_reference
{
	tag_reference child_scenario;	// scnr
	char pad[16];
};
ASSERT_STRUCT_SIZE(scenario_child_scenario_reference, 24);

// max count: MAXIMUM_FUNCTIONS_PER_SCENARIO 32
struct scenario_function
{
	e_scenario_function_flags flags;
	char name[k_tag_string_length];
	real32 period_seconds;               // Period for above function (lower values make function oscillate quickly; higher values make it oscillate slowly).
	
	// Multiply this function by above period
	// This is the tagblock index for another scenario_function
	int16 scale_period_by;
	e_periodic_function_type function;

	// Multiply this function by result of above function.
	// This is the tagblock index for another scenario_function
	int16 scale_function_by;
	e_periodic_function_type wobble_function;   // Curve used for wobble.
	real32 wobble_period_seconds;                // Time it takes for magnitude of this function to complete a wobble.
	real32 wobble_magnitude_percent;             // Amount of random wobble in the magnitude.
	real32 square_wave_threshold;                // If non-zero, all values above square wave threshold are snapped to 1.0, and all values below it are snapped to 0.0 to create a square wave.
	int16 step_count;                           // Number of discrete values to snap to (e.g., step count of 5 snaps function to 0.00, 0.25, 0.50,0.75, or 1.00).

	e_transition_function_type map_result_to_transition_function;
	
	int16 sawtooth_count;                       // Number of times this function should repeat (e.g., sawtooth count of 5 gives function value of 1.0 at each of 0.25, 0.50, and 0.75, as well as at 1.0).
	int16 pad;

	// Multiply this function (e.g., from a weapon, vehicle) final result of all of the above math.
	// This is the tagblock index for another scenario_function
	int16 scale_result_by_function_index;

	e_bounds_mode bounds_mode;                   // Controls how bounds, below, are used.
	real32 lower_bound;
	real32 upper_bound;
	int16 pad1[3];

	// If specified function is off, so is this function.
	// This is the tagblock index for another scenario_function
	int16 turn_off_with_function_index;

	int32 unused1[4];
	real32 runtime_reciprocal_bounds_range;
	real32 runtime_reciprocal_sawtooth_count;
	real32 runtime_reciprocal_step_count;
	real32 runtime_one_over_period;
};
ASSERT_STRUCT_SIZE(scenario_function, 120);

// max count: MAXIMUM_ENVIRONMENT_OBJECTS_PER_SCENARIO 4096
struct scenario_environment_object
{
	// envf 
	int16 bsp;      // Block index: scenario_structure_bsp_reference
	int16 runtime_object_type;
	int32 unique_id;
	int32 pad;
	datum object_definition_tag;
	int32 object;
	int32 pad1[11];
};
ASSERT_STRUCT_SIZE(scenario_environment_object, 64);

// max count: MAXIMUM_OBJECT_NAMES_PER_SCENARIO 640
struct scenario_object_name
{
	char name[k_tag_string_length];
	int16/*e_object_type*/ runtime_object_type;
	int16 runtime_scenario_datum_index;
};
ASSERT_STRUCT_SIZE(scenario_object_name, 36);

// max count: MAXIMUM_STRUCTURE_BSPS_PER_SCENARIO 16
struct pathfinding_object_index_list
{
	int16 bsp_index;
	int16 pathfinding_object_index;
};
ASSERT_STRUCT_SIZE(pathfinding_object_index_list, 4);

// max count: MAXIMUM_SCENARIO_PLAYERS_PER_BLOCK 256
struct scenario_starting_profile
{
	char name[k_tag_string_length];
	real32 starting_health_damage;
	real32 starting_shield_damage;
	tag_reference primary_weapon;    // weap
	int16 primary_weapon_rounds_loaded;
	int16 primary_weapon_rounds_total;
	tag_reference secondary_weapon;  // weap
	int16 secondary_weapon_rounds_loaded;
	int16 secondary_weapon_rounds_total;
	uint8 starting_fragmentation_grenade_count;
	uint8 starting_plasma_grenade_count;
	uint8 starting_spike_grenade_count;
	uint8 starting_firebomb_grenade_count;
};
ASSERT_STRUCT_SIZE(scenario_starting_profile, 68);

enum e_item_spawn_game_type : short
{
	item_spawn_game_type_game_type_none = 0,
	item_spawn_game_type_capture_the_flag = 1,
	item_spawn_game_type_slayer = 2,
	item_spawn_game_type_oddball = 3,
	item_spawn_game_type_king_of_the_hill = 4,
	item_spawn_game_type_race = 5,
	item_spawn_game_type_headhunter = 6,
	item_spawn_game_type_juggernaut = 7,
	item_spawn_game_type_territories = 8,
	item_spawn_game_type_stub = 9,
	item_spawn_game_type_ignored3 = 10,
	item_spawn_game_type_ignored4 = 11,
	item_spawn_game_type_all_game_types = 12,
	item_spawn_game_type_all_except_ctf = 13,
	item_spawn_game_type_all_except_ctf_race = 14
};

enum e_spawn_type : short
{
	spawn_type_both = 0,
	spawn_type_initial_spawn_only = 1,
	spawn_type_respawn_only = 2
};

enum e_campaign_player_type : short
{
	campaign_player_type_masterchief = 0,
	campaign_player_type_dervish = 1,
	campaign_player_type_chief_multiplayer = 2,
	campaign_player_type_elite_multiplayer = 3,
};

// max count: MAXIMUM_SCENARIO_PLAYERS_PER_BLOCK 256
struct scenario_player
{
	// cflt 
	real_point3d position;
	real32 facing_degrees;
	e_game_team team_designator;
	int16 bsp_index;
	e_item_spawn_game_type game_type_1;
	e_item_spawn_game_type game_type_2;
	e_item_spawn_game_type game_type_3;
	e_item_spawn_game_type game_type_4;
	e_spawn_type spawn_type_0;
	e_spawn_type spawn_type_1;
	e_spawn_type spawn_type_2;
	e_spawn_type spawn_type_3;
	string_id unused_name_0;
	string_id unused_name_1;
	e_campaign_player_type campaign_player_type;
	int16 pad[3];
};
ASSERT_STRUCT_SIZE(scenario_player, 52);

enum e_netpoint_type : short
{
	netpoint_type_ctf_flag_spawn = 0,
	netpoint_type_ctf_flag_return = 1,
	netpoint_type_assault_bomb_spawn = 2,
	netpoint_type_assault_bomb_return = 3,
	netpoint_type_oddball_spawn = 4,
	netpoint_type_unused = 5,
	netpoint_type_race_checkpoint = 6,
	netpoint_type_teleporter_src = 7,
	netpoint_type_teleporter_dest = 8,
	netpoint_type_headhunter_bin = 9,
	netpoint_type_territories_flag = 10,
	netpoint_type_king_hill_0 = 11,
	netpoint_type_king_hill_1 = 12,
	netpoint_type_king_hill_2 = 13,
	netpoint_type_king_hill_3 = 14,
	netpoint_type_king_hill_4 = 15,
	netpoint_type_king_hill_5 = 16,
	netpoint_type_king_hill_6 = 17,
	netpoint_type_king_hill_7 = 18
};

enum e_scenario_netpoint_flags : short
{
	scenario_netpoint_flag_multiple_flag_bomb = FLAG(0),
	scenario_netpoint_flag_single_flag_bomb = FLAG(1),
	scenario_netpoint_flag_neutral_flag_bomb = FLAG(2),
};

// max count: MAXIMUM_SCENARIO_NETPOINTS_PER_SCENARIO 200
struct scenario_netpoint
{
	real_point3d position;
	float facing_degrees;

	// filx
	e_netpoint_type type;
	e_game_team team_designator;
	e_scenario_netpoint_flags flags;
	string_id spawn_object_name;
	string_id spawn_marker_name;
};
ASSERT_STRUCT_SIZE(scenario_netpoint, 32);

enum e_respawn_timer_starts : short
{
	respawn_timer_starts_on_pick_up = 0,
	respawn_timer_starts_on_body_depletion = 1
};

enum e_netgame_item_classification : byte
{
	netgame_item_classification_weapon = 0,
	netgame_item_classification_primary_light_land = 1,
	netgame_item_classification_secondary_light_land = 2,
	netgame_item_classification_primary_heavy_land = 3,
	netgame_item_classification_primary_flying = 4,
	netgame_item_classification_scondary_heavy_land = 5,
	netgame_item_classification_primary_turret = 6,
	netgame_item_classification_secondary_turret = 7,
	netgame_item_classification_grenade = 8,
	netgame_item_classification_powerup = 9
};

// max count: MAXIMUM_SCENARIO_NETGAME_EQUIPMENT_PER_SCENARIO 100
struct scenario_netgame_equipment
{
	e_item_spawn_game_type game_type_1;
	e_item_spawn_game_type game_type_2;
	e_item_spawn_game_type game_type_3;
	e_item_spawn_game_type game_type_4;
	short pad;
	short spawn_time_in_seconds;            // 0 Default;
	short respawn_on_empty_time_seconds;
	e_respawn_timer_starts respawn_timer_starts;
	e_netgame_item_classification classification;
	byte pad1[3];
	int64_t pad2[5];
	real_point3d position;
	real_euler_angles3d orientation;
	tag_reference item_vehicle_collection;  // itmc/vehc but allows any tag
	int64_t pad3[6];
};
ASSERT_STRUCT_SIZE(scenario_netgame_equipment, 144);

enum e_scenario_starting_equipment_flags : int
{
	scenario_starting_equipment_flag_no_grenades = FLAG(0),
	scenario_starting_equipment_flag_plasma_grenades = FLAG(1)
};

// max count: MAXIMUM_SCENARIO_STARTING_EQUIPMENT_PER_SCENARIO 200
struct scenario_starting_equipment
{
	e_scenario_starting_equipment_flags flags;
	e_item_spawn_game_type game_type_1;
	e_item_spawn_game_type game_type_2;
	e_item_spawn_game_type game_type_3;
	e_item_spawn_game_type game_type_4;
	int pad[12];
	tag_reference item_collection_1;    // itmc
	tag_reference item_collection_2;    // itmc
	tag_reference item_collection_3;    // itmc
	tag_reference item_collection_4;    // itmc
	tag_reference item_collection_5;    // itmc
	tag_reference item_collection_6;    // itmc
	int pad1[12];
};
ASSERT_STRUCT_SIZE(scenario_starting_equipment, 156);

// max count: MAXIMUM_TRIGGER_VOLUMES_PER_SCENARIO 256
struct scenario_bsp_switch_trigger_volume
{
	short trigger_volume_index; // Block index: scenario_trigger_volume
	short source_bsp_index;     
	short destination_bsp_index; 
	short pad[4];
};
ASSERT_STRUCT_SIZE(scenario_bsp_switch_trigger_volume, 14);

// max count: MAXIMUM_DECALS_PER_SCENARIO 65536
struct scenario_decal
{
	short decal_type;       // Block index: decals_palette

	char yaw;               // -127 - 127
	char pitch;             // -127 - 127
	real_point3d position;
};
ASSERT_STRUCT_SIZE(scenario_decal, 16);

// max count: MAXIMUM_DECAL_PALETTES_PER_SCENARIO 128
struct scenario_decal_palette_entry
{
	tag_reference reference;    // deca
};
ASSERT_STRUCT_SIZE(scenario_decal_palette_entry, 8);

// max count: MAXIMUM_DETAIL_OBJECT_LAYERS_PER_STRUCTURE 32
struct scenario_detail_object_collection_palette_entry
{
	tag_reference name;         // dobc
	int64_t pad[4];
};
ASSERT_STRUCT_SIZE(scenario_detail_object_collection_palette_entry, 40);

// max count: 50
struct style_palette_entry
{
	tag_reference reference;    // styl
};
ASSERT_STRUCT_SIZE(style_palette_entry, 8);

// max count: 64
struct character_palette_entry
{
	tag_reference reference;    // char
};
ASSERT_STRUCT_SIZE(character_palette_entry, 8);

// max count: k_maximum_hs_scripts_per_scenario 1024
struct hs_script
{
	char name[k_tag_string_length];
	int16 script_type;	// e_hs_script_type
	int16 return_type;	// e_hs_type
	int32 root_expression_index;
};
ASSERT_STRUCT_SIZE(hs_script, 40);

// max count: MAXIMUM_CUTSCENE_FLAGS_PER_SCENARIO 512
struct scenario_cutscene_flag
{
	int32 pad;
	char name[k_tag_string_length];
	real_point3d position;
	real_euler_angles2d facing;
};
ASSERT_STRUCT_SIZE(scenario_cutscene_flag, 56);

enum e_cutscene_camera_point_flags : short
{
	cutscene_camera_point_flag_edit_as_relative = FLAG(0),
};

enum e_cutscene_camera_point_type : short
{
	cutscene_camera_point_type_normal = 0,
	cutscene_camera_point_type_ignore_target_orientation = 1,
	cutscene_camera_point_type_dolly = 2,
	cutscene_camera_point_type_ignore_target_updates = 3
};

// max count: MAXIMUM_CUTSCENE_CAMERA_POINTS_PER_SCENARIO 512
struct scenario_cutscene_camera_point
{
	e_cutscene_camera_point_flags flags;
	e_cutscene_camera_point_type type;
	char name[k_tag_string_length];

	/* cmed */
	int32 pad;
	real_point3d position;
	real_vector3d orientation;
};
ASSERT_STRUCT_SIZE(scenario_cutscene_camera_point, 64);

enum e_structure_bsp_reference_flags : short
{
	structure_bsp_reference_flag_default_sky_enabled = FLAG(0)
};

// max count: MAXIMUM_STRUCTURE_BSPS_PER_SCENARIO 16
struct scenario_structure_bsp_reference
{
	uint32 structure_bsp_offset;
	uint32 structure_bsp_size;
	uint32 structure_bsp_address;
	uint32 unk;
	tag_reference structure_bsp;        // sbsp
	tag_reference structure_lightmap;   // ltmp
	uint32 unk2;
	real32 radiance_estimated_search_distance;
	int32 pad;
	real32 luminels_per_world_unit;      // unused
	real32 output_white_reference;       // unused
	int32 pad1[2];
	e_structure_bsp_reference_flags flags;
	int16 pad2;
	int16 default_sky;   // Block Index: scenario_sky_reference
	int16 pad3;
};
ASSERT_STRUCT_SIZE(scenario_structure_bsp_reference, 68);

// max count: NUMBER_OF_SCENARIO_RESOURCE_TYPES 16
struct scenario_resource_reference
{
	tag_reference reference;    // any tag type allowed
};
ASSERT_STRUCT_SIZE(scenario_resource_reference, 8);

// max count: k_maximum_hs_source_files_per_scenario 8
struct s_hs_source_reference
{
	tag_reference reference;    // hsc*
};
ASSERT_STRUCT_SIZE(s_hs_source_reference, 8);

// max count: k_number_of_scenario_ai_types 2
struct s_ai_resource_reference
{
	tag_reference reference;    // ai**
};
ASSERT_STRUCT_SIZE(s_ai_resource_reference, 8);

// max count: 1
struct scenario_resources_definition
{
	s_tag_block references;		// struct: scenario_resource_reference
	s_tag_block script_source;	// struct: s_hs_source_reference
	s_tag_block ai_resources;	// struct: s_ai_resource_reference
};
ASSERT_STRUCT_SIZE(scenario_resources_definition, 24);

// max count: MAXIMUM_STRUCTURE_BSPS_PER_SCENARIO 16
// What a wonderful structure name...
struct s_here_but_for_the_grace_of_god_go_this_poor_soul
{
	/****************************************
	* definition_name: old_unused_mopp_code_data
	* flags: 0
	* alignment_bit: 16
	* byteswap_proc: 0x00531b20
	****************************************/
	// DataSize: 1048576
	data_reference mopp_code;
	s_tag_block environment_object_identifiers;	// struct: c_object_identifier

	int32 pad;
	real_point3d mopp_bounds_min;
	real_point3d mopp_bounds_max;
};
ASSERT_STRUCT_SIZE(s_here_but_for_the_grace_of_god_go_this_poor_soul, 44);

// max count: 1
struct s_scenario_spawn_data
{
	// Explaination("Dynamic Spawn", "Non-0 values here overload what appears in multiplayer_globals.")

	real32 dynamic_spawn_lower_height;
	real32 dynamic_spawn_upper_height;
	real32 game_object_reset_height;
	int32 pad[15];
	s_tag_block dynamic_spawn_overloads;	// struct: s_dynamic_spawn_zone_overload
	s_tag_block static_respawn_zones;		// struct: s_static_spawn_zone
	s_tag_block	static_initial_spawn_zones;	// struct: s_static_spawn_zone
};
ASSERT_STRUCT_SIZE(s_scenario_spawn_data, 96);

// max count: k_maximum_decorator_sets_per_scenario 32
struct s_scenario_decorator_palette_entry
{
	tag_reference decorator_set;  // DECR
};
ASSERT_STRUCT_SIZE(s_scenario_decorator_palette_entry, 8);

// max count: MAXIMUM_TRIGGER_VOLUMES_PER_SCENARIO 256
struct s_scenario_bsp_switch_transition_volume
{
	int32 bsp_index_key;
	int16 trigger_volume;   // Block index: scenario_trigger_volume
	int16 pad;
};
ASSERT_STRUCT_SIZE(s_scenario_bsp_switch_transition_volume, 8);

// max count: MAXIMUM_STRUCTURE_BSPS_PER_SCENARIO 16
struct s_scenario_structure_bsp_spherical_harmonic_lighting
{
	tag_reference bsp;					// sbsp

	// max count: k_maximum_structure_bsp_spherical_harmonic_lighting_points 32768
	s_tag_block lighting_points;		// struct: real_point3d
};
ASSERT_STRUCT_SIZE(s_scenario_structure_bsp_spherical_harmonic_lighting, 16);

// max count: k_maximum_scenario_editor_folders 32767
struct s_scenario_editor_folder
{
	int32 parent_folder;				// Block index: scenario_editor_folder
	char name[256];
};
ASSERT_STRUCT_SIZE(s_scenario_editor_folder, 260);

// max count: 1
struct s_scenario_level_data
{
	tag_reference level_description;
	s_tag_block campaign_level_data;	// struct: s_campaign_ui_level_definition
	s_tag_block multiplayer;			// struct: s_multiplayer_ui_level_definition
};
ASSERT_STRUCT_SIZE(s_scenario_level_data, 24);

// max count: 1
struct ai_scenario_mission_dialogue
{
	tag_reference mission_dialogue;     // mdlg
};
ASSERT_STRUCT_SIZE(ai_scenario_mission_dialogue, 8);

// max count: k_maximum_screen_effect_references_per_scenario 16
struct s_scenario_screen_effect_reference
{
	int32 pad_0[4];
	tag_reference screen_effect;         // egor
	string_id primary_input_interpolator;
	string_id secondary_input_interpolator;
	int16 primary_scenario_interpolator_index;
	int16 secondary_scenario_interpolator_index;
};
ASSERT_STRUCT_SIZE(s_scenario_screen_effect_reference, 36);

// max count: k_maximum_simulation_definition_table_elements_per_scenario 512
struct s_scenario_simulation_definition_table_element
{
	datum tag_datum;
};
ASSERT_STRUCT_SIZE(s_scenario_simulation_definition_table_element, 4);

struct scenario
{
	tag_reference do_not_use;			// sbsp
	s_tag_block skies;					// struct: tag_reference
	e_scenario_type type;
	e_scenario_flags flags;
	s_tag_block child_scenarios;		// struct: scenario_child_scenario_reference

	real32 local_north;
	s_tag_block predicted_resources;	// struct: predicted_resource
	s_tag_block functions;				// struct: scenario_function

	/****************************************
	* definition_name: editor_scenario_data_definition
	* flags: 4
	* alignment_bit: 0
	****************************************/    
	data_reference editor_scenario_data;                // DataSize: 65536
	
	s_tag_block comments;					// struct: editor_comment_definition
	s_tag_block dont_use_me_scenario_environment_object_block;	// struct: scenario_environment_object
	s_tag_block object_names;				// struct: scenario_object_name
	s_tag_block scenery;					// struct: s_scenario_scenery
	s_tag_block scenery_palette;			// struct: scenario_object_palette_entry
	s_tag_block bipeds;						// struct: s_scenario_biped
	s_tag_block biped_palette;				// struct: scenario_object_palette_entry
	s_tag_block vehicles;					// struct: s_scenario_vehicle
	s_tag_block vehicle_palette;			// struct: scenario_object_palette_entry
	s_tag_block equipment;					// struct: s_scenario_equipment
	s_tag_block equipment_palette;			// struct: scenario_object_palette_entry
	s_tag_block weapons;					// struct: s_scenario_weapon
	s_tag_block weapon_palette;				// struct: scenario_object_palette_entry
	s_tag_block device_groups;				// struct: scenario_device_group
	s_tag_block machines;					// struct: scenario_machine_block
	s_tag_block machine_palette;			// struct: scenario_object_palette_entry
	s_tag_block controls;					// struct: s_scenario_control
	s_tag_block control_palette;			// struct: scenario_object_palette_entry
	s_tag_block light_fixtures;				// struct: s_scenario_light_fixture
	s_tag_block light_fixtures_palette;		// struct: scenario_object_palette_entry
	s_tag_block sound_scenery;				// struct: s_scenario_sound_scenery
	s_tag_block sound_scenery_palette;		// struct: scenario_object_palette_entry
	s_tag_block light_volumes;				// struct: s_scenario_light
	s_tag_block light_volumes_palette;		// struct: scenario_object_palette_entry
	s_tag_block player_starting_profile;	// struct: scenario_starting_profile
	s_tag_block player_starting_locations;	// struct: scenario_player
	s_tag_block trigger_volumes;			// struct: scenario_trigger_volume
	s_tag_block recorded_animations;		// struct: recorded_animation_definition
	s_tag_block netgame_flags;				// struct: scenario_netpoint
	s_tag_block netgame_equipment;			// struct: scenario_netgame_equipment
	s_tag_block starting_equipment;			// struct: scenario_starting_equipment
	s_tag_block bsp_switch_trigger_volumes;	// struct: scenario_bsp_switch_trigger_volume
	s_tag_block decals;						// struct: scenario_decal
	s_tag_block decals_palette;				// struct: scenario_decal_palette_entry
	s_tag_block detail_object_collection_palette;	// struct: scenario_detail_object_collection_palette_entry
	s_tag_block style_palette;				// struct: style_palette_entry
	s_tag_block squad_groups;				// struct: squad_group_definition
	s_tag_block squads;						// struct: squad_definition
	s_tag_block zones;						// struct: zone_definition
	s_tag_block mission_scenes;				// struct: ai_scene              
	s_tag_block character_palette;			// struct: character_palette_entry

	// Not populated in cache builds
	s_tag_block ai_pathfinding_data;		// struct: pathfinding_data         

	s_tag_block ai_animation_references;	// struct: ai_animation_reference_definition
	s_tag_block ai_script_references;		// struct: ai_script_reference_definition
	s_tag_block ai_recording_references;	// struct: ai_recording_reference_definition
	s_tag_block ai_conversations;			// struct: ai_conversation

	data_reference script_syntax_data;		// DataSize: 737356
	data_reference hs_string_constants;		// DataSize: 614400
	s_tag_block hs_scripts;					// struct: hs_script
	s_tag_block hs_globals;					// struct: hs_global_internal
	s_tag_block hs_references;				// struct: hs_tag_reference
	s_tag_block source_files;				// struct: hs_source_file
	s_tag_block cs_scripting_data;			// struct: cs_script_data
	s_tag_block cutscene_flags;				// struct: scenario_cutscene_flag
	s_tag_block cutscene_camera_points;		// struct: scenario_cutscene_camera_point
	s_tag_block cutscene_chapter_titles;	// struct: s_scenario_cutscene_title

	tag_reference custom_object_names;      // unic
	tag_reference chapter_title_text;       // unic
	tag_reference hud_messages;             // hmt

	s_tag_block structure_bsps;				// struct: scenario_structure_bsp_reference
	s_tag_block scenario_resources;			// struct: scenario_resources_definition
	s_tag_block old_unused_structure_physics_block;	// struct: s_here_but_for_the_grace_of_god_go_this_poor_soul
	s_tag_block hs_unit_seats;				// struct: s_hs_unit_seat_mapping
	s_tag_block scenario_kill_triggers;		// struct: s_scenario_kill_trigger_volume
	s_tag_block hs_syntax_datums;			// struct: hs_syntax_node
	s_tag_block orders;						// struct: orders_definition
	s_tag_block triggers;					// struct: trigger_definition
	s_tag_block background_sound_palette;	// struct: structure_background_sound_palette_entry
	s_tag_block sound_environment_palette;	// struct: structure_sound_environment_palette_entry
	s_tag_block weather_palette;			// struct: structure_weather_palette_entry
	s_tag_block null_tagblocks[5];          // Unused tagblocks, free real estate if we want to add anything
	s_tag_block scenario_cluster_data;		// struct:

	int32 next_object_identifier_salts[32];

	s_tag_block spawn_data;					// struct: s_scenario_spawn_data
	tag_reference sound_effect_collection;  // sfx+
	s_tag_block crates;						// struct: s_scenario_crate
	s_tag_block crates_palette;				// struct: scenario_object_palette_entry

	// Explaination("Global Lighting Override", "EMPTY STRING")

	tag_reference global_lighting;          // gldf

	// Explaination("WARNING", "Editing Fog palette data will not behave as expected with split scenarios.")

	s_tag_block atmospheric_fog_palette;	// struct: s_scenario_atmospheric_fog_palette_entry
	s_tag_block planar_fog_palette;			// struct: s_scenario_planar_fog_palette_entry
	s_tag_block flocks;						// struct: flock_definition

	tag_reference subtitles;                // unic
	s_tag_block decorators;					// struct: c_decorator_placement_definition
	s_tag_block creatures;					// struct: s_scenario_creature
	s_tag_block creatures_palette;			// struct: scenario_object_palette_entry
	s_tag_block decorators_palette;			// struct: s_scenario_decorator_palette_entry
	s_tag_block bsp_transition_volumes;		// struct: s_scenario_bsp_switch_transition_volume
	s_tag_block structure_bsp_lighting;		// struct: s_scenario_structure_bsp_spherical_harmonic_lighting
	s_tag_block editor_folders;				// struct: s_scenario_editor_folder
	s_tag_block level_data;					// struct: s_scenario_level_data
	tag_reference territory_location_names; // unic
	
	int64_t pad;

	s_tag_block mission_dialogue;				// struct: ai_scenario_mission_dialogue
	tag_reference objectives;					// unic
	s_tag_block interpolators;					// struct: s_scenario_interpolator
	s_tag_block shared_references;				// struct: hs_tag_reference
	s_tag_block screen_effect_references;		// struct: s_scenario_screen_effect_reference
	s_tag_block simulation_definition_table;	// struct: s_scenario_simulation_definition_table_element
};
ASSERT_STRUCT_SIZE(scenario, 992);
