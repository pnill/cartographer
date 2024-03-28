#pragma once
#include "ai_dialogue.h"
#include "actor_moving.h"
#include "ai_scenario_definitions.h"
#include "zone.h"


#include "cseries/cseries_strings.h"

enum e_order_flags : int
{
    order_flag_locked = FLAG(0),
    order_flag_always_active = FLAG(1),
    order_flag_debug_on = FLAG(2),
    order_flag_strict_area_def = FLAG(3),
    order_flag_follow_closest_player = FLAG(4),
    order_flag_follow_squad = FLAG(5),
    order_flag_active_camo = FLAG(6),
    order_flag_suppress_combat_until_engaged = FLAG(7),
    order_flag_inhibit_vehicle_use = FLAG(8)
};

enum e_force_combat_status : short
{
    force_combat_status_none = 0,
    force_combat_status_asleep = 1,
    force_combat_status_idle = 2,
    force_combat_status_alert = 3,
    force_combat_status_combat = 4
};

enum e_order_trigger_reference : int
{
    order_trigger_reference_flag_not = FLAG(0)
};

// max_count: 10
struct order_trigger_reference
{
    e_order_trigger_reference trigger_flags;
    short trigger;                  // Block index: triggers_block
    short pad;
};
TAG_BLOCK_SIZE_ASSERT(order_trigger_reference, 8);

// max count: 1
struct secondary_set_triggers
{
    e_combination_rule combination_rule;
    e_dialogue_type dialogue_type;              // when this ending is triggered, launch a dialogue event of the given type
    tag_block<order_trigger_reference> triggers;
};
TAG_BLOCK_SIZE_ASSERT(secondary_set_triggers, 12);

// max count: 12
struct order_ending_definition
{
    short next_order;        // Block Index: orders_block
    e_combination_rule combination_rule;
    float delay_time;
    e_dialogue_type dialogue_type;              // when this ending is triggered, launch a dialogue event of the given type
    short pad;
    tag_block<order_trigger_reference> triggers;
};

// max count: 300
struct orders_definition
{
	/* filt */
	static_string32 name;
	short style;            // Block Index: style_palette_block
	short pad;
    e_order_flags flags;
    e_force_combat_status force_combat_status;
    short pad1;
    static_string32 entry_script;
    short pad2;
    short follow_squad;      // Block Index: squads_block
    float follow_radius;

    tag_block<orders_area_reference_definition> primary_area_set;
    tag_block<orders_area_reference_definition> secondary_area_set;
    tag_block<secondary_set_triggers> secondary_set_trigger;
    tag_block<special_movement_definition> special_movement;
    tag_block<order_ending_definition> order_endings;
};
TAG_BLOCK_SIZE_ASSERT(orders_definition, 124);

enum e_trigger_flags : int
{
    trigger_flag_latch_on_when_triggered = FLAG(0)
};

enum e_order_rule_type : short
{
    order_rule_type_a_or_greater_alive = 0,
    order_rule_type_a_or_fewer_alive = 1,
    order_rule_type_x_or_greater_strength = 2,
    order_rule_type_x_or_less_strength = 3,
    order_rule_type_if_enemy_sighted = 4,
    order_rule_type_after_a_ticks = 5,
    order_rule_type_if_alerted_by_squad_a = 6,
    order_rule_type_script_ref_true = 7,
    order_rule_type_script_ref_false = 8,
    order_rule_type_if_player_in_trigger_volume = 9,
    order_rule_type_if_all_players_in_trigger_volume = 10,
    order_rule_type_combat_status_a_or_more = 11,
    order_rule_type_combat_status_a_or_less = 12,
    order_rule_type_arrived = 13,
    order_rule_type_in_vehicle = 14,
    order_rule_type_sighted_player = 15,
    order_rule_type_a_or_greater_fighting = 16,
    order_rule_type_a_or_fewer_fighting = 17,
    order_rule_type_player_within_x_world_units = 18,
    order_rule_type_player_shot_more_than_x_seconds_ago = 19,
    order_rule_type_game_safe_to_save = 20
};

enum e_order_completion_condition_flags
{
    order_completion_condition_flag_not = FLAG(0)
};

// max count: 5
struct order_completion_condition_definition
{
    e_order_rule_type rule_type;
    short squad;            // Block Index: squads_block
    short squad_group;      // Block Index: squad_groups_block
    short a;
    float x;
    short trigger_volume;    // Block Index: scenario_trigger_volume_block
    short pad;
    static_string32 exit_condition_script;
    short exit_condition_script_index;
    short pad1;
    e_order_completion_condition_flags flags;
};
TAG_BLOCK_SIZE_ASSERT(order_completion_condition_definition, 56);

// max count: 256
struct triggers_block
{
    /* filt */
    static_string32 name;
    e_trigger_flags trigger_flags;
    e_combination_rule combination_rule;
    short pad;
    tag_block<order_completion_condition_definition> conditions;
};
TAG_BLOCK_SIZE_ASSERT(triggers_block, 48);
