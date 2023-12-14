#pragma once
#include "particle.h"
#include "particle_emitter.h"
#include "particle_location.h"
#include "particle_system.h"

typedef void(__cdecl* update_particle_position_and_velocity_callback_t)(c_particle*, s_particle_state*, real_vector3d*, real_vector3d*, real32, real_argb_color*, c_particle_definition**);
void __cdecl particle_system_update_particle_position_and_velocity(
	c_particle_system* particle_system,
	c_particle_location* particle_location,
	s_particle_state* particle_state,
	update_particle_position_and_velocity_callback_t callback,
	c_particle_definition** particle_definition);

void apply_particle_update_patches();
