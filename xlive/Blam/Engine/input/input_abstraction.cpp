#include "stdafx.h"
#include "input_abstraction.h"

s_input_abstraction_globals* input_abstraction_globals;

void __cdecl input_abstraction_initialize()
{
	INVOKE(0x61D43, 0x0, input_abstraction_initialize);
}

void __cdecl input_abstraction_dispose()
{
	INVOKE(0x5E296, 0x0, input_abstraction_dispose);
}

void __cdecl input_abstraction_get_controller_preferences(e_controller_index controller_index, s_gamepad_input_preferences* preferences)
{
	INVOKE(0x61BF4, 0x0, input_abstraction_get_controller_preferences, controller_index, preferences);
}

void __cdecl input_abstraction_get_input_state(e_controller_index controller_index, s_game_input_state* state)
{
	INVOKE(0x61C3B, 0x0, input_abstraction_get_input_state, controller_index, state);
}

void __cdecl input_abstraction_get_player_look_angular_velocity(e_controller_index controller_index, real_euler_angles2d* angular_velocity)
{
	INVOKE(0x61D0B, 0x0, input_abstraction_get_player_look_angular_velocity, controller_index, angular_velocity);
}

void __cdecl input_abstraction_get_player_look_angular_velocity_for_mouse(e_controller_index controller_index, real_euler_angles2d* angular_velocity)
{
	INVOKE(0x61CD3, 0x0, input_abstraction_get_player_look_angular_velocity_for_mouse, controller_index, angular_velocity);
}

bool __cdecl input_abstraction_controller_button_test(e_controller_index controller_index, e_button_functions button_index)
{
	return INVOKE(0x61C5B, 0x0, input_abstraction_controller_button_test, controller_index, button_index);
}

e_button_functions __cdecl input_abstraction_get_primary_fire_button(datum unit)
{
	return INVOKE(0x5E2B6, 0x0, input_abstraction_get_primary_fire_button, unit);
}

e_button_functions __cdecl input_abstraction_get_secondary_fire_button(datum unit)
{
	return INVOKE(0x5E2ED, 0x0, input_abstraction_get_secondary_fire_button, unit);
}


bool __cdecl input_abstraction_get_key_state(int16 key)
{
	return INVOKE(0x2EF86, 0x0, input_abstraction_get_key_state, key);
}


void input_abstraction_patches_apply()
{
	input_abstraction_globals = Memory::GetAddress<s_input_abstraction_globals*>(0x4A89B0);
}
