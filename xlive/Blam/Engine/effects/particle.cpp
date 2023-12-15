#include "stdafx.h"
#include "particle.h"

#include "particle_system.h"
#include "Blam/Engine/game/game_time.h"
#include "Util/Hooks/Hook.h"

s_data_array* get_particle_table()
{
	return *Memory::GetAddress<s_data_array**>(0x4DD08C, 0x5053B4);
}
real32 g_accumulated_particle_delta = 0.f;
real32 g_particle_interpolator_delta = 0.f;
bool particle_interpolator_enabled = false;
s_particle_frame_data_storage* g_particle_frame_storage;
s_particle_interpolation_data_storage* g_previous_particle_interpolation_frame_data;
s_particle_interpolation_data_storage* g_target_particle_interpolation_frame_data;

bool halo_particle_interpolator_enabled()
{
	return particle_interpolator_enabled;
}

void halo_particle_interpolator_set_state(bool state)
{
	particle_interpolator_enabled = state;
}

void halo_particle_interpolator_update()
{
	s_particle_interpolation_data_storage* target_storage_ptr = g_target_particle_interpolation_frame_data;
	g_target_particle_interpolation_frame_data = g_previous_particle_interpolation_frame_data;
	g_previous_particle_interpolation_frame_data = target_storage_ptr;

	s_data_array* particle_table = get_particle_table();
	s_data_iterator<c_particle> particle_it(particle_table);

	while (particle_it.get_next_datum())
	{
		c_particle* particle = particle_it.get_current_datum();
		s_particle_interpolation_data* particle_interp = &g_target_particle_interpolation_frame_data->particle_data[particle_it.get_current_absolute_index()];
		particle_interp->particle_index = particle_it.get_current_datum_index();
		particle_interp->position = particle->m_position;
		particle_interp->velocity = particle->m_velocity;
	}
	g_target_particle_interpolation_frame_data->initialized = true;
}

real_point3d* halo_particle_interpolator_get_interpolated_position(datum particle_index, real_point3d* out_point)
{
	uint16 particle_abs_index = DATUM_INDEX_TO_ABSOLUTE_INDEX(particle_index);
	c_particle* particle = (c_particle*)datum_get(get_particle_table(), particle_index);
	real_point3d particle_position = particle->m_position;

	if (g_previous_particle_interpolation_frame_data->initialized
		&& g_previous_particle_interpolation_frame_data->particle_data[particle_abs_index].particle_index == particle_index)
	{
		if (g_target_particle_interpolation_frame_data->initialized
			&& g_target_particle_interpolation_frame_data->particle_data[particle_abs_index].particle_index == particle_index)
		{
			points_interpolate(
				&g_target_particle_interpolation_frame_data->particle_data[particle_abs_index].position,
				&g_previous_particle_interpolation_frame_data->particle_data[particle_abs_index].position,
				g_particle_interpolator_delta,
				&particle_position
			);
			*out_point = particle_position;
			return out_point;
		}

		*out_point = g_previous_particle_interpolation_frame_data->particle_data[particle_abs_index].position;
		return out_point;
	}

	*out_point = particle_position;
	return out_point;
}

real_vector3d* halo_particle_interpolator_get_interpolated_velocity(datum particle_index, real_vector3d* out_vector)
{
	uint16 particle_abs_index = DATUM_INDEX_TO_ABSOLUTE_INDEX(particle_index);
	c_particle* particle = (c_particle*)datum_get(get_particle_table(), particle_index);
	real_vector3d particle_velocity = particle->m_velocity;

	if (g_previous_particle_interpolation_frame_data->initialized
		&& g_previous_particle_interpolation_frame_data->particle_data[particle_abs_index].particle_index == particle_index)
	{
		if (g_target_particle_interpolation_frame_data->initialized
			&& g_target_particle_interpolation_frame_data->particle_data[particle_abs_index].particle_index == particle_index)
		{
			points_interpolate(
				&g_target_particle_interpolation_frame_data->particle_data[particle_abs_index].velocity,
				&g_previous_particle_interpolation_frame_data->particle_data[particle_abs_index].velocity,
				g_particle_interpolator_delta,
				&particle_velocity
			);
			*out_vector = particle_velocity;
			return out_vector;
		}

		*out_vector = g_previous_particle_interpolation_frame_data->particle_data[particle_abs_index].velocity;
		return out_vector;
	}

	*out_vector = particle_velocity;
	return out_vector;
}

void particle_update(real32 delta)
{

	g_particle_interpolator_delta += delta;

	real32 game_tick_length = time_globals::get()->seconds_per_tick ;
	real32 accumulated_delta = g_particle_interpolator_delta;
	real32 nearest_whole_tick_delta = 0.f;
	if(g_particle_interpolator_delta >= game_tick_length)
	{
		do
		{
			accumulated_delta -= game_tick_length;
			nearest_whole_tick_delta += game_tick_length;
		} while (accumulated_delta >= game_tick_length);

		g_particle_interpolator_delta = accumulated_delta;

		if(nearest_whole_tick_delta > 0.f)
		{
			s_data_iterator<c_particle_system> particle_system_it(get_particle_system_table());

			while (particle_system_it.get_next_datum())
			{
				c_particle_system* particle_system = particle_system_it.get_current_datum();
				if (particle_system->field_4C == NONE && !particle_system->update(nearest_whole_tick_delta))
				{
					if (particle_system->parent_effect_index != NONE)
					{
						particle_syste_remove_from_effects_cache(particle_system->parent_effect_index, particle_system_it.get_current_datum_index());
					}
					c_particle_system::destroy(particle_system_it.get_current_datum_index());
				}
			}
			halo_particle_interpolator_update();
		}
	}
}

void apply_particle_patches()
{
	PatchCall(Memory::GetAddress(0xAA636, 0xB1C5D), particle_update);
}
