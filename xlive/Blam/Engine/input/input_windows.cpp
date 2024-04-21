#include "stdafx.h"
#include "input_xinput.h"
#include "input_windows.h"
#include "input_abstraction.h"
#include "controllers.h"
#include "shell/shell_windows.h"

#include "interface/user_interface_controller.h"

extern input_device** g_xinput_devices;

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
	//INVOKE(0x2E7A4, 0x0, input_update_gamepads, duration_ms);

	/*
	* Reimplemented function removes dependency on input_globals.main_controller_index
	* Actually Updates input_globals.gamepad_states[index] buffer from device state
	*/

	bool input_handled = false;
	for (uint16 gamepad_index = 0; gamepad_index < k_number_of_controllers; gamepad_index++)
	{
		if (input_has_gamepad(gamepad_index, nullptr))
		{

			s_gamepad_input_button_state* gamepad_state = input_get_gamepad_state(gamepad_index);

			if (input_xinput_update_gamepad(gamepad_index, duration_ms, gamepad_state))
			{
				//handled successfully for any device
				input_handled = true;
			}
		}
	}
	if (!input_handled)
		return;

	HWND g_window_handle = *Memory::GetAddress<HWND*>(0x46D9C4);

	if (input_handled
		&& g_window_handle == GetFocus()
		&& g_window_handle == GetForegroundWindow()
		&& !game_is_minimized())
	{
		if ((input_globals->field7D8 & 1) == 0)
		{
			input_globals->field7D8 |= 1u;
			//v26 = 0;
			input_globals->field7D0 = system_milliseconds();
			//v26 = 0xFFFFFFFF;
		}
		uint32 time = system_milliseconds();
		if (time - input_globals->field7D0 > 15000 || time - input_globals->field7D0 < 0)
		{
			input_globals->field7D0 = time;
			tagINPUT pInputs;
			//csmemset(&pInputs, 0, sizeof(pInputs));
			pInputs.type = INPUT_KEYBOARD;
			pInputs.ki.wVk = 0;
			pInputs.ki.wScan = 0;
			pInputs.ki.dwFlags = KEYEVENTF_KEYUP;
			pInputs.ki.dwExtraInfo = 0;
			SendInput(1, &pInputs, sizeof(pInputs));
		}
	}
}

void __cdecl input_update_mouse(DIMOUSESTATE2* mouse_state, uint32 duration_ms)
{
	INVOKE(0x2E60C, 0x0, input_update_mouse, mouse_state, duration_ms);
}

bool __cdecl input_has_gamepad(uint16 gamepad_index, bool* a2)
{
	return INVOKE(0x2F3CD, 0x0, input_has_gamepad, gamepad_index, a2);
}
bool __cdecl input_has_gamepad_plugged(uint16 gamepad_index)
{
	//return INVOKE_TYPE(0x2E186, 0x0, bool(__cdecl*)(uint16), gamepad_index);
	return input_globals->gamepad_states[gamepad_index].connected;
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

void __cdecl input_update_main_device_state()
{
	//INVOKE(0x2E709, 0x0, input_update_main_device_state);

	/*
	* Reimplemented function removes dependency on input_globals.main_controller_index
	* Updates input_globals.gamepad_states[index].connected
	* Allows game to actually detect multiple controllers as multi inputs rather single input
	*/

	uint8 device_index = _controller_index_0;
	do
	{
		input_device* device = g_xinput_devices[device_index];
		XINPUT_STATE state;
		s_gamepad_input_state* gamepad = input_get_gamepad(device_index);
		uint32 error_code = ERROR_DEVICE_NOT_CONNECTED;


		if (!device
			|| (error_code = device->XGetState(&state)) == ERROR_SEVERITY_SUCCESS
			|| error_code == ERROR_DEVICE_NOT_CONNECTED)
		{

			bool dev_connected = gamepad->connected;
			bool success = error_code == ERROR_SEVERITY_SUCCESS;
			bool initially_not_connected = !gamepad->connected;
			bool dev_state_joined = initially_not_connected && success;
			bool dev_state_left = dev_connected && !success;

			gamepad->connected = success;
			gamepad->m_device_just_joined = dev_state_joined;
			gamepad->m_device_just_left = dev_state_left;
		}

		uint32 device_flags = 0;
		if (gamepad->m_device_just_left)
			device_flags = 1;
		if (gamepad->m_device_just_joined)
			device_flags |= 0x2000;

		input_abstraction_handle_device_change(device_flags);
		device_index++;

	} while (device_index < k_number_of_controllers);
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

	PatchCall(Memory::GetAddress(0x2FA62), input_update_main_device_state);		// Replace call in input_update
	PatchCall(Memory::GetAddress(0x2FC2F), input_update_main_device_state);		// Replace call in input_update
	PatchCall(Memory::GetAddress(0x2FBD2), input_update_gamepads);				// Replace call in input_update
	return;
}