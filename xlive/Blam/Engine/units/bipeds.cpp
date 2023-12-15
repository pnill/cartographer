#include "stdafx.h"
#include "bipeds.h"

#include "Blam/Engine/cache/cache_files.h"
#include "Blam/Engine/main/interpolator.h"
#include "Blam/Engine/math/periodic_functions.h"
#include "Blam/Engine/physics/collisions.h"
#include "Blam/Engine/units/biped_definitions.h"

#include "Util/Hooks/Hook.h"

bool __cdecl biped_is_running_invisible_crouched_uber_melee(datum object_index)
{
    return INVOKE(0x152744, 0x136A00, biped_is_running_invisible_crouched_uber_melee, object_index);
}


real_vector2d* biped_build_2d_camera_frame(const real_vector3d* forward, const real_vector3d* up, real_vector2d* forward_out, real_vector2d* up_out)
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
    return perpendicular2d(forward_out, up_out);
}

void biped_offset_first_person_camera(const real_vector3d* camera_forward, datum object_index, real_point3d* camera_position, const real_vector3d* camera_up)
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
            function_result = transition_function_evaluate(5, function_value);
        }
        else
        {
            function_result = transition_function_evaluate(5, 0.0f);
        }
        if (function_result > 0.0f && biped->field_3E0)
        {
            real_vector2d forward_out;
            real_vector2d up_out;
            biped_build_2d_camera_frame(camera_forward, camera_up, &forward_out, &up_out);
            real_matrix4x3* node_matrix = object_get_node_matrix(object_index, 0);
            real_vector2d camera_diff;
            camera_diff.i = camera_position->x - node_matrix->position.x;
            camera_diff.j = camera_position->y - node_matrix->position.y;
            real32 camera_diff_z = camera_position->z - node_matrix->position.z;
            real32 up_product = dot_product2d(&up_out, &camera_diff);
            real32 forward_product = dot_product2d(&forward_out, &camera_diff);

            forward_out.i = biped->vector_3E4.i;
            forward_out.j = biped->vector_3E4.j;
            real32 i = forward_out.i;

            if (forward_out.i <= biped_def->camera_exclusion_distance)
            {
                i = biped_def->camera_exclusion_distance;
            }
            real32 z_diff = biped->vector_3E4.k - camera_diff_z;
            real32 i_diff = i - up_product;

            real32 v2 = (biped_def->camera_forward_movement_scale * i_diff) * function_result;
            real32 v3 = (biped_def->camera_side_movement_scale * (forward_out.j - forward_product)) * function_result;
            camera_position->x = ((forward_out.i * v3) + (up_out.i * v2)) + camera_position->x;
            camera_position->y = ((forward_out.j * v3) + (up_out.j * v2)) + camera_position->y;
            camera_position->z = camera_position->z + ((biped_def->camera_vertical_movement_scale * z_diff) * function_result);
        }
    }
    return;
}

void __cdecl biped_get_sight_position(datum object_index,
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
            real_vector3d direction;
            direction.i = -desired_facing_vector->j;
            direction.j = desired_facing_vector->i;
            direction.k = 0.0f;

            point_from_line3d(object_origin, desired_facing_vector, desired_gun_offset->i, object_origin);
            point_from_line3d(object_origin, &direction, desired_gun_offset->j, object_origin);
            object_origin->z += desired_gun_offset->k;
            return;
        }
    }
    else
    {
        object_get_origin_interpolated(object_index, object_origin);
    }

    biped_datum* biped = (biped_datum*)object_get_fast_unsafe(object_index);
    _biped_definition* biped_def = (_biped_definition*)tag_get_fast(biped->unit.object.tag_definition_index);

    real32 crouching = 0.0f;
    if (estimate_mode != _unit_estimate_1)
    {
        if (estimate_mode == _unit_estimate_2)
        {
            crouching = 1.0f;
        }
        if (!biped_is_running_invisible_crouched_uber_melee(object_index))
        {
            crouching = biped->unit.crouching;
        }
    }

    object_origin->z = ((((1.0f - crouching) * biped_def->standing_camera_height) + (biped_def->crouching_camera_height * crouching)) * biped->unit.object.scale) + object_origin->z;
    real_point3d origin_copy = *object_origin;

    real_vector3d up;
    generate_up_vector3d(&biped->unit.aiming_vector, &up);
    biped_offset_first_person_camera(&biped->unit.aiming_vector, object_index, &origin_copy, &up);
    real_vector3d origin_vector;
    vector_from_points3d(&origin_copy, object_origin, &origin_vector);
    if (normalize3d(&origin_vector) > 0.0)
    {
        collision_result collision;
        real_point3d placement;
        real_vector3d direction;

        scale_vector3d(&origin_vector, -0.25f, &direction);
        if (collision_test_vector((e_collision_test_flags)0x4808C2Du, object_origin, &direction, NONE, NONE, &collision))
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
        vector_from_points3d(&placement, &collision.point, &collision_vector);
        vector_from_points3d(&placement, &origin_copy, &position_vector);


        if (collision_test_vector((e_collision_test_flags)0x4808C2Du, &placement, &origin_vector, NONE, NONE, &collision))
        {
            point_from_line3d(&collision.point, &origin_vector, -0.05f, object_origin);
        }
        else
        {
            *object_origin = origin_copy;
        }
    }

    return;
}

void bipeds_apply_patches(void)
{
    PatchCall(Memory::GetAddress(0x13D35A, 0x12C1A9), biped_get_sight_position);
    PatchCall(Memory::GetAddress(0x1422B2, 0x131102), biped_get_sight_position);
    return;
}