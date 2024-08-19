#include "stdafx.h"
#include "input_abstraction.h"
#include "game/game_time.h"
#include "H2MOD/Modules/Shell/Config.h"
#include "saved_games/cartographer_player_profile.h"

/* globals */

s_input_abstraction_globals* input_abstraction_globals;
extern uint16 radialDeadzone[k_number_of_controllers];
//we need this because theres only a single abstracted_inputs inside input_abstraction_globals for h2v
s_game_abstracted_input_state g_abstract_input_states[k_number_of_controllers];
//buffers to store old windows input states
DIMOUSESTATE2 old_mouse_state;
uint16 old_mouse_buttons[8];
s_keyboard_input_state old_keyboard_state;
uint16 updating_gamepad_index = _controller_index_0;

/* public code */

void __cdecl input_abstraction_initialize()
{
	INVOKE(0x61D43, 0x0, input_abstraction_initialize);
}

void __cdecl input_abstraction_dispose()
{
	INVOKE(0x5E296, 0x0, input_abstraction_dispose);
}

void __cdecl input_abstraction_handle_device_change(uint32 flags)
{
	INVOKE(0x61C72, 0x0, input_abstraction_handle_device_change, flags);
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

uint32 s_input_abstraction_globals_sub_45E501(e_button_functions button, void* a3)
{
	return INVOKE_TYPE(0x5E501, 0x0, uint32(__thiscall*)(s_input_abstraction_globals*, e_button_functions, void*), input_abstraction_globals, button, a3);
}

bool input_abstraction_preferences_new(s_gamepad_input_preferences* preferences, int16 a2, bool a3, bool a4)
{
	return INVOKE(0x5EE72, 0, input_abstraction_preferences_new, preferences, a2, a3, a4);
}

uint32 input_abstraction_get_stick_type_for_function(e_button_functions function)
{
	// Todo : define enum
	// -1 = bad type
	// 0  = left_stick,
	// 1  = right_stick,
	uint32 button = s_input_abstraction_globals_sub_45E501(function, NULL);
	if (button > _gamepad_analog_left_stick_right)
		return button > _gamepad_analog_right_stick_right ? NONE : 1;
	return 0;
}


void input_abstraction_update_default_throttle(point2d* thumb, real_euler_angles2d* stick)
{
	constexpr real32 scale = 1.0f / INT16_MAX;

	real32 pointy = (real32)thumb->y;
	real32 pointx = (real32)thumb->x;
	real_angle angle = atan2(pointy, pointx);

	real32 magnitude = MAX(fabs(sin(angle)), fabs(cos(angle)));
	real32 inverse_magnitude = 1.0f / magnitude;

	real32 normalized_yaw = (real32)(pointx * inverse_magnitude) * scale;
	real32 normalized_pitch = (real32)(pointy * inverse_magnitude) * scale;


	stick->yaw = PIN(normalized_yaw, -1.0f, 1.0f);
	stick->pitch = PIN(normalized_pitch, -1.0f, 1.0f);

}
void input_abstraction_post_update_throttle(real_euler_angles2d* stick, real_angle angle, bool right_stick)
{
	real_angle flt_7B9F78[] =
	{
		DEGREES_TO_RADIANS(45.0f),
		DEGREES_TO_RADIANS(135.0f),
		DEGREES_TO_RADIANS(-45.0f),
		DEGREES_TO_RADIANS(-135.0f)
	};

	uint8 index = ((stick->pitch >= 0.0f) ? 0 : 2) + (stick->yaw < 0.0f);

	real_vector2d vec = { stick->yaw,stick->pitch };
	real32 magnitude = square_root(dot_product2d(&vec, &vec));

	real32 delta = fabs(angle - flt_7B9F78[index]);
	real_angle min_delta = right_stick ? DEGREES_TO_RADIANS(10.0f) : DEGREES_TO_RADIANS(35.0f);

	if (delta >= min_delta)
	{

		float sign = 0.0f;
		if (fabs(stick->yaw) <= fabs(stick->pitch))
		{
			sign = (stick->pitch >= 0.0f) ? 1.0f : -1.0f;

			stick->pitch = sign * magnitude;
			stick->yaw = 0.0f;
		}
		else
		{
			sign = (stick->yaw >= 0.0f) ? 1.0f : -1.0f;

			stick->yaw = sign * magnitude;
			stick->pitch = 0.0f;
		}
	}
	else
	{
		constexpr real32 scale = 1.0f / DEGREES_TO_RADIANS(35.0f);

		real_angle angle_abs = fabs(angle);
		float sign = 0.f;
		if (angle_abs < DEGREES_TO_RADIANS(45.0f) || (angle_abs > DEGREES_TO_RADIANS(135.0f)))
		{
			sign = (stick->yaw >= 0.0f) ? 1.0f : -1.0f;
			stick->yaw = sign * magnitude;

			sign = (stick->pitch >= 0.0f) ? 1.0f : -1.0f;
			stick->pitch = (1.0f - (delta * scale)) * sign * magnitude;
		}
		else
		{
			sign = (stick->pitch >= 0.0f) ? 1.0f : -1.0f;
			stick->pitch = sign * magnitude;

			sign = (stick->yaw >= 0.0f) ? 1.0f : -1.0f;
			stick->yaw = (1.0f - (delta * scale)) * sign * magnitude;
		}
	}
}

void input_abstraction_post_update_all_throttles(real_euler_angles2d* left_stick, real_euler_angles2d* right_stick, point2d* lthumb, point2d* rthumb)
{
	bool adjust_left_stick = 0;
	bool adjust_right_stick = 0;

	uint32 move_fwd_stick_type = input_abstraction_get_stick_type_for_function(_button_move_forward);
	uint32 extended_yaw_left_stick_type = input_abstraction_get_stick_type_for_function(_extended_button_gamepad_yaw_left);
	uint32 strafe_left_stick_type = input_abstraction_get_stick_type_for_function(_button_strafe_left);
	uint32 extended_pitch_fwd_stick_type = input_abstraction_get_stick_type_for_function(_extended_button_gamepad_pitch_forward);


	if (move_fwd_stick_type == extended_yaw_left_stick_type && move_fwd_stick_type != NONE)
	{
		if (move_fwd_stick_type)
		{
			if (move_fwd_stick_type == 1)
				adjust_right_stick = true;
		}
		else
		{
			adjust_left_stick = true;
		}
	}
	if (strafe_left_stick_type == extended_pitch_fwd_stick_type && strafe_left_stick_type != NONE)
	{
		if (!strafe_left_stick_type)
			adjust_left_stick = true;
		if (strafe_left_stick_type == 1)
			adjust_right_stick = true;
	}

	real_angle left_stick_angle = atan2((real32)lthumb->y, (real32)lthumb->x);
	real_angle right_stick_angle = atan2((real32)rthumb->y, (real32)rthumb->x);

	if (adjust_left_stick)
	{
		input_abstraction_post_update_throttle(left_stick, left_stick_angle, false);

		left_stick->yaw = PIN(left_stick->yaw, -1.0f, 1.0f);
		left_stick->pitch = PIN(left_stick->pitch, -1.0f, 1.0f);

	}

	if (adjust_right_stick)
	{
		input_abstraction_post_update_throttle(right_stick, right_stick_angle, true);

		right_stick->yaw = PIN(right_stick->yaw, -1.0f, 1.0f);
		right_stick->pitch = PIN(right_stick->pitch, -1.0f, 1.0f);
	}

}
void input_abstraction_update_throttles_legacy(s_gamepad_input_button_state* gamepad_state, real_euler_angles2d* left_stick, real_euler_angles2d* right_stick)
{
	input_abstraction_update_default_throttle(&gamepad_state->thumb_left, left_stick);
	input_abstraction_update_default_throttle(&gamepad_state->thumb_right, right_stick);

	input_abstraction_post_update_all_throttles(left_stick, right_stick, &gamepad_state->thumb_left, &gamepad_state->thumb_right);
}

void input_abstraction_update_throttles_modern(s_gamepad_input_button_state* gamepad_state, real_euler_angles2d* left_stick, real_euler_angles2d* right_stick)
{
	constexpr real32 scale = 1.0f / INT16_MAX;

	left_stick->yaw = gamepad_state->thumb_left.x * scale;
	left_stick->pitch = gamepad_state->thumb_left.y * scale;
	right_stick->yaw = gamepad_state->thumb_right.x * scale;
	right_stick->pitch = gamepad_state->thumb_right.y * scale;

	left_stick->yaw = PIN(left_stick->yaw, -1.0f, 1.0f);
	left_stick->pitch = PIN(left_stick->pitch, -1.0f, 1.0f);

	right_stick->yaw = PIN(right_stick->yaw, -1.0f, 1.0f);
	right_stick->pitch = PIN(right_stick->pitch, -1.0f, 1.0f);
}

void input_abstraction_set_controller_thumb_deadzone(e_controller_index controller)
{
	s_gamepad_input_preferences* preference = &input_abstraction_globals->preferences[controller];
	s_saved_game_cartographer_player_profile_v1* profile_settings = cartographer_player_profile_get(controller);

	if (profile_settings->controller_deadzone_type == Axial || profile_settings->controller_deadzone_type == Both) {
		preference->gamepad_axial_deadzone_right_x = (uint16)((real32)INT16_MAX * (profile_settings->deadzone_axial_x / 100));
		preference->gamepad_axial_deadzone_right_y = (uint16)((real32)INT16_MAX * (profile_settings->deadzone_axial_y / 100));

	}
	else
	{
		preference->gamepad_axial_deadzone_right_x = 0;
		preference->gamepad_axial_deadzone_right_y = 0;
	}
	if (profile_settings->controller_deadzone_type == Radial || profile_settings->controller_deadzone_type == Both)
	{
		radialDeadzone[controller] = (uint16)((real32)INT16_MAX * (profile_settings->deadzone_radial / 100));
	}
	else
	{
		radialDeadzone[controller] = 0;
	}
}
void input_abstraction_set_controller_look_sensitivity(e_controller_index controller, real32 value)
{
	if (value == 0.0f) return;

	value = MAX(value - 1.0f, 0.0f);

	s_gamepad_input_preferences* preference = &input_abstraction_globals->preferences[controller];

	preference->gamepad_yaw_rate = 80.0f + 20.0f * value; //x-axis
	preference->gamepad_pitch_rate = 40.0f + 10.0f * value; //y-axis
}



void input_abstraction_set_mouse_look_sensitivity(e_controller_index controller, real32 value)
{
	if (value == 0.0f)
		return;
	if (H2Config_raw_input)
		value = 1.0f;

	value = MAX(value - 1.0f, 0.0f);

	s_gamepad_input_preferences* preference = &input_abstraction_globals->preferences[controller];

	preference->mouse_yaw_rate = (80.0f + 20.0f * value) - 30.0f;

	if (H2Config_mouse_uniform)
		preference->mouse_pitch_rate = preference->mouse_yaw_rate;
	else
		preference->mouse_pitch_rate = (40.0f + 10.0f * value) - 15.0f;
}

void input_abstraction_apply_raw_mouse_update(e_controller_index controller, s_game_input_state* input_state)
{

	if (H2Config_raw_input)
	{
		DIMOUSESTATE2* mouse_state = input_get_mouse_state();
		time_globals* time = time_globals::get();

		input_abstraction_set_mouse_look_sensitivity(controller, 1.0f);
		input_state->mouse.yaw = time->tick_length * (real32)mouse_state->lX * -(H2Config_raw_mouse_scale / 100);
		input_state->mouse.pitch = time->tick_length * (real32)mouse_state->lY * -(H2Config_raw_mouse_scale / 100);

		s_gamepad_input_preferences* preference = &input_abstraction_globals->preferences[controller];
		if (preference->mouse_invert_look)
		{
			input_state->mouse.pitch = -0.0f - input_state->mouse.pitch;
		}

	}
	else
	{
		input_abstraction_set_mouse_look_sensitivity(controller, H2Config_mouse_sens);
	}
}

void input_abstraction_store_windows_inputs()
{
	DIMOUSESTATE2* mouse_state = input_get_mouse_state();
	if (mouse_state)
	{
		csmemcpy(&old_mouse_state, mouse_state, sizeof(*mouse_state));

		uint16* mouse_buttons = input_get_mouse_button_state();

		if (mouse_buttons)
		{
			csmemcpy(old_mouse_buttons, mouse_buttons, sizeof(old_mouse_buttons));
		}
	}
	csmemcpy(&old_keyboard_state, &input_globals->keyboard, sizeof(input_globals->keyboard));
}

void input_abstraction_restore_windows_inputs()
{
	DIMOUSESTATE2* mouse_state = input_get_mouse_state();
	if (mouse_state)
	{
		csmemcpy(mouse_state, &old_mouse_state, sizeof(*mouse_state));

		uint16* mouse_buttons = input_get_mouse_button_state();
		if (mouse_buttons)
		{
			csmemcpy(mouse_buttons, old_mouse_buttons, sizeof(input_globals->mouse_buttons));
		}
	}
	csmemcpy(&input_globals->keyboard, &old_keyboard_state, sizeof(input_globals->keyboard));

}

void input_abstraction_clear_windows_inputs()
{
	DIMOUSESTATE2* mouse_state = input_get_mouse_state();
	if(mouse_state)
	{
		csmemset(mouse_state, 0, sizeof(*mouse_state));

		uint16* mouse_buttons = input_get_mouse_button_state();
		if(mouse_buttons)
		{
			csmemset(mouse_buttons, 0, sizeof(old_mouse_buttons));
		}
	}
	csmemset(&input_globals->keyboard, 0, sizeof(input_globals->keyboard));
}

void input_abstraction_store_abstracted_inputs(e_controller_index controller)
{
	csmemcpy(
		&g_abstract_input_states[controller],
		&input_abstraction_globals->abstracted_inputs,
		sizeof(s_game_abstracted_input_state));
}

void input_abstraction_restore_abstracted_inputs(e_controller_index controller)
{
	csmemcpy(
		&input_abstraction_globals->abstracted_inputs,
		&g_abstract_input_states[controller],
		sizeof(s_game_abstracted_input_state));
}

void __cdecl input_abstraction_update()
{
	//INVOKE(0x628A8, 0x0, input_abstraction_update);

	real_euler_angles2d left_stick, right_stick;

	input_abstraction_store_windows_inputs();

	for (e_controller_index controller = first_controller();
		controller != k_no_controller;
		controller = next_controller(controller))
	{
		left_stick.yaw = 0.0f;
		left_stick.pitch = 0.0f;
		right_stick.yaw = 0.0f;
		right_stick.pitch = 0.0f;


		s_gamepad_input_button_state* gamepad_state = input_get_gamepad_state(controller);
		s_game_input_state* game_input_state = &input_abstraction_globals->input_states[controller];
		s_gamepad_input_preferences* preference = &input_abstraction_globals->preferences[controller];
		s_saved_game_cartographer_player_profile_v1* profile_settings = cartographer_player_profile_get(controller);

		//restore last state from global array before processing
		input_abstraction_restore_abstracted_inputs(controller);

		if (!gamepad_state)
		{
			// controls players when gamepad is disconnected or no gamepad
			input_abstraction_globals->input_has_gamepad[controller] = false;

			if(controller != k_windows_device_controller_index)
			{
				// this needs to be done when a controller disconnects for active player, so it doesnt get controlled by m/k
				input_abstraction_clear_windows_inputs();		
				input_abstraction_update_input_state(
					controller,
					preference,
					gamepad_state,
					&left_stick,
					&right_stick,
					game_input_state);
			}
			else
			{
				input_abstraction_restore_windows_inputs();
				input_abstraction_update_input_state(
					k_windows_device_controller_index,
					preference,
					gamepad_state,
					&left_stick,
					&right_stick,
					game_input_state);
				input_abstraction_apply_raw_mouse_update(k_windows_device_controller_index, game_input_state);
			}	

		}
		else
		{

			if (!profile_settings->controller_modern)
			{
				input_abstraction_update_throttles_legacy(gamepad_state, &left_stick, &right_stick);
			}
			else
			{
				input_abstraction_update_throttles_modern(gamepad_state, &left_stick, &right_stick);
			}

			if (!input_abstraction_globals->input_has_gamepad[controller])
				input_abstraction_globals->input_has_gamepad[controller] = true;



			if (controller == k_windows_device_controller_index)
			{
				input_abstraction_restore_windows_inputs();
				input_abstraction_update_input_state(
					k_windows_device_controller_index,
					preference,
					gamepad_state,
					&left_stick,
					&right_stick,
					game_input_state);

				input_abstraction_apply_raw_mouse_update(k_windows_device_controller_index, game_input_state);

			}
			else
			{
				input_abstraction_clear_windows_inputs();
				input_abstraction_update_input_state(
					controller,
					preference,
					gamepad_state,
					&left_stick,
					&right_stick,
					game_input_state);
			}

		}
		//store to array after processing is done
		input_abstraction_store_abstracted_inputs(controller);
	}
	//restore mouse and keyboard states if it was cleared at any point
	input_abstraction_restore_windows_inputs();
}

void __cdecl input_abstraction_update_input_state(int controller_index, s_gamepad_input_preferences* preference, s_gamepad_input_button_state* gamepad_state, real_euler_angles2d* left_stick_analog, real_euler_angles2d* right_stick_analog, s_game_input_state* input_state)
{
	updating_gamepad_index = controller_index;

	INVOKE(0x61EA2, 0x0, input_abstraction_update_input_state, controller_index, preference, gamepad_state, left_stick_analog, right_stick_analog, input_state);
	////https://github.com/pnill/cartographer/blob/development-patches/xlive/H2MOD/Modules/Splitscreen/InputFixes.cpp#L311
}

bool __cdecl input_abstraction_controller_plugged_hook(uint16 gamepad_index)
{
	//fixes a hardcode check to _controller_index_0 that prevents other controllers from working without _controller_index_0 being connected
	return input_has_gamepad_plugged(updating_gamepad_index);
}

void input_abstraction_patches_apply()
{
	input_abstraction_globals = Memory::GetAddress<s_input_abstraction_globals*>(0x4A89B0);

	PatchCall(Memory::GetAddress(0x39B82), input_abstraction_update);
	PatchCall(Memory::GetAddress(0x61FBD), input_abstraction_controller_plugged_hook); //inside input_abstraction_update_input_state
	input_abstraction_set_mouse_look_sensitivity(_controller_index_0, H2Config_mouse_sens);
	input_abstraction_set_controller_look_sensitivity(_controller_index_0, H2Config_controller_sens);
	input_abstraction_set_controller_thumb_deadzone(_controller_index_0);
}
