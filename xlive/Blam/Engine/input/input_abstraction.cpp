#include "stdafx.h"
#include "input_abstraction.h"

#include "input_windows.h"
#include "input_xinput.h"

#include "game/player_constants.h"
#include "main/game_preferences.h"
#include "saved_games/cartographer_player_profile/cartographer_player_profile.h"

#ifndef IMGUI_DISABLE
#include "H2MOD/GUI/imgui_integration/imgui_handler.h"
#endif

/* constants */

static const real32 k_gamepad_stick_angles[] =
{
	DEGREES_TO_RADIANS(45.f),
	DEGREES_TO_RADIANS(135.f),
	DEGREES_TO_RADIANS(-45.f),
	DEGREES_TO_RADIANS(-135.f)
};

// Set this to a default of 8
const real32 k_default_right_thumbstick_deadzone_radial_percentage = THUMBSTICK_POINT_TO_PERCENTAGE(THUMBSTICK_PERCENTAGE_TO_POINT(8));

const real_point2d k_default_right_thumbstick_deadzone_axial_percentage =
{
	THUMBSTICK_POINT_TO_PERCENTAGE(XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE),
	THUMBSTICK_POINT_TO_PERCENTAGE(XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE)
};

/* globals */

s_input_abstraction_globals* g_input_abstraction_globals;

//we need this because theres only a single abstracted_inputs inside input_abstraction_globals for h2v
s_game_abstracted_input_state g_abstract_input_states[k_number_of_controllers];

//buffers to store old windows input states
DIMOUSESTATE2 g_old_mouse_state;

uint16 g_old_mouse_buttons[8];

s_keyboard_input_state g_old_keyboard_state;

e_controller_index g_updating_gamepad_index = _controller_index_0;

bool g_controller_advanced_settings_toggle[k_number_of_users]{};

/* macros */

#define HELPER_GET_INT_KEY(code, language) (language != _language_english ? language_get_international_key(language, (code)) : (code))

/* prototypes */

static void input_abstraction_store_windows_inputs(void);

static void input_abstraction_restore_windows_inputs(void);

static void input_abstraction_clear_windows_inputs(void);

static void input_abstraction_store_abstracted_inputs(e_controller_index controller);

static void input_abstraction_restore_abstracted_inputs(e_controller_index controller);

static e_abstract_gamepad_stick_types input_abstraction_get_stick_type_for_action(e_button_action action);

static void input_abstraction_update_dead_zones(const point2d* thumb, real_euler_angles2d* out_stick_euler_angles);

static void input_abstraction_apply_raw_mouse_update(e_controller_index controller, s_game_input_state* input_state);

static void input_abstraction_post_update_throttle(real_euler_angles2d* stick, real_angle angle, e_abstract_gamepad_stick_types stick_index);

static void input_abstraction_post_update_all_throttles(real_euler_angles2d* left_stick, real_euler_angles2d* right_stick, point2d* lthumb, point2d* rthumb);

static void input_abstraction_update_throttles_legacy(s_gamepad_input_button_state* gamepad_state, real_euler_angles2d* left_stick, real_euler_angles2d* right_stick);

static void input_abstraction_update_throttles_modern(s_gamepad_input_button_state* gamepad_state, real_euler_angles2d* left_stick, real_euler_angles2d* right_stick);

static void input_abstraction_set_global_input_preferences(s_gamepad_input_preferences* preference);

static void input_abstraction_add_default_gamepad_baseline_controls(s_gamepad_input_preferences* preference);

static void input_abstraction_gamepad_add_default_preset_controls(s_gamepad_input_preferences* preference);

static void input_abstraction_gamepad_add_southpaw_preset_controls(s_gamepad_input_preferences* preference);

static void input_abstraction_gamepad_add_boxer_preset_controls(s_gamepad_input_preferences* preference);

static void input_abstraction_gamepad_add_greenthumb_preset_controls(s_gamepad_input_preferences* preference);

static void input_abstraction_gamepad_add_jumpy_preset_controls(s_gamepad_input_preferences* preference);

static void input_abstraction_gamepad_set_default_stick_controls(s_gamepad_input_preferences* preference, e_joystick_preset_types thumbstick_layout);

static void input_abstraction_keyboard_and_mouse_add_default_controls(s_gamepad_input_preferences* preference, e_language language);

static void input_abstraction_keyboard_and_mouse_set_default_mapping(s_gamepad_input_preferences* preference, e_language language, bool kb_type_legacy);

static void input_abstraction_keyboard_set_normal_default_mapping(s_gamepad_input_preferences* preference, e_language language, bool kb_type_legacy);

static void input_abstraction_keyboard_set_split_default_mapping(s_gamepad_input_preferences* preference, e_language language, bool kb_type_legacy);

static void input_abstraction_mouse_set_default_mapping(s_gamepad_input_preferences* preference, e_language language, bool kb_type_legacy, bool kb_type_split);

static bool __cdecl input_abstraction_controller_plugged_hook(uint16 gamepad_index);

/* public code */

void input_abstraction_patches_apply(void)
{
	g_input_abstraction_globals = Memory::GetAddress<s_input_abstraction_globals*>(0x4A89B0);

	PatchCall(Memory::GetAddress(0x61FBD), input_abstraction_controller_plugged_hook); //inside input_abstraction_update_input_state
	return;
}

bool c_input_control::add_button(e_input_device_types device_type, uint32 keycode, uint32 held_time, bool return_value_handled)
{
	return INVOKE_TYPE(0x5E473, 0x0, bool(__thiscall*)(c_input_control*, e_input_device_types, uint32, uint32, bool), this, device_type, keycode, held_time, return_value_handled);

	//if (this->button_count < NUMBEROF(buttons))
	//{
	//	if (button_count >= 0)
	//	{
	//		this->buttons[button_count].m_device_type = device_type;
	//		this->buttons[this->button_count].m_device_key = keycode;
	//		this->buttons[this->button_count].m_device_key_held_time_msec = held_time;

	//		++this->button_count;
	//		return true;
	//	}
	//}
	//ASSERT(button_count >= 0);
	//ASSERT(return_value_handled || (button_count < NUMBEROF(buttons)));
	//return false;
}

bool c_input_control::remove_button(uint32 button_index)
{
	return INVOKE_TYPE(0x5E4B2, 0x0, bool(__thiscall*)(c_input_control*, uint32), this, button_index);

	//ASSERT(button_index >= 0);
	//ASSERT(button_index < MAX_BUTTONS_PER_CONTROL);

	//if (this->button_count <= 0)
	//	return false;
	//if (button_index != (MAX_BUTTONS_PER_CONTROL - 1))
	//	csmemmove(&this->buttons[button_index], &this->buttons[button_index + 1], sizeof(s_input_button) * (MAX_BUTTONS_PER_CONTROL - button_index -1));
	//if (button_index < this->button_count)
	//	--this->button_count;
	//return true;
}


void __cdecl input_abstraction_initialize(void)
{
	INVOKE(0x61D43, 0x0, input_abstraction_initialize);
	return;
}

void __cdecl input_abstraction_dispose(void)
{
	INVOKE(0x5E296, 0x0, input_abstraction_dispose);
	return;
}

void __cdecl input_abstraction_handle_device_change(uint32 flags)
{
	INVOKE(0x61C72, 0x0, input_abstraction_handle_device_change, flags);
	return;
}

void __cdecl input_abstraction_get_controller_preferences(e_controller_index controller_index, s_gamepad_input_preferences* preferences)
{
	INVOKE(0x61BF4, 0x0, input_abstraction_get_controller_preferences, controller_index, preferences);
	return;
}

void __cdecl input_abstraction_get_input_state(e_controller_index controller_index, s_game_input_state* state)
{
	INVOKE(0x61C3B, 0x0, input_abstraction_get_input_state, controller_index, state);
	return;
}

void __cdecl input_abstraction_get_player_look_angular_velocity(e_controller_index controller_index, real_euler_angles2d* angular_velocity)
{
	INVOKE(0x61D0B, 0x0, input_abstraction_get_player_look_angular_velocity, controller_index, angular_velocity);
	return;
}

void __cdecl input_abstraction_get_player_look_angular_velocity_for_mouse(e_controller_index controller_index, real_euler_angles2d* angular_velocity)
{
	INVOKE(0x61CD3, 0x0, input_abstraction_get_player_look_angular_velocity_for_mouse, controller_index, angular_velocity);
	return;
}

void __cdecl input_abstraction_get_default_preferences(s_gamepad_input_preferences* preference, e_joystick_preset_types thumbstick_layout, e_button_preset_types button_preset_type, e_custom_keyboard_preset_types kb_layout, bool caller_handles_failures)
{
	//INVOKE(0x5EE72, 0x0, input_abstraction_get_default_preferences, preference, thumbstick_layout, button_preset_type, kb_layout, unused);


	//--- input settings pre-process --- //

	for (int32 current_action_idx = 0; current_action_idx < NUMBER_OF_EXTENDED_CONTROL_BUTTONS; current_action_idx++)
	{
		e_button_action current_action = (e_button_action)current_action_idx;
		c_input_control* current_control = &preference->game_controls_to_hardware[current_action];

		if (kb_layout != _custom_keyboard_preset_custom
			|| current_action == _button_start
			|| current_action == _button_back
			|| current_action == _button_accept
			|| current_action == _button_cancel
			|| current_action == _button_ui_scroll_up
			|| current_action == _button_ui_scroll_down)
		{
			current_control->button_count = 0;
		}
		else
		{
			for (uint8 button_index = 0; button_index < MAX_BUTTONS_PER_CONTROL; button_index++)
			{
				if (current_control->buttons[button_index].m_device_type == _input_device_type_gamepad && current_control->button_count > 0)
				{
					if (button_index != (MAX_BUTTONS_PER_CONTROL - 1))
					{
						csmemmove(&current_control->buttons[button_index], &current_control->buttons[button_index + 1], sizeof(s_input_button) * (MAX_BUTTONS_PER_CONTROL - button_index - 1));
					}
					if (button_index < current_control->button_count)
					{
						--current_control->button_count;
					}
				}
			}
		}
	}

	//--- gamepad input settings --- //

	if (thumbstick_layout != _joystick_preset_unused4 && button_preset_type != _button_preset_unused5)
	{
		//add baseline controls
		input_abstraction_add_default_gamepad_baseline_controls(preference);

		switch (button_preset_type)
		{
		case _button_preset_default:
			input_abstraction_gamepad_add_default_preset_controls(preference);
			break;
		case _button_preset_south_paw:
			input_abstraction_gamepad_add_southpaw_preset_controls(preference);
			break;
		case _button_preset_boxer:
			input_abstraction_gamepad_add_boxer_preset_controls(preference);
			break;
		case _button_preset_green_thumb:
			input_abstraction_gamepad_add_greenthumb_preset_controls(preference);
			break;
		case _button_preset_jumpy:
			input_abstraction_gamepad_add_jumpy_preset_controls(preference);
			break;
		default:
			unreachable();
			break;
		}

		input_abstraction_gamepad_set_default_stick_controls(preference, thumbstick_layout);

		//--- keyboard and mouse input settings --- //

		const e_language language = get_current_language();

		bool kb_type_south_paw = kb_layout == _custom_keyboard_preset_left_hold || kb_layout == _custom_keyboard_preset_left_split;
		bool kb_type_legacy = !kb_type_south_paw;
		bool kb_type_split = kb_layout == _custom_keyboard_preset_right_split || kb_layout == _custom_keyboard_preset_left_split;

		input_abstraction_keyboard_and_mouse_add_default_controls(preference, language);

		if (kb_layout != _custom_keyboard_preset_custom)
		{
			//default_mapping presets;

			input_abstraction_keyboard_and_mouse_set_default_mapping(preference, language, kb_type_legacy);

			if (kb_type_split)
			{
				input_abstraction_keyboard_set_split_default_mapping(preference, language, kb_type_legacy);
			}
			else
			{
				input_abstraction_keyboard_set_normal_default_mapping(preference, language, kb_type_legacy);
			}

			input_abstraction_mouse_set_default_mapping(preference, language, kb_type_legacy, kb_type_split);

			//default_mapping presets end;
		}

		preference->field_1658 = 0; // last used device being gamepad = false ?

		/* verify mapping */
#ifdef ASSERTS_ENABLED
		bool mapping_ok = true;
		for (int32 current_action_idx = 0; current_action_idx < NUMBER_OF_EXTENDED_CONTROL_BUTTONS; ++current_action_idx)
		{
			e_button_action current_action = (e_button_action)current_action_idx;
			c_input_control* current_control = &preference->game_controls_to_hardware[current_action];

			if (current_action == _button_scope_zoom
				|| current_action == _button_scope_zoom_in
				|| current_action == _button_scope_zoom_out)
			{

				mapping_ok = preference->game_controls_to_hardware[_button_scope_zoom].button_count > 0
					|| preference->game_controls_to_hardware[_button_scope_zoom_in].button_count > 0
					&& preference->game_controls_to_hardware[_button_scope_zoom_out].button_count > 0;
			}
			else
			{
				mapping_ok = current_control->button_count > 0;
			}
		}
		ASSERT(mapping_ok || caller_handles_failures);
#endif
	}

	//--- global input preferences --- //

	input_abstraction_set_global_input_preferences(preference);
	return;
}

void __cdecl input_abstraction_set_controller_settings_from_preferences(s_gamepad_input_preferences* preferences, s_saved_game_profile_input_preferences* controller_settings)
{
	INVOKE(0x61B62, 0, input_abstraction_set_controller_settings_from_preferences, preferences, controller_settings);
	return;
}

void __cdecl input_abstraction_set_preferences_from_controller_settings(s_gamepad_input_preferences* preferences, s_saved_game_profile_input_preferences* controller_settings)
{
	INVOKE(0x61AD0, 0, input_abstraction_set_preferences_from_controller_settings, preferences, controller_settings);
	return;
}

bool __cdecl input_abstraction_controller_button_test(e_controller_index controller_index, e_button_action button_index)
{
	return INVOKE(0x61C5B, 0x0, input_abstraction_controller_button_test, controller_index, button_index);
}

e_button_action __cdecl input_abstraction_get_primary_fire_button(datum unit)
{
	return INVOKE(0x5E2B6, 0x0, input_abstraction_get_primary_fire_button, unit);
}

e_button_action __cdecl input_abstraction_get_secondary_fire_button(datum unit)
{
	return INVOKE(0x5E2ED, 0x0, input_abstraction_get_secondary_fire_button, unit);
}

uint32 s_input_abstraction_globals_sub_45E501(e_button_action button, void* a3)
{
	return INVOKE_TYPE(0x5E501, 0x0, uint32(__thiscall*)(s_input_abstraction_globals*, e_button_action, void*), g_input_abstraction_globals, button, a3);
}

int32 __cdecl input_abstraction_get_last_used_device(e_controller_index controller)
{
	return INVOKE(0x5E30E, 0x0, input_abstraction_get_last_used_device, controller);
}

void input_abstraction_set_controller_right_thumb_deadzone(e_controller_index controller)
{
	s_gamepad_input_preferences* preference = &g_input_abstraction_globals->preferences[controller];
	s_saved_game_cartographer_player_profile* profile_settings = cartographer_player_profile_get_by_controller_index(controller);

	if (profile_settings->controller_deadzone_type == _controller_deadzone_type_axial
		|| profile_settings->controller_deadzone_type == _controller_deadzone_type_combined)
	{
		preference->gamepad_axial_deadzone_right.x = (uint16)THUMBSTICK_PERCENTAGE_TO_POINT(profile_settings->deadzone_axial.x);
		preference->gamepad_axial_deadzone_right.y = (uint16)THUMBSTICK_PERCENTAGE_TO_POINT(profile_settings->deadzone_axial.y);
	}
	else
	{
		preference->gamepad_axial_deadzone_right = { 0, 0 };
	}

	if (profile_settings->controller_deadzone_type == _controller_deadzone_type_radial
		|| profile_settings->controller_deadzone_type == _controller_deadzone_type_combined)
	{
		g_controller_radial_deadzones[controller] = (uint16)THUMBSTICK_PERCENTAGE_TO_POINT(profile_settings->deadzone_radial);
	}
	else
	{
		g_controller_radial_deadzones[controller] = 0;
	}
	return;
}

void input_abstraction_set_controller_look_sensitivity(e_controller_index controller, real32 value)
{
	//s_saved_game_cartographer_player_profile* cartographer_player_profile = cartographer_player_profile_get_by_user_index(controller);

	if (value == 0.0f) return;

	value = MAX(value - 1.0f, 0.0f);

	s_gamepad_input_preferences* preference = &g_input_abstraction_globals->preferences[controller];

	preference->gamepad_yaw_rate = 80.0f + 20.0f * value; //x-axis
	preference->gamepad_pitch_rate = 40.0f + 10.0f * value; //y-axis

	// ### FIXME: uniform yaw, pitch sensitivity
	return;
}

void input_abstraction_set_mouse_look_sensitivity(e_controller_index controller, real32 value)
{
	s_saved_game_cartographer_player_profile* cartographer_player_profile = cartographer_player_profile_get_by_user_index(0);

	if (value == 0.0f)
		return;
	if (cartographer_player_profile->raw_mouse_input)
		value = 1.0f;

	value = MAX(value - 1.0f, 0.0f);

	s_gamepad_input_preferences* preference = &g_input_abstraction_globals->preferences[controller];

	preference->mouse_yaw_rate = (80.0f + 20.0f * value) - 30.0f;

	if (cartographer_player_profile->mouse_uniform)
		preference->mouse_pitch_rate = preference->mouse_yaw_rate;
	else
		preference->mouse_pitch_rate = (40.0f + 10.0f * value) - 15.0f;
	
	return;
}

void __cdecl input_abstraction_update(void)
{
	//INVOKE(0x628A8, 0x0, input_abstraction_update);

	input_abstraction_store_windows_inputs();

	for (e_controller_index controller = first_controller();
		controller != k_no_controller;
		controller = next_controller(controller))
	{
		real_euler_angles2d left_stick = { 0.f, 0.f };
		real_euler_angles2d right_stick = { 0.f, 0.f };

		s_gamepad_input_button_state* gamepad_state = input_get_gamepad_state((uint16)controller);
		s_game_input_state* game_input_state = &g_input_abstraction_globals->input_states[controller];
		s_gamepad_input_preferences* preference = &g_input_abstraction_globals->preferences[controller];
		s_saved_game_cartographer_player_profile* profile_settings = cartographer_player_profile_get_by_controller_index(controller);

		//restore last state from global array before processing
		input_abstraction_restore_abstracted_inputs(controller);

		if (!gamepad_state)
		{
			// controls players when gamepad is disconnected or no gamepad
			g_input_abstraction_globals->input_has_gamepad[controller] = false;

			if (controller != k_windows_device_controller_index)
			{
				// this needs to be done when a controller disconnects for active player, so it doesn't get controlled by m/k
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

			if (!g_input_abstraction_globals->input_has_gamepad[controller])
				g_input_abstraction_globals->input_has_gamepad[controller] = true;


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

			// crappy but it works
			if (gamepad_state->button_frames_down[_xinput_gamepad_back] > 10 && gamepad_state->button_msec_down[_xinput_gamepad_dpad_up] > 10)
			{
				if (!g_controller_advanced_settings_toggle[controller])
				{
#ifndef IMGUI_DISABLE
					ImGuiHandler::ImAdvancedSettings::set_controller_index(controller);
					ImGuiHandler::ToggleWindow(k_advanced_settings_window_name);
					g_controller_advanced_settings_toggle[controller] = true;
#endif
				}
			}
			else
			{
				g_controller_advanced_settings_toggle[controller] = false;
			}
		}

		//store to array after processing is done
		input_abstraction_store_abstracted_inputs(controller);
	}
	//restore mouse and keyboard states if it was cleared at any point
	input_abstraction_restore_windows_inputs();
	return;
}

void __cdecl input_abstraction_update_input_state(e_controller_index controller_index, s_gamepad_input_preferences* preference, s_gamepad_input_button_state* gamepad_state, real_euler_angles2d* left_stick_analog, real_euler_angles2d* right_stick_analog, s_game_input_state* input_state)
{
	g_updating_gamepad_index = controller_index;

	INVOKE(0x61EA2, 0x0, input_abstraction_update_input_state, controller_index, preference, gamepad_state, left_stick_analog, right_stick_analog, input_state);
	
	// Rewritten function: https://github.com/pnill/cartographer/blob/development-patches/xlive/H2MOD/Modules/Splitscreen/InputFixes.cpp#L311
	return;
}

/* private code */

static void input_abstraction_store_windows_inputs(void)
{
	mouse_state* mouse_state = input_get_mouse_state();
	if (mouse_state)
	{
		csmemcpy(&g_old_mouse_state, mouse_state, sizeof(*mouse_state));

		uint16* mouse_buttons = input_get_mouse_button_state();

		if (mouse_buttons)
		{
			csmemcpy(g_old_mouse_buttons, mouse_buttons, sizeof(g_old_mouse_buttons));
		}
	}
	csmemcpy(&g_old_keyboard_state, &input_globals->keyboard, sizeof(input_globals->keyboard));
	return;
}

static void input_abstraction_restore_windows_inputs(void)
{
	mouse_state* mouse_state = input_get_mouse_state();
	if (mouse_state)
	{
		csmemcpy(mouse_state, &g_old_mouse_state, sizeof(*mouse_state));

		uint16* mouse_buttons = input_get_mouse_button_state();
		if (mouse_buttons)
		{
			csmemcpy(mouse_buttons, g_old_mouse_buttons, sizeof(input_globals->mouse_buttons));
		}
	}
	csmemcpy(&input_globals->keyboard, &g_old_keyboard_state, sizeof(input_globals->keyboard));
	return;
}

static void input_abstraction_clear_windows_inputs(void)
{
	mouse_state* mouse_state = input_get_mouse_state();
	if (mouse_state)
	{
		csmemset(mouse_state, 0, sizeof(*mouse_state));

		uint16* mouse_buttons = input_get_mouse_button_state();
		if (mouse_buttons)
		{
			csmemset(mouse_buttons, 0, sizeof(g_old_mouse_buttons));
		}
	}
	csmemset(&input_globals->keyboard, 0, sizeof(input_globals->keyboard));
	return;
}

static void input_abstraction_store_abstracted_inputs(e_controller_index controller)
{
	g_abstract_input_states[controller] = g_input_abstraction_globals->abstracted_inputs;
	return;
}

static void input_abstraction_restore_abstracted_inputs(e_controller_index controller)
{
	g_input_abstraction_globals->abstracted_inputs = g_abstract_input_states[controller];
	return;
}

static e_abstract_gamepad_stick_types input_abstraction_get_stick_type_for_action(e_button_action action)
{
	const uint32 button = s_input_abstraction_globals_sub_45E501(action, NULL);
	if (button > _gamepad_analog_left_stick_right)
		return button > _gamepad_analog_right_stick_right ? _abstract_gamepad_stick_unknown : _abstract_gamepad_stick_right;
	return _abstract_gamepad_stick_left;
}

static void input_abstraction_update_dead_zones(const point2d* thumb, real_euler_angles2d* out_stick_euler_angles)
{
	constexpr real32 normalize_scale = 1.f / INT16_MAX;

	real_point2d thumbstick_points = { (real32)thumb->x, (real32)thumb->y };
	real_angle angle = arctangent(thumbstick_points.y, thumbstick_points.x);

	real32 magnitude = MAX(::abs(sine(angle)), ::abs(cosine(angle)));
	real32 inverse_magnitude = 1.0f / magnitude;

	out_stick_euler_angles->yaw = (real32)(thumbstick_points.x * inverse_magnitude) * normalize_scale;
	out_stick_euler_angles->pitch = (real32)(thumbstick_points.y * inverse_magnitude) * normalize_scale;

	out_stick_euler_angles->yaw = PIN(out_stick_euler_angles->yaw, -1.f, 1.f);
	out_stick_euler_angles->pitch = PIN(out_stick_euler_angles->pitch, -1.f, 1.f);
	return;
}

static void input_abstraction_apply_raw_mouse_update(e_controller_index controller, s_game_input_state* input_state)
{
	s_gamepad_input_preferences* preference = &g_input_abstraction_globals->preferences[controller];
	s_saved_game_cartographer_player_profile* cartographer_player_profile = cartographer_player_profile_get_by_user_index(0);

	if (cartographer_player_profile->raw_mouse_input)
	{
		mouse_state* mouse_state = input_get_mouse_state();

		// ### FIXME this is fucking shit
		real32 raw_mouse_sensitivity = (cartographer_player_profile->raw_mouse_sensitivity / 100.f);

		input_abstraction_set_mouse_look_sensitivity(controller, 1.0f);
		input_state->mouse.yaw = (real32)-mouse_state->state.lX;
		input_state->mouse.pitch = (real32)-mouse_state->state.lY;

		// multiply by 0.016 milliseconds, while this is likely wrong
		// emulate current behaviour at all tickrates, instead of scaling with tick length lol
		// which is a higher value at 30 tick, resulting in higher mouse sensitivity
		input_state->mouse.yaw *= raw_mouse_sensitivity * (1.f / 60.f);
		input_state->mouse.pitch *= raw_mouse_sensitivity * (1.f / 60.f);

		if (preference->mouse_invert_look)
		{
			input_state->mouse.pitch = -0.0f - input_state->mouse.pitch;
		}
	}
	else
	{
		input_abstraction_set_mouse_look_sensitivity(controller, cartographer_player_profile_get_by_user_index(0)->mouse_sensitivity);
	}
	return;
}

static void input_abstraction_post_update_throttle(real_euler_angles2d* stick, real_angle angle, e_abstract_gamepad_stick_types stick_index)
{
	uint8 index = ((stick->pitch >= 0.0f) ? 0 : 2) + (stick->yaw < 0.0f);

	real_vector2d vec = { stick->yaw,stick->pitch };
	real32 magnitude = magnitude2d(&vec);

	real32 delta = ::abs(angle - k_gamepad_stick_angles[index]);
	real_angle min_delta = stick_index == _abstract_gamepad_stick_right ? DEGREES_TO_RADIANS(10.0f) : DEGREES_TO_RADIANS(35.0f);

	if (delta >= min_delta)
	{
		real32 sign = 0.0f;
		if (::abs(stick->yaw) <= ::abs(stick->pitch))
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
		constexpr real32 normalize_scale = 1.0f / DEGREES_TO_RADIANS(35.0f);

		real_angle angle_abs = ::abs(angle);
		real32 sign = 0.f;
		if (angle_abs < DEGREES_TO_RADIANS(45.0f) || (angle_abs > DEGREES_TO_RADIANS(135.0f)))
		{
			sign = (stick->yaw >= 0.0f) ? 1.0f : -1.0f;
			stick->yaw = sign * magnitude;

			sign = (stick->pitch >= 0.0f) ? 1.0f : -1.0f;
			stick->pitch = (1.0f - (delta * normalize_scale)) * sign * magnitude;
		}
		else
		{
			sign = (stick->pitch >= 0.0f) ? 1.0f : -1.0f;
			stick->pitch = sign * magnitude;

			sign = (stick->yaw >= 0.0f) ? 1.0f : -1.0f;
			stick->yaw = (1.0f - (delta * normalize_scale)) * sign * magnitude;
		}
	}
	return;
}

static void input_abstraction_post_update_all_throttles(real_euler_angles2d* left_stick, real_euler_angles2d* right_stick, point2d* lthumb, point2d* rthumb)
{
	bool adjust_left_stick = false;
	bool adjust_right_stick = false;

	e_abstract_gamepad_stick_types move_fwd_stick_type = input_abstraction_get_stick_type_for_action(_button_move_forward);
	e_abstract_gamepad_stick_types extended_yaw_left_stick_type = input_abstraction_get_stick_type_for_action(_extended_button_gamepad_yaw_left);
	e_abstract_gamepad_stick_types strafe_left_stick_type = input_abstraction_get_stick_type_for_action(_button_strafe_left);
	e_abstract_gamepad_stick_types extended_pitch_fwd_stick_type = input_abstraction_get_stick_type_for_action(_extended_button_gamepad_pitch_forward);

	if (move_fwd_stick_type == extended_yaw_left_stick_type && move_fwd_stick_type != _abstract_gamepad_stick_unknown)
	{
		if (move_fwd_stick_type == _abstract_gamepad_stick_right)
			adjust_right_stick = true;
		else
			adjust_left_stick = true;

	}
	if (strafe_left_stick_type == extended_pitch_fwd_stick_type && strafe_left_stick_type != _abstract_gamepad_stick_unknown)
	{
		if (strafe_left_stick_type == _abstract_gamepad_stick_left)
			adjust_left_stick = true;
		else
			adjust_right_stick = true;
	}

	real_angle left_stick_angle = arctangent((real32)lthumb->y, (real32)lthumb->x);
	real_angle right_stick_angle = arctangent((real32)rthumb->y, (real32)rthumb->x);

	if (adjust_left_stick)
	{
		input_abstraction_post_update_throttle(left_stick, left_stick_angle, _abstract_gamepad_stick_left);

		left_stick->yaw = PIN(left_stick->yaw, -1.0f, 1.0f);
		left_stick->pitch = PIN(left_stick->pitch, -1.0f, 1.0f);
	}

	if (adjust_right_stick)
	{
		input_abstraction_post_update_throttle(right_stick, right_stick_angle, _abstract_gamepad_stick_right);

		right_stick->yaw = PIN(right_stick->yaw, -1.0f, 1.0f);
		right_stick->pitch = PIN(right_stick->pitch, -1.0f, 1.0f);
	}
	return;
}

static void input_abstraction_update_throttles_legacy(s_gamepad_input_button_state* gamepad_state, real_euler_angles2d* left_stick, real_euler_angles2d* right_stick)
{
	input_abstraction_update_dead_zones(&gamepad_state->thumb_left, left_stick);
	input_abstraction_update_dead_zones(&gamepad_state->thumb_right, right_stick);

	input_abstraction_post_update_all_throttles(left_stick, right_stick, &gamepad_state->thumb_left, &gamepad_state->thumb_right);
	return;
}

static void input_abstraction_update_throttles_modern(s_gamepad_input_button_state* gamepad_state, real_euler_angles2d* left_stick, real_euler_angles2d* right_stick)
{
	constexpr real32 normalize_scale = 1.0f / INT16_MAX;

	left_stick->yaw = gamepad_state->thumb_left.x * normalize_scale;
	left_stick->pitch = gamepad_state->thumb_left.y * normalize_scale;
	right_stick->yaw = gamepad_state->thumb_right.x * normalize_scale;
	right_stick->pitch = gamepad_state->thumb_right.y * normalize_scale;

	left_stick->yaw = PIN(left_stick->yaw, -1.0f, 1.0f);
	left_stick->pitch = PIN(left_stick->pitch, -1.0f, 1.0f);

	right_stick->yaw = PIN(right_stick->yaw, -1.0f, 1.0f);
	right_stick->pitch = PIN(right_stick->pitch, -1.0f, 1.0f);
	return;
}

static void input_abstraction_set_global_input_preferences(s_gamepad_input_preferences* preference)
{
	preference->gamepad_yaw_rate = 120.f;
	preference->mouse_yaw_rate = 90.f;
	preference->gamepad_pitch_rate = 60.f;
	preference->mouse_pitch_rate = 45.f;
	preference->mouse_acceleration = 0.7f;
	preference->binary_yaw_rate = 1.f;
	preference->binary_pitch_rate = 1.f;
	preference->gamepad_axial_deadzone_left = { XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE,XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE };
	preference->stick_threshold = 0.25f;
	preference->gamepad_axial_deadzone_right = { XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE,XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE };
	preference->gamepad_invert_look = false;
	preference->mouse_invert_look = false;
	preference->invert_aircraft_control = false;
	preference->invert_dual_wield = true;
	preference->mouse_delta_threshold = 0.05f;
	preference->mouse_wheel_threshold = 0.05f;
	return;
}

static void input_abstraction_add_default_gamepad_baseline_controls(s_gamepad_input_preferences* preference)
{
	preference->game_controls_to_hardware[_button_start].add_button(_input_device_type_gamepad, _gamepad_binary_button_start, 0, false);
	preference->game_controls_to_hardware[_button_back].add_button(_input_device_type_gamepad, _gamepad_binary_button_back, 0, false);
	preference->game_controls_to_hardware[_button_crouch].add_button(_input_device_type_gamepad, _gamepad_binary_button_left_thumb, 0, false);
	preference->game_controls_to_hardware[_button_lean_left].add_button(_input_device_type_gamepad, _gamepad_binary_button_dpad_left, 0, false);
	preference->game_controls_to_hardware[_button_lean_right].add_button(_input_device_type_gamepad, _gamepad_binary_button_dpad_right, 0, false);
	preference->game_controls_to_hardware[_button_accept].add_button(_input_device_type_gamepad, _gamepad_binary_button_a, 0, false);
	preference->game_controls_to_hardware[_button_cancel].add_button(_input_device_type_gamepad, _gamepad_binary_button_b, 0, false);
	return;
}

static void input_abstraction_gamepad_add_default_preset_controls(s_gamepad_input_preferences* preference)
{
	preference->game_controls_to_hardware[_button_jump].add_button(_input_device_type_gamepad, _gamepad_binary_button_a, 0, false);
	preference->game_controls_to_hardware[_button_trick].add_button(_input_device_type_gamepad, _gamepad_binary_button_a, 0, false);
	preference->game_controls_to_hardware[_button_brake].add_button(_input_device_type_gamepad, _gamepad_binary_button_a, 0, false);
	preference->game_controls_to_hardware[_button_switch_grenade].add_button(_input_device_type_gamepad, _gamepad_binary_button_right_shoulder, 0, false);
	preference->game_controls_to_hardware[_button_flip_vehicle].add_button(_input_device_type_gamepad, _gamepad_binary_button_x, 0, false);
	preference->game_controls_to_hardware[_button_reload].add_button(_input_device_type_gamepad, _gamepad_binary_button_x, 0, false);
	preference->game_controls_to_hardware[_button_exit_vehicle].add_button(_input_device_type_gamepad, _gamepad_binary_button_x, 0, false);
	preference->game_controls_to_hardware[_button_touch_device].add_button(_input_device_type_gamepad, _gamepad_binary_button_x, k_key_hold_threshold_msec, false);
	preference->game_controls_to_hardware[_button_enter_vehicle].add_button(_input_device_type_gamepad, _gamepad_binary_button_x, k_key_hold_threshold_msec, false);
	preference->game_controls_to_hardware[_button_board_vehicle].add_button(_input_device_type_gamepad, _gamepad_binary_button_x, k_key_hold_threshold_msec, false);
	preference->game_controls_to_hardware[_button_evict_from_vehicle].add_button(_input_device_type_gamepad, _gamepad_binary_button_x, k_key_hold_threshold_msec, false);
	preference->game_controls_to_hardware[_button_trade_weapon].add_button(_input_device_type_gamepad, _gamepad_binary_button_x, k_key_hold_threshold_msec, false);
	preference->game_controls_to_hardware[_button_pick_up_primary_weapon].add_button(_input_device_type_gamepad, _gamepad_binary_button_x, k_key_hold_threshold_msec, false);
	preference->game_controls_to_hardware[_button_pick_up_primary_multiplayer_weapon].add_button(_input_device_type_gamepad, _gamepad_binary_button_x, k_key_hold_threshold_msec, false);
	preference->game_controls_to_hardware[_button_put_away_secondary_weapon].add_button(_input_device_type_gamepad, _gamepad_binary_button_y, 0, false);
	preference->game_controls_to_hardware[_button_put_away_or_drop_secondary_weapon].add_button(_input_device_type_gamepad, _gamepad_binary_button_y, 0, false);
	preference->game_controls_to_hardware[_button_switch_weapon].add_button(_input_device_type_gamepad, _gamepad_binary_button_y, 0, false);
	preference->game_controls_to_hardware[_button_pick_up_secondary_weapon].add_button(_input_device_type_gamepad, _gamepad_binary_button_y, k_key_hold_threshold_msec, false);
	preference->game_controls_to_hardware[_button_pick_up_secondary_multiplayer_weapon].add_button(_input_device_type_gamepad, _gamepad_binary_button_y, k_key_hold_threshold_msec, false);
	preference->game_controls_to_hardware[_button_team_voice].add_button(_input_device_type_gamepad, _gamepad_binary_button_left_shoulder, 0, false);
	preference->game_controls_to_hardware[_button_flashlight].add_button(_input_device_type_gamepad, _gamepad_binary_button_left_shoulder, 0, false);


	preference->game_controls_to_hardware[_button_melee_attack].add_button(_input_device_type_gamepad, _gamepad_binary_button_b, 0, false);
	preference->game_controls_to_hardware[_button_throw_grenade].add_button(_input_device_type_gamepad, _gamepad_analog_button_left_trigger, 0, false);
	preference->game_controls_to_hardware[_button_speed_boost].add_button(_input_device_type_gamepad, _gamepad_analog_button_left_trigger, 0, false);
	preference->game_controls_to_hardware[_button_e_brake].add_button(_input_device_type_gamepad, _gamepad_analog_button_left_trigger, 0, false);
	preference->game_controls_to_hardware[_button_fire].add_button(_input_device_type_gamepad, _gamepad_analog_button_right_trigger, 0, false);
	preference->game_controls_to_hardware[_button_dual_wield_primary_fire].add_button(_input_device_type_gamepad, _gamepad_analog_button_right_trigger, 0, false);
	preference->game_controls_to_hardware[_button_vehicle_primary_fire].add_button(_input_device_type_gamepad, _gamepad_analog_button_right_trigger, 0, false);
	preference->game_controls_to_hardware[_button_banshee_bomb].add_button(_input_device_type_gamepad, _gamepad_binary_button_b, 0, false);
	preference->game_controls_to_hardware[_button_scope_zoom].add_button(_input_device_type_gamepad, _gamepad_binary_button_right_thumb, 0, false);
	preference->game_controls_to_hardware[_button_dual_wield_secondary_fire].add_button(_input_device_type_gamepad, _gamepad_analog_button_left_trigger, 0, false);
	preference->game_controls_to_hardware[_button_vehicle_secondary_fire].add_button(_input_device_type_gamepad, _gamepad_analog_button_left_trigger, 0, false);
	return;
}

static void input_abstraction_gamepad_add_southpaw_preset_controls(s_gamepad_input_preferences* preference)
{
	preference->game_controls_to_hardware[_button_jump].add_button(_input_device_type_gamepad, _gamepad_binary_button_a, 0, false);
	preference->game_controls_to_hardware[_button_trick].add_button(_input_device_type_gamepad, _gamepad_binary_button_a, 0, false);
	preference->game_controls_to_hardware[_button_brake].add_button(_input_device_type_gamepad, _gamepad_binary_button_a, 0, false);
	preference->game_controls_to_hardware[_button_switch_grenade].add_button(_input_device_type_gamepad, _gamepad_binary_button_right_shoulder, 0, false);
	preference->game_controls_to_hardware[_button_flip_vehicle].add_button(_input_device_type_gamepad, _gamepad_binary_button_x, 0, false);
	preference->game_controls_to_hardware[_button_reload].add_button(_input_device_type_gamepad, _gamepad_binary_button_x, 0, false);
	preference->game_controls_to_hardware[_button_exit_vehicle].add_button(_input_device_type_gamepad, _gamepad_binary_button_x, 0, false);
	preference->game_controls_to_hardware[_button_touch_device].add_button(_input_device_type_gamepad, _gamepad_binary_button_x, k_key_hold_threshold_msec, false);
	preference->game_controls_to_hardware[_button_enter_vehicle].add_button(_input_device_type_gamepad, _gamepad_binary_button_x, k_key_hold_threshold_msec, false);
	preference->game_controls_to_hardware[_button_board_vehicle].add_button(_input_device_type_gamepad, _gamepad_binary_button_x, k_key_hold_threshold_msec, false);
	preference->game_controls_to_hardware[_button_evict_from_vehicle].add_button(_input_device_type_gamepad, _gamepad_binary_button_x, k_key_hold_threshold_msec, false);
	preference->game_controls_to_hardware[_button_trade_weapon].add_button(_input_device_type_gamepad, _gamepad_binary_button_x, k_key_hold_threshold_msec, false);
	preference->game_controls_to_hardware[_button_pick_up_primary_weapon].add_button(_input_device_type_gamepad, _gamepad_binary_button_x, k_key_hold_threshold_msec, false);
	preference->game_controls_to_hardware[_button_pick_up_primary_multiplayer_weapon].add_button(_input_device_type_gamepad, _gamepad_binary_button_x, k_key_hold_threshold_msec, false);
	preference->game_controls_to_hardware[_button_put_away_secondary_weapon].add_button(_input_device_type_gamepad, _gamepad_binary_button_y, 0, false);
	preference->game_controls_to_hardware[_button_put_away_or_drop_secondary_weapon].add_button(_input_device_type_gamepad, _gamepad_binary_button_y, 0, false);
	preference->game_controls_to_hardware[_button_switch_weapon].add_button(_input_device_type_gamepad, _gamepad_binary_button_y, 0, false);
	preference->game_controls_to_hardware[_button_pick_up_secondary_weapon].add_button(_input_device_type_gamepad, _gamepad_binary_button_y, k_key_hold_threshold_msec, false);
	preference->game_controls_to_hardware[_button_pick_up_secondary_multiplayer_weapon].add_button(_input_device_type_gamepad, _gamepad_binary_button_y, k_key_hold_threshold_msec, false);
	preference->game_controls_to_hardware[_button_team_voice].add_button(_input_device_type_gamepad, _gamepad_binary_button_left_shoulder, 0, false);
	preference->game_controls_to_hardware[_button_flashlight].add_button(_input_device_type_gamepad, _gamepad_binary_button_left_shoulder, 0, false);


	preference->game_controls_to_hardware[_button_melee_attack].add_button(_input_device_type_gamepad, _gamepad_binary_button_b, 0, false);
	preference->game_controls_to_hardware[_button_throw_grenade].add_button(_input_device_type_gamepad, _gamepad_analog_button_right_trigger, 0, false);
	preference->game_controls_to_hardware[_button_speed_boost].add_button(_input_device_type_gamepad, _gamepad_analog_button_right_trigger, 0, false);
	preference->game_controls_to_hardware[_button_e_brake].add_button(_input_device_type_gamepad, _gamepad_analog_button_right_trigger, 0, false);
	preference->game_controls_to_hardware[_button_fire].add_button(_input_device_type_gamepad, _gamepad_analog_button_left_trigger, 0, false);
	preference->game_controls_to_hardware[_button_dual_wield_primary_fire].add_button(_input_device_type_gamepad, _gamepad_analog_button_left_trigger, 0, false);
	preference->game_controls_to_hardware[_button_vehicle_primary_fire].add_button(_input_device_type_gamepad, _gamepad_analog_button_left_trigger, 0, false);
	preference->game_controls_to_hardware[_button_banshee_bomb].add_button(_input_device_type_gamepad, _gamepad_binary_button_b, 0, false);
	preference->game_controls_to_hardware[_button_scope_zoom].add_button(_input_device_type_gamepad, _gamepad_binary_button_right_thumb, 0, false);
	preference->game_controls_to_hardware[_button_dual_wield_secondary_fire].add_button(_input_device_type_gamepad, _gamepad_analog_button_right_trigger, 0, false);
	preference->game_controls_to_hardware[_button_vehicle_secondary_fire].add_button(_input_device_type_gamepad, _gamepad_analog_button_right_trigger, 0, false);
	return;
}

static void input_abstraction_gamepad_add_boxer_preset_controls(s_gamepad_input_preferences* preference)
{
	preference->game_controls_to_hardware[_button_jump].add_button(_input_device_type_gamepad, _gamepad_binary_button_a, 0, false);
	preference->game_controls_to_hardware[_button_trick].add_button(_input_device_type_gamepad, _gamepad_binary_button_a, 0, false);
	preference->game_controls_to_hardware[_button_brake].add_button(_input_device_type_gamepad, _gamepad_binary_button_a, 0, false);
	preference->game_controls_to_hardware[_button_switch_grenade].add_button(_input_device_type_gamepad, _gamepad_binary_button_right_shoulder, 0, false);
	preference->game_controls_to_hardware[_button_flip_vehicle].add_button(_input_device_type_gamepad, _gamepad_binary_button_x, 0, false);
	preference->game_controls_to_hardware[_button_reload].add_button(_input_device_type_gamepad, _gamepad_binary_button_x, 0, false);
	preference->game_controls_to_hardware[_button_exit_vehicle].add_button(_input_device_type_gamepad, _gamepad_binary_button_x, 0, false);
	preference->game_controls_to_hardware[_button_touch_device].add_button(_input_device_type_gamepad, _gamepad_binary_button_x, k_key_hold_threshold_msec, false);
	preference->game_controls_to_hardware[_button_enter_vehicle].add_button(_input_device_type_gamepad, _gamepad_binary_button_x, k_key_hold_threshold_msec, false);
	preference->game_controls_to_hardware[_button_board_vehicle].add_button(_input_device_type_gamepad, _gamepad_binary_button_x, k_key_hold_threshold_msec, false);
	preference->game_controls_to_hardware[_button_evict_from_vehicle].add_button(_input_device_type_gamepad, _gamepad_binary_button_x, k_key_hold_threshold_msec, false);
	preference->game_controls_to_hardware[_button_trade_weapon].add_button(_input_device_type_gamepad, _gamepad_binary_button_x, k_key_hold_threshold_msec, false);
	preference->game_controls_to_hardware[_button_pick_up_primary_weapon].add_button(_input_device_type_gamepad, _gamepad_binary_button_x, k_key_hold_threshold_msec, false);
	preference->game_controls_to_hardware[_button_pick_up_primary_multiplayer_weapon].add_button(_input_device_type_gamepad, _gamepad_binary_button_x, k_key_hold_threshold_msec, false);
	preference->game_controls_to_hardware[_button_put_away_secondary_weapon].add_button(_input_device_type_gamepad, _gamepad_binary_button_y, 0, false);
	preference->game_controls_to_hardware[_button_put_away_or_drop_secondary_weapon].add_button(_input_device_type_gamepad, _gamepad_binary_button_y, 0, false);
	preference->game_controls_to_hardware[_button_switch_weapon].add_button(_input_device_type_gamepad, _gamepad_binary_button_y, 0, false);
	preference->game_controls_to_hardware[_button_pick_up_secondary_weapon].add_button(_input_device_type_gamepad, _gamepad_binary_button_y, k_key_hold_threshold_msec, false);
	preference->game_controls_to_hardware[_button_pick_up_secondary_multiplayer_weapon].add_button(_input_device_type_gamepad, _gamepad_binary_button_y, k_key_hold_threshold_msec, false);
	preference->game_controls_to_hardware[_button_team_voice].add_button(_input_device_type_gamepad, _gamepad_binary_button_left_shoulder, 0, false);
	preference->game_controls_to_hardware[_button_flashlight].add_button(_input_device_type_gamepad, _gamepad_binary_button_left_shoulder, 0, false);


	preference->game_controls_to_hardware[_button_melee_attack].add_button(_input_device_type_gamepad, _gamepad_analog_button_left_trigger, 0, false);
	preference->game_controls_to_hardware[_button_throw_grenade].add_button(_input_device_type_gamepad, _gamepad_binary_button_b, 0, false);
	preference->game_controls_to_hardware[_button_speed_boost].add_button(_input_device_type_gamepad, _gamepad_analog_button_left_trigger, 0, false);
	preference->game_controls_to_hardware[_button_e_brake].add_button(_input_device_type_gamepad, _gamepad_analog_button_left_trigger, 0, false);
	preference->game_controls_to_hardware[_button_fire].add_button(_input_device_type_gamepad, _gamepad_analog_button_right_trigger, 0, false);
	preference->game_controls_to_hardware[_button_dual_wield_primary_fire].add_button(_input_device_type_gamepad, _gamepad_analog_button_right_trigger, 0, false);
	preference->game_controls_to_hardware[_button_vehicle_primary_fire].add_button(_input_device_type_gamepad, _gamepad_analog_button_right_trigger, 0, false);
	preference->game_controls_to_hardware[_button_banshee_bomb].add_button(_input_device_type_gamepad, _gamepad_binary_button_b, 0, false);
	preference->game_controls_to_hardware[_button_scope_zoom].add_button(_input_device_type_gamepad, _gamepad_binary_button_right_thumb, 0, false);
	preference->game_controls_to_hardware[_button_dual_wield_secondary_fire].add_button(_input_device_type_gamepad, _gamepad_analog_button_left_trigger, 0, false);
	preference->game_controls_to_hardware[_button_vehicle_secondary_fire].add_button(_input_device_type_gamepad, _gamepad_analog_button_left_trigger, 0, false);
	return;
}

static void input_abstraction_gamepad_add_greenthumb_preset_controls(s_gamepad_input_preferences* preference)
{
	preference->game_controls_to_hardware[_button_jump].add_button(_input_device_type_gamepad, _gamepad_binary_button_a, 0, false);
	preference->game_controls_to_hardware[_button_trick].add_button(_input_device_type_gamepad, _gamepad_binary_button_a, 0, false);
	preference->game_controls_to_hardware[_button_brake].add_button(_input_device_type_gamepad, _gamepad_binary_button_a, 0, false);
	preference->game_controls_to_hardware[_button_switch_grenade].add_button(_input_device_type_gamepad, _gamepad_binary_button_right_shoulder, 0, false);
	preference->game_controls_to_hardware[_button_flip_vehicle].add_button(_input_device_type_gamepad, _gamepad_binary_button_x, 0, false);
	preference->game_controls_to_hardware[_button_reload].add_button(_input_device_type_gamepad, _gamepad_binary_button_x, 0, false);
	preference->game_controls_to_hardware[_button_exit_vehicle].add_button(_input_device_type_gamepad, _gamepad_binary_button_x, 0, false);
	preference->game_controls_to_hardware[_button_touch_device].add_button(_input_device_type_gamepad, _gamepad_binary_button_x, k_key_hold_threshold_msec, false);
	preference->game_controls_to_hardware[_button_enter_vehicle].add_button(_input_device_type_gamepad, _gamepad_binary_button_x, k_key_hold_threshold_msec, false);
	preference->game_controls_to_hardware[_button_board_vehicle].add_button(_input_device_type_gamepad, _gamepad_binary_button_x, k_key_hold_threshold_msec, false);
	preference->game_controls_to_hardware[_button_evict_from_vehicle].add_button(_input_device_type_gamepad, _gamepad_binary_button_x, k_key_hold_threshold_msec, false);
	preference->game_controls_to_hardware[_button_trade_weapon].add_button(_input_device_type_gamepad, _gamepad_binary_button_x, k_key_hold_threshold_msec, false);
	preference->game_controls_to_hardware[_button_pick_up_primary_weapon].add_button(_input_device_type_gamepad, _gamepad_binary_button_x, k_key_hold_threshold_msec, false);
	preference->game_controls_to_hardware[_button_pick_up_primary_multiplayer_weapon].add_button(_input_device_type_gamepad, _gamepad_binary_button_x, k_key_hold_threshold_msec, false);
	preference->game_controls_to_hardware[_button_put_away_secondary_weapon].add_button(_input_device_type_gamepad, _gamepad_binary_button_y, 0, false);
	preference->game_controls_to_hardware[_button_put_away_or_drop_secondary_weapon].add_button(_input_device_type_gamepad, _gamepad_binary_button_y, 0, false);
	preference->game_controls_to_hardware[_button_switch_weapon].add_button(_input_device_type_gamepad, _gamepad_binary_button_y, 0, false);
	preference->game_controls_to_hardware[_button_pick_up_secondary_weapon].add_button(_input_device_type_gamepad, _gamepad_binary_button_y, k_key_hold_threshold_msec, false);
	preference->game_controls_to_hardware[_button_pick_up_secondary_multiplayer_weapon].add_button(_input_device_type_gamepad, _gamepad_binary_button_y, k_key_hold_threshold_msec, false);
	preference->game_controls_to_hardware[_button_team_voice].add_button(_input_device_type_gamepad, _gamepad_binary_button_left_shoulder, 0, false);
	preference->game_controls_to_hardware[_button_flashlight].add_button(_input_device_type_gamepad, _gamepad_binary_button_left_shoulder, 0, false);


	preference->game_controls_to_hardware[_button_melee_attack].add_button(_input_device_type_gamepad, _gamepad_binary_button_right_thumb, 0, false);
	preference->game_controls_to_hardware[_button_throw_grenade].add_button(_input_device_type_gamepad, _gamepad_analog_button_left_trigger, 0, false);
	preference->game_controls_to_hardware[_button_speed_boost].add_button(_input_device_type_gamepad, _gamepad_analog_button_left_trigger, 0, false);
	preference->game_controls_to_hardware[_button_e_brake].add_button(_input_device_type_gamepad, _gamepad_analog_button_left_trigger, 0, false);
	preference->game_controls_to_hardware[_button_fire].add_button(_input_device_type_gamepad, _gamepad_analog_button_right_trigger, 0, false);
	preference->game_controls_to_hardware[_button_dual_wield_primary_fire].add_button(_input_device_type_gamepad, _gamepad_analog_button_right_trigger, 0, false);
	preference->game_controls_to_hardware[_button_vehicle_primary_fire].add_button(_input_device_type_gamepad, _gamepad_analog_button_right_trigger, 0, false);
	preference->game_controls_to_hardware[_button_banshee_bomb].add_button(_input_device_type_gamepad, _gamepad_binary_button_b, 0, false);
	preference->game_controls_to_hardware[_button_scope_zoom].add_button(_input_device_type_gamepad, _gamepad_binary_button_b, 0, false);
	preference->game_controls_to_hardware[_button_dual_wield_secondary_fire].add_button(_input_device_type_gamepad, _gamepad_analog_button_left_trigger, 0, false);
	preference->game_controls_to_hardware[_button_vehicle_secondary_fire].add_button(_input_device_type_gamepad, _gamepad_analog_button_left_trigger, 0, false);
	return;
}

static void input_abstraction_gamepad_add_jumpy_preset_controls(s_gamepad_input_preferences* preference)
{
	preference->game_controls_to_hardware[_button_jump].add_button(_input_device_type_gamepad, _gamepad_binary_button_left_shoulder, 0, false);
	preference->game_controls_to_hardware[_button_trick].add_button(_input_device_type_gamepad, _gamepad_binary_button_left_shoulder, 0, false);
	preference->game_controls_to_hardware[_button_brake].add_button(_input_device_type_gamepad, _gamepad_binary_button_left_shoulder, 0, false);
	preference->game_controls_to_hardware[_button_switch_grenade].add_button(_input_device_type_gamepad, _gamepad_binary_button_dpad_right, 0, false);
	preference->game_controls_to_hardware[_button_switch_grenade].add_button(_input_device_type_gamepad, _gamepad_binary_button_a, 0, false);
	preference->game_controls_to_hardware[_button_flip_vehicle].add_button(_input_device_type_gamepad, _gamepad_binary_button_b, 0, false);
	preference->game_controls_to_hardware[_button_reload].add_button(_input_device_type_gamepad, _gamepad_binary_button_b, 0, false);
	preference->game_controls_to_hardware[_button_exit_vehicle].add_button(_input_device_type_gamepad, _gamepad_binary_button_b, 0, false);
	preference->game_controls_to_hardware[_button_touch_device].add_button(_input_device_type_gamepad, _gamepad_binary_button_b, k_key_hold_threshold_msec, false);
	preference->game_controls_to_hardware[_button_enter_vehicle].add_button(_input_device_type_gamepad, _gamepad_binary_button_b, k_key_hold_threshold_msec, false);
	preference->game_controls_to_hardware[_button_board_vehicle].add_button(_input_device_type_gamepad, _gamepad_binary_button_b, k_key_hold_threshold_msec, false);
	preference->game_controls_to_hardware[_button_evict_from_vehicle].add_button(_input_device_type_gamepad, _gamepad_binary_button_b, k_key_hold_threshold_msec, false);
	preference->game_controls_to_hardware[_button_trade_weapon].add_button(_input_device_type_gamepad, _gamepad_binary_button_b, k_key_hold_threshold_msec, false);
	preference->game_controls_to_hardware[_button_pick_up_primary_weapon].add_button(_input_device_type_gamepad, _gamepad_binary_button_b, k_key_hold_threshold_msec, false);
	preference->game_controls_to_hardware[_button_pick_up_primary_multiplayer_weapon].add_button(_input_device_type_gamepad, _gamepad_binary_button_b, k_key_hold_threshold_msec, false);
	preference->game_controls_to_hardware[_button_put_away_secondary_weapon].add_button(_input_device_type_gamepad, _gamepad_binary_button_y, 0, false);
	preference->game_controls_to_hardware[_button_put_away_or_drop_secondary_weapon].add_button(_input_device_type_gamepad, _gamepad_binary_button_y, 0, false);
	preference->game_controls_to_hardware[_button_switch_weapon].add_button(_input_device_type_gamepad, _gamepad_binary_button_y, 0, false);
	preference->game_controls_to_hardware[_button_pick_up_secondary_weapon].add_button(_input_device_type_gamepad, _gamepad_binary_button_a, k_key_hold_threshold_msec, false);
	preference->game_controls_to_hardware[_button_pick_up_secondary_multiplayer_weapon].add_button(_input_device_type_gamepad, _gamepad_binary_button_a, k_key_hold_threshold_msec, false);
	preference->game_controls_to_hardware[_button_team_voice].add_button(_input_device_type_gamepad, _gamepad_binary_button_dpad_up, 0, false);
	preference->game_controls_to_hardware[_button_flashlight].add_button(_input_device_type_gamepad, _gamepad_binary_button_dpad_up, 0, false);


	preference->game_controls_to_hardware[_button_melee_attack].add_button(_input_device_type_gamepad, _gamepad_binary_button_right_shoulder, 0, false);
	preference->game_controls_to_hardware[_button_throw_grenade].add_button(_input_device_type_gamepad, _gamepad_analog_button_left_trigger, 0, false);
	preference->game_controls_to_hardware[_button_speed_boost].add_button(_input_device_type_gamepad, _gamepad_analog_button_left_trigger, 0, false);
	preference->game_controls_to_hardware[_button_e_brake].add_button(_input_device_type_gamepad, _gamepad_analog_button_left_trigger, 0, false);
	preference->game_controls_to_hardware[_button_fire].add_button(_input_device_type_gamepad, _gamepad_analog_button_right_trigger, 0, false);
	preference->game_controls_to_hardware[_button_dual_wield_primary_fire].add_button(_input_device_type_gamepad, _gamepad_analog_button_right_trigger, 0, false);
	preference->game_controls_to_hardware[_button_vehicle_primary_fire].add_button(_input_device_type_gamepad, _gamepad_analog_button_right_trigger, 0, false);
	preference->game_controls_to_hardware[_button_banshee_bomb].add_button(_input_device_type_gamepad, _gamepad_binary_button_right_shoulder, 0, false);
	preference->game_controls_to_hardware[_button_scope_zoom].add_button(_input_device_type_gamepad, _gamepad_binary_button_right_thumb, 0, false);
	preference->game_controls_to_hardware[_button_dual_wield_secondary_fire].add_button(_input_device_type_gamepad, _gamepad_analog_button_left_trigger, 0, false);
	preference->game_controls_to_hardware[_button_vehicle_secondary_fire].add_button(_input_device_type_gamepad, _gamepad_analog_button_left_trigger, 0, false);
	return;
}

static void input_abstraction_gamepad_set_default_stick_controls(s_gamepad_input_preferences* preference, e_joystick_preset_types thumbstick_layout)
{
	e_gamepad_buttons fwd_key = _gamepad_analog_right_stick_up;
	e_gamepad_buttons bkwd_key = _gamepad_analog_right_stick_down;
	e_gamepad_buttons pitch_fwd_key = _gamepad_analog_left_stick_up;
	e_gamepad_buttons pitch_bkwd_key = _gamepad_analog_left_stick_down;

	e_gamepad_buttons strafe_left_key = _gamepad_analog_right_stick_left;
	e_gamepad_buttons strafe_right_key = _gamepad_analog_right_stick_right;
	e_gamepad_buttons yaw_left_key = _gamepad_analog_left_stick_left;
	e_gamepad_buttons yaw_right_key = _gamepad_analog_left_stick_right;

	if (thumbstick_layout == _joystick_preset_default || thumbstick_layout == _joystick_preset_legacy)
	{
		fwd_key = _gamepad_analog_left_stick_up;
		bkwd_key = _gamepad_analog_left_stick_down;
		pitch_fwd_key = _gamepad_analog_right_stick_up;
		pitch_bkwd_key = _gamepad_analog_right_stick_down;
	}

	if (thumbstick_layout == _joystick_preset_default || thumbstick_layout == _joystick_preset_legacy_south_paw)
	{
		strafe_left_key = _gamepad_analog_left_stick_left;
		strafe_right_key = _gamepad_analog_left_stick_right;
		yaw_left_key = _gamepad_analog_right_stick_left;
		yaw_right_key = _gamepad_analog_right_stick_right;
	}

	preference->game_controls_to_hardware[_button_move_forward].add_button(_input_device_type_gamepad, fwd_key, 0, false);
	preference->game_controls_to_hardware[_button_move_backward].add_button(_input_device_type_gamepad, bkwd_key, 0, false);
	preference->game_controls_to_hardware[_button_strafe_left].add_button(_input_device_type_gamepad, strafe_left_key, 0, false);
	preference->game_controls_to_hardware[_button_strafe_right].add_button(_input_device_type_gamepad, strafe_right_key, 0, false);
	preference->game_controls_to_hardware[_extended_button_gamepad_pitch_forward].add_button(_input_device_type_gamepad, pitch_fwd_key, 0, false);
	preference->game_controls_to_hardware[_extended_button_gamepad_pitch_backward].add_button(_input_device_type_gamepad, pitch_bkwd_key, 0, false);
	preference->game_controls_to_hardware[_extended_button_gamepad_yaw_left].add_button(_input_device_type_gamepad, yaw_left_key, 0, false);
	preference->game_controls_to_hardware[_extended_button_gamepad_yaw_right].add_button(_input_device_type_gamepad, yaw_right_key, 0, false);
	return;
}

static void input_abstraction_keyboard_and_mouse_add_default_controls(s_gamepad_input_preferences* preference, e_language language)
{
	preference->game_controls_to_hardware[_button_start].add_button(_input_device_type_keyboard, VK_PAUSE, 0, 0);
	preference->game_controls_to_hardware[_button_start].add_button(_input_device_type_keyboard, VK_ESCAPE, 0, 0);
	preference->game_controls_to_hardware[_button_back].add_button(_input_device_type_keyboard, VK_BACK, 0, 0);


	preference->game_controls_to_hardware[_button_accept].add_button(_input_device_type_keyboard, HELPER_GET_INT_KEY('A', language), 0, 0);
	preference->game_controls_to_hardware[_button_accept].add_button(_input_device_type_keyboard, VK_RETURN, 0, 0);
	preference->game_controls_to_hardware[_button_accept].add_button(_input_device_type_keyboard, VK_SPACE, 0, 0);
	preference->game_controls_to_hardware[_button_accept].add_button(_input_device_type_mouse, _mouse_button_left, 0, 0);
	preference->game_controls_to_hardware[_button_ui_scroll_up].add_button(_input_device_type_mouse, _mouse_delta_w_forward, 0, 0);
	preference->game_controls_to_hardware[_button_ui_scroll_down].add_button(_input_device_type_mouse, _mouse_delta_w_back, 0, 0);
	return;
}

static void input_abstraction_keyboard_and_mouse_set_default_mapping(s_gamepad_input_preferences* preference, e_language language, bool kb_type_legacy)
{
	preference->game_controls_to_hardware[_button_back].add_button(_input_device_type_keyboard, (kb_type_legacy ? 'C' : 'Y'), 0, 0);
	preference->game_controls_to_hardware[_button_mouse_pitch_forward].add_button(_input_device_type_mouse, _mouse_delta_w_forward, 0, 0);
	preference->game_controls_to_hardware[_button_mouse_pitch_backward].add_button(_input_device_type_mouse, _mouse_delta_y_down, 0, 0);
	preference->game_controls_to_hardware[_button_mouse_yaw_left].add_button(_input_device_type_mouse, _mouse_delta_x_left, 0, 0);
	preference->game_controls_to_hardware[_button_mouse_yaw_right].add_button(_input_device_type_mouse, _mouse_delta_x_right, 0, 0);
	preference->game_controls_to_hardware[_button_move_forward].add_button(_input_device_type_keyboard, (kb_type_legacy ? HELPER_GET_INT_KEY('W', language) : 'I'), 0, 0);
	preference->game_controls_to_hardware[_button_move_backward].add_button(_input_device_type_keyboard, (kb_type_legacy ? 'S' : HELPER_GET_INT_KEY('K', language)), 0, 0);
	preference->game_controls_to_hardware[_button_strafe_left].add_button(_input_device_type_keyboard, (kb_type_legacy ? HELPER_GET_INT_KEY('A', language) : 'J'), 0, 0);
	preference->game_controls_to_hardware[_button_strafe_right].add_button(_input_device_type_keyboard, (kb_type_legacy ? 'D' : HELPER_GET_INT_KEY('L', language)), 0, 0);
	preference->game_controls_to_hardware[_button_move_forward].add_button(_input_device_type_keyboard, VK_UP, 0, 0);
	preference->game_controls_to_hardware[_button_move_backward].add_button(_input_device_type_keyboard, VK_DOWN, 0, 0);
	preference->game_controls_to_hardware[_button_move_backward].add_button(_input_device_type_keyboard, VK_DOWN, 0, 0);
	preference->game_controls_to_hardware[_button_strafe_left].add_button(_input_device_type_keyboard, VK_LEFT, 0, 0);
	preference->game_controls_to_hardware[_button_strafe_right].add_button(_input_device_type_keyboard, VK_RIGHT, 0, 0);
	preference->game_controls_to_hardware[_button_crouch].add_button(_input_device_type_keyboard, (kb_type_legacy ? VK_LSHIFT : VK_RSHIFT), 0, 0);
	preference->game_controls_to_hardware[_button_jump].add_button(_input_device_type_keyboard, VK_SPACE, 0, 0);
	preference->game_controls_to_hardware[_button_brake].add_button(_input_device_type_keyboard, VK_SPACE, 0, 0);
	preference->game_controls_to_hardware[_button_trick].add_button(_input_device_type_keyboard, VK_SPACE, 0, 0);
	preference->game_controls_to_hardware[_button_text_chat_team].add_button(_input_device_type_keyboard, (kb_type_legacy ? 'B' : 'V'), 0, 0);
	preference->game_controls_to_hardware[_button_text_chat_toggle].add_button(_input_device_type_keyboard, VK_F1, 0, 0);
	preference->game_controls_to_hardware[_button_team_voice].add_button(_input_device_type_keyboard, (kb_type_legacy ? HELPER_GET_INT_KEY('N', language) : HELPER_GET_INT_KEY('B', language)), 0, 0);
	preference->game_controls_to_hardware[_button_cancel].add_button(_input_device_type_keyboard, 'B', 0, 0);
	return;
}

static void input_abstraction_keyboard_set_normal_default_mapping(s_gamepad_input_preferences* preference, e_language language, bool kb_type_legacy)
{
	preference->game_controls_to_hardware[_button_reload].add_button(_input_device_type_keyboard, (kb_type_legacy ? 'R' : 'U'), 0, 0);
	preference->game_controls_to_hardware[_button_pick_up_secondary_weapon].add_button(_input_device_type_keyboard, (kb_type_legacy ? HELPER_GET_INT_KEY('Q', language) : HELPER_GET_INT_KEY('O', language)), k_key_hold_threshold_msec, 0);
	preference->game_controls_to_hardware[_button_pick_up_secondary_multiplayer_weapon].add_button(_input_device_type_keyboard, (kb_type_legacy ? HELPER_GET_INT_KEY('Q', language) : HELPER_GET_INT_KEY('O', language)), k_key_hold_threshold_msec, 0);
	preference->game_controls_to_hardware[_button_put_away_secondary_weapon].add_button(_input_device_type_keyboard, (kb_type_legacy ? HELPER_GET_INT_KEY('Q', language) : 'O'), k_key_hold_threshold_msec, 0);
	preference->game_controls_to_hardware[_button_put_away_or_drop_secondary_weapon].add_button(_input_device_type_keyboard, (kb_type_legacy ? HELPER_GET_INT_KEY('Q', language) : 'O'), k_key_hold_threshold_msec, 0);
	preference->game_controls_to_hardware[_button_pick_up_primary_weapon].add_button(_input_device_type_keyboard, (kb_type_legacy ? 'E' : 'U'), k_key_hold_threshold_msec, 0);
	preference->game_controls_to_hardware[_button_pick_up_primary_multiplayer_weapon].add_button(_input_device_type_keyboard, (kb_type_legacy ? 'E' : 'U'), k_key_hold_threshold_msec, 0);
	preference->game_controls_to_hardware[_button_trade_weapon].add_button(_input_device_type_keyboard, (kb_type_legacy ? 'E' : 'U'), k_key_hold_threshold_msec, 0);
	preference->game_controls_to_hardware[_button_switch_weapon].add_button(_input_device_type_keyboard, (kb_type_legacy ? HELPER_GET_INT_KEY('Q', language) : 'O'), 0, 0);
	preference->game_controls_to_hardware[_button_melee_attack].add_button(_input_device_type_keyboard, (kb_type_legacy ? 'F' : 'H'), 0, 0);
	preference->game_controls_to_hardware[_button_enter_vehicle].add_button(_input_device_type_keyboard, (kb_type_legacy ? 'E' : 'U'), k_key_hold_threshold_msec, 0);
	preference->game_controls_to_hardware[_button_board_vehicle].add_button(_input_device_type_keyboard, (kb_type_legacy ? 'E' : 'U'), k_key_hold_threshold_msec, 0);
	preference->game_controls_to_hardware[_button_evict_from_vehicle].add_button(_input_device_type_keyboard, (kb_type_legacy ? 'E' : 'U'), k_key_hold_threshold_msec, 0);
	preference->game_controls_to_hardware[_button_exit_vehicle].add_button(_input_device_type_keyboard, (kb_type_legacy ? 'E' : 'U'), 0, 0);
	preference->game_controls_to_hardware[_button_flip_vehicle].add_button(_input_device_type_keyboard, (kb_type_legacy ? 'E' : 'U'), 0, 0);
	preference->game_controls_to_hardware[_button_touch_device].add_button(_input_device_type_keyboard, (kb_type_legacy ? 'E' : 'U'), k_key_hold_threshold_msec, 0);
	preference->game_controls_to_hardware[_button_throw_grenade].add_button(_input_device_type_keyboard, (kb_type_legacy ? HELPER_GET_INT_KEY(VK_TAB, language) : HELPER_GET_INT_KEY('P', language)), 0, 0);
	preference->game_controls_to_hardware[_button_switch_grenade].add_button(_input_device_type_keyboard, (kb_type_legacy ? 'G' : HELPER_GET_INT_KEY(0x10C, language)), 0, 0);
	preference->game_controls_to_hardware[_button_flashlight].add_button(_input_device_type_keyboard, (kb_type_legacy ? 'X' : VK_RMENU), 0, 0);
	return;
}

static void input_abstraction_keyboard_set_split_default_mapping(s_gamepad_input_preferences* preference, e_language language, bool kb_type_legacy)
{
	preference->game_controls_to_hardware[_button_reload].add_button(_input_device_type_keyboard, (kb_type_legacy ? 'R' : 'U'), 0, 0);
	preference->game_controls_to_hardware[_button_pick_up_secondary_weapon].add_button(_input_device_type_keyboard, (kb_type_legacy ? 'E' : 'O'), k_key_hold_threshold_msec, 0);
	preference->game_controls_to_hardware[_button_pick_up_secondary_multiplayer_weapon].add_button(_input_device_type_keyboard, (kb_type_legacy ? 'E' : 'O'), k_key_hold_threshold_msec, 0);
	preference->game_controls_to_hardware[_button_put_away_secondary_weapon].add_button(_input_device_type_keyboard, (kb_type_legacy ? 'E' : 'O'), k_key_hold_threshold_msec, 0);
	preference->game_controls_to_hardware[_button_put_away_or_drop_secondary_weapon].add_button(_input_device_type_keyboard, (kb_type_legacy ? 'E' : 'O'), k_key_hold_threshold_msec, 0);
	preference->game_controls_to_hardware[_button_pick_up_primary_weapon].add_button(_input_device_type_keyboard, (kb_type_legacy ? HELPER_GET_INT_KEY('Q', language) : HELPER_GET_INT_KEY(0x109, language)), 0, 0);
	preference->game_controls_to_hardware[_button_pick_up_primary_multiplayer_weapon].add_button(_input_device_type_keyboard, (kb_type_legacy ? 'Q' : HELPER_GET_INT_KEY(0x109, language)), 0, 0);
	preference->game_controls_to_hardware[_button_trade_weapon].add_button(_input_device_type_keyboard, (kb_type_legacy ? HELPER_GET_INT_KEY('Q', language) : HELPER_GET_INT_KEY(0x109, language)), 0, 0);
	preference->game_controls_to_hardware[_button_switch_weapon].add_button(_input_device_type_keyboard, (kb_type_legacy ? 'E' : HELPER_GET_INT_KEY(0x10A, language)), 0, 0);
	preference->game_controls_to_hardware[_button_melee_attack].add_button(_input_device_type_keyboard, (kb_type_legacy ? 'F' : 0x10B), 0, 0);
	preference->game_controls_to_hardware[_button_enter_vehicle].add_button(_input_device_type_keyboard, (kb_type_legacy ? 'T' : HELPER_GET_INT_KEY(0x10E, language)), 0, 0);
	preference->game_controls_to_hardware[_button_board_vehicle].add_button(_input_device_type_keyboard, (kb_type_legacy ? 'T' : HELPER_GET_INT_KEY(0x10E, language)), 0, 0);
	preference->game_controls_to_hardware[_button_evict_from_vehicle].add_button(_input_device_type_keyboard, (kb_type_legacy ? 'T' : HELPER_GET_INT_KEY(0x10E, language)), 0, 0);
	preference->game_controls_to_hardware[_button_exit_vehicle].add_button(_input_device_type_keyboard, (kb_type_legacy ? 'T' : HELPER_GET_INT_KEY(0x10E, language)), 0, 0);
	preference->game_controls_to_hardware[_button_flip_vehicle].add_button(_input_device_type_keyboard, (kb_type_legacy ? 'T' : HELPER_GET_INT_KEY(0x10E, language)), 0, 0);
	preference->game_controls_to_hardware[_button_touch_device].add_button(_input_device_type_keyboard, (kb_type_legacy ? 'T' : HELPER_GET_INT_KEY(0x10E, language)), 0, 0);
	preference->game_controls_to_hardware[_button_throw_grenade].add_button(_input_device_type_keyboard, (kb_type_legacy ? HELPER_GET_INT_KEY(VK_TAB, language) : HELPER_GET_INT_KEY('P', language)), 0, 0);
	preference->game_controls_to_hardware[_button_switch_grenade].add_button(_input_device_type_keyboard, (kb_type_legacy ? 'G' : HELPER_GET_INT_KEY(0x10F, language)), 0, 0);
	preference->game_controls_to_hardware[_button_flashlight].add_button(_input_device_type_keyboard, (kb_type_legacy ? 'X' : HELPER_GET_INT_KEY('M', language)), 0, 0);
	return;
}

static void input_abstraction_mouse_set_default_mapping(s_gamepad_input_preferences* preference, e_language language, bool kb_type_legacy, bool kb_type_split)
{
	preference->game_controls_to_hardware[_button_fire].add_button(_input_device_type_mouse, (kb_type_legacy ? _mouse_button_left : _mouse_button_right), 0, 0);
	preference->game_controls_to_hardware[_button_throw_grenade].add_button(_input_device_type_mouse, (kb_type_legacy ? _mouse_button_right : _mouse_button_left), 0, 0);

	if (!kb_type_legacy || kb_type_split)
	{
		preference->game_controls_to_hardware[_button_switch_grenade].add_button(_input_device_type_mouse, _mouse_button_middle, 0, 0);
	}
	else
	{
		preference->game_controls_to_hardware[_button_scope_zoom].add_button(_input_device_type_mouse, _mouse_button_middle, 0, 0);
	}

	preference->game_controls_to_hardware[_button_dual_wield_primary_fire].add_button(_input_device_type_mouse, _mouse_button_left, 0, 0);
	preference->game_controls_to_hardware[_button_dual_wield_secondary_fire].add_button(_input_device_type_mouse, _mouse_button_right, 0, 0);
	preference->game_controls_to_hardware[_button_vehicle_primary_fire].add_button(_input_device_type_mouse, (kb_type_legacy ? _mouse_button_left : _mouse_button_right), 0, 0);
	preference->game_controls_to_hardware[_button_vehicle_secondary_fire].add_button(_input_device_type_mouse, (kb_type_legacy ? _mouse_button_right : _mouse_button_left), 0, 0);
	preference->game_controls_to_hardware[_button_speed_boost].add_button(_input_device_type_mouse, (kb_type_legacy ? _mouse_button_right : _mouse_button_left), 0, 0);
	preference->game_controls_to_hardware[_button_e_brake].add_button(_input_device_type_mouse, (kb_type_legacy ? _mouse_button_right : _mouse_button_left), 0, 0);
	preference->game_controls_to_hardware[_button_banshee_bomb].add_button(_input_device_type_mouse, _mouse_button_middle, 0, 0);
	preference->game_controls_to_hardware[_button_scope_zoom_in].add_button(_input_device_type_mouse, _mouse_delta_w_forward, 0, 0);
	preference->game_controls_to_hardware[_button_scope_zoom_out].add_button(_input_device_type_mouse, _mouse_delta_w_back, 0, 0);
	preference->game_controls_to_hardware[_button_scope_zoom].add_button(_input_device_type_mouse, (kb_type_legacy ? HELPER_GET_INT_KEY('Z', language) : 'N'), 0, 0);
	preference->game_controls_to_hardware[_button_move_forward].add_button(_input_device_type_mouse, _mouse_button_5, 0, 0);
	preference->game_controls_to_hardware[_button_move_backward].add_button(_input_device_type_mouse, _mouse_button_4, 0, 0);
	preference->game_controls_to_hardware[_button_strafe_left].add_button(_input_device_type_mouse, _mouse_button_6, 0, 0);
	preference->game_controls_to_hardware[_button_strafe_right].add_button(_input_device_type_mouse, _mouse_button_7, 0, 0);
	return;
}

static bool __cdecl input_abstraction_controller_plugged_hook(uint16 gamepad_index)
{
	//fixes a hardcode check to _controller_index_0 that prevents other controllers from working without _controller_index_0 being connected
	return input_has_gamepad_plugged((uint16)g_updating_gamepad_index);
}
