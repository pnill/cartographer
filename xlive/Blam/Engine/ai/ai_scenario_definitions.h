#pragma once
#include "Blam/Common/Common.h"
#include "Blam/Cache/DataTypes/TagBlock.h"
#include "Blam/Engine/objects/objects.h"
#include "Blam/Math/real_math.h"

enum e_combination_rule : short
{
    combination_rule_or = 0,
    combination_rule_and = 1
};

// max count: 100
struct squad_group_definition
{
    static_string32 name;
    short parent;           // Block index: squad_groups_definition
    short initial_orders;   // Block index: orders_definition
};
TAG_BLOCK_SIZE_ASSERT(squad_group_definition, 36);

enum e_squad_definition_flags : int
{
    squad_definition_flag_unused = FLAG(0),
    squad_definition_flag_never_search = FLAG(1),
    squad_definition_flag_start_timer_immediately = FLAG(2),
    squad_definition_flag_no_timer_delay_forever = FLAG(3),
    squad_definition_flag_magic_sight_after_timer = FLAG(4),
    squad_definition_flag_automatic_migration = FLAG(5),
    squad_definition_flag_depreciated = FLAG(6),
    squad_definition_flag_respawn_enabled = FLAG(7),
    squad_definition_flag_blind = FLAG(8),
    squad_definition_flag_deaf = FLAG(9),
    squad_definition_flag_braindead = FLAG(10),
    squad_definition_flag_3d_firing_positions = FLAG(11),
    squad_definition_flag_initially_placed = FLAG(12),
    squad_definition_flag_units_not_enterable_by_player = FLAG(13)
};

enum e_major_upgrade : short
{
    major_upgrade_normal = 0,
    major_upgrade_few = 1,
    major_upgrade_many = 2,
    major_upgrade_none = 3,
    major_upgrade_all = 4
};

enum e_grenade_type : short
{
    grenade_type_none = 0,
    grenade_type_human_grenade = 1,
    grenade_type_covenant_plasma = 2
};

enum actor_starting_location_flags : int
{
    actor_starting_location_flag_initially_asleep = FLAG(0),
    actor_starting_location_flag_infection_form_explode = FLAG(1),
    actor_starting_location_flag_na = FLAG(2),
    actor_starting_location_flag_always_place = FLAG(3),
    actor_starting_location_flag_initially_hidden = FLAG(4)
};

enum e_seat_type : short
{
    seat_type_default = 0,
    seat_type_passenger = 1,
    seat_type_gunner = 2,
    seat_type_driver = 3,
    seat_type_small_cargo = 4,
    seat_type_large_cargo = 5,
    seat_type_no_driver = 6,
    seat_type_no_vehicle = 7
};

enum e_initial_movement_mode : short
{
    initial_movement_mode_default = 0,
    initial_movement_mode_climbing = 1,
    initial_movement_mode_flying = 2
};

// max count: 32
struct actor_starting_location_definition
{
    string_id name;             // old_string_id
    real_point3d position;
    short reference_frame;
    short pad;
    real_euler_angles2d facing; //yaw_pitch_degrees
    actor_starting_location_flags flags;

    short character_type;                   // Block index: character_palette
    short initial_weapon;                   // Block index: weapon_palette
    short initial_secondary_weapon;         // Block index: weapon_palette
    short pad1;
    short vehicle_type;                     // Block index: vehicle_palette
    e_seat_type seat_type;
    e_grenade_type grenade_type;
    short swarm_count;                      // number of cretures in swarm if a swarm is spawned at this location
    string_id actor_variant_name;
    string_id vehicle_variant_name;
    float initial_movement_distance;        // before doing anything else, the actor will travel the given distance in its forward direction
    short emitter_vehicle;                  // Block index: scenario_vehicle
    e_initial_movement_mode initial_movement_mode;
    static_string32 placement_script;
    short placement_script_index;
    short unk;
};
TAG_BLOCK_SIZE_ASSERT(actor_starting_location_definition, 100);

// max count: 335
struct squad_definition
{
    static_string32 name;
    e_squad_definition_flags flags;
    e_game_team team;
    short parent;                           // Block index: squad_group_definition
    float squad_delay_time_seconds;
    short normal_diff_count;                // initial number of actors on normal difficulty
    short insane_diff_count;                // initial number of actors on insane difficulty (hard difficulty is midway between normal and insane)
    e_major_upgrade major_upgrade;
    short pad;

    // Explaination("Actor defaults", "The following default values are used for spawned actors")

    short vehicle_type;                     // Block index: scenario_vehicle_palette
    short character_type;                   // Block index: character_palette
    short initial_zone;                     // Block index: zone_definition
    short pad1;
    short initial_weapon;                   // Block index: weapon_palette
    short initial_secondary_weapon;         // Block index: weapon_palette
    e_grenade_type grenade_type;
    short initial_order;                    // Block index: orders
    string_id vehicle_variant;
    tag_block<actor_starting_location_definition> starting_locations;
    static_string32 placement_script;
    short placement_script_index;
    short unk;
};
TAG_BLOCK_SIZE_ASSERT(squad_definition, 116);

// max count: 128
struct ai_animation_reference_definition
{
    static_string32 animation_name;
    // leave this blank to use the unit's normal animation graph
    tag_reference animation_graph;   // jmad
    int padding540[3];
};
TAG_BLOCK_SIZE_ASSERT(ai_animation_reference_definition, 52);

// max count: 128
struct ai_script_reference_definition
{
    static_string32 script_name;
    int64_t pad;
};
TAG_BLOCK_SIZE_ASSERT(ai_script_reference_definition, 40);

// max count: 128
struct ai_recording_reference_definition
{
    static_string32 recording_name;
    int64_t pad;
};
TAG_BLOCK_SIZE_ASSERT(ai_recording_reference_definition, 40);
