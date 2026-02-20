#pragma once

/* constants */

/* enums */

enum e_map_memory_configuration
{
	_map_memory_configuration_none = 0,
	_map_memory_configuration_multiplayer,
	_map_memory_configuration_online_campaign,
	_map_memory_configuration_online_campaign_splitscreen,
	_map_memory_configuration_solo_campaign,
	_map_memory_configuration_campaign_splitscreen,
	k_map_memory_configuration_count,
};

/* prototypes */

void main_game_apply_patches(void);

// Initialize options structure for the map_name command
void main_game_initialize(void);

bool __cdecl main_game_loaded_map(void);

bool __cdecl main_game_loaded_pregame(void);

void __cdecl main_game_launch_default(void);

void __cdecl main_game_reset_map(void);

void __cdecl main_game_unload_and_prepare_for_next_game(void);

bool  __cdecl main_game_change_update(void);

// Set the difficulty for the map launched with the map_name command
void main_game_launch_set_map_name(char const* map_name);

void main_game_load_from_core(void);

void main_game_load_from_core_name(char const* core_name);

void main_game_load_post_game_launch(void);

// Starts a new game with the options provided
bool __cdecl main_game_change(struct s_game_options const* options);

bool __cdecl main_game_change_immediate(struct s_game_options const* options);

// Set the difficulty for the map launched with the map_name command
void main_game_launch_set_difficulty(int16 difficulty);

// Set the splitscreen player count for the singleplayer map launched with the map_name command
void main_game_launch_set_coop_player_count(int32 player_count);

// Set the splitscreen player count for the multiplayer map launched with the map_name command
void main_game_launch_set_multiplayer_splitscreen_count(int32 player_count);

// Set the multiplayer variant for the map launched with the map_name command
void main_game_launch_set_multiplayer_variant(char const* variant_name);

// Set the game mode for the map launched with the map_name command
void main_game_launch_set_game_mode(int32 game_mode);

void main_game_launch_legacy(char const* map_name);

// Start a new game with the following map name
void main_game_launch(char const* map_name);

void main_menu_launch(uint32 context);

void main_menu_launch_force(void);

bool map_memory_configuration_is_campaign(e_map_memory_configuration configuration);

bool map_memory_configuration_is_campaign_splitscreen(e_map_memory_configuration configuration);

bool map_memory_configuration_is_campaign_online(e_map_memory_configuration configuration);

bool map_memory_configuration_is_campaign_offline(e_map_memory_configuration configuration);

/* globals */

extern int32 g_main_game_launch_user_count;
extern struct s_game_options g_main_game_launch_options;
extern bool debug_load_panic_to_main_menu;
