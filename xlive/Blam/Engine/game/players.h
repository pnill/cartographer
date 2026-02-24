#pragma once
#include "player_constants.h"

#include "input/controllers.h"
#include "memory/data.h"
#include "networking/network_game_definitions.h"
#include "simulation/machine_id.h"

/* enums */

enum e_player_flags : int16
{
	_player_active_in_game_bit = 0, // not entirely sure about this one, but the code uses the bit below
	_player_left_game_bit = 1,
	_player_first_spawn_bit = 3,
};

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

/* structures */

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

struct s_players_globals
{
	int32 players_in_game_count;
	bool all_players_dead;
	bool any_players_dead;
	bool input_disabled;
	bool disable_movement;
	int16 local_player_count;
	int16 player_controller_count;
	datum player_user_mapping[k_number_of_users];
	datum player_controller_mapping[k_number_of_users];
	int32 machine_valid_mask;
	s_machine_identifier machine_identifier[17];
	bool local_machine_exists;
	s_machine_identifier local_machine_identifier;
	int8 gap_A5[3];
	int32 local_machine_index;
	int16 coop_respawn_hud_message_type;
	bool display_coop_respawn_message;
	int8 display_fail_respawn_message;
	int32 respawn_time;
	int16 bsp_switch_trigger_volume_index;
	int16 unk_AE;
	int32 player_datum_that_triggered_bsp_switch;
	int32 teleported_unit_datum;
	int8 gap_B8[128];
};
ASSERT_STRUCT_SIZE(s_players_globals, 312);

s_players_globals* get_players_globals(void);

data_array* player_data_get(void);

datum __cdecl player_index_from_user_index(int32 user_index);

int32 players_first_active_user(void);

e_controller_index players_get_controller_index_from_user_index(int32 user_index);

bool __cdecl player_user_is_elite_or_dervish(int32 user_index);

bool __cdecl players_user_is_active(int32 user_index);

datum __cdecl player_index_from_absolute_player_index(uint16 abs_player_index);

void __cdecl players_set_machines(uint32 new_machine_valid_mask, const s_machine_identifier* new_machine_identifiers);

// gets required bits for player_appearance
uint32 player_appearance_required_bits(void);

// Validate player configuration
void __cdecl player_validate_configuration(datum player_index, s_player_configuration* configuration_data);

// Sets whether or not the passed user can interact with weapons
void player_user_weapon_interaction_set(int32 user_index, bool enabled);

// Enables weapon interaction for all users
void player_user_weapon_interaction_reset(void);

int16 local_player_count(void);

void __cdecl players_update_before_game(const struct simulation_update* update);

void __cdecl players_update_after_game(const struct simulation_update* update);

void players_update_for_checkpoint(void);

void players_joined_in_progress_allow_spawn(void);

void players_apply_patches(void);

/* macros */

#define player_get(index) datum_get(player_data_get(), index);
