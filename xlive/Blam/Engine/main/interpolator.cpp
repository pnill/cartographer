#include "stdafx.h"
#include "interpolator.h"

#include "Blam/Engine/cutscene/cinematics.h"
#include "Blam/Engine/game/game_time.h"
#include "Blam/Engine/math/real_math.h"

s_frame_data_storage* g_frame_data_storage = NULL;
s_interpolation_data* g_frame_data_intermediate = NULL;
s_interpolation_data* g_previous_interpolation_frame_data = NULL;
s_interpolation_data* g_target_interpolation_frame_data = NULL;

real32 g_interpolator_delta = 0.0f;
bool g_update_in_progress = false;
bool interpolation_enabled = false;
c_static_flags<k_maximum_objects_per_map> g_interpolator_object_updated;
c_static_flags<k_maximum_objects_per_map> g_interpolator_object_has_updated;

bool is_interpolation_enabled(void)
{
    return interpolation_enabled;
}

void set_interpolation_enabled(bool enabled)
{
    interpolation_enabled = enabled;
    return;
}

float get_interpolation_time(void)
{
    return MAX(0.0f, MIN(time_globals::get_ticks_fraction_leftover(), 1.0f));
}

void halo_interpolator_update_delta(void)
{
    g_interpolator_delta = get_interpolation_time();
    return;
}

void interpolation_clear_data_buffer(s_interpolation_data* interpolation_data)
{
    interpolation_data->initialized = 0;
    for (size_t i = 0; i < k_number_of_users; i++)
    {
        for (size_t j = 0; j < k_interpolation_first_person_weapon_slot_count; j++)
        {
            interpolation_data->weapon_data[i][j].animation_index = NONE;
            interpolation_data->weapon_data[i][j].node_count = 0;
        }
    }
    memset(interpolation_data->position_data, 0, sizeof(s_position_interpolation_data));
    return;
}

void halo_interpolator_clear_buffers(void)
{
    if (interpolation_enabled)
    {
        memset(g_frame_data_storage, 0, sizeof(s_frame_data_storage));
        memset(g_frame_data_intermediate, 0, sizeof(s_interpolation_data));
        g_interpolator_object_updated.clear();
        g_interpolator_object_has_updated.clear();
        interpolation_clear_data_buffer(g_previous_interpolation_frame_data);
        interpolation_clear_data_buffer(g_target_interpolation_frame_data);
        interpolation_clear_data_buffer(g_frame_data_intermediate);
        g_interpolator_delta = 0.0f;
        g_update_in_progress = false;
    }
    return;
}

void halo_interpolator_update_begin(void)
{
    s_interpolation_data* p_frame_data;

    if (interpolation_enabled)
    {
        p_frame_data = g_previous_interpolation_frame_data;
        g_update_in_progress = true;
        g_previous_interpolation_frame_data = g_target_interpolation_frame_data;
        g_target_interpolation_frame_data = p_frame_data;
        interpolation_clear_data_buffer(p_frame_data);
        g_interpolator_object_updated.clear();
    }
    return;
}

void halo_interpolator_update_end(void)
{
    if (g_frame_data_storage)
    {
        for (uint32 abs_object_index = 0; abs_object_index < k_maximum_objects_per_map; abs_object_index++)
        {
            if (!g_interpolator_object_updated.test(abs_object_index))
            {
                if (!g_interpolator_object_has_updated.test(abs_object_index))
                {
                    memcpy(&g_target_interpolation_frame_data->object_data[abs_object_index], 
                        &g_previous_interpolation_frame_data->object_data[abs_object_index], 
                        sizeof(s_object_interpolation_data));
                    g_interpolator_object_has_updated.set(abs_object_index, true);
                }
            }
        }

        g_update_in_progress = false;
        g_target_interpolation_frame_data->initialized = true;
    }
    return;
}

object_datum* halo_interpolator_object_can_interpolate(datum object_index, uint32* out_abs_object_index)
{
    *out_abs_object_index = NONE;
    uint16 abs_object_index = DATUM_INDEX_TO_ABSOLUTE_INDEX(object_index);
    if (!interpolation_enabled || cinematic_in_progress())
        return NULL;
    if (g_update_in_progress)
        return NULL;
    if (!g_target_interpolation_frame_data->initialized)
        return NULL;
    if (!g_previous_interpolation_frame_data->initialized)
        return NULL;
    if (abs_object_index >= k_maximum_objects_per_map)
        return NULL;
    if (g_interpolator_object_has_updated.test(abs_object_index))
        return NULL;

    *out_abs_object_index = abs_object_index;

    object_datum* result;
    if (g_previous_interpolation_frame_data->object_data[abs_object_index].object_index != object_index
        || g_target_interpolation_frame_data->object_data[abs_object_index].object_index != object_index)
    {
        result = NULL;
    }
    else
    {
        result =(object_datum*)object_try_and_get_and_verify_type(object_index, -1);
    }
    return result;
}

bool halo_interpolator_interpolate_center_of_mass(datum object_datum, real_point3d* center_of_mass)
{
    bool mass_interpolated = false;
    uint32 object_index;

    if (!halo_interpolator_object_can_interpolate(object_datum, &object_index)) { return 0; }

    float distance = distance_squared3d(
        &g_previous_interpolation_frame_data->object_data[object_index].center_of_mass,
        &g_target_interpolation_frame_data->object_data[object_index].center_of_mass);

    if (distance < 900.0f)
    {
        mass_interpolated = true;
        points_interpolate(
            &g_previous_interpolation_frame_data->object_data[object_index].center_of_mass,
            &g_target_interpolation_frame_data->object_data[object_index].center_of_mass,
            g_interpolator_delta,
            center_of_mass);
    }
    return mass_interpolated;
}

real_point3d* object_get_center_of_mass_interpolated(datum object_datum, real_point3d* center_of_mass)
{
    real_point3d* result;
    if (halo_interpolator_interpolate_center_of_mass(object_datum, center_of_mass))
    {
        result = center_of_mass;
    }
    else
    {
        result = object_get_center_of_mass(object_datum, center_of_mass);
    }
    return result;
}

bool halo_interpolator_interpolate_object_node_matrices(datum object_index, real_matrix4x3** node_matrices, int32* out_node_count)
{
    bool result = false;
    uint32 out_abs_object_index;
    if (halo_interpolator_object_can_interpolate(object_index, &out_abs_object_index))
    {
        result = true;

        object_get_node_matrices(object_index, out_node_count);
        (*node_matrices) = g_frame_data_intermediate->object_data[out_abs_object_index].node_matrices;
        if (*out_node_count > 0)
        {
            for (uint32 node_index = 0; node_index < *out_node_count; node_index++)
            {
                real_point3d* target_position = &g_target_interpolation_frame_data->object_data[out_abs_object_index].node_matrices[node_index].position;
                real_point3d* previous_position = &g_previous_interpolation_frame_data->object_data[out_abs_object_index].node_matrices[node_index].position;
                
                real_point3d position_difference;
                subtract_vector3d(target_position, previous_position, &position_difference);

                // Don't interpolate if the distance interpolated is greater than k_interpolation_distance_cutoff
                if (magnitude3d_squared(&position_difference) >= k_interpolation_distance_cutoff)
                {
                    (*node_matrices)[node_index] = g_target_interpolation_frame_data->object_data[out_abs_object_index].node_matrices[node_index];
                }
                else
                {
                    matrix4x3_interpolate(
                        &g_previous_interpolation_frame_data->object_data[out_abs_object_index].node_matrices[node_index],
                        &g_target_interpolation_frame_data->object_data[out_abs_object_index].node_matrices[node_index],
                        g_interpolator_delta,
                        &(*node_matrices)[node_index]);
                }
            }
        }
    }
    return result;
}

void halo_interpolator_setup_new_object(datum object_index)
{
    uint16 abs_object_index = DATUM_INDEX_TO_ABSOLUTE_INDEX(object_index);

    if (interpolation_enabled && g_update_in_progress && abs_object_index < k_maximum_objects_per_map)
    {
        if (g_previous_interpolation_frame_data->object_data[abs_object_index].object_index == object_index)
        {
            g_previous_interpolation_frame_data->object_data[abs_object_index].object_index = NONE;
        }

        g_interpolator_object_updated.set(abs_object_index, false);
        g_interpolator_object_has_updated.set(abs_object_index, false);
    }
    return;
}

void halo_interpolator_setup_weapon_data(int32 user_index, datum animation_index, int32 weapon_slot, real_matrix4x3* node_matrices, int32 nodes_count)
{
    if (g_frame_data_storage && g_update_in_progress)
    {
        memcpy(g_target_interpolation_frame_data->weapon_data[user_index][weapon_slot].nodes, node_matrices, sizeof(real_matrix4x3) * nodes_count);
        g_target_interpolation_frame_data->weapon_data[user_index][weapon_slot].animation_index = animation_index;
        g_target_interpolation_frame_data->weapon_data[user_index][weapon_slot].node_count = nodes_count;
    }
    return;
}

void halo_interpolator_set_target_position_data(int32 user_index, int32 position_index, real_matrix4x3* matrix)
{
    if (g_frame_data_storage)
    {
        if (g_update_in_progress)
        {
            g_target_interpolation_frame_data->position_data[user_index][position_index].node = *matrix;
            g_target_interpolation_frame_data->position_data[user_index][position_index].initialized = true;
        }
    }
    return;
}

void halo_interpolator_object_populate_interpolation_data(
    datum object_index,
    const real_matrix4x3* node_matrices,
    int32 nodes_count,
    const real_point3d* position,
    const real_vector3d* forward,
    const real_vector3d* up,
    const real_point3d* center_of_mass)
{
    object_datum* object = object_get_fast_unsafe(object_index);
    uint16 abs_object_index = DATUM_INDEX_TO_ABSOLUTE_INDEX(object_index);
    if (g_frame_data_storage && g_update_in_progress)
    {
        if (abs_object_index < k_maximum_objects_per_map)
        {
            object_datum* object = object_get_fast_unsafe(object_index);
            if (object->object_flags.test(_object_hidden_bit))
            {
                g_target_interpolation_frame_data->object_data[abs_object_index].object_index = NONE;
            }
            else
            {
                memcpy(g_target_interpolation_frame_data->object_data[abs_object_index].node_matrices, node_matrices, sizeof(real_matrix4x3) * nodes_count);
                g_target_interpolation_frame_data->object_data[abs_object_index].object_index = object_index;
                g_target_interpolation_frame_data->object_data[abs_object_index].position = *position;
                g_target_interpolation_frame_data->object_data[abs_object_index].forward = *forward;
                g_target_interpolation_frame_data->object_data[abs_object_index].up = *up;
                g_target_interpolation_frame_data->object_data[abs_object_index].center_of_mass = *center_of_mass;
                g_interpolator_object_updated.set(abs_object_index, true);
                g_interpolator_object_has_updated.set(abs_object_index, false);
            }
        }
    }
    return;
}

bool halo_interpolator_get_interpolated_matrix_from_user_index(int32 user_index, int32 position_index, real_matrix4x3* out)
{
    bool result = false;

    if (g_frame_data_storage)
    {
        s_interpolation_data* target = g_target_interpolation_frame_data;
        s_interpolation_data* previous = g_previous_interpolation_frame_data;
        bool target_initialized = target->position_data[user_index][position_index].initialized;
        if (previous->position_data[user_index][position_index].initialized == target_initialized
            && target_initialized
            && interpolation_enabled
            && !cinematic_in_progress()
            && !g_update_in_progress)
        {
            matrix4x3_interpolate(&previous->position_data[user_index][position_index].node, &target->position_data[user_index][position_index].node, g_interpolator_delta, out);
            result = true;
        }
    }
    return result;
}

bool halo_interpolator_interpolate_weapon(datum user_index, datum animation_index, int32 weapon_slot, real_matrix4x3** nodes, int32* node_matrices_count)
{
    bool result = false;
    if (interpolation_enabled || !cinematic_in_progress())
    {
        if (g_target_interpolation_frame_data->initialized
            && g_previous_interpolation_frame_data->initialized
            && user_index != NONE)
        {
            datum target_animation_index = g_target_interpolation_frame_data->weapon_data[user_index][weapon_slot].animation_index;
            datum previous_animation_index = g_previous_interpolation_frame_data->weapon_data[user_index][weapon_slot].animation_index;
            if (target_animation_index == animation_index && previous_animation_index == target_animation_index)
            {
                int32 target_node_count = g_target_interpolation_frame_data->weapon_data[user_index][weapon_slot].node_count;
                int32 previous_node_count = g_previous_interpolation_frame_data->weapon_data[user_index][weapon_slot].node_count;
                if (target_node_count > 0 && target_node_count == previous_node_count)
                {
                    *nodes = g_frame_data_intermediate->weapon_data[user_index][weapon_slot].nodes;
                    for (uint32 node_index = 0; node_index < target_node_count; node_index++)
                    {
                        matrix4x3_interpolate(&g_previous_interpolation_frame_data->weapon_data[user_index][weapon_slot].nodes[node_index],
                            &g_target_interpolation_frame_data->weapon_data[user_index][weapon_slot].nodes[node_index],
                            g_interpolator_delta,
                            &g_frame_data_intermediate->weapon_data[user_index][weapon_slot].nodes[node_index]);
                    }
                    *node_matrices_count = target_node_count;
                    result = true;
                }
            }
        }
    }

    return result;
}