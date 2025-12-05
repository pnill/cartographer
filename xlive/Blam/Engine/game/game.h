#pragma once
#include "game_options.h"
#include "structures/cluster_partitions.h"

/* constants */

enum
{
	k_game_system_count = 69,

	k_game_maximum_ragdolls = 3,
	k_game_maximum_ragdolls_new = 6,
	k_game_maximum_players_to_allow_ragdolls = 8,
	k_game_maximum_players_to_allow_ragdolls_new = 16
};

/* typedefs */

typedef void(__cdecl* t_initialize_proc)(void);
typedef void(__cdecl* t_dispose_proc)(void);
typedef void(__cdecl* t_initialize_for_new_map_proc)(void);
typedef void(__cdecl* t_dispose_from_old_map_proc)(void);
typedef void(__cdecl* t_activation_proc)(s_game_cluster_bit_vectors*, s_game_cluster_bit_vectors*);

/* structures */

struct game_globals_storage
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
ASSERT_STRUCT_SIZE(game_globals_storage, 0x1270);

struct s_game_systems
{
	t_initialize_proc initialize_proc;
	t_dispose_proc dispose_proc;
	t_initialize_for_new_map_proc initialize_for_new_map_proc;
	t_dispose_from_old_map_proc dispose_from_old_map_proc;
	void* unk4;
	void* unk5;
	void* unk6;
	void* unk7;
	t_activation_proc activation_proc;
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

/* prototypes */

void game_apply_pre_winmain_patches(void);

s_game_systems* get_game_systems(void);

game_globals_storage* get_main_game_globals(void);

bool map_initialized(void);
s_game_options* game_options_get(void);
s_game_variant* current_game_variant(void);
e_game_mode game_mode_get(void);
bool game_in_editor(void);
int16 game_get_active_structure_bsp_index();
bool game_is_campaign(void);
bool game_is_multiplayer(void);
bool game_is_ui_shell(void);

bool game_is_cooperative(void);

void __cdecl game_shell_set_in_progress(void);
bool game_in_progress(void);
bool game_is_active(void);
bool game_is_predicted(void);
bool game_is_distributed(void);

e_game_simulation game_simulation_get(void);

bool game_is_networked(void);

bool game_is_server(void);
bool game_is_playback(void);
bool game_is_authoritative(void);
s_game_cluster_bit_vectors* game_get_cluster_activation(void);

// Setup default player data in the game options structure
void __cdecl game_options_setup_default_players(int32 player_count, s_game_options* game_options);

void game_time_get_date_and_time(s_date_and_time* date_and_time);

void game_direct_connect_to_session(XNKID kid, XNKEY key, const XNADDR* addr, int8 exe_type, int32 exe_version, int32 comp_version);

void __cdecl game_initialize(void);

void __cdecl game_dispose(void);

bool __cdecl main_events_pending(void);

void __cdecl game_tick(void);

void __cdecl game_update(int32 desired_ticks, real32* elapsed_game_dt);

void __cdecl game_initialize_for_new_map(const s_game_options* options);

void __cdecl game_frame(real32 dt);

/* globals */

extern char const* global_campaign_difficulty_level_names[4];

