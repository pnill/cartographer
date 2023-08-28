#pragma once
#include "Blam/Engine/game/game_options.h"

#define k_campaign_difficulty_levels_count 4

// Initialize options structure for the map_name command
void main_game_initialize(void);

// Set the difficulty for the map launched with the map_name command
void main_game_launch_set_map_name(const char* map_name);

// Starts a new game with the options provided
bool main_game_change(const s_game_options* options);

// Set the difficulty for the map launched with the map_name command
void main_game_launch_set_difficulty(short difficulty);

// Set the splitscreen player count for the singleplayer map launched with the map_name command
void main_game_launch_set_coop_player_count(int player_count);

// Set the splitscreen player count for the multiplayer map launched with the map_name command
void main_game_launch_set_multiplayer_splitscreen_count(int player_count);

// Set the multiplayer variant for the map launched with the map_name command
void main_game_launch_set_multiplayer_variant(const char* variant_name);

// Set the game mode for the map launched with the map_name command
void main_game_launch_set_game_mode(int game_mode);

// Start a new game with the following map name
void main_game_launch(const char* map_name);

void main_game_apply_patches(void);