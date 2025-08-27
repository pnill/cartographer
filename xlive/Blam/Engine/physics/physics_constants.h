#pragma once

constexpr real32 k_physics_constants_default_gravity = 4.1712594f;

struct s_physics_constants
{
	float gravity;
	float dword4;
	float dword8;
	real_vector3d zero_vector;
	DWORD dword18;
};

s_physics_constants* physics_constants_get();

void __cdecl physics_constants_initialize();

void __cdecl physics_constants_reset();

void __cdecl physics_constants_setup_scenario();

void physics_constants_apply_patches();