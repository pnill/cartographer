#include "stdafx.h"
#include "character_physics_mode_ground.h"

#include "game/game_time.h"

/* prototypes */

static void bipeds_physics_apply_patches(void);

static void __stdcall c_character_physics_mode_ground_datum__update_internal_hook(
	class c_character_physics_mode_ground_datum* thisx,
	struct s_character_physics_update_output_datum* physics_output,
	const struct s_character_physics_update_input_datum* physics_input,
	bool localize_airborne,
	const real_vector3d* localized_velocity);

// fixes the biped unit movement physics from applying too much movement, especially when edge-dropping by adjusting the default constant (0.117) value to tickrate
__declspec(naked) static void c_character_physics_mode_ground_datum__update_internal_update_constant(void)
{
#define _stack_pointer_offset 4h + 4Ch
#define _last_param_offset 4h + 10h
	__asm
	{
		movss xmm2, [esp + _stack_pointer_offset + _last_param_offset]
		ret
	}
#undef _stack_pointer_offset
#undef _last_param_offset
}

__declspec(naked) static void c_character_physics_mode_ground_datum__update_internal_to_stdcall(void)
{
	__asm
	{
		pop eax // pop return address
		push ecx // push ecx as first param
		push eax // push the return address back on stack
		jmp c_character_physics_mode_ground_datum__update_internal_hook
	}
}

/* public code */

void character_physics_mode_ground_apply_patches(void)
{
	// fixes edge drop fast fall when using higher tickrates than 30
	PatchCall(Memory::GetAddress(0x1082B4, 0xFA5D4), c_character_physics_mode_ground_datum__update_internal_to_stdcall);
	Codecave(Memory::GetAddress(0x106E23, 0xF9143), c_character_physics_mode_ground_datum__update_internal_update_constant, 3);
	return;
}


static void __stdcall c_character_physics_mode_ground_datum__update_internal_hook(
	class c_character_physics_mode_ground_datum* thisx,
	struct s_character_physics_update_output_datum* physics_output,
	const struct s_character_physics_update_input_datum* physics_input,
	bool localize_airborne,
	const real_vector3d* localized_velocity)
{
	const real32 k_edge_drop_value = 0.117f;
	const real32 edge_drop_per_tick = 30.f * k_edge_drop_value * game_tick_length();

	// push last parameter despite the function taking just 5 parameters
	INVOKE_TYPE(
		0x1067F0,
		0xF8B10,
		void(__thiscall*)(
			class c_character_physics_mode_ground_datum*,
			struct s_character_physics_update_output_datum*,
			const struct s_character_physics_update_input_datum*,
			bool,
			const real_vector3d*,
			real32
		),
		thisx,
		physics_output,
		physics_input,
		localize_airborne,
		localized_velocity,
		edge_drop_per_tick
	);


	// account for the last parameter that doesn't get handled by the actual function
	__asm add esp, 4;
	return;
}