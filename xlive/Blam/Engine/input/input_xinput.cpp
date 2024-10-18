#include "stdafx.h"
#include "input_xinput.h"
#include "input_abstraction.h"

#include "game/game.h"
#include "game/game_time.h"
#include "networking/logic/life_cycle_manager.h"
#include "H2MOD/Modules/Input/ControllerInput.h"
#include "saved_games/cartographer_player_profile.h"

/* globals */
typedef DWORD(WINAPI* XInputGetStateEx_t)(DWORD dwUserIndex, XINPUT_STATE* pState);
XInputGetStateEx_t XInputGetStateEx;

HMODULE g_xinput1_4_module;

bool g_input_feedback_suppress = false;
XINPUT_VIBRATION g_xinput_vibration{};
input_device** g_xinput_devices;
uint32* g_main_controller_index;
uint16 radialDeadzone[k_number_of_controllers] = {0,0,0,0};

bool g_controller_home_button_state[k_number_of_controllers] = { false, false, false, false };

uint32 XINPUT_BUTTON_FLAGS[k_number_of_xinput_buttons] =
{
	XINPUT_GAMEPAD_DPAD_UP,
	XINPUT_GAMEPAD_DPAD_DOWN,
	XINPUT_GAMEPAD_DPAD_LEFT,
	XINPUT_GAMEPAD_DPAD_RIGHT,
	XINPUT_GAMEPAD_START,
	XINPUT_GAMEPAD_BACK,
	XINPUT_GAMEPAD_LEFT_THUMB,
	XINPUT_GAMEPAD_RIGHT_THUMB,
	XINPUT_GAMEPAD_LEFT_SHOULDER,
	XINPUT_GAMEPAD_RIGHT_SHOULDER,
	XINPUT_GAMEPAD_A,
	XINPUT_GAMEPAD_B,
	XINPUT_GAMEPAD_X,
	XINPUT_GAMEPAD_Y,
}; // 0x39DEE0


/* forward declaration */

void input_xinput_update_get_gamepad_buttons(uint32 gamepad_index, uint16* out_buttons);

/* public code */


uint32 xinput_device::get_port() const
{
	return dwUserIndex;
}

void input_xinput_clear_rumble_state(void)
{
	// Originally 20
	// We set to 4 and check if if the controller is plugged in
	XINPUT_VIBRATION vibration{};

	for (uint32 i = 0; i < 4; i++)
	{
		input_device* device = g_xinput_devices[i];
		if (device && controller_button_state_get((e_controller_index)i)->plugged_in)
		{
			device->XSetState(&vibration);
		}
	}
	return;
}

void input_xinput_update_rumble_state(void)
{
	bool global_suppress_rumble = false;

	if (g_input_feedback_suppress
		|| *input_suppress_global_get()
		|| !game_in_progress()
		|| game_time_get_paused())
	{
		global_suppress_rumble = true;
	}

	for (uint32 device_index = 0; device_index < k_number_of_controllers; ++device_index)
	{
		e_controller_index controller_index = (e_controller_index)device_index;

		if (controller_button_state_get(controller_index)->plugged_in)
		{
			input_device* device = g_xinput_devices[device_index];
			bool rumble_enabled = controller_profile_get(controller_index)->field_1658;

			if (!global_suppress_rumble && rumble_enabled)
			{
				csmemcpy(&g_xinput_vibration, &g_vibration_state[controller_index], sizeof(g_xinput_vibration));
			}
			else
			{
				csmemset(&g_xinput_vibration, 0, sizeof(g_xinput_vibration));
			}

			if (device)
			{
				device->XSetState(&g_xinput_vibration);
			}
		}
	}

	return;
}

void input_xinput_update_button(uint8* frames_down, uint16* msec_down, bool button_down, uint16 duration_ms)
{
	if (button_down)
	{
		*frames_down = MIN(*frames_down + 1, UINT8_MAX);
		*msec_down = MIN(*msec_down + duration_ms, UINT16_MAX);
	}
	else
	{
		*frames_down = 0;
		*msec_down = 0;
	}
}

void input_xinput_update_trigger(uint8* last_msec_down, bool trigger_down, uint8 current_msec_down)
{
	if (trigger_down)
	{
		uint8 msec_down = CLAMP_LOWER(current_msec_down, 0, 32);
		*last_msec_down = MAX(msec_down, *last_msec_down);
	}
	else
	{
		uint8 msec_down = CLAMP_UPPER(current_msec_down, 64, 255);
		*last_msec_down = MIN(msec_down, *last_msec_down);
	}
}

int16 input_xinput_adjust_thumb_axis_deadzone(int16 thumb_axis, int16 thumb_deadzone)
{
	if (thumb_axis > thumb_deadzone)
		return  0x7FFF * (thumb_axis - thumb_deadzone) / (0x7FFF - thumb_deadzone);

	if (thumb_axis < -thumb_deadzone)
		return ~0x7FFF * (thumb_axis + thumb_deadzone) / (~0x7FFF + thumb_deadzone);

	return 0;
}

// Radial deadzone is being calculated using the Pythagorean Theorem, if the point is outside of the given Radius 
// it is accepted as valid input otherwise it is rejected.
void input_xinput_adjust_thumb_radial_deadzones(uint32 gamepad_index, s_gamepad_input_button_state* gamepad_state)
{
	int16 lx = gamepad_state->thumb_left.x;
	int16 ly = gamepad_state->thumb_left.y;
	int16 rx = gamepad_state->thumb_right.x;
	int16 ry = gamepad_state->thumb_right.y;
	
	// Calculate radius of the left and right sticks
	uint32 lh = (lx * lx) + (ly * ly);
	uint32 rh = (rx * rx) + (ry * ry);

	// If the radius of the stick moved is less than the deadzone radius set the stick positions to zero
	uint32 radius = radialDeadzone[gamepad_index] * radialDeadzone[gamepad_index];
	if (lh <= radius)
	{
		gamepad_state->thumb_left.x = 0;
		gamepad_state->thumb_left.y = 0;
	}
	if (rh <= radius)
	{
		gamepad_state->thumb_right.x = 0;
		gamepad_state->thumb_right.y = 0;
	}

	return;
}

bool input_xinput_update_gamepad(uint32 gamepad_index, uint32 duration_ms, s_gamepad_input_button_state* gamepad_state)
{
	input_device* gamepad = g_xinput_devices[gamepad_index];
	XINPUT_STATE state;
	bool any_button_pressed;


	if (!gamepad || gamepad->XGetState(&state) != ERROR_SEVERITY_SUCCESS)
	{
		return false;
	}

	for (uint8 trigger_index = 0; trigger_index < 2; trigger_index++)
	{
		uint8& trigger_msec_down = gamepad_state->trigger_msec_down[trigger_index];
		uint8& max_trigger_msec_down = gamepad_state->max_trigger_msec_down[trigger_index];
		uint8& button_frames_down = gamepad_state->trigger_button_frames_down[trigger_index];
		uint16& button_msec_down = gamepad_state->trigger_button_msec_down[trigger_index];

		trigger_msec_down = trigger_index ? state.Gamepad.bRightTrigger : state.Gamepad.bLeftTrigger;
		bool trigger_down = trigger_msec_down > max_trigger_msec_down;

		input_xinput_update_button(&button_frames_down, &button_msec_down, trigger_down, (uint16)duration_ms);
		input_xinput_update_trigger(&max_trigger_msec_down, trigger_down, trigger_msec_down);
	}

	uint16 gamepad_buttons = 0;
	input_xinput_update_get_gamepad_buttons(gamepad_index, &gamepad_buttons);

	for (uint8 button_index = 0; button_index < k_number_of_xinput_buttons; button_index++)
	{
		uint8& frames_down = gamepad_state->button_frames_down[button_index];
		uint16& msec_down = gamepad_state->button_msec_down[button_index];
		
		bool button_down = TEST_FLAG(gamepad_buttons, XINPUT_BUTTON_FLAGS[button_index]);
		//if (button_down)
		//	LOG_DEBUG_FUNC(" down {}", button_index);
		if (button_down)
			any_button_pressed = true;

		input_xinput_update_button(&frames_down, &msec_down, button_down, (uint16)duration_ms);
	}

	e_controller_index controller = (e_controller_index)gamepad_index;
	s_gamepad_input_preferences preference;
	input_abstraction_get_controller_preferences(controller, &preference);

	gamepad_state->thumb_left.x = input_xinput_adjust_thumb_axis_deadzone(state.Gamepad.sThumbLX, preference.gamepad_axial_deadzone_left_x);
	gamepad_state->thumb_left.y = input_xinput_adjust_thumb_axis_deadzone(state.Gamepad.sThumbLY, preference.gamepad_axial_deadzone_left_y);
	gamepad_state->thumb_right.x = input_xinput_adjust_thumb_axis_deadzone(state.Gamepad.sThumbRX, preference.gamepad_axial_deadzone_right_x);
	gamepad_state->thumb_right.y = input_xinput_adjust_thumb_axis_deadzone(state.Gamepad.sThumbRY, preference.gamepad_axial_deadzone_right_y);

	if (radialDeadzone[gamepad_index])
	{
		input_xinput_adjust_thumb_radial_deadzones(gamepad_index, gamepad_state);
	}

	return gamepad_state->thumb_left.x > 0
		|| gamepad_state->thumb_left.y > 0
		|| gamepad_state->thumb_right.x > 0
		|| gamepad_state->thumb_right.y > 0
		|| any_button_pressed;
}

void input_xinput_update_get_gamepad_buttons(uint32 gamepad_index, uint16* out_buttons)
{
	input_device* gamepad = g_xinput_devices[gamepad_index];
	uint16 custom_button_flags[k_number_of_xinput_buttons];
	s_saved_game_cartographer_player_profile* profile_settings = cartographer_player_profile_get_by_controller_index(((e_controller_index)gamepad_index));

	XINPUT_STATE state;
	profile_settings->custom_layout.ToArray(custom_button_flags);

	ASSERT(out_buttons != nullptr);
	if (gamepad && gamepad->XGetState(&state) == ERROR_SEVERITY_SUCCESS)
	{
		if (get_game_life_cycle() == _life_cycle_in_game || game_mode_get() == _game_mode_campaign)
		{
			for (uint8 button_index = 0; button_index < k_number_of_xinput_buttons; button_index++)
			{
				if (TEST_FLAG(state.Gamepad.wButtons, XINPUT_BUTTON_FLAGS[button_index]))
				{
					*out_buttons |= custom_button_flags[button_index];
				}
			}
		}
		else
		{
			*out_buttons = state.Gamepad.wButtons;
		}
	}
}

bool __cdecl xinput_load()
{
	g_xinput1_4_module = LoadLibraryW(L"xinput1_4.dll");

	if (g_xinput1_4_module)
	{
		XInputGetStateEx = (XInputGetStateEx_t)GetProcAddress(g_xinput1_4_module, (LPCSTR)100);
	}

	return INVOKE(0x8AD1B, 0, xinput_load);
}

void xinput_apply_patches(void)
{
	g_xinput_devices = Memory::GetAddress<input_device**>(0x479F00);
	g_main_controller_index = Memory::GetAddress<uint32*>(0x47A714);

	PatchCall(Memory::GetAddress(0x2FBDA), input_xinput_update_rumble_state);
	PatchCall(Memory::GetAddress(0x2FC34), input_xinput_clear_rumble_state);
	PatchCall(Memory::GetAddress(0x2FD7E), xinput_load);
	return;
}