#pragma once

#include "Blam/Common/Common.h"
#include "Blam/Engine/game/players.h"
#include "Blam/Engine/math/random_math.h"
#include "Blam/Engine/Networking/Session/NetworkSession.h"
#include "Blam/Engine/Simulation/MachineID.h"
#include "Blam/Engine/saved_games/game_variant.h"

enum e_game_simulation : __int8
{
	_game_simulation_none = 0x0,
	_game_simulation_local = 0x1,
	_game_simulation_synchronous_client = 0x2,
	_game_simulation_synchronous_server = 0x3,
	_game_simulation_distributed_client = 0x4,
	_game_simulation_distributed_server = 0x5,
	k_game_simulation_count = 0x6,
};

enum e_game_mode : int
{
	_game_mode_campaign = 1,
	_game_mode_multiplayer = 2,
	_game_mode_ui_shell = 3,
	_game_mode_mutiplayer_shared = 4,
	_game_mode_single_player_shared = 5,
	k_game_mode_count
};

struct game_player_options
{
	bool player_valid;
	bool player_left_game;
	short user_index;
	long controller_index;
	s_machine_identifier machine_identifier;
	s_player_identifier player_identifier;
	short field_16;
	s_player_properties properties;
	s_persistent_campaign_player player_type[2]; // index 0 is masterchief, index 1 is dervish
};
CHECK_STRUCT_SIZE(game_player_options, 212);

#pragma pack(push,1)
struct s_game_options
{
	e_game_mode game_mode;
	e_game_simulation simulation_type;
	char network_protocol_related;
	bool session_host_is_dedicated;
	bool scenario_custom;
	short game_tick_rate;
	short pad_C[3];
	byte random_data[8];
	int random_seed;
	bool is_custom_map;
	byte field_1D;
	static_wchar_string<48> custom_map_name;
	short pad_7E;
	int campaign_id;						// This should always be 1, since there can be multiple "camapaigns". However, this isn't taken advantage of in the retail game 
	int map_id;
	static_wchar_string<260> scenario_path;
	short initial_bsp_index;
	short field_292;
	bool load_level_only;
	byte local_peer_index;
	bool dump_object_log;
	bool dump_random_seeds;
	bool playtest_mode;
	byte pad_299;
	short difficulty;
	bool coop;
	byte player_count;
	short pad_29E;
	s_game_variant game_variant;
	DWORD menu_context;
	DWORD valid_machine_mask;
	s_machine_identifier machines[17];
	bool local_machine_exists;
	s_machine_identifier local_machine_identifier;
	char pad_444[3];
	game_player_options players[16]; 
};
CHECK_STRUCT_SIZE(s_game_options, 4488);
#pragma pack(pop)

// Initializes game options
void game_options_new(s_game_options* game_options);
