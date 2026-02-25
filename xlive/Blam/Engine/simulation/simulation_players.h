#pragma once
#include "machine_id.h"

#include "game/player_control.h"
#include "game/player_constants.h"
#include "networking/network_game_definitions.h"

/* enums */

enum e_simulation_player_type   //TODO: determine actual names for this enum field
{
	simulation_player_type_local_authority = 0x0,
	simulation_player_type_local_no_authority = 0x1,
	simulation_player_type_local_unk = 0x2,
	simulation_player_type_foreign_synchronous = 0x3,
	simulation_player_type_foreign_no_authority = 0x4,
	simulation_player_type_foreign_distributed = 0x5,
	k_simulation_player_type_count = 0x6,
};

enum e_simulation_player_update_type
{
	_simulation_player_update_type_left_game = 0,
	_simulation_player_update_type_swap,
	_simulation_player_update_type_remove,
	_simulation_player_update_type_added,
	_simulation_player_update_type_configuration,
	k_simulation_player_update_type_count
};

/* structures */

struct simulation_player_update
{
	int32 player_index;
	s_player_identifier player_identifier;
	int32 update_type;
	s_machine_identifier machine_identifier;
	int32 user_index;
	int32 controller_index;
	bool player_left_game;
	int8 pad[3];
	s_player_configuration properties;
	int32 swap_player_index;
	s_player_identifier swap_player_identifier;
};
ASSERT_STRUCT_SIZE(simulation_player_update, 0xB4);

struct s_player_collection_player
{
	s_player_identifier identifier;
	bool left_game;
	uint32 left_game_time;
	s_machine_identifier machine_identifier;
	uint32 user_index;
	e_controller_index controller_index;
	s_player_configuration configuration_data;
};
ASSERT_STRUCT_SIZE(s_player_collection_player, 0xA4);

struct s_player_collection
{
	uint32 player_valid_mask;
	s_player_collection_player players[k_maximum_players];
};
ASSERT_STRUCT_SIZE(s_player_collection, 0xA44);

/* classes */

#pragma pack(push, 1)
struct c_simulation_player
{
	int32 m_player_index;
	datum m_player_datum_index;
	e_simulation_player_type m_player_type;
	uint64 m_player_identifier;
	s_machine_identifier m_player_machine_identifier;
	int16 pad;
	void* m_simulation_world;
	char m_field_20;
	bool m_active;
	int16 pad_1;
	uint32 m_start_commit_ticks;
	player_action m_actions;
};
#pragma pack(pop)
ASSERT_STRUCT_SIZE(c_simulation_player, 0x88);

/* prototypes */

void simulation_players_apply_patches(void);

bool __cdecl simulation_players_apply_update(simulation_player_update* player_update);

void __cdecl simulation_player_joined_game(datum player_index);

void __cdecl simulation_player_left_game(datum player_index);
