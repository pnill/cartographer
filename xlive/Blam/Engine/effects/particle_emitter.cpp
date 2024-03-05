#include "stdafx.h"
#include "particle_emitter.h"

#include "effects.h"
#include "camera/camera.h"
#include "main/interpolator.h"
#include "objects/objects.h"

s_data_array* get_particle_emitter_table(void)
{
	return *Memory::GetAddress<s_data_array**>(0x4DD090, 0x5053B8);
}

void c_particle_emitter::adjust_matrix_and_vector_to_effect_camera(bool use_effect_camera, real_matrix3x3* out_matrix, real_point3d* out_point) const
{
	if (use_effect_camera)
	{
		s_camera* effect_camera = get_effect_camera();
		real_matrix3x3 effect_camera_matrix;
		real_matrix3x3 temp_matrix;

		memcpy(&temp_matrix, &this->matrix, sizeof(real_matrix3x3));
		real_point3d temp_point = this->position;
		matrix3x3_from_forward_and_up(&effect_camera_matrix, &effect_camera->forward, &effect_camera->up);
		matrix3x3_multiply(&effect_camera_matrix, &temp_matrix, &temp_matrix);
		matrix3x3_transform_vector(&effect_camera_matrix, &temp_point, &temp_point);

		temp_point.x += effect_camera->point.x;
		temp_point.y += effect_camera->point.y;
		temp_point.z += effect_camera->point.z;

		*out_matrix = temp_matrix;
		*out_point = temp_point;
	}
	else
	{
		*out_matrix = this->matrix;
		*out_point = this->position;
	}
}

void c_particle_emitter::update_children(
	s_particle_state* particle_state,
	c_particle_system* particle_system,
	c_particle_emitter_definition* emitter_definition,
	real32 a5,
	real32 a6,
	real32 delta,
	real32 a7)
{
	typedef void(__thiscall* update_children_t)(c_particle_emitter*, s_particle_state*, c_particle_system*, c_particle_emitter_definition*, real32, real32, real32, real32);
	auto function = Memory::GetAddress<update_children_t>(0x104FFE);
	function(this, particle_state, particle_system, emitter_definition, a5, a6, delta, a7);
}

void c_particle_emitter::update_particles(
	c_particle_system* particle_system,
	c_particle_location* particle_location,
	c_particle_system_definition* particle_system_definition,
	c_particle_emitter_definition* particle_emitter_definition,
	real32 delta_time,
	real_vector3d* particle_location_position)
{
	typedef void(__thiscall* update_particles_t)(c_particle_emitter*, c_particle_system*, c_particle_location*, c_particle_system_definition*, c_particle_emitter_definition*, real32, real_vector3d*);
	auto function = Memory::GetAddress<update_particles_t>(0x104C91);
	function(this, particle_system, particle_location, particle_system_definition, particle_emitter_definition, delta_time, particle_location_position);
}

void c_particle_emitter::destroy_particles()
{
	typedef void(__thiscall* destroy_particles_t)(c_particle_emitter*);
	auto function = Memory::GetAddress<destroy_particles_t>(0x104AF5);
	function(this);
}

void c_particle_emitter::calc_matrix(
	c_particle_emitter* _this,
	real32 delta,
	c_particle_system* particle_system,
	c_particle_emitter_definition* emitter_definition,
	s_particle_state* particle_state,
	real_matrix4x3* in_martix,
	real32 alpha)
{
	c_particle_system_definition* particle_system_definition = particle_system->get_particle_system_definition();
	real32 scale = 1.0f;
	effect_datum* effect = (effect_datum*)datum_get(get_effects_table(), particle_system->parent_effect_index);
	object_datum* object = (object_datum*)object_get_fast_unsafe(effect->multi_purpose_origin_index);

	_this->previous_position = _this->position;
	if (in_martix)
	{
		// Use sky scale if the particle is in the sky
		scale = (particle_system->get_in_sky() ? in_martix->scale : scale);

		_this->matrix = in_martix->vectors;
		_this->position = in_martix->position;
		scale_vector3d(&_this->position, scale, &_this->position);

		real_vector3d translated_vector;
		matrix3x3_transform_vector(&_this->matrix, &emitter_definition->translational_offset, &translated_vector);

		if (fabs(emitter_definition->relative_direction.yaw) >= k_real_math_epsilon ||
			fabs(emitter_definition->relative_direction.pitch) >= k_real_math_epsilon)
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
		bool particle_system_flag_check = TEST_BIT(particle_system->flags, _particle_system_bit_11) && TEST_BIT(particle_system->flags, _particle_system_bit_9);
		bool particle_system_definition_flag_check = (particle_system_definition->flags >> 7) & 1;

		real32 emitter_time_a = 0.f;
		real32 emitter_time_b = 0.f;

		if (particle_system_flag_check && particle_system_definition_flag_check)
		{
			emitter_time_a = 1.f / _this->emission_time;
		}
		else
		{
			emitter_time_b = 1.f;
			delta = 0.f;
		}

		while (_this->emission_time + k_real_math_epsilon >= 1.0f)
		{
			_this->emission_time -= 1.0f;

			_this->update_children(particle_state, particle_system, emitter_definition, alpha, emitter_time_b, delta, scale);
			emitter_time_b += emitter_time_a;
		}
	}
	return;
}

__declspec(naked) void c_particle_emitter__calc_matrix()
{
	__asm
	{
		pop eax
		push ecx
		push eax
		jmp c_particle_emitter::calc_matrix
	}
}
void particle_emitter_apply_patches(void)
{
	PatchCall(Memory::GetAddress(0x106423), c_particle_emitter__calc_matrix);
	return;
}