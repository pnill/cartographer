#pragma once

/* forward declarations */

enum e_user_interface_screen_id : uint32;

/* prototypes */

void user_interface_utilities_apply_patches(void);

void __cdecl user_interface_global_string_get(string_id id, c_maximum_interface_text* dest);

bool __cdecl user_interface_exit_to_dash();

bool __cdecl user_interface_save_map_and_exit();

void user_interface_transition_to_offline();

void __cdecl user_interface_get_cursor_position(point2d* out_position);

void __cdecl user_interface_get_cursor_position_scaled(point2d* out_position);

void __cdecl user_interface_utilities_play_sound(enum e_user_interface_global_sound sound_type);
