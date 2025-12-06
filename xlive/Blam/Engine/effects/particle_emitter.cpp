#include "stdafx.h"
#include "particle_emitter.h"

#include "camera/camera.h"
#include "render/render_cameras.h"

data_array* get_particle_emitter_table()
{
	return *Memory::GetAddress<data_array**>(0x4DD090, 0x5053B8);
}

void c_particle_emitter::adjust_matrix_and_vector_to_effect_camera(bool use_effect_camera, real_matrix3x3* out_matrix, real_vector3d* out_vector) const
{
	if (use_effect_camera)
	{
		render_camera* effect_camera = get_effect_camera();
		real_matrix3x3 effect_camera_matrix;

		*out_matrix = this->m_matrix;
		*out_vector = { this->m_position.x, this->m_position.y, this->m_position.z };
		matrix3x3_from_forward_and_up(&effect_camera_matrix, &effect_camera->forward, &effect_camera->up);
		matrix3x3_multiply(&effect_camera_matrix, out_matrix, out_matrix);
		matrix3x3_transform_vector(&effect_camera_matrix, out_vector, out_vector);

		out_vector->i += effect_camera->point.x;
		out_vector->j += effect_camera->point.y;
		out_vector->k += effect_camera->point.z;
	}
	else
	{
		*out_matrix = this->m_matrix;
		*out_vector = { this->m_position.x, this->m_position.y, this->m_position.z };
	}
}

void c_particle_emitter::spawn_particle(
	s_particle_state* particle_state,
	c_particle_system* particle_system,
	c_particle_emitter_definition* emitter_definition,
	real32 a5,
	real32 a6,
	real32 delta,
	real32 a7)
{
	typedef void(__thiscall* spawn_particle_t)(c_particle_emitter*, s_particle_state*, c_particle_system*, c_particle_emitter_definition*, real32, real32, real32, real32);
	INVOKE_BY_TYPE(0x104FFE, 0x0, spawn_particle_t, this, particle_state, particle_system, emitter_definition, a5, a6, delta, a7);
}

void c_particle_emitter::pulse(
	c_particle_emitter* _this,
	real32 delta,
	c_particle_system* particle_system,
	c_particle_emitter_definition* emitter_definition,
	s_particle_state* particle_state,
	const real_matrix4x3* matrix,
	real32 alpha)
{
	c_particle_system_definition* particle_system_definition = particle_system->get_definition();
	real32 scale = 1.0f;

	_this->m_previous_position = _this->m_position;
	if (matrix)
	{
		// Use sky scale if the particle is in the sky
		if (particle_system->get_in_sky())
		{
			scale = matrix->scale;
		}

		calc_matrix(emitter_definition, particle_system, scale, matrix);
	}

	if (!particle_system_definition->system_is_looping_particle() || _this->m_particle_index == NONE)
	{
		_this->particles_to_emit = emitter_definition->get_particle_emissions_per_tick(particle_state) * delta + _this->particles_to_emit;
	}
	if (_this->particles_to_emit + k_real_epsilon >= 1.0f)
	{
		real32 spread = 0.f;
		real32 accumulator = 0.f;

		if (particle_system->get_ever_pulsed_or_frame_updated() && particle_system_definition->spread_between_ticks())
		{
			spread = 1.f / _this->particles_to_emit;
		}
		else
		{
			accumulator = 1.f;
			delta = 0.f;
		}

		// while there are multiple particles to emit, spawn them
		while (_this->particles_to_emit + k_real_epsilon >= 1.0f)
		{
			// subtract 1 particle
			_this->particles_to_emit -= 1.0f;
			_this->spawn_particle(particle_state, particle_system, emitter_definition, alpha, accumulator, delta, scale);
			accumulator += spread;
		}
	}
	return;
}

void c_particle_emitter::calc_matrix(
	c_particle_emitter_definition* definition, 
	c_particle_system* particle_system, 
	real32 scale, 
	const real_matrix4x3* matrix
)
{
	this->m_matrix = matrix->vectors;
	this->m_position = matrix->position;

	real_vector3d translated_vector;
	matrix3x3_transform_vector(&this->m_matrix, &definition->translational_offset, &translated_vector);

	if (::abs(definition->relative_direction.yaw) >= k_real_epsilon ||
		::abs(definition->relative_direction.pitch) >= k_real_epsilon)
	{
		real_matrix3x3 rotations_matrix;
		matrix3x3_from_angles(&rotations_matrix, definition->relative_direction.yaw, definition->relative_direction.pitch, 0.0f);
		matrix3x3_multiply(&this->m_matrix, &rotations_matrix, &this->m_matrix);
	}

	add_vectors3d((real_vector3d*)&m_position, &translated_vector, (real_vector3d*)&m_position);
	return;
}

