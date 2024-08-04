#include "stdafx.h"
#include "particle.h"

#include "effects.h"
#include "particle_state.h"
#include "physics/collisions.h"


s_data_array* get_particle_table()
{
	return *Memory::GetAddress<s_data_array**>(0x4DD08C, 0x5053B4);
}

void c_particle::adjust_initial_position(
	c_particle_emitter_definition* emitter_def,
	c_particle_emitter* particle_emitter,
	s_particle_state* state,
	c_particle_system* particle_system,
	real32 fraction,
	real32 dt,
	real32 scale)
{
	c_particle_system_definition* particle_system_def = particle_system->get_definition();
	if (particle_system_def->coordinate_system == _particle_coordinate_system_world)
	{
		state->set_particle(this);

		c_flags<e_particle_state_flags, uint32, k_particle_state_flags> flags(0xF80Fu);
		state->state_update(flags);
		emitter_def->initialize_particle(state, this, particle_system);
		m_position.x *= scale;
		m_position.y *= scale;
		m_position.z *= scale;

		const real_vector3d* parent_velocity;
		real_point3d interpolated_position;

		if (particle_system->parent_effect_index == NONE)
		{
			parent_velocity = &global_zero_vector3d;
		}
		else
		{
			parent_velocity = effect_get_velocity(particle_system->parent_effect_index);
		}

		points_interpolate(&particle_emitter->m_previous_position, &particle_emitter->m_position, fraction, &interpolated_position);
		matrix3x3_transform_vector(&particle_emitter->m_matrix, (real_vector3d*)&m_position, (real_vector3d*)&m_position);
		m_position.x += interpolated_position.x;
		m_position.y += interpolated_position.y;
		m_position.z += interpolated_position.z;
		matrix3x3_transform_vector(&particle_emitter->m_matrix, &m_velocity, &m_velocity);

		if (emitter_def->emission_shape == _particle_system_emitter_shape_debris)
		{
			collision_result collision_res;
			collision_res.global_material_index = NONE;

			real_point3d placement;
			real_vector3d direction = { 0.0f, 0.0f, -1.0f };

			placement.x = m_position.x;
			placement.y = m_position.y;
			placement.z = m_position.z + 1.0f;

			if (collision_test_vector(0x800005u, &placement, &direction, NONE, NONE, &collision_res))
			{
				// fog plane??
				point_from_line3d(&collision_res.point, &collision_res.fog_plane.n, 0.005f, &m_position);
				this->m_flags |= 0xAu;
				m_velocity = collision_res.fog_plane.n;
			}
			else
			{
				this->m_flags |= 1u;
			}
		}

		if (TEST_FLAG(particle_system_def->flags, _particle_system_definition_flags_inherit_effect_velocity))
		{
			add_vectors3d(parent_velocity, &m_velocity, &m_velocity);
		}
	}
}