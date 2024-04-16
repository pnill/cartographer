#pragma once
#include "ai_scenario_definitions.h"
#include "ai_orders.h"


#include "tag_files/tag_block.h"
#include "tag_files/string_id.h"

#define k_max_roles_per_scene 10
#define k_max_variants_per_line 10

enum e_ai_scene_flags : int
{
    ai_scene_flag_scene_can_play_multiple_times = FLAG(0),
    ai_scene_flag_enable_combat_dialogue = FLAG(1)
};

// max count: 1
struct ai_scene_trigger
{
    e_combination_rule combination_rule;
    short pad;
    tag_block<order_trigger_reference> triggers;
};
ASSERT_STRUCT_SIZE(ai_scene_trigger, 12);

enum e_ai_scene_role_group : short
{
    ai_scene_role_group_group1 = 0,
    ai_scene_role_group_group2 = 1,
    ai_scene_role_group_group3 = 2
};

// max count: k_max_variants_per_line 10
struct ai_scene_role_variant
{
    string_id variant_designation;
};
ASSERT_STRUCT_SIZE(ai_scene_role_variant, 4);

// max count: k_max_roles_per_scene 10
struct ai_scene_role
{
    string_id name;
    e_ai_scene_role_group group;
    short pad;
    tag_block<ai_scene_role_variant> role_variants;
};
ASSERT_STRUCT_SIZE(ai_scene_role, 16);

// max count: k_max_scenes_per_scenario 100
struct ai_scene
{
    string_id name;
    e_ai_scene_flags flags;
    tag_block<ai_scene_trigger> trigger_conditions;
    tag_block<ai_scene_role> roles;
};
ASSERT_STRUCT_SIZE(ai_scene, 24);

enum e_ai_conversation_flags : short
{
    ai_conversation_flag_stop_if_death = FLAG(0),               // This Conversation Will Be Aborted If Any Participant Dies
    ai_conversation_flag_stop_if_damaged = FLAG(1),             // An Actor Will Abort This Conversation If They Are Damaged
    ai_conversation_flag_stop_if_visible_enemy = FLAG(2),       // An Actor Will Abort This Conversation If They See An Enemy
    ai_conversation_flag_stop_if_alerted_to_enemy = FLAG(3),    // An Actor Will Abort This Conversation If They Suspect An Enemy 
    ai_conversation_flag_player_must_be_visible = FLAG(4),      // This Conversation Cannot Take Place Unless The Participants Can See A Nearby Player
    ai_conversation_flag_stop_other_actions = FLAG(5),          // Participants Stop Doing Whatever They Were Doing In Order To Perform This Conversation
    ai_conversation_flag_keep_trying_to_play = FLAG(6),         // If This Conversation Fails Initially It Will Keep Testing To See When It Can Play
    ai_conversation_flag_player_must_be_looking = FLAG(7)       // This Conversation Will Not Start Until The Player Is Looking At One Of The Participants
};

// max count: 8
struct ai_conversation_participant
{
    int pad[2];

    // if a unit with this name exists, we try to pick them to start the conversation
    // Block index: scenario_object_names_block
    short use_this_object;
    // once we pick a unit, we name it this
    // Block index: scenario_object_names_block
    short set_new_name;

    int pad1[6];
    static_string32 encounter_name;
    int pad2[4];
};
ASSERT_STRUCT_SIZE(ai_conversation_participant, 84);

enum e_ai_conversation_line_flags : short
{
    ai_conversation_line_flag_addressee_look_at_speaker = FLAG(0),
    ai_conversation_line_flag_everyone_look_at_speaker = FLAG(1),
    ai_conversation_line_flag_everyone_look_at_addressee = FLAG(2),
    ai_conversation_line_flag_wait_after_until_told_to_advance = FLAG(3),
    ai_conversation_line_flag_wait_until_speaker_nearby = FLAG(4),
    ai_conversation_line_flag_wait_until_everyone_nearby = FLAG(5)
};

enum e_conversation_line_addressee : short
{
    conversation_line_addressee_none = 0,
    conversation_line_addressee_player = 1,
    conversation_line_addressee_participant = 2
};

// max count: 32
struct ai_conversation_line
{
    e_ai_conversation_line_flags flags;
    short participant;           // Block index: ai_conversation_participant
    e_conversation_line_addressee addressee;

    // this field is only used if the addressee type is 'participant'
    // Block index: ai_conversation_participant
    short addressee_participant;
    int pad;
    float line_delay_time;
    int pad1[3];
    tag_reference variant_1;     // snd!
    tag_reference variant_2;     // snd!
    tag_reference variant_3;     // snd!
    tag_reference variant_4;     // snd!
    tag_reference variant_5;     // snd!
    tag_reference variant_6;     // snd!
};
ASSERT_STRUCT_SIZE(ai_conversation_participant, 84);

// max count: 128
struct ai_conversation
{
    static_string32 name;
    e_ai_conversation_flags flags;
    short pad;
    float trigger_distance_world_units;         // distance the player must enter before the conversation can trigger
    float run_to_player_distance_world_units;   // if the 'involves player' flag is set, when triggered the conversation's participant(s) will run to within this distance of the player
    
    tag_block<ai_conversation_participant> participants;
    tag_block<ai_conversation_line> lines;
    tag_block<> null_tagblock;
};
ASSERT_STRUCT_SIZE(ai_scene, 24);
