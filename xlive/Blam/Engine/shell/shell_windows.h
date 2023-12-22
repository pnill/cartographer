#pragma once

#define k_process_system_time_startup_offset (1000 * 60 * 60)

#define k_shell_time_sec_denominator 1
#define k_shell_time_msec_denominator 1000
#define k_shell_time_usec_denominator 1000000

void shell_windows_initialize();
void shell_windows_apply_patches();

void shell_windows_throttle_framerate(int desired_framerate);

bool __cdecl game_is_minimized(void);
