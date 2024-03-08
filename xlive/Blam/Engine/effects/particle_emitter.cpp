#include "stdafx.h"
#include "particle_emitter.h"

#include "effects.h"
#include "particle_state.h"

#include "objects/objects.h"

#include "Blam/Engine/camera/camera.h"

s_data_array* get_particle_emitter_table()
{
	return *Memory::GetAddress<s_data_array**>(0x4DD090, 0x5053B8);
}

void c_particle_emitter::adjust_matrix_and_vector_to_effect_camera(bool use_effect_camera, real_matrix3x3* out_matrix, real_point3d* out_point) const
{
	if (use_effect_camera)
	{
		s_camera* effect_camera = get_effect_camera();
		real_matrix3x3 effect_camera_matrix;
		real_matrix3x3 m1;

		csmemcpy(&m1, &this->matrix, sizeof(real_matrix3x3));
		real_point3d temp_point = this->position;
		matrix3x3_from_forward_and_up(&effect_camera_matrix, &effect_camera->forward, &effect_camera->up);
		matrix3x3_multiply(&effect_camera_matrix, &m1, &m1);
		matrix3x3_transform_vector(&effect_camera_matrix, &temp_point, &temp_point);

		temp_point.x += effect_camera->point.x;
		temp_point.y += effect_camera->point.y;
		temp_point.z += effect_camera->point.z;

		*out_matrix = m1;
		*out_point = temp_point;
	}
	else
	{
		*out_matrix = this->matrix;
		*out_point = this->position;
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
	c_particle_definition_interface* particle_system_interface = particle_system_definition->get_particle_system_interface();
	real32 scale = 1.0f;
	effect_datum* effect = (effect_datum*)datum_get(get_effects_table(), particle_system->parent_effect_index);
	object_datum* object = (object_datum*)object_get_fast_unsafe(effect->multi_purpose_origin_index);

	_this->previous_position = _this->position;
	if (matrix)
	{
		// Use sky scale if the particle is in the sky
		if (particle_system->get_in_sky())
		{
			scale = matrix->scale;
		}

		_this->matrix = matrix->vectors;
		_this->position = matrix->position;
		scale_vector3d(&_this->position, scale, &_this->position);

		real_vector3d translated_vector;
		matrix3x3_transform_vector(&_this->matrix, &emitter_definition->translational_offset, &translated_vector);

		if (abs(emitter_definition->relative_direction.yaw) >= k_real_math_epsilon ||
			abs(emitter_definition->relative_direction.pitch) >= k_real_math_epsilon)
		{
			real_matrix3x3 rotations_matrix;
			matrix3x3_from_angles(&rotations_matrix, emitter_definition->relative_direction.yaw, emitter_definition->relative_direction.pitch, 0.0f);
			matrix3x3_multiply(&_this->matrix, &rotations_matrix, &_this->matrix);
		}

		add_vectors3d(&_this->position, &translated_vector, &_this->position);
	}

	if (!particle_system_definition->system_is_cinematic() || _this->particle_index == NONE)
	{
		_this->emission_time = emitter_definition->get_particle_emissions_per_tick(particle_state) * delta + _this->emission_time;
	}
	if (_this->emission_time + k_real_math_epsilon >= 1.0f)
	{
		bool particle_system_flag_check = particle_system->flags.test(_particle_system_bit_11) && particle_system->flags.test(_particle_system_bit_9);
		bool particle_system_definition_flag_check = TEST_BIT(particle_system_definition->flags, 7);

		real32 time_slice = 0.f;
		real32 accumulator = 0.f;

		if (particle_system_flag_check && particle_system_definition_flag_check)
		{
			time_slice = 1.f / _this->emission_time;
		}
		else
		{
			accumulator = 1.f;
			delta = 0.f;
		}

		while (_this->emission_time + k_real_math_epsilon >= 1.0f)
		{
			_this->emission_time -= 1.0f;
			_this->spawn_particle(particle_state, particle_system, emitter_definition, alpha, accumulator, delta, scale);
			accumulator += time_slice;
		}
	}
	return;
}

// ### TODO de-duplication with c_particle_emitter::pulse
void c_particle_emitter::adjust_initial_particle_position(
	c_particle_system* particle_system,
	c_particle_emitter_definition* emitter_definition,
	s_particle_state* particle_state,
	const real_matrix4x3* matrix,
	real32 dt
)
{
	c_particle_system_definition* particle_system_definition = particle_system->get_definition();
	real32 scale = 1.0f;
	effect_datum* effect = (effect_datum*)datum_get(get_effects_table(), particle_system->parent_effect_index);
	object_datum* object = (object_datum*)object_get_fast_unsafe(effect->multi_purpose_origin_index);

	this->previous_position = this->position;
	if (matrix)
	{
		// Use sky scale if the particle is in the sky
		if (particle_system->get_in_sky())
		{
			scale = matrix->scale;
		}

		this->matrix = matrix->vectors;
		this->position = matrix->position;
		scale_vector3d(&this->position, scale, &this->position);

		real_vector3d translated_vector;
		matrix3x3_transform_vector(&this->matrix, &emitter_definition->translational_offset, &translated_vector);

		if (abs(emitter_definition->relative_direction.yaw) >= k_real_math_epsilon ||
			abs(emitter_definition->relative_direction.pitch) >= k_real_math_epsilon)
		{
			real_matrix3x3 rotations_matrix;
			matrix3x3_from_angles(&rotations_matrix, emitter_definition->relative_direction.yaw, emitter_definition->relative_direction.pitch, 0.0f);
			matrix3x3_multiply(&this->matrix, &rotations_matrix, &this->matrix);
		}

		add_vectors3d(&this->position, &translated_vector, &this->position);
	}

	datum particle_index = this->particle_index;
	while (particle_index != NONE)
	{
		c_particle_definition_interface* particle_system_interface = particle_system_definition->get_particle_system_interface();
		c_particle* particle = (c_particle*)datum_get(get_particle_table(), particle_index);

		if (!particle_system_definition->system_is_cinematic() || particle->next_particle == NONE)
		{
			this->emission_time = emitter_definition->get_particle_emissions_per_tick(particle_state) * dt + this->emission_time;
		}

		if (this->emission_time + k_real_math_epsilon >= 1.0f)
		{
			bool particle_system_flag_check = particle_system->flags.test(_particle_system_bit_11) && particle_system->flags.test(_particle_system_bit_9);
			bool particle_system_definition_flag_check = TEST_BIT(particle_system_definition->flags, 7);

			real32 time_slice = 0.f;
			real32 accumulator = 0.f;

			if (particle_system_flag_check && particle_system_definition_flag_check)
			{
				time_slice = 1.f / this->emission_time;
			}
			else
			{
				accumulator = 1.f;
				dt = 0.f;
			}

			while (this->emission_time + k_real_math_epsilon >= 1.0f)
			{
				this->emission_time -= 1.0f;
				particle->adjust_initial_position(emitter_definition, this, particle_state, particle_system, accumulator, dt, scale);
				accumulator += time_slice;
			}
		}

		particle_index = particle->next_particle;
	}
	
	return;
}

