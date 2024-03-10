#include "stdafx.h"
#include "particle_location.h"

#include "memory/data.h"
#include "particle_state.h"
#include "particle_emitter.h"

s_data_array* get_particle_location_table()
{
	return *Memory::GetAddress<s_data_array**>(0x4DD094, 0);
}

void c_particle_location::frame_advance(c_particle_system* particle_system, c_particle_system_definition* particle_system_definition, real32 delta)
{
	return INVOKE_BY_TYPE(0x106055, 0x0, void(__thiscall*)(c_particle_location*, c_particle_system*, c_particle_system_definition*, real32), this, particle_system, particle_system_definition, delta);
}

void c_particle_location::adjust_emitter_particle_intial_position(
	c_particle_system* particle_system,
	real32 dt,
	const real_matrix4x3* matrix
)
{
	c_particle_system_definition* particle_def = particle_system->get_definition();
	int32 emitter_definition_index = 0;

	s_particle_state particle_state;
	particle_state.set_particle_system(particle_system);
	particle_state.set_particle_location(this);
	particle_state.state_update(0x1FFFFu);
	particle_state.m_states[7] = this->field_1C;

	for (datum particle_emitter_index = this->particle_emitter_index; particle_emitter_index != NONE; )
	{
		c_particle_emitter_definition* emitter_definition = NULL;
		c_particle_emitter* particle_emitter = (c_particle_emitter*)datum_get(get_particle_emitter_table(), particle_emitter_index);

		if (emitter_definition_index < particle_def->emitters.count)
		{
			emitter_definition = particle_def->emitters[emitter_definition_index];
			emitter_definition_index++;
		}

		particle_emitter->adjust_initial_particle_position(particle_system, emitter_definition, &particle_state, matrix, dt);
		particle_emitter_index = particle_emitter->m_next_emitter_index;
	}
}