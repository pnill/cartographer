#include "stdafx.h"
#include "player_effects.h"

#include "Blam/Engine/game/game_time.h"
#include "Blam/Engine/game/player_vibration.h"
#include "Blam/Engine/main/interpolator.h"
#include "Blam/Engine/math/random_math.h"

void player_effect_apply_camera_effect_matrix_internal(real_matrix4x3* matrix, real32 a2, real32 a3);
real32 player_effect_transition_function_evaluate(e_transition_function_type function_type, real32 a2, real32 a3, real32 a4);

s_player_effect_globals* player_effect_globals_get(void)
{
    return Memory::GetAddress<s_player_effect_globals*>(0x4CE860, 0x4F504C);
}

/*
void __cdecl player_effect_apply_camera_effect_matrix(int32 user_index, real_matrix4x3* matrix)
{
	INVOKE(0xA432D, 0x963AA, player_effect_apply_camera_effect_matrix, user_index, matrix);
	return;
}*/

void player_effect_apply_camera_effect_matrix(int32 user_index, real_matrix4x3* matrix)
{
    real_matrix4x3 calculated_matrix;
    if (user_index != NONE)
    {
        s_player_effect_globals* player_effects_globals = player_effect_globals_get();
        if (player_effects_globals->flags.test(_player_effect_global_bit_0))
        {
            real32 vibration_intensity = player_effects_globals->max_intensity;
            calculated_matrix = global_identity4x3;
            
            if (player_effects_globals->attack_time_ticks > 0)
            {
                real32 attack_time = player_effects_globals->attack_time_ticks / player_effects_globals->attack_time_ticks_1;
                if (!player_effects_globals->flags.test(_player_effect_global_bit_1))
                {
                    attack_time = 1.0f - attack_time;
                }

                vibration_intensity *= attack_time;
            }

            if (vibration_intensity >= 0.0f)
            {
                if (vibration_intensity > 1.0f)
                {
                    vibration_intensity = 1.0f;
                }
            }
            else
            {
                vibration_intensity = 0.0f;
            }

            rumble_player_set_scripted_scale(vibration_intensity);

            real_vector3d random_angles;
            random_angles.k = _real_random_range(get_local_random_seed_address(), -1.0f, 1.0f);
            random_angles.j = _real_random_range(get_local_random_seed_address(), -1.0f, 1.0f);
            random_angles.i = _real_random_range(get_local_random_seed_address(), -1.0f, 1.0f);
            multiply_vectors3d(&random_angles, &player_effects_globals->position.orientation, &random_angles);
            scale_vector3d(&random_angles, vibration_intensity, &random_angles);

            matrix4x3_rotation_from_angles(&calculated_matrix, random_angles.i, random_angles.j, random_angles.k);
            
            random_angles.i = _real_random_range(get_local_random_seed_address(), -1.0f, 1.0f);
            random_angles.j = _real_random_range(get_local_random_seed_address(), -1.0f, 1.0f);
            random_angles.k = _real_random_range(get_local_random_seed_address(), -1.0f, 1.0f);

            calculated_matrix.position.x = random_angles.i * player_effects_globals->position.position.y * vibration_intensity;
            calculated_matrix.position.y = random_angles.j * player_effects_globals->position.position.x * vibration_intensity;
            calculated_matrix.position.z = random_angles.k * player_effects_globals->position.position.z * vibration_intensity;
            matrix4x3_multiply(matrix, &calculated_matrix, matrix);
        }
        else
        {
            s_player_effect_user_globals* user_effect = &player_effects_globals->user_effects[user_index];
            bool bit_1_result = user_effect->flags.test(_player_effect_user_global_bit_1);
            if (user_effect->field_80 > 0 || bit_1_result)
            {
                real32 function_result;
                if (bit_1_result)
                {
                    function_result = 1.0f;
                }
                else
                {
                    real32 timing = user_effect->camera_impulse.duration - (real32)user_effect->field_80 - halo_interpolator_get_interpolation_time();
                    function_result = player_effect_transition_function_evaluate(user_effect->camera_impulse.fade_function, user_effect->transition_function_scale_9C, timing, user_effect->camera_impulse.duration);
                }
                
                real_vector3d vector;
                cross_product3d(&global_up3d, &user_effect->vector_0, &vector);

                real32 function_result_scaled = user_effect->camera_impulse.rotation * function_result;
                matrix4x3_rotation_from_axis_and_angle(&calculated_matrix, &vector, sin(function_result_scaled), cos(function_result_scaled));

                real32 position_scaler = user_effect->camera_impulse.pushback * function_result;
                calculated_matrix.position.x = user_effect->vector_0.i * position_scaler;
                calculated_matrix.position.y = user_effect->vector_0.j * position_scaler;
                calculated_matrix.position.z = user_effect->vector_0.k * position_scaler;
                point_from_line3d(&calculated_matrix.position, &user_effect->vector_C, function_result, &calculated_matrix.position);
                matrix4x3_multiply(matrix, &calculated_matrix, matrix);
            }

            bool bit_2_result = user_effect->flags.test(_player_effect_user_global_bit_2);
            if (user_effect->field_82 > 0 || bit_2_result)
            {
                calculated_matrix = global_identity4x3;

                real32 transition_function_result;
                real32 timing = user_effect->camera_shaking.duration - game_ticks_to_seconds((real32)user_effect->field_82) - halo_interpolator_get_interpolation_time();
                if (bit_2_result)
                {
                    transition_function_result = 1.0f;
                }
                else
                {
                    transition_function_result = player_effect_transition_function_evaluate(user_effect->camera_shaking.falloff_function, user_effect->transition_function_scale_98, timing, user_effect->camera_shaking.duration);
                }

                real32 seconds_result = timing / user_effect->camera_shaking.wobble_function_period;
                real32 periodic_function_result = periodic_function_evaluate(user_effect->camera_shaking.wobble_function, seconds_result);
                real32 periodic_function_result_scaled = periodic_function_result * transition_function_result * user_effect->camera_shaking.wobble_weight + transition_function_result * (1.0 - user_effect->camera_shaking.wobble_weight);

                // Set v1 to 0 if v1_value is less than 0
                real32 v1_value = user_effect->camera_shaking.random_translation * periodic_function_result_scaled;
                real32 v1 = (v1_value <= 0.0f ? 0.0f : v1_value);

                // Set v2 to 0 if v2_value is less than 0
                real32 v2_value = user_effect->camera_shaking.random_rotation * periodic_function_result_scaled;
                real32 v2 = (v2_value <= 0.0f ? 0.0f : v2_value);

                if (user_effect->field_7C > 0)
                {
                    real32 seconds_7C = game_ticks_to_seconds((real32)user_effect->field_7C);
                    real32 seconds_7C_x2 = seconds_7C + seconds_7C;

                    v1 += seconds_7C_x2 * user_effect->field_74;
                    v2 += seconds_7C_x2 * user_effect->field_78;
                    
                    rumble_player_continuous(user_index, user_effect->rumble_intensity_left, user_effect->rumble_intensity_right);
                }
                player_effect_apply_camera_effect_matrix_internal(&calculated_matrix, v1, v2);
                matrix4x3_multiply(matrix, &calculated_matrix, matrix);
            }
        }
    }

    return;
}


void player_effect_apply_camera_effect_matrix_internal(real_matrix4x3* matrix, real32 a2, real32 a3)
{
    real_vector3d vector;

    if (a3 != 0.0f)
    {
        _random_direction3d(get_local_random_seed_address(), NULL, __FILE__, __LINE__, &vector);
        matrix4x3_rotation_from_axis_and_angle(matrix, &vector, sin(a3), cos(a3));
    }
    if (a2 != 0.0f)
    {
        _random_direction3d(get_local_random_seed_address(), NULL, __FILE__, __LINE__, &vector);
        matrix->position.x = vector.i * a2;
        matrix->position.y = vector.j * a2;
        matrix->position.z = vector.k * a2;
    }
    return;
}

real32 player_effect_transition_function_evaluate(e_transition_function_type function_type, real32 a2, real32 a3, real32 a4)
{
    real32 function_value = 1.0f - (a3 / a4);
    return transition_function_evaluate(function_type, function_value) * a2;
}