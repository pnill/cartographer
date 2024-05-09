#pragma once
#include "camera/observer.h"
#include "math/matrix_math.h"
#include "math/periodic_functions.h"
#include "Networking/Session/NetworkSession.h"

enum e_player_effect_user_global_flags : uint8
{
	_player_effect_user_global_bit_0 = 0,
	_player_effect_apply_camera_impulse = 1,
	_player_effect_apply_camera_shake = 2,
	k_player_effect_user_global_flag_count
};

enum e_player_effect_global_flags : uint32
{
	_player_effect_global_bit_0 = 0,
	_player_effect_global_bit_1 = 1,
	k_player_effect_global_flag_count
};

enum e_screen_flash_type : int16
{
	_screen_flash_type_none = 0,
	_screen_flash_type_lighten = 1,
	_screen_flash_type_darken = 2,
	_screen_flash_type_max = 3,
	_screen_flash_type_min = 4,
	_screen_flash_type_invert = 5,
	_screen_flash_type_tint = 6
};

enum e_screen_flash_priority : int16
{
	_screen_flash_priority_low = 0,
	_screen_flash_priority_medium = 1,
	_screen_flash_priority_high = 2
};

// equivalent to e_transition_function but zero and one spots are swapped?
enum e_screen_flash_fade_function : int16
{
	_screen_flash_fade_function_linear = 0,
	_screen_flash_fade_function_early = 1,
	_screen_flash_fade_function_very_early = 2,
	_screen_flash_fade_function_late = 3,
	_screen_flash_fade_function_very_late = 4,
	_screen_flash_fade_function_cosine = 5,
	_screen_flash_fade_function_zero = 6,
	_screen_flash_fade_function_one = 7
};

// max count: 1
struct screen_flash_definition
{
	e_screen_flash_type type;
	e_screen_flash_priority priority;
	real32 duration;	// Seconds
	e_screen_flash_fade_function fade_function;
	int16 pad;
	real32 maximum_intensity;
	real_argb_color color;
};
ASSERT_STRUCT_SIZE(screen_flash_definition, 32);

struct s_temporary_camera_impulse
{
	real32 duration;
	int8 fade_function; // e_transition_function_type
	int8 pad;
	real32 rotation;
	real32 pushback;
	real_bounds jitter;
};
ASSERT_STRUCT_SIZE(s_temporary_camera_impulse, 24);

struct s_player_effect_camera_shaking
{
	real32 duration;
	int8 falloff_function;	// e_transition_function_type
	int8 pad;
	real32 random_translation;
	real_angle random_rotation;
	e_periodic_function_type wobble_function;
	real32 wobble_function_period;
	real32 wobble_weight;
};
ASSERT_STRUCT_SIZE(s_player_effect_camera_shaking, 28);

struct s_player_effect_user_globals
{
	real_vector3d vector_0;
	real_vector3d vector_C;
	int16 field_18;
	int16 pad_1A;
	real32 field_1C;
	e_transition_function_type screen_flash_function;
	int16 pad;
	real32 field_24;
	real_argb_color screen_flash_color;
	s_temporary_camera_impulse camera_impulse;
	s_player_effect_camera_shaking camera_shake;
	real32 rumble_intensity_left;
	real32 rumble_intensity_right;
	real32 field_74;
	real32 field_78;
	int16 field_7C;
	int16 game_time;
	int16 camera_impulse_countdown;
	int16 camera_shake_countdown;
	int8 field_84;
	int8 field_85;
	int8 damage_indicator;
	int8 field_87;
	c_flags<e_player_effect_user_global_flags, uint8, k_player_effect_user_global_flag_count> flags;
	int8 field_89;
	int16 pad_8A;
	real_point3d origin;
	real32 camera_shake_transition_scale;
	real32 camera_impulse_transition_scale;
};
ASSERT_STRUCT_SIZE(s_player_effect_user_globals, 160);

struct s_player_effect_globals
{
	int32 field_0;
	int16 field_4;
	bool field_6;
	int8 field_7;
	s_observer_command_displacement position;
	real32 max_intensity;
	int16 attack_time_ticks;
	int16 attack_time_ticks_1;
	c_flags<e_player_effect_global_flags, uint32, k_player_effect_global_flag_count> flags;
	uint32 current_time_ticks;
	s_player_effect_user_globals user_effects[k_number_of_users];
};
ASSERT_STRUCT_SIZE(s_player_effect_globals, 688);

struct s_screen_flash
{
	int32 type;	// e_screen_flash_type
	real32 intensity;
	real_argb_color color;
};

void player_effect_apply_camera_effect_matrix(int32 user_index, real_matrix4x3* matrix);

// Render screen flash
void __cdecl render_screen_flash(int32 player_index, s_screen_flash* screen_flash);
