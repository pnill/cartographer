#pragma once
#include "Blam/Engine/camera/observer.h"
#include "Blam/Engine/math/matrix_math.h"
#include "Blam/Engine/math/periodic_functions.h"
#include "Blam/Engine/Networking/Session/NetworkSession.h"

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

struct s_temporary_camera_impulse
{
	real32 duration;
	int8 fade_function; // e_transition_function_type
	int8 pad;
	real32 rotation;
	real32 pushback;
	real_bounds jitter;
};
CHECK_STRUCT_SIZE(s_temporary_camera_impulse, 24);

struct s_player_effect_camera_shaking
{
	real32 duration;
	int8 falloff_function;	// e_transition_function_type
	real32 random_translation;
	real_angle random_rotation;
	e_periodic_function_type wobble_function;
	real32 wobble_function_period;
	real32 wobble_weight;
};
CHECK_STRUCT_SIZE(s_player_effect_camera_shaking, 28);

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
	s_player_effect_camera_shaking camera_shaking;
	real32 rumble_intensity_left;
	real32 rumble_intensity_right;
	real32 field_74;
	real32 field_78;
	int16 field_7C;
	int16 game_time;
	int16 camera_impulse_countdown;
	int16 camera_shake_passed_time;
	int8 field_84[4];
	c_flags<e_player_effect_user_global_flags, uint8, k_player_effect_user_global_flag_count> flags;
	int8 field_89;
	int16 pad_8A;
	real_point3d origin;
	real32 transition_function_scale_98;
	real32 transition_function_scale_9C;
};
CHECK_STRUCT_SIZE(s_player_effect_user_globals, 160);

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
CHECK_STRUCT_SIZE(s_player_effect_globals, 688);

void player_effect_apply_camera_effect_matrix(int32 user_index, real_matrix4x3* matrix);
