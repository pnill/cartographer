#pragma once

/* constants */

constexpr real32 k_game_time_default_game_speed = 1.f;

/* prototypes */

void game_time_apply_patches();

bool game_time_initialized(void);

uint32 game_time_get(void);

real32 game_time_get_speed(void);

void game_time_set_speed(real32 speed);

bool game_time_get_paused(void);

void game_time_set_paused(bool pause);

void game_time_discard(int32 desired_ticks, int32 actual_ticks, real32* elapsed_game_dt);

int32 game_seconds_integer_to_ticks(int32 seconds);

bool __cdecl game_time_update(real32 world_seconds_elapsed, real32* game_seconds_elapsed, int32* game_ticks_elapsed);

void game_time_advance(void);

int32 game_tick_rate(void);

real32 game_tick_length(void);

real32 game_ticks_to_seconds(real32 ticks);

real32 game_seconds_to_ticks_real(real32 seconds);

real32 game_tick_rate_legacy_ratio_real();

int32 game_seconds_to_ticks_round(real32 seconds);

real32 game_time_get_max_frame_time(void);

real32 game_time_get_leftover(void);

void __cdecl game_time_initialize_for_new_map(void);
