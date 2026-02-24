#pragma once

/* macros */

// #define TEST_DISCORD_INSTANCE

/* enums */

enum e_context_variant
{
	_context_variant_ctf = 0,
	_context_variant_slayer = 1,
	_context_variant_oddball = 2,
	_context_variant_koth = 3,
	_context_variant_juggernaut = 4,
	_context_variant_territories = 5,
	_context_variant_assault = 6,
	k_context_variant_count
};

enum e_context_presence
{
	_context_presence_mainmenu = 0,
	_context_presence_settings = 1,
	_context_presence_server_browser = 2,
	_context_presence_singleplayer = 3,
	_context_presence_lobby = 4,
	_context_presence_results = 5,
	_context_presence_live_in_game = 6,
	_context_presence_public_game = 7,
	_context_presence_invite_only_game = 8,
	_context_presence_network_in_game = 9
};

/* prototypes */

// Initialize discord interface
void discord_game_status_create(HMODULE module);

// Deinitialize discord interface
void discord_game_status_dispose(void);

// Set small image for discord interface
void discord_interface_set_small_image(const char* small_image, const char* small_text);

// Set large image for discord interface
void discord_interface_set_large_image(const char* large_image, const char* large_text);

// Set details for discord interface
void discord_interface_set_details(const char* details);

// Set state for discord interface
void discord_interface_set_state(const char* state);

// Update map info for discord interface
void discord_interface_set_map_name(const utf8* scenario_name, const utf8* map_name);

// Update variant info for discord interface
void discord_interface_set_variant(e_context_variant variant, const utf8* variant_name);

// Update difficulty info for discord interface
void discord_interface_set_difficulty(int16 difficulty);

// Sets player counts to 0 and removes player info text from status
void discord_interface_zero_player_count(void);

// Update player count for discord interface
void discord_interface_set_player_counts(void);

void discord_interface_update_map_info_campaign(int32 map_id, const utf8* scenario_name);

void discord_interface_set_context(uint32 context_id, uint32 contex_value);
