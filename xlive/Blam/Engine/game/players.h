#pragma once
#include "aim_assist.h"
#include "game_globals.h"

#include "input/controllers.h"
#include "memory/data.h"
#include "objects/emblems.h"
#include "units/unit_control.h"

/* constants */

enum
{
	k_shot_entries_per_player = 8
};

/* macros */

#define player_get(index) ((struct player_datum*)datum_get(player_data_get(), index))

#define player_try_and_get(index) ((struct player_datum*)datum_try_and_get(player_data_get(), index))

#define player_try_and_get_absolute(index) ((struct player_datum*)datum_try_and_get_absolute(player_data_get(), index))

/* enums */

enum e_player_flags
{
	_player_active_in_game_bit = 0,
	_player_left_game_bit,
	_player_blocking_teleporter_bit,
	_player_initial_spawn_bit,
	_player_action_suppress_primary_bit,
	_player_action_suppress_secondary_bit,
	_player_action_suppress_tertiary_bit,
	_player_action_dual_wield_grenade_latch_bit,
	_player_standing_in_teleporter_bit,
	_player_standing_in_teleporter_teleported_bit,
	_player_vehicle_entrance_ban_bit,
	_player_play_coop_spawn_effect_bit,
	_player_sitting_out_bit,
	_player_joined_in_progress_bit,
	_player_is_tracked_target_bit,
	_player_is_locked_target_bit,
	k_player_flag_count,
};

/* structures */

struct s_player_appearance
{
	c_enum<enum e_player_color_index, int8, _player_color_index_min_value, k_player_color_count> change_color_index[4];
	c_enum<enum e_character_type, int8, NONE, k_player_character_type_count> player_character_type;
	s_emblem_info emblem_info;
	int32 gap_48;
	int32 gap_4C;
};
ASSERT_STRUCT_SIZE(s_player_appearance, 16);

struct s_player_shot_info
{
	int16 shot_id;
	// damage_reporting_info is not damage_reporting_type there is some bit manipulation behind it using the prediction type
	// on the projectile/weapon being tracked. below is how it is computed maybe add helper macros or define a type later on.
	// (((projectile.prediction_type == instant) ^ damage_reporting_type) & 0x3F ^ (prediction_type == instant)) & 0x3F
	int8 damage_reporting_info;
	int8 pad;
	int32 game_time;
};
ASSERT_STRUCT_SIZE(s_player_shot_info, 8);

struct s_player_identifier
{
	uint8 identifier[8];
};
ASSERT_STRUCT_SIZE(s_player_identifier, 8);

struct s_clan_identifier
{
	uint8 identifier[12];
};
ASSERT_STRUCT_SIZE(s_clan_identifier, 12);

struct s_machine_identifier
{
	uint8 machine_identifier[6];
};

struct s_player_configuration
{
	wchar_t name[32];
	s_player_appearance appearance;
	wchar_t clan_name[16];
	s_clan_identifier clan_identifiers;

	int8 team_index;
	int8 player_handicap_level;	// e_user_interface_controller_handicap
	int8 player_displayed_skill;
	int8 player_overall_skill;
	int8 player_is_griefer;
	int8 bungie_user_role;
	int8 achievement_flags;
	int8 unk2;
};
ASSERT_STRUCT_SIZE(s_player_configuration, 132);

struct s_player_interaction
{
	int16 type;
	int16 data;
	datum object_index;
};
ASSERT_STRUCT_SIZE(s_player_interaction, 8);

struct s_player_action_context
{
	s_player_interaction interaction;
	datum melee_target_unit_index;
};
ASSERT_STRUCT_SIZE(s_player_action_context, 12);

struct player_action
{
	uint32 control_flags1;
	uint32 control_flags2;
	real_euler_angles2d desired_facing_angles;
	real_point2d throttle;
	real32 primary_trigger;
	real32 secondary_trigger;
	uint32 action_flags;
	s_unit_weapon_set desired_weapon_set;
	int16 desired_grenade_index;
	int16 desired_zoom_level;
	s_player_action_context action_context;
	s_aim_assist_targeting_result aim_assist_data;
	int32 field_5C;
};
ASSERT_STRUCT_SIZE(player_action, 0x60);

#pragma pack(push, 1)
struct player_datum
{
	uint16 identifier;
	uint16 flags;
	s_player_identifier player_identifier;
	int32 player_creation_tick;
	s_machine_identifier machine_identifier;
	int16 machine_index;
	int32 machine_user_index;
	int32 machine_controller_index;
	e_controller_index controller_index;
	int16 user_index;
	int16 player_bsp_location_index;
	datum unit_index;
	datum dead_unit_index;
	datum possibly_datum;
	int32 InputFlags;
	int32 InputFlags2;
	int8 field_3C[4];
	s_player_configuration configuration;
	s_player_configuration desired_configuration;
	int32 field_148;
	int32 respawn_penalty;
	int32 teleporter_blocked_tick_count;
	int32 respawn_time;
	int32 unk_12;
	int8 gap_15C[4];
	int32 field_160;
	int16 field_164;
	int8 gap_166[14];
	int32 betrayal_encountered_tick;
	int32 spawn_protection_time;
	int16 field_17C[2];
	real32 unit_speed;
	int32 field_184;
	int8 gap_188[2];
	int16 field_18A;
	int8 gap_18C[2];
	int16 field_18E;
	int16 player_lives_count;
	int8 gap_192[2];
	int32 betraying_player_index;
	int8 gap_198[2];
	int32 field_19C;
	int8 gap_19E[30];
	s_player_shot_info shot_info[k_shot_entries_per_player];
	int16 random_index;
	int8 gap_1FE[2];
	int32 is_chatting;
};
ASSERT_STRUCT_SIZE(player_datum, 516);
#pragma pack(pop)

/* classes */

class c_player_in_game_iterator 
{
public:
	c_player_in_game_iterator(void);
	
	bool next(void);
	player_datum* get_datum(void) const;
	datum get_index(void) const;
	int32 get_absolute_index(void) const;

private:
	player_datum* m_current_player;
	data_iterator m_data_iterator;
};

class c_player_with_unit_iterator
{
public:
	c_player_with_unit_iterator(void);

	bool next(void);
	player_datum* get_datum(void) const;
	datum get_index(void) const;
	int32 get_absolute_index(void) const;

private:
	player_datum* m_current_player;
	data_iterator m_data_iterator;
};

/* prototypes */

data_array* player_data_get(void);

datum __cdecl player_index_from_user_index(int32 user_index);

int32 players_first_active_user(void);

e_controller_index players_get_controller_index_from_user_index(int32 user_index);

bool __cdecl player_user_is_elite_or_dervish(int32 user_index);

bool __cdecl players_user_is_active(int32 user_index);

datum __cdecl player_index_from_absolute_player_index(uint16 abs_player_index);

void players_get_machines(uint32* machine_valid_mask, s_machine_identifier* machine_identifiers);

void __cdecl players_set_machines(uint32 new_machine_valid_mask, const s_machine_identifier* new_machine_identifiers);

// gets required bits for player_appearance
uint32 player_appearance_required_bits(void);

// Validate player configuration
void __cdecl players_validate_configuration(int32 player_index, s_player_configuration* configuration_data);

// Sets whether or not the passed user can interact with weapons
void player_user_weapon_interaction_set(int32 user_index, bool enabled);

// Enables weapon interaction for all users
void player_user_weapon_interaction_reset(void);

int16 local_player_count(void);

datum* player_user_mapping_get(void);

datum* player_controller_mapping_get(void);

void __cdecl players_update_before_game(const struct simulation_update* update);

void __cdecl players_update_after_game(const struct simulation_update* update);

void players_update_for_checkpoint(void);

void players_joined_in_progress_allow_spawn(void);

void clan_identifier_clear(struct s_clan_identifier* clan_id);

void player_appearance_initialize(struct s_player_appearance* player_appearance);

char const* player_identifier_get_string(struct s_player_identifier const* player_id);

char const* clan_identifier_get_string(struct s_clan_identifier const* clan_id);

void players_apply_patches(void);
