#pragma once
#include "controllers.h"
#include "input_abstraction.h"
#include "input_constants.h"

/* constants */

enum
{
	NUMBER_OF_VIRTUAL_CODES = 256,
	MAXIMUM_BUFFERED_KEYSTROKES = 64,
	k_number_of_memory_units = 9,
	k_maximum_delay_for_split_inputs = 250,
};

// Needs to be a define...
#define k_windows_device_controller_index _controller_index_0		// controller_index which the mouse/keyboard combo will use

/* forward declarations */

class c_input_dx9_mouse_cursor; //TODO

/* enums */

enum e_xinput_gamepad_buttons
{
	_xinput_gamepad_dpad_up = 0,
	_xinput_gamepad_dpad_down,
	_xinput_gamepad_dpad_left,
	_xinput_gamepad_dpad_right,
	_xinput_gamepad_start,
	_xinput_gamepad_back,
	_xinput_gamepad_left_thumb,
	_xinput_gamepad_right_thumb,
	_xinput_gamepad_left_shoulder,
	_xinput_gamepad_right_shoulder,
	_xinput_gamepad_a,
	_xinput_gamepad_b,
	_xinput_gamepad_x,
	_xinput_gamepad_y,
	k_number_of_xinput_buttons,

	_xinput_gamepad_left_trigger = k_number_of_xinput_buttons,
	_xinput_gamepad_right_trigger,
};

enum input_dx9_mouse_state : int32
{
	k_mouse_state_normal = 0,
	k_mouse_state_busy,
	k_mouse_state_hover,
	k_mouse_state_text_cursor,
	k_maximum_cursor_bitmaps
};

enum
{
	_key_modifier_shift_bit = 0,
	_key_modifier_control_bit,
	_key_modifier_alt_bit,
	NUMBER_OF_KEY_MODIFIER_FLAGS
};

/* structures */

struct key_stroke
{
	uint8 modifier_flags;
	int8 ascii_code;
	wchar_t utf16_code;
	e_input_key_code key_code;
	bool repeating;
};
ASSERT_STRUCT_SIZE(key_stroke, 0x8);

struct s_gamepad_input_button_state
{
	uint8 trigger_msec_down[2];
	uint8 max_trigger_msec_down[2];
	uint8 trigger_button_frames_down[2];
	uint8 button_frames_down[k_number_of_xinput_buttons];
	uint16 trigger_button_msec_down[2];
	uint16 button_msec_down[k_number_of_xinput_buttons];
	point2d thumb_left;
	point2d thumb_right;
};
ASSERT_STRUCT_SIZE(s_gamepad_input_button_state, 0x3C);

struct s_gamepad_input_state
{
	bool connected;
	bool m_device_just_joined;
	bool m_device_just_left;
	uint8 gap_3;
	s_gamepad_input_button_state state;
};
ASSERT_STRUCT_SIZE(s_gamepad_input_state, 0x40);

struct s_keyboard_input_state
{
	uint8 frames_down[NUMBER_OF_KEYS];
	uint16 msec_down[NUMBER_OF_KEYS];
	bool key_bool[NUMBER_OF_KEYS];
};
ASSERT_STRUCT_SIZE(s_keyboard_input_state, 0x400);

struct mouse_state
{
	DIMOUSESTATE2 state;
};

struct input_globals_windows
{
	bool initialized;
	bool active_flag;
	bool input_suppressed;
	bool feedback_suppress;
	uint32 update_time;
	uint32 update_msec;
	IDirectInput8A* direct_input;
	s_keyboard_input_state keyboard;
	int16 buffered_key_read_index;
	int16 buffered_key_read_count;
	key_stroke buffered_keys[MAXIMUM_BUFFERED_KEYSTROKES];

	LPDIRECTINPUTDEVICE8A mouse_dinput_device;
	bool mouse_acquired;
	int32 mouse_wheel_granularity;
	mouse_state mouse;
	uint16 mouse_buttons[NUMBER_OF_MOUSE_BUTTONS];
	mouse_state mouse_suppressed;
	uint8 gap_658[24];
	input_dx9_mouse_state cursor_state;
	class c_input_dx9_mouse_cursor* mouse_cursor_dx9;
	
	s_gamepad_input_state gamepad_states[k_number_of_controllers];
	s_gamepad_input_button_state suppressed_gamepad_state;
	XINPUT_VIBRATION rumble_states[k_number_of_controllers];
	uint32 main_controller_index;
	bool hardware_device_changed;
	char gap[3];
	int debug_simulate_gamepad;
	int field7D0;
	int field7D8;
};
ASSERT_STRUCT_SIZE(input_globals_windows, 2008);

/* classes */

class input_device
{
public:
	// input_device virtual functions

	virtual void XInputOpen(void) = 0;
	virtual void XInputClose(void) = 0;
	virtual void XUpdateState(void) = 0;
	virtual uint32 XGetState(XINPUT_STATE* state) = 0;
	virtual void XSetState(XINPUT_VIBRATION* state) = 0;
	virtual void XUpdateImmediate(void) = 0;
};

class dinput_device : public input_device
{
protected:
	GUID m_rguid;
	LPDIRECTINPUTDEVICE8A dinput_device;
	uint32 error_level;
	XINPUT_STATE gamepad_state;
	bool m_device_acquired;
	bool byte2D;
	uint32 field30;
	uint32 field34;
	uint32 field38;
	uint32 field3C;

public:
	// dinput_device virtual functions

	virtual void XInputOpen(void) override;
	virtual void XInputClose(void) override;
	virtual void XUpdateState(void) override;
	virtual uint32 XGetState(XINPUT_STATE* state) override;
	virtual void XSetState(XINPUT_VIBRATION* state) override { return; };
	virtual void XUpdateImmediate(void) override { return; };
};
ASSERT_STRUCT_SIZE(dinput_device, 0x40);

/* globals */

extern input_globals_windows* input_globals;
extern bool* g_input_windows_request_terminate;
extern uint32 input_device_change_delay_timer;

/* public code */

void input_windows_apply_patches(void);

void __cdecl input_initialize();
void __cdecl input_dispose();
void __cdecl input_windows_update(void);

void input_suppress(void);

void input_add_key(int32 msg, uint32 wParam, uint32 lParam, bool fHandled);

void __cdecl input_update_gamepads(uint32 duration_ms);
//void __cdecl input_update_mouse(DIMOUSESTATE2* mouse_state, uint32 duration_ms);
bool __cdecl input_has_gamepad(uint16 gamepad_index, bool* a2);
bool __cdecl input_has_gamepad_plugged(uint16 gamepad_index);
bool __cdecl input_gamepad_just_left(uint16 gamepad_index);
uint8 __cdecl input_get_connected_gamepads_count();

s_gamepad_input_state* input_get_gamepad(uint16 gamepad_index);

s_gamepad_input_button_state* __cdecl input_get_gamepad_state(uint16 gamepad_index);

mouse_state* __cdecl input_get_mouse_state(void);
uint16* __cdecl input_get_mouse_button_state();

bool input_peek_key(key_stroke* key);

bool input_abstraction_get_key(key_stroke* key);

void __cdecl input_update_main_device_state();

bool input_windows_processing_device_change();
bool input_windows_has_split_device_active();
void input_windows_notify_change_device_mapping();

void input_windows_release_key(WCHAR param);

void input_windows_clear_keyboard_input_state(void);

void __cdecl input_set_gamepad_rumbler_state(int16 gamepad_index, uint16 left, uint16 right);


uint8 __cdecl input_windows_key_frames_down(int16 key);
uint16 __cdecl input_windows_key_msec_down(int16 key);
bool __cdecl input_windows_drive_letter_test(int32 memory_unit, int8* drive_letter);
