#include "stdafx.h"
#include "bipeds.h"

#include "Blam/Engine/game/game_time.h"
#include "Blam/Engine/cache/cache_files.h"
#include "Blam/Engine/game/game.h"
#include "Blam/Engine/game/game_time.h"
#include "Blam/Engine/main/interpolator.h"
#include "Blam/Engine/math/periodic_functions.h"
#include "Blam/Engine/physics/collisions.h"
#include "Blam/Engine/render/render.h"
#include "Blam/Engine/units/biped_definitions.h"

#include "Util/Hooks/Hook.h"

bool __cdecl biped_is_running_invisible_crouched_uber_melee(datum object_index)
{
    return INVOKE(0x152744, 0x136A00, biped_is_running_invisible_crouched_uber_melee, object_index);
}


void biped_build_2d_camera_frame(const real_vector3d* forward, const real_vector3d* up, real_vector2d* forward_out, real_vector2d* up_out)
{
    forward_out->i = forward->i;
    forward_out->j = forward->j;
    if (normalize2d(forward_out) == 0.0)
    {
        forward_out->i = up->i;
        forward_out->j = up->j;
        if (forward->k > 0.0)
        {
            forward_out->i = -forward_out->i;
            forward_out->j = -forward_out->j;
        }
        if (normalize2d(forward_out) == 0.0)
        {
            *forward_out = global_forward2d;
        }
    }
    perpendicular2d(forward_out, up_out);
}

void __cdecl biped_offset_first_person_camera(const real_vector3d* camera_forward, datum object_index, real_point3d* camera_position, const real_vector3d* camera_up)
{
    biped_datum* biped = (biped_datum*)object_get_fast_unsafe(object_index);
    _biped_definition* biped_def = (_biped_definition*)tag_get_fast(biped->unit.object.tag_definition_index);

    if (biped_def->camera_interpolation_end > biped_def->camera_interpolation_start)
    {
        real32 v1;
        real32 angle_range = biped_def->camera_interpolation_end - biped_def->camera_interpolation_start;
        if (camera_forward->k > -1.0f)
        {
            if (camera_forward->k >= 0.0f)
            {
                v1 = 0.0f;
            }
            else
            {
                v1 = M_PI_2 - acosf(-camera_forward->k);
            }
        }
        else
        {
            v1 = M_PI_2;
        }

        real32 function_value = (v1 - biped_def->camera_interpolation_start) / angle_range;
        real32 function_result;
        if (function_value >= 0.0f)
        {
            if (function_value > 1.0f)
            {
                function_value = 1.0f;
            }
            function_result = transition_function_evaluate(_transition_function_cosine, function_value);
        }
        else
        {
            function_result = transition_function_evaluate(_transition_function_cosine, 0.0f);
        }


        if (function_result > 0.0f && biped->field_3E0)
        {
            real_vector2d forward_out;
            real_vector2d up_out;
            biped_build_2d_camera_frame(camera_forward, camera_up, &forward_out, &up_out);

            real_matrix4x3 interpolated_node_matrix;
            real_matrix4x3* node_matrix;
            
            if (halo_interpolator_interpolate_object_node_matrix(object_index, 0, &interpolated_node_matrix))
            {
                node_matrix = &interpolated_node_matrix;
            }
            else
            {
                node_matrix = object_get_node_matrix(object_index, 0);
            }

            real_vector3d camera_diff;
            vector_from_points3d(camera_position, &node_matrix->position, &camera_diff);


            real32 forward_product = dot_product2d(&forward_out, &camera_diff.vector2d);
            real32 up_product = dot_product2d(&up_out, &camera_diff.vector2d);

            real_vector3d biped_vector = biped->vector_3E4;

            if (biped->vector_3E4.i <= biped_def->camera_exclusion_distance)
            {
                biped_vector.i = biped_def->camera_exclusion_distance;
            }

            real32 i_diff = biped_vector.i - forward_product;
            real32 j_diff = biped_vector.j - up_product;
            real32 k_diff = biped_vector.k - camera_diff.k;

            real32 forward_scale = biped_def->camera_forward_movement_scale * i_diff * function_result;
            real32 side_scale = biped_def->camera_side_movement_scale * j_diff * function_result;
            real32 vertical_offset = biped_def->camera_vertical_movement_scale * k_diff * function_result;

            camera_position->x += (forward_out.i * forward_scale) + (up_out.i * side_scale);
            camera_position->y += (forward_out.j * forward_scale) + (up_out.j * side_scale);

            camera_position->z += vertical_offset;
        }
    }
    return;
}

real32 local_user_crouch_delta_accum[4]{ 0.f, 0.f, 0.f, 0.f };
real32 local_user_crouch_last_frame_time[4]{ 0.f, 0.f, 0.f, 0.f };

void __cdecl biped_get_sight_position(
    datum biped_index,
    e_unit_estimate_mode estimate_mode,
    real_point3d* estimated_body_position,
    real_vector3d* desired_facing_vector,
    real_vector3d* desired_gun_offset,
    real_point3d* object_origin)
{
    if (estimate_mode)
    {
        *object_origin = *estimated_body_position;
        if (estimate_mode == _unit_estimate_gun_position)
        {
            const real_vector3d direction = { -desired_facing_vector->j, desired_facing_vector->i, 0.0f };
            point_from_line3d(object_origin, desired_facing_vector, desired_gun_offset->i, object_origin);
            point_from_line3d(object_origin, &direction, desired_gun_offset->j, object_origin);
            object_origin->z += desired_gun_offset->k;
            return;
        }
    }
    else
    {
        object_get_origin_interpolated(biped_index, object_origin);
    }

    biped_datum* biped = (biped_datum*)object_get_fast_unsafe(biped_index);
    _biped_definition* biped_def = (_biped_definition*)tag_get_fast(biped->unit.object.tag_definition_index);

    real32 crouching = 0.0f;

    //real32 crouch_time = biped_def->crouch_transition_time_seconds;

    //if (estimate_mode != _unit_estimate_1)
    //{
    //    // New crouching implementation for interpolation
    //    // Original functionality would take the biped_datum->crouching_progress float and use it to calculate the camera position->z
    //    // Re-implemented to update on game frame if the unit is in a crouch position
    //    // This is achieved by taking the current frame delta and accumulating it for each player and then dividing it by the biped definitions
    //    // crouch_transition_time_in_seconds to get a progress result between 0 and 1

    //	s_player* player = s_player::get_from_unit_index(biped_index);

    //    // If the current biped_datum is linked to a player run the new crouching interpolation
    //    if (player)
    //    {
    //        // Check if the crouching delta accumulator has already been updated in this current frame by comparing it to c_render::render_time
    //        if (local_user_crouch_last_frame_time[player->user_index] != get_current_render_time())
    //        {
    //            // If the bit flag for crouching is set on the unit_datum add to the accumulator for the local_user else subtract from it
    //            if (TEST_BIT(biped->unit.unit_flags, 23))
    //            {
    //                local_user_crouch_delta_accum[player->user_index] += g_current_game_frame_delta;
    //                if (local_user_crouch_delta_accum[player->user_index] >= crouch_time)
    //                    local_user_crouch_delta_accum[player->user_index] = crouch_time;
    //            }
    //            else
    //            {
    //                local_user_crouch_delta_accum[player->user_index] -= g_current_game_frame_delta;
    //                if (local_user_crouch_delta_accum[player->user_index] <= 0)
    //                    local_user_crouch_delta_accum[player->user_index] = 0;
    //            }
    //            // Update the last frame time for the local_user_index to prevent applying the delta more than once per frame
    //            local_user_crouch_last_frame_time[player->user_index] = get_current_render_time();
    //        }
    //        // Check that the local_user_index accumulator is not greater than crouch time
    //        real32 t_crouch = local_user_crouch_delta_accum[player->user_index] > crouch_time ? crouch_time : local_user_crouch_delta_accum[player->user_index];

    //    	// calculate the final crouching progress
    //        crouching = t_crouch / crouch_time;
    //    }

    //    // If the current biped_datum is not a player run the original crouching behavior
    //    else
    //    {
    //        if (estimate_mode == _unit_estimate_2)
    //        {
    //            crouching = 1.0f;
    //        }
    //        if (!biped_is_running_invisible_crouched_uber_melee(biped_index))
    //        {
    //            crouching = biped->unit.crouching;
    //        }
    //    }
    //}

    if (estimate_mode != _unit_estimate_1)
    {
        if (estimate_mode == _unit_estimate_2)
        {
            crouching = 1.0f;
        }
        if (!biped_is_running_invisible_crouched_uber_melee(biped_index))
        {
            if (!halo_interpolator_interpolate_biped_crouch(biped_index, &crouching))
                crouching = biped->unit.crouching;
        }
    }

    // Camera height calculations
    real32 standing_camera_height = (1.0f - crouching) * biped_def->standing_camera_height;
    real32 crouching_camera_height = biped_def->crouching_camera_height * crouching;
    object_origin->z += (standing_camera_height + crouching_camera_height) * biped->unit.object.scale;

    real_point3d origin_copy = *object_origin;
    real_vector3d forward = biped->unit.aiming_vector;
    real_vector3d up;
    generate_up_vector3d(&forward, &up);
    biped_offset_first_person_camera(&forward, biped_index, &origin_copy, &up);
    real_vector3d origin_vector;
    vector_from_points3d(&origin_copy, object_origin, &origin_vector);
    if (normalize3d(&origin_vector) > 0.0f)
    {
        collision_result collision;
        collision.global_material_index = NONE;

        real_point3d placement;
        real_vector3d direction;
        scale_vector3d(&origin_vector, -0.25f, &direction);

        e_collision_test_flags flags = (e_collision_test_flags)(
            FLAG(_collision_test_bit_26) | 
            FLAG(_collision_test_bit_23) |
            FLAG(_collision_test_bit_15) | 
            FLAG(_collision_test_bit_11) | 
            FLAG(_collision_test_bit_10) | 
            FLAG(_collision_test_bit_5) | 
            FLAG(_collision_test_objects_bit) | 
            FLAG(_collision_test_instanced_geometry_bit) |
            FLAG(_collision_test_structure_bit));

        if (collision_test_vector(flags, object_origin, &direction, NONE, NONE, &collision))
        {
            real32 length = collision.t * 0.9f;
            point_from_line3d(object_origin, &direction, length, &placement);
        }
        else
        {
            placement = collision.point;
        }

        real_vector3d collision_vector;
        real_vector3d position_vector;


        if (collision_test_vector(flags, &placement, &origin_vector, NONE, NONE, &collision)
            && 
            // Secondary check that we only run if the collision_test_vector returns true
            (
                // Populate values
                vector_from_points3d(&collision.point, &placement, &collision_vector),
                vector_from_points3d(&origin_copy, &placement, &position_vector),

                // Actual check here
                dot_product3d(&origin_vector, &position_vector) + 0.05f > dot_product3d(&collision_vector, &origin_vector)
            ))
        {
            point_from_line3d(&collision.point, &origin_vector, -0.05f, object_origin);
        }
        else
        {
            *object_origin = origin_copy;
        }

        datum player_index = player_index_from_unit_index(biped_index);
        if (player_index != NONE)
        {
            s_player* player = (s_player*)datum_get(s_player::get_data(), player_index);
            if (player->user_index != NONE)
            {
                real_point3d point;
                if (halo_interpolator_interpolate_position_backwards(player->user_index, 0, &point))
                {
                    *object_origin = point;
                }
            }
        }
    }

    return;
}

__declspec(naked) void biped_offset_first_person_camera_usercall_to_rewritten(void)
{
    __asm
    {
        // Preserve original 3 registers
        push ebx
        push ecx
        push edx

        // Move values for function call into registers (add 0xC ontop because we pushed 3 registers above)
        mov ebx, [esp + 0xC + 0xC]    // grab camera_up
        mov ecx, [esp + 0x8 + 0xC]    // grab camera_position
        mov edx, [esp + 0x4 + 0xC]    // grab object_index

        // Push values for function call
        push ebx                // push camera_up
        push ecx                // push camera_position
        push edx                // push object_index
        push eax                // push camera_forward

        call biped_offset_first_person_camera

        add esp, 4 * 4

        // Restore original 3 registers
        pop edx
        pop ecx
        pop ebx

        retn
    }
}

void bipeds_apply_patches(void)
{
    PatchCall(Memory::GetAddress(0x13D35A, 0x12C1A9), biped_get_sight_position);
    PatchCall(Memory::GetAddress(0x1422B2, 0x131102), biped_get_sight_position);

    PatchCall(Memory::GetAddress(0x154B88, 0x138E44), biped_offset_first_person_camera_usercall_to_rewritten);
    PatchCall(Memory::GetAddress(0x15602E, 0x13A2EA), biped_offset_first_person_camera_usercall_to_rewritten);
    PatchCall(Memory::GetAddress(0x156C03, 0x13AEBF), biped_offset_first_person_camera_usercall_to_rewritten);
    PatchCall(Memory::GetAddress(0x157392, 0x13B652), biped_offset_first_person_camera_usercall_to_rewritten);
    return;
}
