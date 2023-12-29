#include "stdafx.h"
#include "particle_update.h"

#include "particle_state.h"
#include "Blam/Engine/render/render_sky.h"
#include "Util/Hooks/Hook.h"




typedef void(__cdecl* particle_system_update_particle_position_and_velocity_t)(
	c_particle_system* particle_system,
	c_particle_location* particle_location,
	s_particle_state* particle_state,
	update_particle_position_and_velocity_callback_t callback,
	c_particle_definition** particle_definition);

particle_system_update_particle_position_and_velocity_t p_particle_system_update_particle_position_and_velocity;
void __cdecl particle_system_update_particle_position_and_velocity(
	c_particle_system* particle_system,
	c_particle_location* particle_location,
	s_particle_state* particle_state,
	update_particle_position_and_velocity_callback_t callback,
	c_particle_definition** particle_definition)
{
	//p_particle_system_update_particle_position_and_velocity(particle_system, particle_location, unk_particle_structure, callback, particle_definition);
	c_particle_system_definition* particle_system_definition = particle_system->get_particle_system_definition();
	if (particle_system != particle_state->particle_system)
	{
		particle_state->flags = (e_particle_state_flags)(particle_state->flags & ~(_particle_update_flag_10 | _particle_update_flag_9 | _particle_update_flag_6 | _particle_update_flag_5 | _particle_update_flag_4));
		particle_state->particle_system = particle_system;
	}
	if (particle_location != particle_state->particle_location)
	{
		particle_state->flags = (e_particle_state_flags)(particle_state->flags & ~(_particle_update_flag_16 | _particle_update_flag_13 | _particle_update_flag_12 | _particle_update_flag_7));
		particle_state->particle_location = particle_location;
	}

	datum current_particle_emitter_index = particle_location->particle_emitter_index;
	if (current_particle_emitter_index != NONE)
	{
		s_data_array* particle_emitter_table = get_particle_emitter_table();
		s_data_array* particle_table = get_particle_table();
		datum current_emitter_index = 0;
		do
		{
			c_particle_emitter* particle_emitter = (c_particle_emitter*)datum_get(particle_emitter_table, DATUM_INDEX_TO_ABSOLUTE_INDEX(current_particle_emitter_index));
			c_particle_emitter_definition* emitter_definition = particle_system_definition->emitters[current_emitter_index];
			datum current_particle_index = particle_emitter->particle_index;

			if (current_particle_index != NONE)
			{
				real32 particle_scale = 1.0f;
				real32 sky_render_model_scale = 1.0f;
				real_matrix3x3 particle_camera_matrix{};
				real_point3d particle_camera_position{};
				real_argb_color particle_color;

				if (TEST_BIT(particle_system->flags, _particle_system_scale_with_sky_render_model_bit))
					sky_render_model_scale = get_current_sky_render_model_scale();

				

				if (particle_system_definition->coordinate_system != _particle_coordinate_system_world)
					particle_emitter->adjust_matrix_and_vector_to_effect_camera(particle_location->parent_effect_has_bit_15_set, &particle_camera_matrix, &particle_camera_position);

				particle_state_update
				(
					emitter_definition->runtime_flags_2 & 0x107F0 & ~particle_state->flags,
					particle_state->particle_system,
					particle_state->particle_location,
					particle_state->particle,
					particle_state
				);

				emitter_definition->get_emitter_particle_color(particle_state, &particle_color);

				if (!(TEST_BIT(emitter_definition->runtime_flags, 4)))
					particle_scale = emitter_definition->particle_size.get_result(particle_state) * sky_render_model_scale;

				int32 emitter_runtime_flag_check = emitter_definition->runtime_flags_2 & 0xF80F;

				do
				{
					c_particle* particle = (c_particle*)datum_get(particle_table, DATUM_INDEX_TO_ABSOLUTE_INDEX(current_particle_index));
					if(particle != particle_state->particle)
					{
						particle_state->flags = (e_particle_state_flags)(particle_state->flags & ~0xF80Fu);
						particle_state->particle = particle;
					}

					real_point3d particle_position;
					real_vector3d particle_velocity;
					halo_particle_interpolator_get_interpolated_position(current_particle_index, &particle_position);
					halo_interpolator_particle_get_interpolated_velocity(current_particle_index, &particle_velocity);

					particle_state_update
					(
						emitter_runtime_flag_check & ~particle_state->flags,
						particle_state->particle_system,
						particle_state->particle_location,
						particle_state->particle,
						particle_state
					);

					emitter_definition->get_emitter_particle_inverse_color(particle_state, &particle_color);

					if (!(TEST_BIT(emitter_definition->runtime_flags, 4)))
						particle_scale = emitter_definition->particle_size.get_result(particle_state) * sky_render_model_scale;

					if(particle_system_definition->coordinate_system != _particle_coordinate_system_world)
					{
						matrix3x3_transform_vector(&particle_camera_matrix, &particle_position, &particle_position);
						particle_position.x += particle_camera_position.x;
						particle_position.y += particle_camera_position.y;
						particle_position.z += particle_camera_position.z;
						matrix3x3_transform_vector(&particle_camera_matrix, &particle_velocity, &particle_velocity);
					}

					callback(
						particle,
						particle_state,
						&particle_position,
						&particle_velocity,
						particle_scale,
						&particle_color,
						particle_definition
					);

					current_particle_index = particle->next_particle;
				} while (current_particle_index != NONE);
			}
			current_particle_emitter_index = particle_emitter->next_emitter_index;
			++current_emitter_index;
		} while (current_particle_emitter_index != NONE);
	}
}

real_vector3d* __cdecl particle_update_points_interpolate_hook(const real_vector3d* previous_point, const real_point3d* target_point, real32 fractional_tick, real_point3d* out)
{
	// The engine would interpolate between the particle_emitters previous and current posisiton to find a rough estimate
	// of where it should be in the world to be as close as possible to the object
	// we can remove this and just force it to be in the previous point as that will always be
	// the interpolated position of the attached object as we let the engine go brrrr
	points_interpolate(previous_point, target_point, 0.0f, out);
	return out;
}

void apply_particle_update_patches()
{
	//p_particle_system_update_particle_position_and_velocity = Memory::GetAddress<particle_system_update_particle_position_and_velocity_t>(0xC4125);
	PatchCall(Memory::GetAddress(0x10459E), particle_system_update_particle_position_and_velocity);
	PatchCall(Memory::GetAddress(0x508AA), particle_system_update_particle_position_and_velocity);
	PatchCall(Memory::GetAddress(0x105380), particle_update_points_interpolate_hook);
}
