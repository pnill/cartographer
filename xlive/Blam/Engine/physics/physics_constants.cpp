#include "stdafx.h"

#include "physics_constants.h"

#include "game/game.h"
#include "saved_games/game_state.h"

/* constants */

static const real32 k_physics_constants_variant_graivty[k_game_gravity_modifier_count]
{
	k_physics_constants_default_gravity,
	k_physics_constants_default_gravity * .25f,
	k_physics_constants_default_gravity * .50f,
	k_physics_constants_default_gravity * .75f,
	k_physics_constants_default_gravity * 1.25f,
	k_physics_constants_default_gravity * 1.50f,
	k_physics_constants_default_gravity * 1.75f,
	k_physics_constants_default_gravity * 2.f
};

/* public code */

s_physics_constants* physics_constants_get()
{
	return *Memory::GetAddress<s_physics_constants**>(0x4D2AB4, 0x4f696C);
}

void physics_constants_initialize()
{
	s_physics_constants** physics_constants = Memory::GetAddress<s_physics_constants**>(0x4D2AB4, 0x4f696C);

	*physics_constants = (s_physics_constants*)game_state_malloc("physics constants", 0, sizeof(s_physics_constants));

	(*physics_constants)->gravity = k_physics_constants_default_gravity;
	(*physics_constants)->water_density = 1.f;
	(*physics_constants)->air_density = 0.0011f;
	(*physics_constants)->character_ground_adhesion_forces_enable_time = 0;
	(*physics_constants)->velocity_frame = *global_zero_vector3d;
}

void physics_constants_reset()
{
	s_physics_constants* physics_constants = physics_constants_get();

	

	if (game_is_multiplayer())
	{
		physics_constants->gravity = physics_constants_get_variant_gravity();
	}
	else
		physics_constants->gravity = k_physics_constants_default_gravity;

	physics_constants->water_density = 1.f;
	physics_constants->air_density = 0.0011f;
	physics_constants->character_ground_adhesion_forces_enable_time = 0;
	physics_constants->velocity_frame = *global_zero_vector3d;
}

void physics_constants_setup_scenario()
{
	physics_constants_reset();
}

real32 physics_constants_get_variant_gravity()
{
	const s_game_variant* variant = get_game_variant();

	real32 result = k_physics_constants_default_gravity;

	if (IN_RANGE(variant->cartographer_settings.gravity, _game_gravity_modifier_none, _game_gravity_modifier_two_hundred))
	{
		result = k_physics_constants_variant_graivty[variant->cartographer_settings.gravity];
	}

	return result;
}

void physics_constants_apply_patches()
{
	WritePointer(Memory::GetAddress(0x3A0810, 0x35D540), physics_constants_initialize);
	PatchCall(Memory::GetAddress(0xEB2D5, 0xEA4DF), physics_constants_reset);
	WritePointer(Memory::GetAddress(0x3A0818, 0x35D548), physics_constants_setup_scenario);
}
