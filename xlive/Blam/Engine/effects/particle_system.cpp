#include "stdafx.h"
#include "particle_system.h"

#include "particle.h"
#include "particle_emitter.h"
#include "particle_location.h"
#include "particle_system_definition.h"
#include "game/game_time.h"

#include "render/render_visibility_collection.h"

int32 particle_system_reset_time[k_max_particle_systems];
real32 particle_system_accumulated_time[k_max_particle_systems];

c_particle_system_definition* c_particle_system::get_definition() const
{
	return INVOKE_TYPE(0xC3CE9, 0xB0BD4, c_particle_system_definition*(__thiscall*)(const c_particle_system*), this);
}

data_array* get_particle_system_table()
{
	return *Memory::GetAddress<data_array**>(0x4D83D4, 0x4FFEA8);
}

void particle_system_update_game_time(datum datum_index)
{
	particle_system_reset_time[DATUM_INDEX_TO_ABSOLUTE_INDEX(datum_index)] = game_time_get();
}

void particle_system_reset_game_time(datum datum_index)
{
	particle_system_reset_time[DATUM_INDEX_TO_ABSOLUTE_INDEX(datum_index)] = 0;
}

void particle_system_update_dt(datum datum_index, real32 dt)
{
	particle_system_accumulated_time[DATUM_INDEX_TO_ABSOLUTE_INDEX(datum_index)] += dt;
}

void particle_system_reset_dt(datum datum_index)
{
	particle_system_accumulated_time[DATUM_INDEX_TO_ABSOLUTE_INDEX(datum_index)] = 0.0f;
}

datum particle_system_get_by_ptr(c_particle_system* particle_system)
{
	return DATUM_INDEX_NEW(((char*)particle_system - (char*)get_particle_system_table()->data) / sizeof(c_particle_system), particle_system->datum_salt);
}

void c_particle_system::destroy_children()
{
	return INVOKE_TYPE(0xC37DE, 0xB06C9, void(__thiscall*)(c_particle_system*), this);
}

void c_particle_system::update_colors(bool v_mirrored_or_one_shot, bool one_shot, pixel32 color, pixel32 color_2)
{
	return INVOKE_TYPE(0xC381F, 0xB070A, void(__thiscall *)(c_particle_system*, bool, bool, pixel32, pixel32), this, v_mirrored_or_one_shot, one_shot, color, color_2);
}

int32 c_particle_system::get_particle_count() const
{
	int32 particle_count = 0;
	datum current_location_index = this->particle_system_location_index;
	while (current_location_index != NONE)
	{
		c_particle_location* particle_location = (c_particle_location*)datum_get(get_particle_location_table(), current_location_index);
		datum current_emitter_index = particle_location->m_particle_emitter_index;
		while (current_emitter_index != NONE)
		{
			c_particle_emitter* particle_emitter = (c_particle_emitter*)datum_get(get_particle_emitter_table(), current_emitter_index);
			particle_count += particle_emitter->m_particle_count;
			current_emitter_index = particle_emitter->m_next_emitter_index;
		}
		current_location_index = particle_location->next_particle_location;
	}
	return particle_count;
}

void c_particle_system::change_parent_effect(datum* datum_1, datum* datum_2)
{
	return INVOKE_TYPE(0xC35F7, 0xB04E2, void(__thiscall*)(c_particle_system*, datum*, datum*), this, datum_1, datum_2);
}

typedef bool(__stdcall* c_particle_system_frame_advance_t)(c_particle_system* thisx, real32 dt);
c_particle_system_frame_advance_t p_c_particle_system_frame_advance;

bool __stdcall c_particle_system::frame_advance(c_particle_system* thisx, real32 dt)
{
	// if the framerate is above the tickrate of the game update,
	// avoid updating the following particle system's flags twice
	// before executing one more game update (i.e in between game ticks)
	// otherwise, _particle_system_bit_9 will mistakenly be set to false during the second frame advance update,
	// preventing the creation of new particles during game tick, breaking effects using
	// 'spread between ticks' system, where particles are spawned interpolated between 2 points
	// (rocket launcher trail effect, scarab gun, etc.)
	if (thisx->flags.test(_particle_system_bit_8) || !thisx->keep_system_alive(dt))
	{
		thisx->flags.set(_particle_system_bit_11, thisx->flags.test(_particle_system_bit_9) && thisx->flags.test(_particle_system_bit_5));
		thisx->flags.set(_particle_system_bit_9, thisx->flags.test(_particle_system_bit_8));
		thisx->flags.set(_particle_system_bit_8, false);
	}

	if (!thisx->flags.test(_particle_system_bit_5)
		&& !render_visibility_check_location_cluster_active(&thisx->location))
	{
		thisx->destroy_children();
	}

	c_particle_system_definition* particle_system_definition = thisx->get_definition();

	datum current_particle_location_index = thisx->particle_system_location_index;
	while (current_particle_location_index != NONE)
	{
		c_particle_location* particle_location = (c_particle_location*)datum_get(get_particle_location_table(), current_particle_location_index);

		particle_location->frame_advance(thisx, particle_system_definition, dt);
		current_particle_location_index = particle_location->next_particle_location;
	}

	c_particle_definition_interface* particle_def_interface = particle_system_definition->get_particle_system_interface();

	effect_location_definition* location_definitions = particle_def_interface->vtbl->get_particle_definiton_locations(particle_def_interface);

	datum current_particle_system_index = thisx->next_particle_system;
	while (current_particle_system_index != NONE)
	{
		c_particle_system* current_particle_system = (c_particle_system*)datum_get(get_particle_system_table(), current_particle_system_index);
		c_particle* particle = (c_particle*)datum_try_and_get(get_particle_table(), current_particle_system->first_particle_index);

		real_matrix4x3 marker_matrix;
		s_particle_system_update particle_system_update;
		if (particle)
		{
			if (current_particle_system->definition_location_index == NONE 
				|| current_particle_system->definition_location_index >= particle_def_interface->vtbl->get_particle_definiton_locations_size(particle_def_interface))
			{
				scale_vector3d(&particle->m_velocity, -1.0f, &marker_matrix.vectors.forward);
				normalize3d(&marker_matrix.vectors.forward);
				perpendicular3d(&marker_matrix.vectors.forward, &marker_matrix.vectors.up);
				cross_product3d(&marker_matrix.vectors.up, &marker_matrix.vectors.forward, &marker_matrix.vectors.left);
			}
			else
			{
				string_id marker_name = location_definitions[current_particle_system->definition_location_index].marker_name;
				switch (marker_name)
				{
				case _string_id_up:
					marker_matrix.vectors.forward = *global_up3d;
					marker_matrix.vectors.up = *global_forward3d;
					marker_matrix.vectors.left = *global_left3d;
					break;
				case _string_id_gravity:
					marker_matrix.vectors.forward = *global_down3d;
					marker_matrix.vectors.up = *global_backward3d;
					marker_matrix.vectors.left = *global_left3d;
					break;
				default:
					scale_vector3d(&particle->m_velocity, -1.0f, &marker_matrix.vectors.forward);
					normalize3d(&marker_matrix.vectors.forward);
					perpendicular3d(&marker_matrix.vectors.forward, &marker_matrix.vectors.up);
					cross_product3d(&marker_matrix.vectors.up, &marker_matrix.vectors.forward, &marker_matrix.vectors.left);
					break;
				}
			}

			marker_matrix.position = particle->m_position;
			marker_matrix.scale = 1.0f;

			particle_system_update.some_delta_calc = (particle->time_accumulator / particle->effect_delay_time) - dt;
			particle_system_update.delta_time = dt;
			particle_system_update.particle_system_location_index = current_particle_system->particle_system_location_index;

			c_particle_definition_interface* current_particle_def_interface = current_particle_system->get_definition()->get_particle_system_interface();

			current_particle_system->update_colors(
				current_particle_def_interface->vtbl->system_is_v_mirrored_or_one_shot(current_particle_def_interface),
				current_particle_def_interface->vtbl->system_is_one_shot(current_particle_def_interface),
				thisx->m_color,
				global_white_pixel32
			);

			// this in h2 might not be restart, could be update or something
			// need more research on this
			c_particle_system::restart(current_particle_system, &particle_system_update, &marker_matrix, 0);
		}
		
		if (!current_particle_system->frame_advance(current_particle_system, dt))
		{
			current_particle_system->change_parent_effect(&thisx->next_particle_system, &thisx->datum_44);
			c_particle_system::destroy(current_particle_system_index);
		}

		current_particle_system_index = current_particle_system->field_38;
	}

	thisx->flags.set(_particle_system_updating_bit, false);

	bool result =
		thisx->get_particle_count() > 0 && thisx->flags.test(_particle_system_bit_3)
		|| thisx->flags.test(_particle_system_updating_bit)
		|| thisx->next_particle_system != NONE
		|| (thisx->flags.test(_particle_system_bit_9) || thisx->flags.test(_particle_system_bit_11));

	if (thisx->keep_system_alive(dt))
	{
		result = true;
	}

	return result;
}

bool c_particle_system::keep_system_alive(real32 dt)
{
	bool result = false;
	datum particle_system_datum_index = particle_system_get_by_ptr(this);
	int32 particle_system_index = DATUM_INDEX_TO_ABSOLUTE_INDEX(particle_system_datum_index);
	ASSERT(VALID_INDEX(particle_system_index, k_max_particle_systems));

	particle_system_update_dt(particle_system_datum_index, dt);

	int32 ticks = game_time_get() - particle_system_reset_time[particle_system_index];
	if (ticks > 1)
	{
		particle_system_reset_game_time(particle_system_datum_index);
		particle_system_reset_dt(particle_system_datum_index);
		result = false;
	}
	else
	{
		result = true;
	}

	return result;
}

typedef void(__stdcall* t_c_particle_system__update_location_time)(c_particle_system*, s_particle_system_update*, real_matrix4x3*, int);
t_c_particle_system__update_location_time p_c_particle_system__restart;

void c_particle_system::restart(c_particle_system* thisx, s_particle_system_update* timings, real_matrix4x3* matrix, int unused)
{
	datum particle_system_datum_index = particle_system_get_by_ptr(thisx);
	particle_system_update_game_time(particle_system_datum_index);
	p_c_particle_system__restart(thisx, timings, matrix, unused);
}

void __cdecl c_particle_system::destroy(datum particle_system_index)
{
	INVOKE(0xC3C49, 0xB0B34, c_particle_system::destroy, particle_system_index);
}

void __cdecl particle_system_remove_from_effects_cache(datum effect_index, datum particle_system_index)
{
	INVOKE(0xA69EB, 0x98A6B, particle_system_remove_from_effects_cache, effect_index, particle_system_index);
}

void apply_particle_system_patches()
{
	p_c_particle_system_frame_advance = (c_particle_system_frame_advance_t)DetourClassFunc(Memory::GetAddress<uint8*>(0xC43F9, 0xB12E4), (uint8*)c_particle_system::frame_advance, 10);
	p_c_particle_system__restart = (t_c_particle_system__update_location_time)DetourClassFunc(Memory::GetAddress<uint8*>(0xC3E32, 0xB0D1D), (uint8*)c_particle_system::restart, 11);

	// TODO fixme info in particle emitter
	// allow world coordinate system particles to be updated
	//NopFill(Memory::GetAddress(0xA9563), 5);
	//NopFill(Memory::GetAddress(0xA9563) + 5 + 4, 6);
}
