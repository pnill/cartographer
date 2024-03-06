#include "stdafx.h"

#include "particle_draw.h"

#include "particle_state.h"
#include "particle_system.h"
#include "particle.h"
#include "particle_emitter.h"
#include "particle_location.h"
#include "render/render_sky.h"
#include "game/game_time.h"

#include "Util/Hooks/Hook.h"

typedef void(__cdecl* particle_draw_callback_t)(c_particle*, s_particle_state*, real_vector3d*, real_vector3d*, real32, real_argb_color*, c_particle_definition**);

typedef void(__cdecl* particle_system_draw_t)(
	c_particle_system* particle_system,
	c_particle_location* particle_location,
	s_particle_state* particle_state,
	particle_draw_callback_t callback,
	c_particle_definition** particle_definition);

particle_system_draw_t p_particle_system_draw;
void __cdecl particle_system_draw(
	c_particle_system* particle_system,
	c_particle_location* particle_location,
	s_particle_state* particle_state,
	particle_draw_callback_t draw_callback,
	c_particle_definition** particle_definition)
{
	c_particle_system_definition* particle_system_definition = particle_system->get_definition();
	if (particle_system != particle_state->particle_system)
	{
		particle_state->flags.set(_particle_update_bit_10, false);
		particle_state->flags.set(_particle_update_bit_9, false);
		particle_state->flags.set(_particle_update_bit_6, false);
		particle_state->flags.set(_particle_update_bit_5, false);
		particle_state->flags.set(_particle_update_bit_4, false);
		particle_state->particle_system = particle_system;
	}

	if (particle_location != particle_state->particle_location)
	{
		particle_state->flags.set(_particle_update_bit_16, false);
		particle_state->flags.set(_particle_update_bit_13, false);
		particle_state->flags.set(_particle_update_bit_12, false);
		particle_state->flags.set(_particle_update_bit_17, false);

		particle_state->particle_location = particle_location;
	}

	int32 emitter_index = 0;
	datum current_particle_emitter_index = particle_location->particle_emitter_index;
	while (current_particle_emitter_index != NONE)
	{
		c_particle_emitter* particle_emitter = (c_particle_emitter*)datum_get(get_particle_emitter_table(), DATUM_INDEX_TO_ABSOLUTE_INDEX(current_particle_emitter_index));
		c_particle_emitter_definition* emitter_definition = particle_system_definition->emitters[emitter_index];
		datum current_particle_index = particle_emitter->particle_index;

		while (current_particle_index != NONE)
		{
			real32 particle_scale = 1.0f;
			real32 sky_render_model_scale = 1.0f;
			real_matrix3x3 relative_matrix;
			real_point3d relative_position;
			real_argb_color particle_color;

			if (particle_system->flags.test(_particle_system_scale_with_sky_render_model_bit)) 
			{
				sky_render_model_scale = get_current_sky_render_model_scale();
			}

			if (particle_system_definition->coordinate_system != _particle_coordinate_system_world) 
			{
				particle_emitter->adjust_matrix_and_vector_to_effect_camera(
					particle_location->parent_effect_has_bit_15_set,
					&relative_matrix,
					&relative_position
				);
			}

			particle_state_update(
				emitter_definition->runtime_flags_2 & 0x107F0 & particle_state->flags.not(),
				particle_state->particle_system,
				particle_state->particle_location,
				particle_state->particle,
				particle_state
			);

			emitter_definition->get_emitter_particle_color(particle_state, &particle_color);

			if (!TEST_BIT(emitter_definition->runtime_flags, 4))
			{
				particle_scale = emitter_definition->particle_size.get_result(particle_state) * sky_render_model_scale;
			}

			while (current_particle_index != NONE)
			{
				c_particle* particle = (c_particle*)datum_get(get_particle_table(), DATUM_INDEX_TO_ABSOLUTE_INDEX(current_particle_index));
				if (particle != particle_state->particle)
				{
					particle_state->flags.set(_particle_update_bit_0, false);
					particle_state->flags.set(_particle_update_bit_1, false);
					particle_state->flags.set(_particle_update_bit_2, false);
					particle_state->flags.set(_particle_update_bit_3, false);
					particle_state->flags.set(_particle_update_bit_11, false);
					particle_state->flags.set(_particle_update_bit_12, false);
					particle_state->flags.set(_particle_update_bit_13, false);
					particle_state->flags.set(_particle_update_bit_14, false);
					particle_state->flags.set(_particle_update_bit_15, false);
					particle_state->particle = particle;
				}

				real_point3d particle_position;
				real_vector3d particle_velocity;
				particle_position = particle->m_position;
				particle_velocity = particle->m_velocity;

				particle_state_update(
					emitter_definition->runtime_flags_2 & 0xF80F & particle_state->flags.not(),
					particle_state->particle_system,
					particle_state->particle_location,
					particle_state->particle,
					particle_state
				);

				emitter_definition->get_emitter_particle_inverse_color(particle_state, &particle_color);

				if (!TEST_BIT(emitter_definition->runtime_flags, 4))
				{
					particle_scale = emitter_definition->particle_size.get_result(particle_state) * sky_render_model_scale;
				}

				if (particle_system_definition->coordinate_system != _particle_coordinate_system_world)
				{
					matrix3x3_transform_vector(&relative_matrix, &particle_position, &particle_position);
					add_vectors3d(&particle_position, &relative_position, &particle_position);
					matrix3x3_transform_vector(&relative_matrix, &particle_velocity, &particle_velocity);
				}

				draw_callback(
					particle,
					particle_state,
					&particle_position,
					&particle_velocity,
					particle_scale,
					&particle_color,
					particle_definition
				);

				current_particle_index = particle->next_particle;
			}
		}

		current_particle_emitter_index = particle_emitter->m_next_emitter_index;
		emitter_index++;
	}
}

void particle_draw_apply_patches()
{
	p_particle_system_draw = Memory::GetAddress<particle_system_draw_t>(0xC4125);
	PatchCall(Memory::GetAddress(0x10459E), particle_system_draw);
	PatchCall(Memory::GetAddress(0x508AA), particle_system_draw);
}