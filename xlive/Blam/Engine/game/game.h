#pragma once

#include "game_options.h"
#include "structures/cluster_partitions.h"

#define k_game_maximum_ragdolls 3
#define k_game_maximum_ragdolls_new 6

#define k_game_maximum_players_to_allow_ragdolls 8
#define k_game_maximum_players_to_allow_ragdolls_new 16

struct s_main_game_globals
{
	bool initializing;
	bool map_active;
	int16 active_structure_bsp_index;
	int unused_0;
	s_game_options options;
	bool game_in_progress;
	bool game_is_lost;
	int ticks_to_reset_game;
	bool game_is_finished;
	bool game_sounds_disabled;
	short pad_1;
	DWORD ticks_till_end;
	int32 game_ragdoll_count;
	int unused_1;
	s_game_cluster_bit_vectors cluster_pvs;
	s_game_cluster_bit_vectors cluster_pvs_local;
	s_game_cluster_bit_vectors cluster_activation;
	byte enable_scripted_camera_pvs;
	byte pad_2;
	WORD pvs_object_is_set;		// If it's 2 then it's set but if it's 1 or 0 then it's not?
	datum pvs_object_datum;
};
ASSERT_STRUCT_SIZE(s_main_game_globals, 0x1270);


typedef void(__cdecl* initialize_proc_t)(void);
typedef void(__cdecl* dispose_proc_t)(void);
typedef void(__cdecl* reset_proc_t)(void);
typedef void(__cdecl* dispose_from_old_map_proc_t)(void);
typedef void(__cdecl* activation_proc_t)(s_game_cluster_bit_vectors*, s_game_cluster_bit_vectors*);

struct s_game_systems
{
	initialize_proc_t initialize_proc;
	dispose_proc_t dispose_proc;
	reset_proc_t reset_proc;
	dispose_from_old_map_proc_t dispose_from_old_map_proc;
	void* unk4;
	void* unk5;
	void* unk6;
	void* unk7;
	activation_proc_t activation_proc;
};
ASSERT_STRUCT_SIZE(s_game_systems, 36);

struct s_date_and_time
{
	int32 year;
	int32 month;
	int32 day;
	int32 hour;
	int32 minute;
	int32 second;
};

s_game_systems* get_game_systems();

s_main_game_globals* get_main_game_globals(void);
bool map_initialized(void);
s_game_options* game_options_get(void);
s_game_variant* current_game_variant(void);
e_game_mode game_mode_get(void);
bool game_in_editor(void);
int16 game_get_active_structure_bsp_index();
bool game_is_campaign(void);
bool game_is_multiplayer(void);
bool game_is_ui_shell(void);
void __cdecl game_shell_set_in_progress();
bool game_in_progress(void);
bool game_is_predicted(void);
bool game_is_distributed(void);
bool game_is_server(void);
bool game_is_playback(void);
bool game_is_authoritative(void);
s_game_cluster_bit_vectors* game_get_cluster_activation(void);

// Setup default player data in the game options structure
void __cdecl game_options_setup_default_players(int32 player_count, s_game_options* game_options);

void game_time_get_date_and_time(s_date_and_time* date_and_time);

void game_direct_connect_to_session(XNKID kid, XNKEY key, const XNADDR* addr, int8 exe_type, int32 exe_version, int32 comp_version);

void game_apply_pre_winmain_patches(void);