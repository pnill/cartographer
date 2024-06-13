#pragma once
#include "scenario_fog.h"
#include "scenario_interpolators.h"
#include "scenario_kill_trigger_volumes.h"


#include "ai/ai_flocks.h"
#include "ai/ai_mission_dialogue.h"
#include "ai/ai_orders.h"
#include "ai/ai_scenario_definitions.h"
#include "ai/cl_engine.h"
#include "ai/path.h"
#include "ai/zone.h"
#include "cache/predicted_resources.h"
#include "cseries/cseries_strings.h"
#include "decorators/decorator_definitions.h"
#include "editor/editor_flags.h"
#include "game/game_engine_spawning.h"
#include "game/players.h"
#include "hs/hs.h"
#include "hs/hs_library_internal_runtime.h"
#include "hs/hs_unit_seats.h"
#include "main/level_definitions.h"
#include "math/color_math.h"
#include "math/periodic_functions.h"
#include "objects/light_definitions.h"
#include "objects/object_identifier.h"
#include "sound/sound_scenery.h"
#include "structures/cluster_partitions.h"
#include "structures/structure_audibility.h"
#include "tag_files/string_id.h"
#include "tag_files/tag_block.h"


#define MAXIMUM_CHILD_SCENARIOS_PER_SCENARIO 16
#define MAXIMUM_FUNCTIONS_PER_SCENARIO 32
#define MAXIMUM_ENVIRONMENT_OBJECTS_PER_SCENARIO 4096
#define MAXIMUM_OBJECT_NAMES_PER_SCENARIO 640
#define MAXIMUM_STRUCTURE_BSPS_PER_SCENARIO 16
#define MAXIMUM_SCENERY_DATUMS_PER_SCENARIO 2000
#define MAXIMUM_SCENARIO_OBJECT_PALETTE_ENTRIES_PER_BLOCK 256
#define MAXIMUM_BIPED_DATUMS_PER_SCENARIO 128
#define MAXIMUM_VEHICLE_DATUMS_PER_SCENARIO 256
#define MAXIMUM_EQUIPMENT_DATUMS_PER_SCENARIO 256
#define MAXIMUM_WEAPON_DATUMS_PER_SCENARIO 128
#define MAXIMUM_DEVICE_GROUPS_PER_SCENARIO 128
#define MAXIMUM_MACHINE_DATUMS_PER_SCENARIO 400
#define MAXIMUM_CONTROL_DATUMS_PER_SCENARIO 100
#define MAXIMUM_LIGHT_FIXTURE_DATUMS_PER_SCENARIO 500
#define MAXIMUM_SOUND_SCENERY_DATUMS_PER_SCENARIO 256
#define MAXIMUM_LIGHTS_PER_SCENARIO 500
#define MAXIMUM_SCENARIO_PLAYERS_PER_BLOCK 256
#define MAXIMUM_TRIGGER_VOLUMES_PER_SCENARIO 256
#define MAXIMUM_RECORDED_ANIMATIONS_PER_MAP 1024
#define MAXIMUM_SCENARIO_NETPOINTS_PER_SCENARIO 200
#define MAXIMUM_SCENARIO_NETGAME_EQUIPMENT_PER_SCENARIO 100
#define MAXIMUM_SCENARIO_STARTING_EQUIPMENT_PER_SCENARIO 200
#define MAXIMUM_DECALS_PER_SCENARIO 65536
#define MAXIMUM_DECAL_PALETTES_PER_SCENARIO 128
#define MAXIMUM_DETAIL_OBJECT_LAYERS_PER_STRUCTURE 32
#define k_max_scenes_per_scenario 100
#define k_maximum_hs_scripts_per_scenario 1024
#define MAXIMUM_CUTSCENE_FLAGS_PER_SCENARIO 512
#define MAXIMUM_CUTSCENE_CAMERA_POINTS_PER_SCENARIO 512
#define k_maximum_scenario_cutscene_titles 128
#define NUMBER_OF_SCENARIO_RESOURCE_TYPES 16
#define k_number_of_scenario_ai_types 2
#define k_maximum_hs_syntax_nodes_per_scenario 36864
#define MAXIMUM_CRATE_DATUMS_PER_SCENARIO 1024
#define MAXIMUM_CREATURE_DATUMS_PER_SCENARIO 128
#define k_maximum_decorator_sets_per_scenario 32
#define k_maximum_structure_bsp_spherical_harmonic_lighting_points 32768
#define k_maximum_scenario_editor_folders 32767
#define k_maximum_screen_effect_references_per_scenario 16
#define k_maximum_simulation_definition_table_elements_per_scenario 512

enum e_scenario_type : short
{
	scenario_type_singleplayer = 0,
	scenario_type_multiplayer = 1,
	scenario_type_mainmenu = 2,
	scenario_type_multiplayer_shared = 3,
	scenario_type_singleplayer_shared = 4
};

enum e_scenario_flags : short
{
	scenario_flag_cortana_hack = FLAG(0),								// Sorts Cortana In Front Of Other Transparent Geometry
	scenario_flag_always_draw_sky = FLAG(1),							// Always Draws Sky Even If No sky Polygons Are Visible
	scenario_flag_dont_strip_pathfinding = FLAG(2),						// Always Leaves Pathfinding In Even For Multiplayer Scenario
	scenario_flag_symmetric_multiplayer_map = FLAG(3),
	scenario_flag_quick_loading_cinematic_only_scenario = FLAG(4),
	scenario_flag_characters_use_previous_mission_weapons = FLAG(5),
	scenario_flag_lightmaps_smooth_palettes_with_neighbors = FLAG(6),
	scenario_flag_snap_to_white_at_start = FLAG(7)
};

// max count: MAXIMUM_CHILD_SCENARIOS_PER_SCENARIO 16
struct scenario_child_scenario_reference
{
	tag_reference childScenario;	// scnr
	char pad[16];
};
ASSERT_STRUCT_SIZE(scenario_child_scenario_reference, 24);

enum e_scenario_function_flags : int
{
    scenario_function_flag_scripted = FLAG(0),      // Level Script Will Set This Value Other Settings Here Will Be Ignored
    scenario_function_flag_invert = FLAG(1),        // Result Of Function Is 1 Minus Actual Result
    scenario_function_flag_additive = FLAG(2),
    scenario_function_flag_always_active = FLAG(3)  // Function Does Not Deactivate When At Or Below Lower Bound
};


enum e_bounds_mode : short
{
    bounds_mode_clip = 0,
    bounds_mode_clip_and_normalize = 1,
    bounds_mode_scale_to_fit = 2
};

// max count: MAXIMUM_FUNCTIONS_PER_SCENARIO 32
struct scenario_function
{
    e_scenario_function_flags flags;
    static_string32 name;
    float period_seconds;               // Period for above function (lower values make function oscillate quickly; higher values make it oscillate slowly).
    
    // Multiply this function by above period
    // This is the tagblock index for another scenario_function
    short scale_period_by;
    e_periodic_function_type function;

    // Multiply this function by result of above function.
    // This is the tagblock index for another scenario_function
    short scale_function_by;
    e_periodic_function_type wobble_function;   // Curve used for wobble.
    float wobble_period_seconds;                // Time it takes for magnitude of this function to complete a wobble.
    float wobble_magnitude_percent;             // Amount of random wobble in the magnitude.
    float square_wave_threshold;                // If non-zero, all values above square wave threshold are snapped to 1.0, and all values below it are snapped to 0.0 to create a square wave.
    short step_count;                           // Number of discrete values to snap to (e.g., step count of 5 snaps function to 0.00, 0.25, 0.50,0.75, or 1.00).

    e_transition_function_type map_to;
    
    short sawtooth_count;                       // Number of times this function should repeat (e.g., sawtooth count of 5 gives function value of 1.0 at each of 0.25, 0.50, and 0.75, as well as at 1.0).
    short pad;

    // Multiply this function (e.g., from a weapon, vehicle) final result of all of the above math.
    // This is the tagblock index for another scenario_function
    short scale_result_by;

    e_bounds_mode bounds_mode;                   // Controls how bounds, below, are used.
    real_vector2d bounds;
    short pad1[3];

    // If specified function is off, so is this function.
    // This is the tagblock index for another scenario_function
    short turn_off_with;
    int pad2[8];
};
ASSERT_STRUCT_SIZE(scenario_function, 120);

// max count: MAXIMUM_ENVIRONMENT_OBJECTS_PER_SCENARIO 4096
struct scenario_environment_object
{
    // envf 
    short bsp;      // Block index: scenario_structure_bsp_reference
    short runtime_object_type;
    datum unique_id;
    int pad;
    datum object_definition_tag;
    int object;
    int pad1[11];
};
ASSERT_STRUCT_SIZE(scenario_environment_object, 64);

// max count: MAXIMUM_OBJECT_NAMES_PER_SCENARIO 640
struct scenario_object_name
{
    static_string32 name;
    e_object_type type;
    int8 pad;
    short placement_index;
};
ASSERT_STRUCT_SIZE(scenario_object_name, 36);

enum e_object_placement_flags : int
{
    object_placement_flag_not_automatically = FLAG(0),
    object_placement_flag_not_on_easy = FLAG(1),
    object_placement_flag_not_on_normal = FLAG(2),
    object_placement_flag_not_on_hard = FLAG(3),
    object_placement_flag_lock_type_to_env_object = FLAG(4),
    object_placement_flag_lock_transform_to_env_object = FLAG(5),
    object_placement_flag_never_placed = FLAG(6),
    object_placement_flag_lock_name_to_env_object = FLAG(7),
    object_placement_flag_create_at_rest = FLAG(8)
};

enum e_transform_flags : short
{
    transform_flag_mirrored = FLAG(0)
};

enum e_manual_bsp_flags : WORD
{
    bsp_0 = FLAG(0),
    bsp_1 = FLAG(1),
    bsp_2 = FLAG(2),
    bsp_3 = FLAG(3),
    bsp_4 = FLAG(4),
    bsp_5 = FLAG(5),
    bsp_6 = FLAG(6),
    bsp_7 = FLAG(7),
    bsp_8 = FLAG(8),
    bsp_9 = FLAG(9),
    bsp_10 = FLAG(10),
    bsp_11 = FLAG(11),
    bsp_12 = FLAG(12),
    bsp_13 = FLAG(13),
    bsp_14 = FLAG(14),
    bsp_15 = FLAG(15)
};

struct s_scenario_object_datum
{
    // obed
    e_object_placement_flags placement_flags;
    real_point3d position;
    real_euler_angles3d rotation;
    float scale;
    e_transform_flags transform_flags;
    e_manual_bsp_flags manual_bsp_flags;
    c_object_identifier object_id;
    e_bsp_policy bsp_policy;
    char pad;
    short editor_folder_index;
};
ASSERT_STRUCT_SIZE(s_scenario_object_datum, 48);

enum e_active_change_colors : int
{
    active_change_color_primary = FLAG(0),
    active_change_color_secondary = FLAG(1),
    active_change_color_tertiary = FLAG(2),
    active_change_color_quaternary = FLAG(3)
};

struct s_scenario_object_permutation
{
    string_id variant_name;
    e_active_change_colors active_change_colors;
    pixel32 primary_color;
    pixel32 secondary_color;
    pixel32 tertiary_color;
    pixel32 quaternary_color;
};
ASSERT_STRUCT_SIZE(s_scenario_object_permutation, 24);

enum e_pathfinding_policy : short
{
    pathfinding_policy_tag_default = 0,
    pathfinding_policy_pathfinding_dynamic = 1,
    pathfinding_policy_pathfinding_cutout = 2,
    pathfinding_policy_pathfinding_static = 3,
    pathfinding_policy_pathfinding_none = 4
};

enum e_lightmapping_policy : short
{
    lightmapping_policy_tag_default = 0,
    lightmapping_policy_dynamic = 1,
    lightmapping_policy_per_vertex = 2
};

// max count: MAXIMUM_STRUCTURE_BSPS_PER_SCENARIO 16
struct pathfinding_object_index_list
{
    short bsp_index;
    short pathfinding_object_index;
};
ASSERT_STRUCT_SIZE(pathfinding_object_index_list, 4);

struct s_scenario_scenery_datum
{
    e_pathfinding_policy pathfinding_policy;
    e_lightmapping_policy lightmapping_policy;
    tag_block<pathfinding_object_index_list> pathfinding_references;
    short pad;
    e_valid_multiplayer_games valid_multiplayer_games;
};
ASSERT_STRUCT_SIZE(s_scenario_scenery_datum, 16);

// max count: MAXIMUM_SCENERY_DATUMS_PER_SCENARIO 2000
struct s_scenario_scenery
{
    // filt
    // Block index: scenario_scenery_palette
    short type;
    // filt
    // Block index: scenario_object_name
    short name;

    s_scenario_object_datum object_data;
    s_scenario_object_permutation permutation_data;
    s_scenario_scenery_datum scenery_data;
};
ASSERT_STRUCT_SIZE(s_scenario_scenery, 92);

// max count: MAXIMUM_SCENARIO_OBJECT_PALETTE_ENTRIES_PER_BLOCK 256
struct scenario_object_palette_entry
{
    tag_reference name;
    uint64_t pad[4];
};
ASSERT_STRUCT_SIZE(scenario_object_palette_entry, 40);

enum e_scenario_unit_datum_flags : int
{
    scenario_unit_datum_flag_dead = FLAG(0),
    scenario_unit_datum_flag_closed = FLAG(1),
    scenario_unit_datum_flag_not_enterable_by_player = FLAG(2),
};

struct s_scenario_unit_datum
{
    float body_vitality;
    e_scenario_unit_datum_flags flags;
};
ASSERT_STRUCT_SIZE(s_scenario_unit_datum, 8);

// max count: MAXIMUM_BIPED_DATUMS_PER_SCENARIO 128
struct s_scenario_biped
{
    // filt
    // Block index: scenario_biped_palette
    short type;
    // filt
    // Block index: scenario_object_name
    short name;

    s_scenario_object_datum object_data;
    s_scenario_object_permutation permutation_data;
    s_scenario_unit_datum unit_data;
};
ASSERT_STRUCT_SIZE(s_scenario_biped, 84);

// max count: MAXIMUM_VEHICLE_DATUMS_PER_SCENARIO 256
struct s_scenario_vehicle
{
    // filt
    // Block index: scenario_vehicle_palette
    short type;
    // filt
    // Block index: scenario_object_name
    short name;

    s_scenario_object_datum object_data;
    s_scenario_object_permutation permutation_data;
    s_scenario_unit_datum unit_data;
};
ASSERT_STRUCT_SIZE(s_scenario_vehicle, 84);

enum e_item_datum_flags : int
{
    equipment_datum_flag_initially_at_rest_does_not_fall = FLAG(0),
    equipment_datum_flag_obsolete = FLAG(1),
    equipment_datum_flag_does_accelerate_moves_due_to_explosions = FLAG(2)
};

struct s_scenario_equipment_datum
{
    e_item_datum_flags flags;
};
ASSERT_STRUCT_SIZE(s_scenario_equipment_datum, 4);

// max count: MAXIMUM_EQUIPMENT_DATUMS_PER_SCENARIO 256
struct s_scenario_equipment
{
    // filt
    // Block index: scenario_equipment_palette
    short type;
    // filt
    // Block index: scenario_object_name
    short name;

    s_scenario_object_datum object_data;
    s_scenario_equipment_datum equipment_data;
};
ASSERT_STRUCT_SIZE(s_scenario_equipment, 56);

struct s_scenario_weapon_datum
{
    short rounds_left;
    short rounds_loaded;
    e_item_datum_flags flags;
};
ASSERT_STRUCT_SIZE(s_scenario_weapon_datum, 8);

// max count: MAXIMUM_WEAPON_DATUMS_PER_SCENARIO 128
struct s_scenario_weapon
{
    // filt
    // Block index: scenario_weapon_palette
    short type;
    // filt
    // Block index: scenario_object_name
    short name;

    s_scenario_object_datum object_data;
    s_scenario_object_permutation permutation_data;
    s_scenario_weapon_datum weapon_data;
};
ASSERT_STRUCT_SIZE(s_scenario_weapon, 84);

enum e_device_group_flags : int
{
    device_group_flag_can_change_only_once = FLAG(0)
};

// max count: MAXIMUM_DEVICE_GROUPS_PER_SCENARIO 128
struct scenario_device_group
{
    static_string32 name;
    float initial_value;
    e_device_group_flags flags;
};
ASSERT_STRUCT_SIZE(scenario_device_group, 40);

enum e_scenario_device_datum_flags : int
{
    scenario_device_datum_flag_initially_open = FLAG(0),
    scenario_device_datum_flag_initially_off = FLAG(1),
    scenario_device_datum_flag_can_change_only_once = FLAG(2),
    scenario_device_datum_flag_position_reversed = FLAG(3),
    scenario_device_datum_flag_not_usable_from_any_side = FLAG(4)
};

struct s_scenario_device_datum
{
    short power_group;       // Block index: scenario_device_group
    short position_group;    // Block index: scenario_device_group
    e_scenario_device_datum_flags flags;
};
ASSERT_STRUCT_SIZE(s_scenario_device_datum, 8);

enum e_scenario_machine_datum_flags : int
{
    scenario_machine_datum_flag_does_not_operate_automatically = FLAG(0),
    scenario_machine_datum_flag_one_sided = FLAG(1),
    scenario_machine_datum_flag_never_appears_locked = FLAG(2),
    scenario_machine_datum_flag_opened_by_melee_attack = FLAG(3),
    scenario_machine_datum_flag_one_sided_for_player = FLAG(4),
    scenario_machine_datum_flag_does_not_close_automatically = FLAG(5)
};

struct s_scenario_machine_datum
{
    e_scenario_machine_datum_flags flags;
    tag_block<pathfinding_object_index_list> pathfinding_references;
};
ASSERT_STRUCT_SIZE(s_scenario_machine_datum, 12);

// max count: MAXIMUM_MACHINE_DATUMS_PER_SCENARIO 400
struct scenario_machine_block
{
    // filt
    // Block index: scenario_machine_palette
    short type;
    // filt
    // Block index: scenario_object_name
    short name;

    s_scenario_object_datum object_data;
    s_scenario_device_datum device_data;
    s_scenario_machine_datum machine_data;
};
ASSERT_STRUCT_SIZE(scenario_machine_block, 72);

enum e_scenario_control_datum_flags : int
{
    scenario_control_datum_flag_usable_from_both_sides = FLAG(0)
};

struct s_scenario_control_datum
{
    e_scenario_control_datum_flags flags;
    short dont_touch_this;
    short pad;
};
ASSERT_STRUCT_SIZE(s_scenario_control_datum, 8);

// max count: MAXIMUM_CONTROL_DATUMS_PER_SCENARIO 100
struct s_scenario_control
{
    // filt
    // Block index: scenario_control_palette
    short type;
    // filt
    // Block index: scenario_object_name
    short name;

    s_scenario_object_datum object_data;
    s_scenario_device_datum device_data;
    s_scenario_control_datum control_data;
};
ASSERT_STRUCT_SIZE(s_scenario_control, 68);

struct s_scenario_light_fixture_datum
{
    real_rgb_color color;
    float intensity;
    float falloff_angle_degrees;
    float cutoff_angle_degrees;
};

// max count: MAXIMUM_LIGHT_FIXTURE_DATUMS_PER_SCENARIO 500
struct s_scenario_light_fixture
{
    // filt
    // Block index: scenario_light_fixture_palette
    short type;
    // filt
    // Block index: scenario_object_name
    short name;

    s_scenario_object_datum object_data;
    s_scenario_device_datum device_data;
    s_scenario_light_fixture_datum lightfixture_data;
};
ASSERT_STRUCT_SIZE(s_scenario_light_fixture, 84);

struct s_scenario_sound_scenery_datum
{
    e_sound_scenery_volume_type volume_type;
    float height;
    real_vector2d override_distance_bounds;
    real_bounds override_cone_real_bounds;
    float override_outer_cone_gain_db;
};
ASSERT_STRUCT_SIZE(s_scenario_sound_scenery_datum, 28);

// max count: MAXIMUM_SOUND_SCENERY_DATUMS_PER_SCENARIO 256
struct s_scenario_sound_scenery
{
    // filt
    // Block index: scenario_sound_scenery_palette
    short type;
    // filt
    // Block index: scenario_object_name
    short name;

    s_scenario_object_datum object_data;
    s_scenario_sound_scenery_datum soundscenery;
};
ASSERT_STRUCT_SIZE(s_scenario_sound_scenery, 80);

// max count: MAXIMUM_LIGHTS_PER_SCENARIO 500
struct s_scenario_light
{
    // Explaination("~Controls", " ")

    // lflg
    // allg
    // filt
    // Block index: scenario_light_palette
    short type;
    // filt
    // Block index: scenario_object_name
    short name;

    s_scenario_object_datum object_data;
    s_scenario_device_datum device_data;
    s_scenario_light_datum light_data;
};
ASSERT_STRUCT_SIZE(s_scenario_light, 108);

// max count: MAXIMUM_SCENARIO_PLAYERS_PER_BLOCK 256
struct scenario_starting_profile
{
    static_string32 name;
    float starting_health_damage;
    float starting_shield_damage;
    tag_reference primary_weapon;    // weap
    short primary_weapon_rounds_loaded;
    short primary_weapon_rounds_total;
    tag_reference secondary_weapon;  // weap
    short secondary_weapon_rounds_loaded;
    short secondary_weapon_rounds_total;
    byte starting_fragmentation_grenade_count;
    byte starting_plasma_grenade_count;
    byte starting_spike_grenade_count;
    byte starting_firebomb_grenade_count;
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
    float facing_degrees;
    e_game_team team_designator;
    short bsp_index;
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
    short pad[3];
};
ASSERT_STRUCT_SIZE(scenario_player, 52);

// max count: MAXIMUM_TRIGGER_VOLUMES_PER_SCENARIO 256
struct scenario_trigger_volume
{
    string_id name;
    short object_name;                      // Block index: scenario_light_palette
    short pad;
    string_id node_name;
    real_vector3d forward;
    real_vector3d up;
    real_point3d position;
    real_point3d extents;
    float unk_float;
    short kill_triggger_volume_index;       // Block index: scenario_trigger_volume
};
ASSERT_STRUCT_SIZE(scenario_trigger_volume, 68);

// max count: MAXIMUM_RECORDED_ANIMATIONS_PER_MAP 1024
struct recorded_animation_definition
{
    static_string32 name;
    byte version;
    byte raw_animation_data;
    byte unit_control_data_version;
    byte pad;
    short length_of_animation_ticks;
    short pad1[3];
    data_reference recorded_animation_event_stream; // DataSize: 2097152
};
ASSERT_STRUCT_SIZE(recorded_animation_definition, 52);

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
    static_string32 name;
    e_hs_script_type script_type;
    e_hs_type return_type;
    datum root_expression_index;
};
ASSERT_STRUCT_SIZE(hs_script, 40);

// max count: MAXIMUM_CUTSCENE_FLAGS_PER_SCENARIO 512
struct scenario_cutscene_flag
{
    int pad;
    static_string32 name;
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
    static_string32 name;
    
    /* cmed */
    int pad;
    real_point3d position;
    real_vector3d orientation;
};
ASSERT_STRUCT_SIZE(scenario_cutscene_camera_point, 64);

enum e_text_justification : short
{
    text_justification_left = 0,
    text_justification_right = 1,
    text_justification_center = 2,
    text_justification_custom_text_entry = 3
};

enum e_text_font : short
{
    text_font_terminal_font = 0,
    text_font_body_text_font = 1,
    text_font_title_font = 2,
    text_font_super_large_font = 3,
    text_font_large_body_text_font = 4,
    text_font_split_screen_hud_message_font = 5,
    text_font_full_screen_hud_message_font = 6,
    text_font_english_body_text_font = 7,
    text_font_hud_number_font = 8,
    text_font_subtitle_font = 9,
    text_font_main_menu_font = 10,
    text_font_text_chat_font = 11
};

// max count: k_maximum_scenario_cutscene_titles 128
struct s_scenario_cutscene_title
{
    string_id name;
    rectangle2d text_bounds_on_screen;

    e_text_justification justification;
    e_text_font font;
    pixel32 text_color;
    pixel32 shadow_color;
    float fade_in_time_seconds;
    float up_time_seconds;
    float fade_out_time_seconds;
};
ASSERT_STRUCT_SIZE(s_scenario_cutscene_title, 36);

enum e_structure_bsp_reference_flags : short
{
    structure_bsp_reference_flag_default_sky_enabled = FLAG(0)
};

// max count: MAXIMUM_STRUCTURE_BSPS_PER_SCENARIO 16
struct scenario_structure_bsp_reference
{
    DWORD structure_bsp_offset;
    DWORD structure_bsp_size;
    DWORD structure_bsp_address;
    DWORD unk;
    tag_reference structure_bsp;        // sbsp
    tag_reference structure_lightmap;   // ltmp
    DWORD unk2;
    float radiance_estimated_search_distance;
    int pad;
    float luminels_per_world_unit;      // unused
    float output_white_reference;       // unused
    int pad1[2];
    e_structure_bsp_reference_flags flags;
    short pad2;
    short default_sky;   // Block Index: scenario_sky_reference
    short pad3;
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
    tag_block<scenario_resource_reference> references;
    tag_block<s_hs_source_reference> scriptSource;
    tag_block<s_ai_resource_reference> aIResources;
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
    tag_block<c_object_identifier> environment_object_identifiers;

    int pad;
    real_point3d mopp_bounds_min;
    real_point3d mopp_bounds_max;
};
ASSERT_STRUCT_SIZE(s_here_but_for_the_grace_of_god_go_this_poor_soul, 44);

// max count: 1
struct s_scenario_spawn_data
{
    // Explaination("Dynamic Spawn", "Non-0 values here overload what appears in multiplayer_globals.")
    float dynamic_spawn_lower_height;
    float dynamic_spawn_upper_height;
    float game_object_reset_height;
    int pad[15];
    tag_block<s_dynamic_spawn_zone_overload> dynamic_spawn_overloads;
    tag_block<s_static_spawn_zone> static_respawn_zones;
    tag_block<s_static_spawn_zone> static_initial_spawn_zones;
};
ASSERT_STRUCT_SIZE(s_scenario_spawn_data, 96);

// max count: MAXIMUM_CRATE_DATUMS_PER_SCENARIO 1024
struct s_scenario_crate
{
    // filt
    // Block index: scenario_crate_palette
    short type;
    // filt
    // Block index: scenario_object_name
    short name;

    s_scenario_object_datum object_data;
    s_scenario_object_permutation permutation_data;
};
ASSERT_STRUCT_SIZE(s_scenario_crate, 76);

// max count: MAXIMUM_CREATURE_DATUMS_PER_SCENARIO 128
struct s_scenario_creature
{
    // filt
    // Block index: scenario_creature_palette
    short type;
    // filt
    // Block index: scenario_object_name
    short name;
    s_scenario_object_datum object_data;
};
ASSERT_STRUCT_SIZE(s_scenario_creature, 52);

// max count: k_maximum_decorator_sets_per_scenario 32
struct s_scenario_decorator_palette_entry
{
    tag_reference decorator_set;  // DECR
};
ASSERT_STRUCT_SIZE(s_scenario_decorator_palette_entry, 8);

// max count: MAXIMUM_TRIGGER_VOLUMES_PER_SCENARIO 256
struct s_scenario_bsp_switch_transition_volume
{
    int bsp_index_key;
    short trigger_volume;   // Block index: scenario_trigger_volume
    short pad;
};
ASSERT_STRUCT_SIZE(s_scenario_bsp_switch_transition_volume, 8);

// max count: MAXIMUM_STRUCTURE_BSPS_PER_SCENARIO 16
struct s_scenario_structure_bsp_spherical_harmonic_lighting
{
    tag_reference bsp;                              // sbsp
    tag_block<real_point3d> lighting_points;        // max count: k_maximum_structure_bsp_spherical_harmonic_lighting_points 32768
};
ASSERT_STRUCT_SIZE(s_scenario_structure_bsp_spherical_harmonic_lighting, 16);

// max count: k_maximum_scenario_editor_folders 32767
struct s_scenario_editor_folder
{
    int parent_folder;                              // Block index: scenario_editor_folder
    static_string256 name;
};
ASSERT_STRUCT_SIZE(s_scenario_editor_folder, 260);

// max count: 1
struct s_scenario_level_data
{
    tag_reference level_description;
    tag_block<s_campaign_ui_level_definition> campaign_level_data;
    tag_block<s_multiplayer_ui_level_definition> multiplayer;
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
	tag_reference do_not_use;	                    // sbsp
	tag_block<tag_reference> skies;
	e_scenario_type type;
	e_scenario_flags flags;
	tag_block<scenario_child_scenario_reference> child_scenarios;

	float local_north;
	tag_block<predicted_resource> predicted_resources;
    tag_block<scenario_function> functions;         // unused?

    /****************************************
    * definition_name: editor_scenario_data_definition
    * flags: 4
    * alignment_bit: 0
    ****************************************/    
    data_reference editor_scenario_data;                // DataSize: 65536
    tag_block<editor_comment_definition> comments;
    tag_block<scenario_environment_object> dont_use_me_scenario_environment_object_block;   
    tag_block<scenario_object_name> object_names;
    tag_block<s_scenario_scenery> scenery;
    tag_block<scenario_object_palette_entry> scenery_palette;
    tag_block<s_scenario_biped> bipeds;
    tag_block<scenario_object_palette_entry> biped_palette;
    tag_block<s_scenario_vehicle> vehicles;
    tag_block<scenario_object_palette_entry> vehicle_palette;
    tag_block<s_scenario_equipment> equipment;
    tag_block<scenario_object_palette_entry> equipment_palette;
    tag_block<s_scenario_weapon> weapons;
    tag_block<scenario_object_palette_entry> weapon_palette;
    tag_block<scenario_device_group> device_groups;
    tag_block<scenario_machine_block> machines;
    tag_block<scenario_object_palette_entry> machine_palette;
    tag_block<s_scenario_control> controls;
    tag_block<scenario_object_palette_entry> control_palette;
    tag_block<s_scenario_light_fixture> light_fixtures;
    tag_block<scenario_object_palette_entry> light_fixtures_palette;
    tag_block<s_scenario_sound_scenery> sound_scenery;
    tag_block<scenario_object_palette_entry> sound_scenery_palette;
    tag_block<s_scenario_light> light_volumes;
    tag_block<scenario_object_palette_entry> light_volumes_palette;
    tag_block<scenario_starting_profile> player_starting_profile;
    tag_block<scenario_player> player_starting_locations;
    tag_block<scenario_trigger_volume> trigger_volumes;
    tag_block<recorded_animation_definition> recorded_animations;
    tag_block<scenario_netpoint> netgame_flags;
    tag_block<scenario_netgame_equipment> netgame_equipment;
    tag_block<scenario_starting_equipment> starting_equipment;
    tag_block<scenario_bsp_switch_trigger_volume> bsp_switch_trigger_volumes;
    tag_block<scenario_decal> decals;
    tag_block<scenario_decal_palette_entry> decals_palette;
    tag_block<scenario_detail_object_collection_palette_entry> detail_object_collection_palette;
    tag_block<style_palette_entry> style_palette;
    tag_block<squad_group_definition> squad_groups;
    tag_block<squad_definition> squads;
    tag_block<zone_definition> zones;
    tag_block<ai_scene> mission_scenes;                         
    tag_block<character_palette_entry> character_palette;
    tag_block<pathfinding_data> ai_pathfinding_data;            // Not populated in cache builds

    tag_block<ai_animation_reference_definition> ai_animation_references;
    tag_block<ai_script_reference_definition> ai_script_references;
    tag_block<ai_recording_reference_definition> ai_recording_references;
    tag_block<ai_conversation> ai_conversations;

    data_reference script_syntax_data;  // DataSize: 737356
    data_reference script_string_data;  // DataSize: 614400
    tag_block<hs_script> scripts;
    tag_block<hs_global_internal> globals;
    tag_block<hs_tag_reference> references;
    tag_block<hs_source_file> source_files;
    tag_block<cs_script_data> scripting_data;
    tag_block<scenario_cutscene_flag> cutscene_flags;
    tag_block<scenario_cutscene_camera_point> cutscene_camera_points;
    tag_block<s_scenario_cutscene_title> cutsceneTitles;

    tag_reference custom_object_names;      // unic
    tag_reference chapter_title_text;       // unic
    tag_reference hud_messages;             // hmt

    tag_block<scenario_structure_bsp_reference> structure_bsps;
    tag_block<scenario_resources_definition> scenario_resources;
    tag_block<s_here_but_for_the_grace_of_god_go_this_poor_soul> old_unused_structure_physics_block;
    tag_block<s_hs_unit_seat_mapping> hs_unit_seats;
    tag_block<s_scenario_kill_trigger_volume> scenario_kill_triggers;
    tag_block<hs_syntax_node> hs_syntax_datums;
    tag_block<orders_definition> orders;
    tag_block<triggers_block> triggers;
    tag_block<structure_background_sound_palette_entry> background_sound_palette;
    tag_block<structure_sound_environment_palette_entry> sound_environment_palette;
    tag_block<structure_weather_palette_entry> weather_palette;
    tag_block<> null_tagblocks[5];          // Unused tagblocks, free real estate if we want to add anything
    tag_block<s_scenario_cluster_data> scenario_cluster_data;

    int objectsalts[32];

    tag_block<s_scenario_spawn_data> spawn_data;
    tag_reference sound_effect_collection;  // sfx+
    tag_block<s_scenario_crate> crates;
    tag_block<scenario_object_palette_entry> crates_palette;

    // Explaination("Global Lighting Override", "EMPTY STRING")
    tag_reference global_lighting;          // gldf

    // Explaination("WARNING", "Editing Fog palette data will not behave as expected with split scenarios.")
    tag_block<s_scenario_atmospheric_fog_palette_entry> atmospheric_fog_palette;
    tag_block<s_scenario_planar_fog_palette_entry> planar_fog_palette;
    tag_block<flock_definition> flocks;

    tag_reference subtitles;                // unic
    tag_block<c_decorator_placement_definition> decorators;
    tag_block<s_scenario_creature> creatures;
    tag_block<scenario_object_palette_entry> creatures_palette;
    tag_block<s_scenario_decorator_palette_entry> decorators_palette;
    tag_block<s_scenario_bsp_switch_transition_volume> bsp_transition_volumes;
    tag_block<s_scenario_structure_bsp_spherical_harmonic_lighting> structure_bsp_lighting;
    tag_block<s_scenario_editor_folder> editor_folders;
    tag_block<s_scenario_level_data> level_data;
    tag_reference territory_location_names; // unic
    
    int64_t pad;

    tag_block<ai_scenario_mission_dialogue> mission_dialogue;
    tag_reference objectives;               // unic
    tag_block<s_scenario_interpolator> interpolators;
    tag_block<hs_tag_reference> shared_references;
    tag_block<s_scenario_screen_effect_reference> screen_effect_references;
    tag_block<s_scenario_simulation_definition_table_element> simulation_definition_table;
};
ASSERT_STRUCT_SIZE(scenario, 992);

typedef s_scenario_scenery s_scenario_object;