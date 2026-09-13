#pragma once

constexpr real32 k_physics_constants_default_gravity = 4.1712594f;

struct s_physics_constants
{
	real32 gravity;
	real32 water_density;
	real32 air_density;
	real_vector3d velocity_frame;
	uint32 character_ground_adhesion_forces_enable_time;
};

s_physics_constants* physics_constants_get();

void __cdecl physics_constants_initialize();

void __cdecl physics_constants_reset();

void __cdecl physics_constants_setup_scenario();

real32 physics_constants_get_variant_gravity();

void physics_constants_apply_patches();