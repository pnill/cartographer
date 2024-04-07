#include "stdafx.h"
#include "input_windows.h"

#include "controllers.h"
#include "input_xinput.h"

#include "interface/user_interface_controller.h"

s_input_globals* input_globals;

XINPUT_VIBRATION g_vibration_state[k_number_of_controllers]{};
real32 g_rumble_factor = 1.0f;

void __cdecl input_initialize()
{
	INVOKE(0x2FD23, 0x0, input_initialize);
}

void __cdecl input_dispose()
{
	INVOKE(0x2E309, 0x0, input_dispose);
}

void __cdecl input_update()
{
	INVOKE(0x2F9AC, 0x0, input_update);
}

void __cdecl input_update_gamepads(uint32 duration_ms)
{
	INVOKE(0x2E7A4, 0x0, input_update_gamepads, duration_ms);
}

void __cdecl input_update_mouse(DIMOUSESTATE2* mouse_state, uint32 duration_ms)
{
	INVOKE(0x2E60C, 0x0, input_update_mouse, mouse_state, duration_ms);
}

bool __cdecl input_has_gamepad(uint16 gamepad_index, bool* a2)
{
	return INVOKE(0x2F3CD, 0x0, input_has_gamepad, gamepad_index, a2);
}
s_gamepad_input_state* __cdecl input_get_gamepad(uint16 gamepad_index)
{
	//s_gamepad_input_state* global = Memory::GetAddress<s_gamepad_input_state*>(0x47A5C8);
	//return &global[gamepad_index];
	return &input_globals->gamepad_states[gamepad_index];
}

s_gamepad_input_button_state* __cdecl input_get_gamepad_state(uint16 gamepad_index)
{
	return INVOKE(0x2F433, 0x0, input_get_gamepad_state, gamepad_index);
}

DIMOUSESTATE2* __cdecl input_get_mouse_state()
{
	//return INVOKE(0x2E404, 0x0, input_get_mouse_state);
	if (!input_globals->mouse_dinput_device)
		return nullptr;
	if (!input_globals->input_suppressed)
		return &input_globals->mouse_state;

	return &input_globals->suppressed_mouse_state;
}

bool __cdecl input_get_key(s_key_state* keystate)
{
	return INVOKE(0x2E3CB, 0x0, input_get_key, keystate);
}


int32* hs_debug_simulate_gamepad_global_get(void)
{
	return Memory::GetAddress<int32*>(0x47A71C);
}

bool* input_suppress_global_get(void)
{
	return Memory::GetAddress<bool*>(0x479F52);
}

void __cdecl input_set_gamepad_rumbler_state(int16 gamepad_index, uint16 left, uint16 right)
{
	ASSERT(VALID_INDEX(gamepad_index, k_number_of_controllers));

	XINPUT_VIBRATION state = { left, right };
	XINPUT_VIBRATION state_none = { 0, 0 };

	state.wLeftMotorSpeed *= g_rumble_factor;
	state.wRightMotorSpeed *= g_rumble_factor;

	bool enabled = user_interface_controller_get_rumble_enabled((e_controller_index)gamepad_index);
	g_vibration_state[gamepad_index] = (enabled ? state : state_none);
	return;
}


void input_windows_apply_patches(void)
{
	input_globals = Memory::GetAddress<s_input_globals*>(0x479F50);

	PatchCall(Memory::GetAddress(0x9020F), input_set_gamepad_rumbler_state);    // Replace call in rumble_clear_all_now
	return;
}