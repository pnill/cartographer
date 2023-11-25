#pragma once
#include "Blam/Cache/DataTypes/TagBlock.h"
#include "Blam/Engine/game/materials.h"
#include "Blam/Engine/math/matrix_math.h"
#include "Blam/Engine/objects/damage_reporting.h"
#include "Blam/Engine/physics/physics_models.h"

#define k_maximum_variants_per_model 64
#define MAXIMUM_PERMUTATIONS_PER_MODEL_REGION 32
#define MAXIMUM_STATES_PER_MODEL_PERMUTATION 10
#define k_maximum_objects_per_model_variant 16
#define MAXIMUM_MATERIALS_PER_MODEL 32
#define MAXIMUM_DAMAGE_SECTIONS_PER_MODEL 16
#define MAXIMUM_RESPONSES_PER_DAMAGE_SECTION 16
#define MAXIMUM_DAMAGE_SEAT_INFOS_PER_MODEL 16
#define MAXIMUM_DAMAGE_CONSTRAINT_INFOS_PER_MODEL 16
#define MAXIMUM_MODEL_TARGETS_PER_MODEL 32
#define k_scenario_load_maximum_parameter_blocks 32

enum e_shadow_fade_distance : int16
{
    _shadow_fade_at_super_high_detail_level = 0,
    _shadow_fade_at_high_detail_level = 1,
    _shadow_fade_at_medium_detail_level = 2,
    _shadow_fade_at_low_detail_level = 3,
    _shadow_fade_at_super_low_detail_level = 4,
    _shadow_fade_never = 5
};

enum e_active_camo_flags : int32
{
    _active_camo_always_on = FLAG(0),
    _active_camo_always_merge = FLAG(1),
    _active_camo_never_merge = FLAG(2)
};

enum e_runtime_flags : int32
{
    _runtime_flag_contains_runtime_nodes = FLAG(0)
};

enum e_sort_order : int16
{
    _sort_order_no_sorting = 0,
    _sort_order_neg5_closest = 1,
    _sort_order_neg4 = 2,
    _sort_order_neg3 = 3,
    _sort_order_neg2 = 4,
    _sort_order_neg1 = 5,
    _sort_order_0_same_as_model = 6,
    _sort_order_1 = 7,
    _sort_order_2 = 8,
    _sort_order_3 = 9,
    _sort_order_4 = 10,
    _sort_order_5_farthest = 11
};

enum e_model_variant_permutation_flags : int8
{
    _model_variant_permutation_copy_states_to_all_permutations = FLAG(0)
};

enum e_property_flags : int8
{
    _property_flag_blurred = FLAG(0),
    _property_flag_hella_blurred = FLAG(1),
    _property_flag_shielded = FLAG(2)
};

enum e_object_damage_state : int16
{
    _object_damage_state_default = 0,
    _object_damage_state_minor_damage = 1,
    _object_damage_state_medium_damage = 2,
    _object_damage_state_major_damage = 3,
    _object_damage_state_destroyed = 4
};

enum e_damage_info_definition_flags : int32
{
    _damage_info_definition_takes_shield_damage_forhildren = FLAG(0),
    _damage_info_definition_takes_body_damage_for_children = FLAG(1),
    _damage_info_definition_always_shields_friendly_damage = FLAG(2),
    _damage_info_definition_passes_area_damage_to_children = FLAG(3),
    _damage_info_definition_parent_never_takes_body_damage_for_us = FLAG(4),
    _damage_info_definition_only_damaged_by_explosives = FLAG(5),
    _damage_info_definition_parent_never_takes_shield_damage_for_us = FLAG(6),
    _damage_info_definition_cannot_die_from_damage = FLAG(7),
    _damage_info_definition_passes_attached_damage_to_riders = FLAG(8)
};

enum e_damage_section_flags : uint32
{
    _damage_section_absorbs_body_damage = FLAG(0),
    _damage_section_takes_full_dmg_when_object_dies = FLAG(1),
    _damage_section_cannot_die_with_riders = FLAG(2),
    _damage_section_takes_full_dmg_when_obj_dstryd = FLAG(3),
    _damage_section_restored_on_ressurection = FLAG(4),
    _damage_section_unused_5 = FLAG(5),
    _damage_section_unused_6 = FLAG(6),
    _damage_section_heatshottable = FLAG(7),
    _damage_section_ignores_shields = FLAG(8)
};

enum e_response_type : int16
{
    _response_type_receives_all_damage = 0,
    _response_type_receives_area_effect_damage = 1,
    _response_type_receives_local_damage = 2,
};

enum e_constraint_damage_type : int16
{
    _constraint_damage_type_none = 0,
    _constraint_damage_type_destroy_one_of_group = 1,
    _constraint_damage_type_destroy_entire_group = 2,
    _constraint_damage_type_loosen_one_of_group = 3,
    _constraint_damage_type_loosen_entire_group = 4
};

enum e_instantaneous_damage_response_flags : uint32
{
    _instantaneous_damage_response_kills_object = FLAG(0),
    _instantaneous_damage_response_inhibits_melee_attack = FLAG(1),
    _instantaneous_damage_response_inhibits_weapon_attack = FLAG(2),
    _instantaneous_damage_response_inhibits_walking = FLAG(3),
    _instantaneous_damage_response_forces_drop_weapon = FLAG(4),
    _instantaneous_damage_response_kills_weapon_primary_trigger = FLAG(5),
    _instantaneous_damage_response_kills_weapon_secondary_trigger = FLAG(6),
    _instantaneous_damage_response_destroys_object = FLAG(7),
    _instantaneous_damage_response_damages_weapon_primary_trigger = FLAG(8),
    _instantaneous_damage_response_damages_weapon_secondary_trigger = FLAG(9),
    _instantaneous_damage_response_light_damage_left_turn = FLAG(10),
    _instantaneous_damage_response_major_damage_left_turn = FLAG(11),
    _instantaneous_damage_response_light_damage_right_turn = FLAG(12),
    _instantaneous_damage_response_major_damage_right_turn = FLAG(13),
    _instantaneous_damage_response_light_damage_engine = FLAG(14),
    _instantaneous_damage_response_major_damage_engine = FLAG(15),
    _instantaneous_damage_response_kills_object_no_player_solo = FLAG(16),
    _instantaneous_damage_response_causes_detonation = FLAG(17),
    _instantaneous_damage_response_destroy_all_group_constraints = FLAG(18),
    _instantaneous_damage_response_kills_variant_objects = FLAG(19),
    _instantaneous_damage_response_force_unattached_effects = FLAG(20),
    _instantaneous_damage_response_fires_under_threshold = FLAG(21),
    _instantaneous_damage_response_triggers_special_death = FLAG(22),
    _instantaneous_damage_response_only_on_special_death = FLAG(23),
    _instantaneous_damage_response_only_not_on_special_death = FLAG(24)
};

enum e_target_flags : int32
{
    _target_locked_by_human_targeting = FLAG(0),
    _target_locked_by_plasma_targeting = FLAG(1),
    _target_headshot = FLAG(2),
    _target_vulnerable = FLAG(3),
    _target_always_locked_by_plasma_targeting = FLAG(4)
};

enum e_model_permutation_flags : int8
{
    _model_permutation_cannot_be_chosen_randomly = FLAG(0)
};

enum e_model_object_data_type : int16
{
    _model_object_data_type_not_set = 0,
    _model_object_data_type_user_defined = 1,
    _model_object_data_type_auto_generated = 2
};

// max count: MAXIMUM_STATES_PER_MODEL_PERMUTATION
struct s_model_variant_state
{
    string_id permutation_name;
    int8 runtime_model_permutation_index;
    e_property_flags property_flags;
    e_object_damage_state state;

    // played while the model is in this state
    tag_reference looping_effect;   // effe
    string_id looping_effect_marker_name;
    real32 initial_probability;
};
TAG_BLOCK_SIZE_ASSERT(s_model_variant_state, 24);

// max count: MAXIMUM_PERMUTATIONS_PER_MODEL_REGION
struct s_model_variant_permutation
{
    string_id permutation_name;
    int8 runtime_model_permutation_index;
    e_model_variant_permutation_flags flags;
    int16 pad;
    real32 probability; // 0 - inf;
    tag_block<s_model_variant_state> states;

    int8 runtime_state_permutation_index[5];
    int8 pad1[7];
};
TAG_BLOCK_SIZE_ASSERT(s_model_variant_permutation, 32);

// max count: MAXIMUM_REGIONS_PER_MODEL
struct s_model_variant_region
{
    string_id region_name;      // Must match region name in render model;
    int8 runtime_model_region_index;
    int8 runtime_flags;
    int16 parent_variant;       // Block index: s_model_variant
    tag_block<s_model_variant_permutation> permutations;
    e_sort_order sort_order;    // negative values mean closer to the camera
    int16 pad;
};
TAG_BLOCK_SIZE_ASSERT(s_model_variant_region, 20);

struct s_model_variant_object
{
    string_id parent_marker;
    string_id child_marker;
    tag_reference child_object; // obje
};
TAG_BLOCK_SIZE_ASSERT(s_model_variant_object, 16);

// max count: k_maximum_variants_per_model
struct s_model_variant
{
    string_id name;
    int8 runtime_model_region_index[16];
    tag_block<s_model_variant_region> regions;
    tag_block<s_model_variant_object> objects;

    int32 pad1[2];
    string_id dialogue_sound_effect;
    tag_reference dialogue;  // udlg
};
TAG_BLOCK_SIZE_ASSERT(s_model_variant, 56);

// max count: MAXIMUM_MATERIALS_PER_MODEL
struct s_model_material
{
    string_id material_name;
    e_material_type material_type;

    int16 damage_section_index;
    int16 collision_global_material_index;
    int16 damage_global_material_index;
    string_id global_material_name;
    int16 global_material_index;
    int16 pad;
};
TAG_BLOCK_SIZE_ASSERT(s_model_material, 20);

// max count: MAXIMUM_RESPONSES_PER_DAMAGE_SECTION
struct s_instantaneous_damage_response
{
    e_response_type response_type;

    /* Explaination("Constraint damage type",
     "* if you specify a constraint group name (see lower section of this block)
        you can specify a constraint damage
        * loosening a constraint takes it out of the rigid state - activates it
        * destroying a constraint sets the attached body free")
    */
    e_constraint_damage_type constraint_damage_type;

    /* Explaination("Damage response flags", "* kills object: when the response fires the object dies regardless of its current health
    *inhibits <x>: from halo 1 - disallows basic behaviors for a unit
        * forces drop weapon : from halo 1 - makes the unit drop its current weapon
        * kills weapon <x> trigger : destroys the <x> trigger on the unit's current weapon
        * destroys object : when the response fires the object is destroyed")
    */
    e_instantaneous_damage_response_flags flags;
    real32 damage_threshold;                // repsonse fires after crossing this threshold.  1=full health
    tag_reference transition_effect;        // effe
    tag_reference transition_damage_effect; // jpt!

    string_id region;
    e_object_damage_state new_state;
    int16 runtime_region_index;
    string_id effect_marker_name;
    string_id damage_effect_marker_name;

    // Explaination("Response delay", "If desired, you can specify a delay until the response fires.This delay is pre-empted if another timed response for the same section fires.The delay effect plays while the timer is counting down")
    real32 response_delay;      // in seconds
    tag_reference delay_effect; // effe
    string_id delay_effect_marker_name;

    /* Explaination("Constraint destruction", "- a response can destroy a single constraint by naming it explicitly.
    -alternatively it can randomly destroy a single constraint from a specified group if the "destroy one group constraint" flag is set
        - also it can destroy all constraints in a specified group if the "destroy all group constraints" flag is set
        ")
    */

    string_id constraintgroup_name;
    // Explaination("seat ejaculation", "")
    string_id ejecting_seat_label;
    // Explaination("skip fraction", "0.0 always fires, 1.0 never fires")
    real32 skip_fraction;
    // Explaination("destroyed child object marker name", "when this response fires, any children objects created at the supplied marker name will be destroyed")
    string_id destroyed_child_object_marker_name;
    // Explaination("total damage threshold", "scale on total damage section vitality")
    real32 total_damage_threshold;
};
TAG_BLOCK_SIZE_ASSERT(s_instantaneous_damage_response, 80);

// max count: MAXIMUM_DAMAGE_SECTIONS_PER_MODEL
struct s_model_damage_section
{
    string_id name;
    // Explaination("damage section flags", "
    /* absorbs body damage: damage to this section does not count against body vitality
        *headshottable: takes extra headshot damage when shot
        * ignores shields : damage to this section bypasses shields")
    */
    
    e_damage_section_flags flags;
    real32 vitality_percentage;    // percentage of total object vitality
    tag_block<s_instantaneous_damage_response> instant_responses;
    tag_block<> unk_block_0;
    tag_block<> unk_block_1;

    real32 stun_time_seconds;
    real32 recharge_time_seconds;
    real32 runtime_recharge_velocity;
    string_id resurrection_restored_region_name;
    int16 resurection_region_runtime_index;
    int16 pad;
};
TAG_BLOCK_SIZE_ASSERT(s_model_damage_section, 56);

// max count: MAXIMUM_NODES_PER_MODEL
struct s_damage_node
{
    int16 runtime_damage_part;
    int16 pad;
    int32 unk[3];
};
TAG_BLOCK_SIZE_ASSERT(s_damage_node, 16);

// max count: MAXIMUM_DAMAGE_SEAT_INFOS_PER_MODEL
struct s_damage_seat_info
{
    string_id seat_label;
    real32 direct_damage_scale;     // 0==no damage, 1==full damage
    real32 damage_transfer_falloff_radius;
    real32 maximum_transfer_damage_scale;
    real32 minimum_transfer_damage_scale;
};
TAG_BLOCK_SIZE_ASSERT(s_damage_seat_info, 20);

// max count: MAXIMUM_DAMAGE_CONSTRAINT_INFOS_PER_MODEL
struct s_damage_constraint_info
{
    string_id physics_model_constraint_name;
    string_id damage_constraint_name;
    string_id damage_constraint_group_name;
    real32 group_probability_scale;
    e_physics_constraint physics_constraint;
    int16 constraint_index;
};
TAG_BLOCK_SIZE_ASSERT(s_damage_constraint_info, 20);

// max count: 1
struct s_model_damage_info
{
    e_damage_info_definition_flags flags;

    string_id global_indirect_material_name;    // absorbes AOE or child damage
    int16 indirect_damage_section;              // absorbes AOE or child damage
    int16 pad;
    int32 pad1;

    e_damage_reporting_type collision_damage_reporting_type;
    e_damage_reporting_type response_damage_reporting_type;
    int16 pad2;
    int32 pad3[5];

    // Explaination("body", "")

    real32 maximum_vitality;
    real32 minimum_stun_damage;     // the minimum damage required to stun this object's health
    real32 stun_time_seconds;       // the length of time the health stay stunned (do not recharge) after taking damage
    real32 recharge_time_seconds;   // the length of time it would take for the shields to fully recharge after being completely depleted
    real32 recharge_fraction;       // 0 defaults to 1 - to what maximum level the body health will be allowed to recharge
    int32 pad4[16];

    // Explaination("shield", "")

    tag_reference shield_damaged_first_person_shader;   // shad
    tag_reference shield_damaged_shader;    // shad
    real32 maximum_shield_vitality;         // the default initial and maximum shield vitality of this object
    string_id global_shield_material_name;
    real32 shield_minimum_stun_damage;             // the minimum damage required to stun this object's shields
    real32 shield_stun_time_seconds;               // the length of time the shields stay stunned (do not recharge) after taking damage
    real32 shield_recharge_time_seconds;           // the length of time it would take for the shields to fully recharge after being completely depleted
    real32 shield_damaged_threshold;

    tag_reference shield_damaged_effect;    // effe
    tag_reference shield_depleted_effect;   // effe
    tag_reference shield_recharging_effect; // effe

    tag_block<s_model_damage_section> damage_sections;
    tag_block<s_damage_node> nodes;

    int16 shield_global_material_index;
    int16 indirect_global_material_index;
    real32 shield_recharge_velocity;
    real32 health_recharge_velocity;
    tag_block<s_damage_seat_info> damage_seats;
    tag_block<s_damage_constraint_info> damage_constraints;

    // Explaination("overshield", "")

    tag_reference overshield_first_person_shader;   // shad
    tag_reference overshield_shader;                // shad
};
TAG_BLOCK_SIZE_ASSERT(s_model_damage_info, 248);

struct s_model_target_lock_on_data
{
    e_target_flags flags;
    real32 lock_on_distance;
};
CHECK_STRUCT_SIZE(s_model_target_lock_on_data, 8);

// max count: MAXIMUM_MODEL_TARGETS_PER_MODEL
struct s_model_target
{
    string_id marker_name;          // multiple markers become multiple spheres of the same radius
    real32 size;                    // sphere radius
    real32 cone_angle;              // the target is only visible when viewed within this angle of the marker's x axis
    int16 damage_section;           // the target is associated with this damage section
    int16 variant;                  // the target will only appear with this variant
    real32 targeting_relevance;     // higher relevances turn into stronger magnetisms
    s_model_target_lock_on_data lockon_data;
};
TAG_BLOCK_SIZE_ASSERT(s_model_target, 28);

// max count: MAXIMUM_PERMUTATIONS_PER_MODEL_REGION
struct s_model_permutation
{
    string_id name;
    e_model_permutation_flags flags;
    int8 collision_permutation_index;
    int8 physics_permutation_index;
    int8 pad;
};
TAG_BLOCK_SIZE_ASSERT(s_model_permutation, 8);

// max count: MAXIMUM_REGIONS_PER_MODEL
struct s_model_region
{
    string_id name;
    int8 collision_region_index;
    int8 physics_region_index;
    int16 pad;
    tag_block<s_model_permutation> permutations;
};
TAG_BLOCK_SIZE_ASSERT(s_model_region, 16);

// max count: MAXIMUM_NODES_PER_MODEL
struct s_model_node
{
    string_id name;
    int16 parent_node;          // Block index: model_node
    int16 first_child_node;     // Block index: model_node
    int16 next_sibling_node;    // Block index: model_node
    int16 pad;

    real_point3d default_translation;
    real_quaternion default_rotation;
    real_matrix4x3 default_inverse_matrix;
};
CHECK_STRUCT_SIZE(s_model_node, 92);

// max count: 1
struct s_model_object_data
{
    e_model_object_data_type type;
    int16 pad;
    real_point3d offset;
    real32 radius;
};
TAG_BLOCK_SIZE_ASSERT(s_model_object_data, 20);

// max count: k_scenario_load_maximum_parameter_blocks
struct s_scenario_load_parameters
{
    // Explaination("SCENARIO LOAD PARAMETERS", "strip-variant <variant-name>
    /*  strips a given variant out of the model tag
        strip - dialogue
        strips all the dialogue for this model i.e.cinematic only")
    */
    tag_reference scenario;     // scnr
    /****************************************
    * definition_name: parameters_text_definition
    * flags: 6
    * alignment_bit: 0
    ****************************************/
    data_block paramaters;
    int8 pad[32];
};

struct s_model_definition
{
    // Explaination("MODEL", "")
    /* hlmt */

    tag_reference render_model;     // mode
    tag_reference collision_model;  // coll
    tag_reference animation;        // jmad
    tag_reference physics;          // phys
    tag_reference physics_model;    // phmo

    // Explaination("level of detail", 
    /* If a model is further away than the LOD reduction distance, it will be reduced to that LOD.
    So the L1 reduction distance should be really long and the L5 reduction distance should be really short.
        This means distances should be in descending order, e.g. 5 4 3 2 1.

        Note that if we run out of memory or too many models are on screen at once, the engine may reduce
        models to a lower LOD BEFORE they reach the reduction distance for that LOD.

        If a model has a begin fade distance and disappear distance, it will begin fading out at that distance,
        reaching zero alpha and disappearing at the disappear distance.These distances should be greater than all
        of the LOD reduction distances.
    ") */
    real32 disappear_distance;  // World units
    real32 begin_fade_distance; // World units
    int32 pad;
    real32 reduce_to_l1;    // World units super low;
    real32 reduce_to_l2;    // World units low;
    real32 reduce_to_l3;    // World units medium;
    real32 reduce_to_l4;    // World units high
    real32 reduce_to_l5;    // World units super high
    int32 pad1;

    e_shadow_fade_distance shadow_fade_distance;
    int16 pad2;

    tag_block<s_model_variant> variants;
    tag_block<s_model_material> materials;
    tag_block<s_model_damage_info> new_damage_info;
    tag_block<s_model_target> targets;
    tag_block <s_model_region> collision_regions;
    tag_block<s_model_node> nodes;
    int32 node_list_checksum;
    tag_block<s_model_object_data> model_object_data;

    // Explaination("more stuff", "")

    // The default dialogue tag for this model (overriden by variants)
    tag_reference default_dialogue;     // udlg
    tag_reference active_camo_shader;   // shad
    
    e_active_camo_flags flags;
    string_id default_dialogue_effect;        // The default dialogue tag for this model (overriden by variants)

    int8 render_only_node_flags[32];
    int8 render_only_section_flags[32];
    
    e_runtime_flags runtime_flags;
    tag_block<s_scenario_load_parameters> scenario_load_parameters;

    // Explaination("HOLOGRAM", "hologram shader is applied whenever the control function from it's object is non-zero")

    tag_reference hologram_shader;   // shad
    string_id hologram_control_function;
};
TAG_GROUP_SIZE_ASSERT(s_model_definition, 252);
