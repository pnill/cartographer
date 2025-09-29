#pragma once
#include "ai_scenario_definitions.h"
#include "ai_orders.h"

#include "tag_files/tag_block.h"
#include "tag_files/string_id.h"

/* constants */

enum
{
	k_max_roles_per_scene = 10,
	k_max_variants_per_line = 10,
	k_max_lines_per_scenario = 500
};

/* enums */

enum e_ai_scene_flags : uint32
{
	_ai_scene_flag_scene_can_play_multiple_times_bit = 0,
	_ai_scene_flag_enable_combat_dialogue_bit = 1,
	k_ai_scene_flag_count
};

enum e_ai_scene_role_group : int16
{
	_ai_scene_role_group_group1 = 0,
	_ai_scene_role_group_group2 = 1,
	_ai_scene_role_group_group3 = 2
};

enum e_ai_conversation_flags : uint16
{
	_ai_conversation_stop_if_anyone_dies_bit = 0,           // This Conversation Will Be Aborted If Any Participant Dies
	_ai_conversation_stop_if_damaged_bit = 1,			// An Actor Will Abort This Conversation If They Are Damaged
	_ai_conversation_stop_if_visible_enemy_bit = 2,     // An Actor Will Abort This Conversation If They See An Enemy
	_ai_conversation_stop_if_alerted_to_enemy_bit = 3,	// An Actor Will Abort This Conversation If They Suspect An Enemy 
	_ai_conversation_player_must_be_visible_bit = 4,	// This Conversation Cannot Take Place Unless The Participants Can See A Nearby Player
	_ai_conversation_stop_other_actions_bit = 5,		// Participants Stop Doing Whatever They Were Doing In Order To Perform This Conversation
	_ai_conversation_keep_trying_to_play_bit = 6,		// If This Conversation Fails Initially It Will Keep Testing To See When It Can Play
	_ai_conversation_player_must_be_looking_at_bit = 7,	// This Conversation Will Not Start Until The Player Is Looking At One Of The Participants
	k_number_of_conversation_definition_flags
};

enum e_ai_conversation_line_flags : uint16
{
	_ai_conversation_line_look_at_speaker_bit = 0,
	_ai_conversation_line_everyone_look_at_speaker_bit = 1,
	_ai_conversation_line_everyone_look_at_addressee_bit = 2,
	_ai_conversation_line_wait_after_until_told_to_advance_bit = 3,
	_ai_conversation_line_wait_until_speaker_nearby_bit = 4,
	_ai_conversation_line_wait_until_everyone_nearby_bit = 5,
	k_number_of_conversation_line_flags
};

enum e_conversation_line_addressee : short
{
	conversation_line_addressee_none = 0,
	conversation_line_addressee_player = 1,
	conversation_line_addressee_participant = 2
};

/* structures */

// max count: 1
struct ai_scene_trigger
{
	e_combination_rule combination_rule;
	int16 pad;
	tag_block<order_trigger_reference> triggers;
};
ASSERT_STRUCT_SIZE(ai_scene_trigger, 12);

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
	int16 pad;
	tag_block<ai_scene_role_variant> role_variants;
};
ASSERT_STRUCT_SIZE(ai_scene_role, 16);

// max count: k_max_scenes_per_scenario 100
struct ai_scene
{
	string_id name;
	c_flags_no_init<e_ai_scene_flags, uint32, k_ai_scene_flag_count> flags;
	tag_block<ai_scene_trigger> trigger_conditions;
	tag_block<ai_scene_role> roles;
};
ASSERT_STRUCT_SIZE(ai_scene, 24);

// max count: 8
struct ai_conversation_participant
{
	uint16 pad;
	uint16 flags;

	int16 selection_type;
	int16 actor_type;

	// if a unit with this name exists, we try to pick them to start the conversation
	// Block index: scenario_object_names_block
	int16 preexisting_object_name_index;
	// once we pick a unit, we name it this
	// Block index: scenario_object_names_block
	int16 new_attach_object_name_index;

	uint32 unused[3];
	int16 dialogue_variants[6];
	char ai_index_name[32];
	int32 runtime_ai_index;
	uint32 unused2[3];
};
ASSERT_STRUCT_SIZE(ai_conversation_participant, 84);


// max count: 32
struct ai_conversation_line
{
	c_flags_no_init<e_ai_conversation_line_flags, uint16, k_number_of_conversation_line_flags> flags;
	int16 participant_index;					// Block index: ai_conversation_participant
	e_conversation_line_addressee address_type;

	// this field is only used if the addressee type is 'participant'
	// Block index: ai_conversation_participant
	int16 address_participant_index;
	uint32 unused;
	real32 delay_time;
	uint32 unused2[3];
	tag_reference dialogue[6];	// snd!
};
ASSERT_STRUCT_SIZE(ai_conversation_participant, 84);

// max count: 128
struct ai_conversation
{
	char name[32];
	c_flags_no_init<e_ai_conversation_flags, uint16, k_number_of_conversation_definition_flags> flags;
	uint16 pad;

	real32 trigger_dist;		// (World Units) distance the player must enter before the conversation can trigger
	real32 run_to_player_dist;	// (World Units) if the 'involves player' flag is set, when triggered the conversation's participant(s) will run to within this distance of the player
	
	tag_block<ai_conversation_participant> participants;
	tag_block<ai_conversation_line> lines;
	tag_block<> null_tagblock;
};
ASSERT_STRUCT_SIZE(ai_scene, 24);

// max count: k_max_variants_per_line
struct mission_dialogue_variant
{
	string_id variant;		// 3-letter designation for the character^
	tag_reference sound;	// snd!
	string_id sound_effect;
};
ASSERT_STRUCT_SIZE(mission_dialogue_variant, 16);

// max count: k_max_lines_per_scenario
struct mission_dialogue_line
{
	string_id name;
	tag_block<mission_dialogue_variant> variants;
	string_id default_sound_effect;
};
ASSERT_STRUCT_SIZE(mission_dialogue_line, 16);

// max count: 1
struct ai_mission_dialogue
{
	tag_block<mission_dialogue_line> lines;
};
ASSERT_STRUCT_SIZE(ai_mission_dialogue, 8);

/* prototypes */

void __cdecl ai_scenes_initialize_for_new_map(void);
