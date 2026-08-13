#include "stdafx.h"
#include "main_time.h"

#include "console.h"
#include "main.h"
#include "main_screenshot.h"

#include "camera/camera.h"
#include "game/game.h"
#include "game/game_time.h"
#include "game/player_control.h"
#include "rasterizer/rasterizer_globals.h"
#include "rasterizer/rasterizer_text.h"
#include "text/draw_string.h"
#include "text/font_group.h"
#include "shell/shell.h"
#include "shell/shell_windows.h"

#include "H2MOD/Modules/Shell/Config.h"

/* constants */

enum
{
	k_target_framerate = 30,
	k_maximum_ticks_per_frame = 8,
	k_flip_delta_count = 15,
};

#define k_use_precise_counters true

//#define MAIN_TIME_DEBUG

/* typedefs */

typedef void(__cdecl* t_main_time_reset)(void);

/* structures */

struct s_main_time_globals
{
	bool wmv_playback_in_progress;
	bool wmv_playback_throttle_framerate;
	bool temporary_throttle_control;
	bool temporary_throttle;
	int32 last_game_time;
	uint64 last_milliseconds;
	bool should_reset;
	int64 last_vblank_index;
	int64 last_initial_vblank_index;
	int64 target_display_vblank_index;
	int64 last_input_timestamp;
};
ASSERT_STRUCT_SIZE(s_main_time_globals, 56);

#ifdef MAIN_TIME_DEBUG
struct s_main_time_debug
{
	real32 dt_default;
	real32 dt_performance_counter;
};
#endif

/* prototypes */

static s_main_time_globals* main_time_globals_get(void);

static real32 main_time_get_delta_sec_precise(LARGE_INTEGER counter_now, LARGE_INTEGER freq);

static uint64 main_time_get_absolute_milliseconds(void);

static real32 main_time_delta_calculate(LARGE_INTEGER counter_now, LARGE_INTEGER freq);

/* globals */

static real32 last_framerate_time = 0.f;

static LARGE_INTEGER g_main_game_time_counter_last_time;
#ifdef MAIN_TIME_DEBUG
static s_main_time_debug g_main_game_time_debug;
#endif

static t_main_time_reset p_main_time_reset;

static int8 g_next_flip_delta_index;
static int16 g_flip_deltas[k_flip_delta_count];

bool debug_frame_rate = false;
bool display_framerate = false;
bool display_frame_deltas = false;
bool debug_disable_frame_rate_throttle = false;
bool debug_frame_rate_based_on_system_time = false;
bool debug_frame_rate_stabilization = false;

bool g_main_game_time_frame_limiter_enabled = false;

/* public code */

void main_time_apply_patches(void)
{
	PatchCall(Memory::GetAddress(0x39BE3, 0xC03E), main_time_update);

	DETOUR_ATTACH(p_main_time_reset, Memory::GetAddress<t_main_time_reset>(0x286C5, 0x24867), main_time_reset);
	return;
}

void __cdecl main_time_initialize(void)
{
	s_main_time_globals* main_time_globals = main_time_globals_get();
	main_time_globals->last_milliseconds = system_milliseconds();
	main_time_globals->should_reset = false;
	main_time_globals->last_vblank_index = 0;
	main_time_globals->temporary_throttle_control = 0;

	csmemset(g_flip_deltas, 0, sizeof(g_flip_deltas));

	g_main_game_time_counter_last_time = shell_time_counter_now(NULL);
	return;
}

void __cdecl main_time_reset(void)
{
	s_main_time_globals* main_time_globals = main_time_globals_get();
	main_time_globals->last_milliseconds = system_milliseconds();
	main_time_globals->should_reset = true;
	g_main_game_time_counter_last_time = shell_time_counter_now(NULL);
	return;
}

int32 main_time_maximum_ticks_per_frame(void)
{
	return k_maximum_ticks_per_frame;
}

bool main_time_is_throttled(void)
{
	const s_main_time_globals* main_time_globals = main_time_globals_get();

	bool result = true;
	if (!main_time_globals->temporary_throttle_control)
	{
		const s_window_globals* window_globals = window_globals_get();
		s_rasterizer_globals* rasterizer_globals = rasterizer_globals_get();

		result =
			rasterizer_globals->clipping_parameters.parameters.field_0
			&& game_in_progress()
			&& !debug_disable_frame_rate_throttle
			&& main_time_globals->wmv_playback_in_progress
			&& main_time_globals->wmv_playback_throttle_framerate
			|| IsIconic(window_globals->hWnd) && !shell_is_dedicated_server();
	}

	return result;
}

int32 main_time_throttle(void)
{
	return INVOKE(0x287A1, 0x0, main_time_throttle);
}

void main_time_continue(void)
{
	s_main_time_globals* main_time_globals = main_time_globals_get();
	main_time_globals->should_reset = false;
	return;
}

void main_time_advance(int32 milliseconds)
{
	s_main_time_globals* main_time_globals = main_time_globals_get();
	main_time_globals->last_milliseconds += milliseconds;
	return;
}

real32 __cdecl main_time_update(void)
{
	//return INVOKE(0x28814, 0x0, main_time_update, fixed_time_step, fixed_time_delta);
	
	real32 dt_sec = 0.f;
	const int32 game_time = game_in_progress() ? game_time_get() : 0;
	const LARGE_INTEGER freq = shell_time_counter_freq();

	shell_idle();
	
	dt_sec = main_time_delta_calculate(shell_time_counter_now(NULL), freq);

	// don't run the frame limiter when time step is fixed, because the code doesn't support it
	// in case of fixed time step, frame limiter should be handled by the other frame limiter
	if (main_time_is_throttled()
		|| g_main_game_time_frame_limiter_enabled)
	{
		if (game_time_initialized())
		{
			// Sleep() once, spin lock the rest
			bool system_sleep_once = false;
			const real32 k_system_sleep_max = 2.0f + k_real_epsilon;

			// if there's game tick leftover time (i.e the actual game tick update executed faster than the actual engine's fixed time step)
			// FIXED by interpolation: 
			// limit the framerate to get back in sync with the renderer to prevent ghosting and jagged movement
			while (dt_sec < game_tick_length())
			{
				real32 fMsSleep = (real32)(game_tick_length() - dt_sec) * 1000.f;

				// to reduce stuttering, spend some of the time to sleep by CPU spinning,
				// Sleep is not precise since Windows is not a RTOS
				if (!system_sleep_once && fMsSleep > k_system_sleep_max)
				{
					int32 yield_time = (int32)(fMsSleep - k_system_sleep_max);
					yield_time = MAX(0, yield_time);

					Sleep((DWORD)yield_time);
					system_sleep_once = true;
				}

				dt_sec = main_time_delta_calculate(shell_time_counter_now(NULL), freq);
			}
		}
		else
		{
			Sleep(15u);
			dt_sec = main_time_delta_calculate(shell_time_counter_now(NULL), freq);
		}
	}
	else
	{
		shell_windows_throttle_framerate(g_main_game_time_counter_last_time, H2Config_fps_limit);
		dt_sec = main_time_delta_calculate(shell_time_counter_now(NULL), freq);
	}
	
	s_main_time_globals* main_time_globals = main_time_globals_get();

#ifdef MAIN_TIME_DEBUG
	if (k_use_precise_counters)
	{
		const uint64 time_now_msec = main_time_get_absolute_milliseconds();
		g_main_game_time_debug.dt_default = (real32)((real32)(time_now_msec - main_time_globals->last_milliseconds) / 1000.f);
		g_main_game_time_debug.dt_performance_counter = dt_sec;
	}
	else
	{
		g_main_game_time_debug.dt_default = dt_sec;
		g_main_game_time_debug.dt_performance_counter = main_time_get_delta_sec_precise(shell_time_counter_now(NULL), freq);
	}
#endif

	g_main_game_time_counter_last_time = shell_time_counter_now(NULL);

	dt_sec = movie_recording() ? movie_recording_timestep() : dt_sec;

	dt_sec = PIN(dt_sec, 0.f, 10.f);
	
	last_framerate_time = dt_sec;
	main_time_globals->last_milliseconds = main_time_get_absolute_milliseconds();
	main_time_globals->last_game_time = game_time;

	const int64 last_initial_vblank_index = *Memory::GetAddress<int64*>(0xA3E440, 0x4905C8);
	main_time_globals->last_vblank_index = last_initial_vblank_index;
	main_time_globals->last_initial_vblank_index = last_initial_vblank_index;
	return dt_sec;
}

bool main_time_halted(void)
{
	bool result = shell_application_is_paused();
#if TERMINAL_ENABLED
	if (debug_console_pauses_game
		&& console_is_active()
		&& (!game_in_progress() || !game_is_networked() || game_is_playback()))
	{
		result = true;
	}
#endif
	return result;
}

bool __cdecl main_time_should_reset(void)
{
	bool result = main_time_globals_get()->should_reset;
	return result;
}

int32 __cdecl main_time_get_tickrate(void)
{
	return INVOKE(0x28707, 0x2489B, main_time_get_tickrate);
}

void main_time_frame_rate_display(void)
{
	if (display_framerate)
	{
		const real32 frame_time = last_framerate_time;
		const real32 framerate = 1.f / frame_time;
		const int32 framerate_rounded = real_to_long_round(framerate);

		char string[64];
		csprintf(string, NUMBEROF(string), "[%.1f] %d", (real32)framerate_rounded, framerate_rounded);
		
		const s_render* render = render_get();
		const int16 width = rectangle2d_width(&render->camera.window_bounds);
		const int16 height = rectangle2d_height(&render->camera.window_bounds);

		rectangle2d rect;
		rasterizer_get_screen_bounds(&rect);
		
		// Scale the offsets so we don't end up with different placement based on the resolution
		const int16 scaled_width_offset = 150 * (width / 640);
		const int16 scaled_height_offset = 50 * (height / 480);

		// If we play at a resolution lower than 640 x 480 use the original offsets (scaled offsets will be 0)
		const int16 width_offset = MAX(scaled_width_offset, 150);	
		const int16 height_offset = MAX(scaled_height_offset, 50);

		rect.left = rect.right - width_offset;
		rect.top = rect.bottom - height_offset;
		
		draw_string_set_format(NONE, 0, 0, true);	// We wrap horizontally since we can display more than 2 sig figures 
		const real_argb_color* color = framerate_rounded < k_target_framerate ? global_real_argb_red : global_real_argb_green;
		draw_string_set_color(color);
		draw_string_set_font(_font_id_6);
		rasterizer_draw_string(&rect, 0, 0, 0, string);
	}

	// This code doesn't technically work on PC but i've restored it anyways
	if (display_frame_deltas)
	{
		rectangle2d rect;
		rasterizer_get_frame_bounds(&rect);

		const s_render* render = render_get();
		const int16 width = rectangle2d_width(&render->camera.window_bounds);
		const int16 height = rectangle2d_height(&render->camera.window_bounds);

		const int16 width_scale_factor = MAX(width / 640, 1);
		const int16 height_scale_factor = MAX(height / 480, 1);

		const int16 scaled_width_offset = 50 * width_scale_factor;
		const int16 scaled_height_offset = 50 * height_scale_factor;

		rect.left = rect.right - scaled_width_offset;
		rect.top = rect.bottom - scaled_height_offset;

		for (
			int8 delta_index = (g_next_flip_delta_index + (k_flip_delta_count - 1)) % k_flip_delta_count;
			delta_index != g_next_flip_delta_index;
			delta_index = (delta_index + (k_flip_delta_count - 1)) % k_flip_delta_count
		)
		{
			rect.top -= (20 * height_scale_factor);
			rect.bottom -= (20 * height_scale_factor);
			char string[4];
			csprintf(string, NUMBEROF(string), "%d", g_flip_deltas[delta_index]);
			draw_string_set_format(NONE, 1, 0, false);
			draw_string_set_font(_font_id_6);
			const real_argb_color* color = g_flip_deltas[delta_index] < 2 ? global_real_argb_red : global_real_argb_green;
			draw_string_set_color(color);
			rasterizer_draw_string(&rect, 0, 0, 0, string);
		}
	}

	return;
}

/* private code */

static s_main_time_globals* main_time_globals_get(void)
{
	return Memory::GetAddress<s_main_time_globals*>(0x479E90, 0x4A2980);
}

static real32 main_time_get_delta_sec_precise(LARGE_INTEGER counter_now, LARGE_INTEGER freq)
{
	real32 result = (real32)(((real64)shell_time_counter_diff(counter_now, g_main_game_time_counter_last_time).QuadPart) / (real64)freq.QuadPart);
	return result;
}

static uint64 main_time_get_absolute_milliseconds(void)
{
	uint64 milliseconds = system_milliseconds();
	s_main_time_globals* main_time_globals = main_time_globals_get();

	// copy the high part
	milliseconds |= (uint64)(main_time_globals->last_milliseconds >> 32) << 32;

	if (milliseconds < main_time_globals->last_milliseconds)
	{
		// adjust the time, a complete cycle of system_milliseconds has passed
		milliseconds += UINT_MAX + 1ULL;
	}

	ASSERT(milliseconds >= main_time_globals->last_milliseconds);
	return milliseconds;
}

static real32 main_time_delta_calculate(LARGE_INTEGER counter_now, LARGE_INTEGER freq)
{
	const s_main_time_globals* main_time_globals = main_time_globals_get();

	real32 dt;
	// We want the return value to be configurable
	if (k_use_precise_counters)
	{
		// Precise dt
		dt = main_time_get_delta_sec_precise(counter_now, freq);
	}
	else
	{
		// Imprecise dt (original logic)
		uint64 time_now_msec = main_time_get_absolute_milliseconds();
		dt = (real32)((real32)(time_now_msec - main_time_globals->last_milliseconds) / 1000.f);
	}

	return dt;
}
