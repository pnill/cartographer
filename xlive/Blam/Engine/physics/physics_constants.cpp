#include "stdafx.h"

#include "physics_constants.h"

#include "game/game.h"
#include "saved_games/game_state.h"

s_physics_constants* physics_constants_get()
{
	return *Memory::GetAddress<s_physics_constants**>(0x4D2AB4, 0x4f696C);
}

void physics_constants_initialize()
{
	s_physics_constants** physics_constants = Memory::GetAddress<s_physics_constants**>(0x4D2AB4, 0x4f696C);

	*physics_constants = (s_physics_constants*)game_state_malloc("physics constants", 0, sizeof(s_physics_constants));

	(*physics_constants)->gravity = k_physics_constants_default_gravity;
	(*physics_constants)->dword4 = 1.f;
	(*physics_constants)->dword8 = 0.0011f;
	(*physics_constants)->dword18 = 0;
	(*physics_constants)->zero_vector = *global_zero_vector3d;
}

void physics_constants_reset()
{
	s_physics_constants* physics_constants = physics_constants_get();

	s_game_variant* variant = get_game_variant();

	if (game_is_multiplayer() && variant)
	{
		switch (variant->cartographer_settings.gravity)
		{
		case _game_gravity_modifier_twenty_five_percent:
			physics_constants->gravity = k_physics_constants_default_gravity * 0.25f;
			break;
		case _game_gravity_modifier_fifty_percent:
			physics_constants->gravity = k_physics_constants_default_gravity * 0.5f;
			break;
		case _game_gravity_modifier_seventy_five_percent:
			physics_constants->gravity = k_physics_constants_default_gravity * 0.75f;
			break;
		case _game_gravity_modifier_hundred_twenty_five_percent:
			physics_constants->gravity = k_physics_constants_default_gravity * 1.25f;
			break;
		case _game_gravity_modifier_hundred_fifty_percent:
			physics_constants->gravity = k_physics_constants_default_gravity * 1.50f;
			break;
		case _game_gravity_modifier_hundred_seventy_five_percent:
			physics_constants->gravity = k_physics_constants_default_gravity * 1.75f;
			break;
		case _game_gravity_modifier_two_hundred:
			physics_constants->gravity = k_physics_constants_default_gravity * 2.f;
			break;
		case _game_gravity_modifier_none:
		default:
			physics_constants->gravity = k_physics_constants_default_gravity;
			break;
		}
	}
	else
		physics_constants->gravity = k_physics_constants_default_gravity;

	physics_constants->dword4 = 1.f;
	physics_constants->dword8 = 0.0011f;
	physics_constants->dword18 = 0;
	physics_constants->zero_vector = *global_zero_vector3d;
}

void physics_constants_setup_scenario()
{
	physics_constants_reset();
}

void physics_constants_apply_patches()
{
	WritePointer(Memory::GetAddress(0x3A0810, 0x35D540), physics_constants_initialize);
	PatchCall(Memory::GetAddress(0xEB2D5, 0xEA4DF), physics_constants_reset);
	WritePointer(Memory::GetAddress(0x3A0818, 0x35D548), physics_constants_setup_scenario);
}