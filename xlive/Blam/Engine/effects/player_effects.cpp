#include "stdafx.h"
#include "player_effects.h"

#include "game/game_time.h"
#include "game/player_vibration.h"
#include "main/interpolator.h"
#include "math/random_math.h"

void player_effect_apply_camera_effect_matrix_internal(real_matrix4x3* matrix, real32 translation, real32 rotation);
real32 player_effect_transition_function_evaluate(e_transition_function_type function_type, real32 scale, real32 elapsed_time, real32 duration);

s_player_effect_globals* player_effect_globals_get(void)
{
    return *Memory::GetAddress<s_player_effect_globals**>(0x4CE860, 0x4F504C);
}

s_player_effect_user_globals* player_effects_get_user_globals(int32 user_index)
{
    return &player_effect_globals_get()->user_effects[user_index];
}

void player_effect_apply_camera_effect_matrix(int32 user_index, real_matrix4x3* matrix)
{
    // INVOKE(0xA432D, 0x963AA, player_effect_apply_camera_effect_matrix, user_index, matrix);
    // return;
	
    real_matrix4x3 effect_matrix;
    if (user_index != NONE)
    {
        s_player_effect_globals* player_effects_globals = player_effect_globals_get();
        if (player_effects_globals->flags.test(_player_effect_global_bit_0))
        {
            real32 vibration_intensity = player_effects_globals->max_intensity;
            effect_matrix = global_identity4x3;

            if (player_effects_globals->attack_time_passed_ticks > 0)
            {
                real32 attack_normalized_time_passed = (real32)player_effects_globals->attack_time_passed_ticks / (real32)player_effects_globals->attack_max_time_in_ticks;
                if (!player_effects_globals->flags.test(_player_effect_global_bit_1))
                {
                    attack_normalized_time_passed = 1.0f - attack_normalized_time_passed;
                }

                vibration_intensity *= attack_normalized_time_passed;
            }

            vibration_intensity = PIN(vibration_intensity, 0.0f, 1.0f);

            rumble_player_set_scripted_scale(vibration_intensity);

            real_vector3d random_angles;
            random_angles.k = _real_random_range(get_local_random_seed_address(), -1.0f, 1.0f);
            random_angles.j = _real_random_range(get_local_random_seed_address(), -1.0f, 1.0f);
            random_angles.i = _real_random_range(get_local_random_seed_address(), -1.0f, 1.0f);
            multiply_vectors3d(&random_angles, &player_effects_globals->position.orientation, &random_angles);
            scale_vector3d(&random_angles, vibration_intensity, &random_angles);

            matrix4x3_rotation_from_angles(&effect_matrix, random_angles.i, random_angles.j, random_angles.k);

            random_angles.i = _real_random_range(get_local_random_seed_address(), -1.0f, 1.0f);
            random_angles.j = _real_random_range(get_local_random_seed_address(), -1.0f, 1.0f);
            random_angles.k = _real_random_range(get_local_random_seed_address(), -1.0f, 1.0f);

            effect_matrix.position.x = random_angles.i * player_effects_globals->position.position.y * vibration_intensity;
            effect_matrix.position.y = random_angles.j * player_effects_globals->position.position.x * vibration_intensity;
            effect_matrix.position.z = random_angles.k * player_effects_globals->position.position.z * vibration_intensity;
            matrix4x3_multiply(matrix, &effect_matrix, matrix);
        }
        else
        {
            s_player_effect_user_globals* user_effect = player_effects_get_user_globals(user_index);
            bool apply_camera_impulse = user_effect->flags.test(_player_effect_apply_camera_impulse);
            if (user_effect->camera_impulse_countdown > 0 || apply_camera_impulse)
            {
                real32 transition_result;
                if (apply_camera_impulse)
                {
                    transition_result = 1.0f;
                }
                else
                {
                    real32 impulse_elapsed_time = user_effect->camera_impulse.duration - (real32)user_effect->camera_impulse_countdown;
                    // do not stale the transition function
                    impulse_elapsed_time += halo_interpolator_get_interpolation_time();

                    transition_result = player_effect_transition_function_evaluate(
                        (e_transition_function_type)user_effect->camera_impulse.fade_function,
                        user_effect->camera_impulse_transition_scale,
                        impulse_elapsed_time,
                        user_effect->camera_impulse.duration);
                }

                real_vector3d v1;
                cross_product3d(&global_up3d, &user_effect->vector_0, &v1);

                real32 rotation = user_effect->camera_impulse.rotation * transition_result;
                matrix4x3_rotation_from_axis_and_angle(&effect_matrix, &v1, sin(rotation), cos(rotation));

                real32 pushback_transition = user_effect->camera_impulse.pushback * transition_result;
                scale_vector3d(&user_effect->vector_0, pushback_transition, &effect_matrix.position);
                point_from_line3d(&effect_matrix.position, &user_effect->vector_C, transition_result, &effect_matrix.position);
                matrix4x3_multiply(matrix, &effect_matrix, matrix);
            }

            bool apply_camera_shake = user_effect->flags.test(_player_effect_apply_camera_shake);
            if (user_effect->camera_shake_countdown > 0 || apply_camera_shake)
            {
                effect_matrix = global_identity4x3;

                real32 transition_function_result;
                if (apply_camera_shake)
                {
                    transition_function_result = 1.0f;
                }
                else
                {
                    real32 shake_elapsed_time = user_effect->camera_shake.duration
                        - game_ticks_to_seconds((real32)user_effect->camera_shake_countdown - halo_interpolator_get_interpolation_time());
                    transition_function_result = player_effect_transition_function_evaluate(
                        (e_transition_function_type)user_effect->camera_shake.falloff_function,
                        user_effect->camera_shake_transition_scale,
                        shake_elapsed_time,
                        user_effect->camera_shake.duration
                    );
                }

                real32 camera_shake_periodic_value = user_effect->camera_shake.duration - game_ticks_to_seconds((real32)user_effect->camera_shake_countdown);
                camera_shake_periodic_value /= user_effect->camera_shake.wobble_function_period;

                real32 periodic_function_result = periodic_function_evaluate(user_effect->camera_shake.wobble_function, camera_shake_periodic_value);

                real32 periodic_function_result_scaled = 
                    transition_function_result * ((1.0f - user_effect->camera_shake.wobble_weight) + (user_effect->camera_shake.wobble_weight * periodic_function_result));

                real32 v1 = MAX(0.0f, user_effect->camera_shake.random_translation * periodic_function_result_scaled);
                real32 v2 = MAX(0.0f, user_effect->camera_shake.random_rotation * periodic_function_result_scaled);

                if (user_effect->field_7C > 0)
                {
                    real32 seconds_7C = game_ticks_to_seconds(user_effect->field_7C);
                    seconds_7C += seconds_7C;

                    v1 += user_effect->field_74 * seconds_7C;
                    v2 += user_effect->field_78 * seconds_7C;

                    rumble_player_continuous(user_index, user_effect->rumble_intensity_left, user_effect->rumble_intensity_right);
                }
                player_effect_apply_camera_effect_matrix_internal(&effect_matrix, v1, v2);
                matrix4x3_multiply(matrix, &effect_matrix, matrix);
            }
        }
    }

    return;
}


void player_effect_apply_camera_effect_matrix_internal(real_matrix4x3* matrix, real32 translation, real32 rotation)
{
    real_vector3d vector;

    if (rotation != 0.0f)
    {
        _random_direction3d(get_local_random_seed_address(), NULL, __FILE__, __LINE__, &vector);
        matrix4x3_rotation_from_axis_and_angle(matrix, &vector, sin(rotation), cos(rotation));
    }
    if (translation != 0.0f)
    {
        _random_direction3d(get_local_random_seed_address(), NULL, __FILE__, __LINE__, &vector);
        scale_vector3d(&vector, translation, &matrix->position);
    }
    return;
}

real32 player_effect_transition_function_evaluate(e_transition_function_type function_type, real32 scale, real32 elapsed_time, real32 duration)
{
    real32 function_value = 1.0f - (elapsed_time / duration);
    return transition_function_evaluate(function_type, function_value) * scale;
}

void __cdecl render_screen_flash(int32 player_index, s_screen_flash* screen_flash)
{
    INVOKE(0xA408F, 0x0, render_screen_flash, player_index, screen_flash);
    return;
}
