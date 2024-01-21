#include "stdafx.h"
#include "interpolator.h"

#include "Blam/Engine/cutscene/cinematics.h"
#include "Blam/Engine/game/game_time.h"
#include "Blam/Engine/math/real_math.h"
#include "Blam/Engine/units/bipeds.h"

s_frame_data_storage* g_frame_data_storage = NULL;
s_interpolation_data* g_frame_data_intermediate = NULL;
s_interpolation_data* g_previous_interpolation_frame_data = NULL;
s_interpolation_data* g_target_interpolation_frame_data = NULL;

real32 g_interpolator_delta = 0.0f;
bool g_update_in_progress = false;
bool interpolation_enabled = false;
c_static_flags<k_maximum_objects_per_map> g_interpolator_object_updated;
c_static_flags<k_maximum_objects_per_map> g_interpolator_object_interpolation_updated;

void halo_interpolator_initialize()
{
    g_frame_data_storage = (s_frame_data_storage*)VirtualAlloc(0, sizeof(s_frame_data_storage), MEM_COMMIT, PAGE_READWRITE);
    g_frame_data_intermediate = (s_interpolation_data*)VirtualAlloc(0, sizeof(s_interpolation_data), MEM_COMMIT, PAGE_READWRITE);
    g_previous_interpolation_frame_data = &g_frame_data_storage->previous_data;
    g_target_interpolation_frame_data = &g_frame_data_storage->target_data;
}

bool halo_interpolator_is_enabled(void)
{
    return interpolation_enabled;
}

bool halo_interpolator_update_in_progress()
{
    return g_update_in_progress;
}

void halo_interpolator_set_interpolation_enabled(bool enabled)
{
    interpolation_enabled = enabled;
    halo_interpolator_clear_buffers();
    return;
}

real32 halo_interpolator_get_interpolation_time_internal(void)
{
    return MAX(0.0f, MIN(time_globals::get_ticks_fraction_leftover(), 1.0f));
}

void halo_interpolator_update_delta(void)
{
    g_interpolator_delta = halo_interpolator_get_interpolation_time_internal();
    return;
}

real32 halo_interpolator_get_update_delta(void)
{
    return g_interpolator_delta;
}

real32 halo_interpolator_get_interpolation_time(void)
{
    return (g_update_in_progress ? 0.0f : halo_interpolator_get_interpolation_time_internal());
}

void halo_interpolator_clear_data_buffer(s_interpolation_data* interpolation_data)
{
    interpolation_data->initialized = false;
    for (size_t i = 0; i < k_number_of_users; i++)
    {
        for (size_t j = 0; j < k_interpolation_first_person_weapon_slot_count; j++)
        {
            interpolation_data->weapon_data[i][j].node_count = 0;
            interpolation_data->weapon_data[i][j].animation_index = NONE;
        }
    }

    csmemset(interpolation_data->position_data, 0, sizeof(interpolation_data->position_data));
    return;
}

void halo_interpolator_clear_buffers(void)
{
    if (interpolation_enabled)
    {
        g_update_in_progress = false;
        g_interpolator_delta = 0.0f;
        csmemset(g_frame_data_storage, 0, sizeof(s_frame_data_storage));
        csmemset(g_frame_data_intermediate, 0, sizeof(s_interpolation_data));
        halo_interpolator_clear_data_buffer(g_previous_interpolation_frame_data);
        halo_interpolator_clear_data_buffer(g_target_interpolation_frame_data);
        halo_interpolator_clear_data_buffer(g_frame_data_intermediate);
        g_interpolator_object_updated.clear();
        g_interpolator_object_interpolation_updated.clear();
    }
    return;
}

void halo_interpolator_update_begin(void)
{
    s_interpolation_data* p_frame_data;

    if (interpolation_enabled)
    {
        g_update_in_progress = true;
        p_frame_data = g_previous_interpolation_frame_data;
        g_previous_interpolation_frame_data = g_target_interpolation_frame_data;
        g_target_interpolation_frame_data = p_frame_data;
        halo_interpolator_clear_data_buffer(p_frame_data);
        g_interpolator_object_updated.clear();
    }
    return;
}

void halo_interpolator_update_end(void)
{
	if (g_frame_data_storage)
	{
		for (int32 abs_object_index = 0; abs_object_index < k_maximum_objects_per_map; abs_object_index++)
		{
			if (g_target_interpolation_frame_data->object_data[abs_object_index].object_index != DATUM_INDEX_NONE)
			{
				if (!g_interpolator_object_updated.test(abs_object_index))
				{
					if (!g_interpolator_object_interpolation_updated.test(abs_object_index))
					{
						csmemcpy(&g_target_interpolation_frame_data->object_data[abs_object_index],
							&g_previous_interpolation_frame_data->object_data[abs_object_index],
							sizeof(s_object_interpolation_data));
						g_interpolator_object_interpolation_updated.set(abs_object_index, true);
					}
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
    if (g_interpolator_object_interpolation_updated.test(abs_object_index))
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
        result =(object_datum*)object_try_and_get_and_verify_type(object_index, NONE);
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

    if (distance < k_interpolation_distance_cutoff)
    {   
        mass_interpolated = true;
        points_interpolate(
            &g_previous_interpolation_frame_data->object_data[object_index].center_of_mass,
            &g_target_interpolation_frame_data->object_data[object_index].center_of_mass,
            halo_interpolator_get_update_delta(),
            center_of_mass);
    }
    return mass_interpolated;
}

bool halo_interpolator_interpolate_object_node_matrices(datum object_index, real_matrix4x3** node_matrices, int32* out_node_count)
{
    bool result = false;
    uint32 out_abs_object_index;
    if (halo_interpolator_object_can_interpolate(object_index, &out_abs_object_index))
    {
        result = true;

        object_get_node_matrices(object_index, out_node_count);
        if (*out_node_count > 0)
        {
            for (int32 node_index = 0; node_index < *out_node_count; node_index++)
            {
                real_point3d* target_position = &g_target_interpolation_frame_data->object_data[out_abs_object_index].node_matrices[node_index].position;
                real_point3d* previous_position = &g_previous_interpolation_frame_data->object_data[out_abs_object_index].node_matrices[node_index].position;
                
                // Don't interpolate if the distance interpolated is greater than k_interpolation_distance_cutoff
                if (distance_squared3d(previous_position, target_position) >= k_interpolation_distance_cutoff)
                {
                    g_frame_data_intermediate->object_data[out_abs_object_index].node_matrices[node_index] 
                        = g_target_interpolation_frame_data->object_data[out_abs_object_index].node_matrices[node_index];
                }
                else
                {
                    matrix4x3_interpolate(
                        &g_previous_interpolation_frame_data->object_data[out_abs_object_index].node_matrices[node_index],
                        &g_target_interpolation_frame_data->object_data[out_abs_object_index].node_matrices[node_index],
                        halo_interpolator_get_update_delta(),
                        &g_frame_data_intermediate->object_data[out_abs_object_index].node_matrices[node_index]);
                }
            }

            *node_matrices = g_frame_data_intermediate->object_data[out_abs_object_index].node_matrices;
        }
        else
        {
            *node_matrices = NULL;
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
        g_interpolator_object_interpolation_updated.set(abs_object_index, false);
    }
    return;
}

void halo_interpolator_setup_weapon_data(int32 user_index, datum animation_index, int32 weapon_slot, real_matrix4x3* node_matrices, int32 nodes_count)
{
    if (g_frame_data_storage && g_update_in_progress)
    {
        csmemcpy(g_target_interpolation_frame_data->weapon_data[user_index][weapon_slot].nodes, node_matrices, sizeof(real_matrix4x3) * nodes_count);
        g_target_interpolation_frame_data->weapon_data[user_index][weapon_slot].animation_index = animation_index;
        g_target_interpolation_frame_data->weapon_data[user_index][weapon_slot].node_count = nodes_count;
    }
    return;
}

void halo_interpolator_set_target_position_data(int32 user_index, int32 position_index, real_matrix4x3* matrix)
{
    if (g_frame_data_storage && g_update_in_progress)
    {
        g_target_interpolation_frame_data->position_data[user_index][position_index].node = *matrix;
        g_target_interpolation_frame_data->position_data[user_index][position_index].initialized = true;
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
    s_object_header* object_header = object_get_header(object_index);
    uint16 abs_object_index = DATUM_INDEX_TO_ABSOLUTE_INDEX(object_index);
    if (g_frame_data_storage && g_update_in_progress)
    {
        if (abs_object_index < k_maximum_objects_per_map)
        {
            object_datum* object = object_get_fast_unsafe(object_index);
            if (object->object_flags.test(_object_hidden_bit))
            {
                g_target_interpolation_frame_data->object_data[abs_object_index].object_index = NONE;
                // discard previous as well
                // because next time this is executed it won't include this object, until it is unhidden again
                g_previous_interpolation_frame_data->object_data[abs_object_index].object_index = NONE;
            }
            else
            {
                bool object_is_biped = TEST_FLAG(FLAG(object_header->object_type), FLAG(_object_type_biped));
                real32 crouch = 0.0f;
                if (object_is_biped)
                {
                    biped_datum* biped = (biped_datum*)object;
                    crouch = biped->unit.crouching;

                    datum player_index = player_index_from_unit_index(object_index);
                    if (player_index != NONE)
                    {
                        s_player* player = (s_player*)datum_get(s_player::get_data(), player_index);
                        if (player->user_index != NONE)
                        {
                            real_point3d point;
                            // during game update/tick, this will return the current sight position of the biped
                            // ### TODO a proper fix, remove all this backwards camera nonsense
                            // because all nodes/positions are calculated using interpolated values
                            biped_get_sight_position(object_index, _unit_estimate_none, NULL, NULL, NULL, &point);
                            halo_interpolator_interpolate_position_data(player->user_index, 0, &point);
                        }
                    }
                }

                g_target_interpolation_frame_data->object_data[abs_object_index].object_index = object_index;
                csmemcpy(g_target_interpolation_frame_data->object_data[abs_object_index].node_matrices, node_matrices, sizeof(real_matrix4x3) * nodes_count);
                g_target_interpolation_frame_data->object_data[abs_object_index].position = *position;
                g_target_interpolation_frame_data->object_data[abs_object_index].forward = *forward;
                g_target_interpolation_frame_data->object_data[abs_object_index].up = *up;
                g_target_interpolation_frame_data->object_data[abs_object_index].center_of_mass = *center_of_mass;
                g_target_interpolation_frame_data->object_data[abs_object_index].crouch = crouch;
                g_interpolator_object_updated.set(abs_object_index, true);
                g_interpolator_object_interpolation_updated.set(abs_object_index, false);
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
            matrix4x3_interpolate(
                &previous->position_data[user_index][position_index].node, 
                &target->position_data[user_index][position_index].node, 
                halo_interpolator_get_update_delta(), 
                out);
            result = true;
        }
    }
    return result;
}

bool halo_interpolator_interpolate_weapon_node(datum user_index, datum animation_index, int32 node_index, int32 weapon_slot, real_matrix4x3* out_node)
{
	bool result = false;
	if (interpolation_enabled && !cinematic_in_progress())
	{
		if (g_target_interpolation_frame_data->initialized
			&& g_previous_interpolation_frame_data->initialized
			&& user_index != NONE)
		{
			datum target_animation_index = g_target_interpolation_frame_data->weapon_data[user_index][weapon_slot].animation_index;
			datum previous_animation_index = g_previous_interpolation_frame_data->weapon_data[user_index][weapon_slot].animation_index;
			int32 target_node_count = g_target_interpolation_frame_data->weapon_data[user_index][weapon_slot].node_count;
			int32 previous_node_count = g_previous_interpolation_frame_data->weapon_data[user_index][weapon_slot].node_count;
			if (target_animation_index == animation_index && previous_animation_index == target_animation_index)
			{
				if (target_node_count > 0 && target_node_count == previous_node_count)
				{
					matrix4x3_interpolate(
                        &g_previous_interpolation_frame_data->weapon_data[user_index][weapon_slot].nodes[node_index],
						&g_target_interpolation_frame_data->weapon_data[user_index][weapon_slot].nodes[node_index],
						halo_interpolator_get_update_delta(),
						out_node);
					result = true;
				}
			}
		}
	}
	return result;
}

bool halo_interpolator_interpolate_weapon(datum user_index, datum animation_index, int32 weapon_slot, real_matrix4x3** nodes, int32* node_matrices_count)
{
    bool result = false;
    if (interpolation_enabled && !cinematic_in_progress())
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
                    for (int32 node_index = 0; node_index < target_node_count; node_index++)
                    {
                        matrix4x3_interpolate(
                            &g_previous_interpolation_frame_data->weapon_data[user_index][weapon_slot].nodes[node_index],
                            &g_target_interpolation_frame_data->weapon_data[user_index][weapon_slot].nodes[node_index],
                            halo_interpolator_get_update_delta(),
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

bool halo_interpolator_interpolate_object_node_matrix(datum object_index, int16 node_index, real_matrix4x3* out_matrix)
{
    bool result = false;
    uint32 object_absolute_index;
    if (halo_interpolator_object_can_interpolate(object_index, &object_absolute_index))
    {
        real32 distance = distance_squared3d(
            &g_previous_interpolation_frame_data->object_data[object_absolute_index].node_matrices[node_index].position,
            &g_target_interpolation_frame_data->object_data[object_absolute_index].node_matrices[node_index].position);

        if (distance < k_interpolation_distance_cutoff)
        {
            matrix4x3_interpolate(
                &g_previous_interpolation_frame_data->object_data[object_absolute_index].node_matrices[node_index],
                &g_target_interpolation_frame_data->object_data[object_absolute_index].node_matrices[node_index],
                halo_interpolator_get_update_delta(),
                out_matrix);
        }
        result = true;
    }

    return result;
}

bool halo_interpolator_interpolate_object_position(datum object_index, real_point3d* point)
{
    bool interpolate_object = false;
    
    uint32 abs_object_index;
    if (halo_interpolator_object_can_interpolate(object_index, &abs_object_index))
    {
        real32 distance = distance_squared3d(
            &g_previous_interpolation_frame_data->object_data[abs_object_index].position,
            &g_target_interpolation_frame_data->object_data[abs_object_index].position);
        
        if (distance < k_interpolation_distance_cutoff)
        {
            interpolate_object = true;
            points_interpolate(
                &g_previous_interpolation_frame_data->object_data[abs_object_index].position,
                &g_target_interpolation_frame_data->object_data[abs_object_index].position,
                halo_interpolator_get_update_delta(),
                point);
        }
    }
    return interpolate_object;
}

bool halo_interpolator_interpolate_biped_crouch(datum object_index, real32* out_crouch)
{
    bool interpolate_object = false;

    // ### TODO add biped check?
    uint32 abs_object_index;
    if (halo_interpolator_object_can_interpolate(object_index, &abs_object_index))
    {
        real32 distance =
            g_previous_interpolation_frame_data->object_data[abs_object_index].crouch - g_target_interpolation_frame_data->object_data[abs_object_index].crouch;
        distance *= distance;

        if (distance < k_interpolation_distance_cutoff)
        {
            interpolate_object = true;
            scale_interpolate(
                g_previous_interpolation_frame_data->object_data[abs_object_index].crouch,
                g_target_interpolation_frame_data->object_data[abs_object_index].crouch,
                halo_interpolator_get_update_delta(),
                out_crouch);
        }
    }
    return interpolate_object;
}

void halo_interpolator_interpolate_position_data(int32 user_index, int32 position_index, real_point3d* position)
{
    if (g_frame_data_storage)
    {
        if (g_update_in_progress)
        {
            matrix4x3_identity(&g_target_interpolation_frame_data->position_data[user_index][position_index].node);
            g_target_interpolation_frame_data->position_data[user_index][position_index].node.position = *position;
            g_target_interpolation_frame_data->position_data[user_index][position_index].initialized = true;
        }
    }
}

bool halo_interpolator_interpolate_position_backwards(int32 user_index, int32 position_index, real_point3d* position)
{
    bool result = false;

    if (g_frame_data_storage)
    {
        bool initialized = g_target_interpolation_frame_data->position_data[user_index][position_index].initialized
            && g_previous_interpolation_frame_data->position_data[user_index][position_index].initialized;
        if (initialized
            && interpolation_enabled
            && !cinematic_in_progress()
            && !g_update_in_progress)
        {
            real32 distance = distance_squared3d(
                &g_previous_interpolation_frame_data->position_data[user_index][position_index].node.position,
                &g_target_interpolation_frame_data->position_data[user_index][position_index].node.position);

            if (distance < k_interpolation_distance_cutoff)
            {
				points_interpolate(
					&g_previous_interpolation_frame_data->position_data[user_index][position_index].node.position,
					&g_target_interpolation_frame_data->position_data[user_index][position_index].node.position,
					halo_interpolator_get_update_delta(),
					position);
				result = true;
            }
        }
    }

    return result;
}
