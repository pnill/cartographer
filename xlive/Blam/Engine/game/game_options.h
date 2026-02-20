#pragma once
#include "game/players.h"
#include "main/map_manager.h"
#include "networking/network_constants.h"
#include "saved_games/game_variant.h"

/* structures */

struct s_campaign_armaments_weapon
{
	int16 damage_reporting_type;
	int16 rounds_inventory;
	int16 rounds_loaded;
	int16 battery;
};

struct s_campaign_armaments_player
{
	bool valid;
	s_campaign_armaments_weapon primary_weapon;
	s_campaign_armaments_weapon backpack_weapon;
	s_campaign_armaments_weapon secondary_weapon;
	uint8 grenade_counts[2];
};
ASSERT_STRUCT_SIZE(s_campaign_armaments_player, 28);

struct s_campaign_armaments
{
	s_campaign_armaments_player player_armaments[2];	// index 0 is masterchief, index 1 is dervish
};

struct game_player_options
{
	bool valid;
	bool player_left_game;
	int16 user_index;
	e_controller_index controller_index;
	s_machine_identifier machine_identifier;
	s_player_identifier player_identifier;
	s_player_configuration properties;
	s_campaign_armaments campaign_armaments;
};
ASSERT_STRUCT_SIZE(game_player_options, 212);

struct game_machine_options
{
	uint32 valid_machine_mask;
	s_machine_identifier machines[k_network_maximum_machines_per_session];
	bool local_machine_exists;
	s_machine_identifier local_machine_identifier;
};
ASSERT_STRUCT_SIZE(game_machine_options, 0x74);

struct s_game_options
{
	e_game_mode game_mode;
	int8 game_simulation;	// e_game_simulation
	int8 game_network_type;
	bool session_host_is_dedicated;
	bool scenario_custom;
	int16 game_tick_rate;
	int64 random_data;
	int32 verify_random_seed;
	bool is_custom_map;
	uint8 field_1D;
	s_secure_map_id custom_map_id;
	int16 pad_7E;
	int32 campaign_id;						// This should always be 1, since there can be multiple "camapaigns". However, this isn't taken advantage of in the retail game 
	int32 map_id;
	wchar_t scenario_path[MAX_PATH];
	int16 initial_bsp_index;
	int16 field_292;
	bool load_level_only;
	uint8 local_peer_index;
	bool dump_object_log;
	bool dump_random_seeds;
	bool playtest_mode;
	uint8 pad_299;
	int16 difficulty;
	bool coop;
	uint8 player_count;
	int16 pad_29E;
	s_game_variant game_variant;
	uint32 menu_context;
	game_machine_options machines;
	game_player_options players[k_maximum_players]; 
};
ASSERT_STRUCT_SIZE(s_game_options, 4488);

/* prototypes */

// Initializes game options
void __cdecl game_options_new(s_game_options* game_options);
