#include "stdafx.h"
#include "input_windows.h"

#include "controllers.h"
#include "input_xinput.h"

#include "interface/user_interface_controller.h"

XINPUT_VIBRATION g_vibration_state[k_number_of_controllers]{};
real32 g_rumble_factor = 1.0f;

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
	PatchCall(Memory::GetAddress(0x9020F), input_set_gamepad_rumbler_state);    // Replace call in rumble_clear_all_now
	return;
}