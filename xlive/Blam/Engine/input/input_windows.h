#pragma once
#include "controllers.h"

#define K_NUMBER_OF_WINDOWS_INPUT_VIRTUAL_CODES 256

class input_device
{
public:
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
	virtual void XInputOpen(void) override;
	virtual void XInputClose(void) override;
	virtual void XUpdateState(void) override;
	virtual uint32 XGetState(XINPUT_STATE* state) override;
	virtual void XSetState(XINPUT_VIBRATION* state) { return; };
	virtual void XUpdateImmediate(void) { return; };
};
ASSERT_STRUCT_SIZE(dinput_device, 0x40);



#pragma pack(push,1)
struct s_key_state
{
	uint8 gap[0x8];
};
ASSERT_STRUCT_SIZE(s_key_state, 0x8);

enum e_xinput_gamepad_buttons
{
	_xinput_gamepad_dpad_up = 0x0,
	_xinput_gamepad_dpad_down = 0x1,
	_xinput_gamepad_dpad_left = 0x2,
	_xinput_gamepad_dpad_right = 0x3,
	_xinput_gamepad_start = 0x4,
	_xinput_gamepad_back = 0x5,
	_xinput_gamepad_left_thumb = 0x6,
	_xinput_gamepad_right_thumb = 0x7,
	_xinput_gamepad_left_shoulder = 0x8,
	_xinput_gamepad_right_shoulder = 0x9,
	_xinput_gamepad_a = 0xa,
	_xinput_gamepad_b = 0xb,
	_xinput_gamepad_x = 0xc,
	_xinput_gamepad_y = 0xd,
	_xinput_gamepad_left_trigger = 0xe,
	_xinput_gamepad_right_trigger = 0xf,

	K_NUMBER_OF_XINPUT_BUTTONS = 0xE,
};

struct s_gamepad_input_button_state
{
	uint8 trigger_msec_down[2];
	uint8 max_trigger_msec_down[2];
	uint8 trigger_button_frames_down[2];
	uint8 button_frames_down[K_NUMBER_OF_XINPUT_BUTTONS];
	uint16 trigger_button_msec_down[2];
	uint16 button_msec_down[K_NUMBER_OF_XINPUT_BUTTONS];
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
	uint8 frames_down[K_NUMBER_OF_WINDOWS_INPUT_VIRTUAL_CODES];
	uint16 msec_down[K_NUMBER_OF_WINDOWS_INPUT_VIRTUAL_CODES];
	bool key_bool[K_NUMBER_OF_WINDOWS_INPUT_VIRTUAL_CODES];
};
ASSERT_STRUCT_SIZE(s_keyboard_input_state, 0x400);

class c_input_dx9_mouse_cursor; //TODO

struct s_input_globals
{
	bool initialized;
	bool mouse_acquired;
	bool input_suppressed;
	bool feedback_suppress;
	uint32 update_time;
	uint32 update_msec;
	IDirectInput8A* dinput;
	s_keyboard_input_state keyboard;
	int16 buffered_key_read_index;
	int16 buffered_key_read_count;
	s_key_state buffered_keys[64];
	LPDIRECTINPUTDEVICE8A mouse_dinput_device;
	bool mouse_show;
	uint8 gap_619[3];
	uint32 field_61C;
	DIMOUSESTATE2 mouse_state;
	int16 mouse_buttons[8];
	DIMOUSESTATE2 suppressed_mouse_state;
	uint8 gap_658[24];
	uint32 mouse_cursor_state;
	void* mouse_cursor_dx9;
	s_gamepad_input_state gamepad_states[4];
	s_gamepad_input_button_state suppressed_gamepad_state;
	XINPUT_VIBRATION rumble_states[4];
	uint32 main_controller_index;
	bool hardware_device_changed;
	char gap[3];
	int debug_simulate_gamepad;
	int field7D0;
	int field7D8;
};
ASSERT_STRUCT_SIZE(s_input_globals, 0x7D8);

#pragma pack(pop)




extern s_input_globals* input_globals;

extern XINPUT_VIBRATION g_vibration_state[k_number_of_controllers];


void __cdecl input_initialize();
void __cdecl input_dispose();
void __cdecl input_update();
void __cdecl input_update_gamepads(uint32 duration_ms);
void __cdecl input_update_mouse(DIMOUSESTATE2* mouse_state, uint32 duration_ms);
bool __cdecl input_has_gamepad(uint16 gamepad_index, bool* a2);
bool __cdecl input_has_gamepad_plugged(uint16 gamepad_index);
s_gamepad_input_state* __cdecl input_get_gamepad(uint16 gamepad_index);
s_gamepad_input_button_state* __cdecl input_get_gamepad_state(uint16 gamepad_index);
DIMOUSESTATE2* __cdecl input_get_mouse_state();
bool __cdecl input_get_key(s_key_state* keystate);
void __cdecl input_update_main_device_state();

void input_windows_apply_patches(void);

int32* hs_debug_simulate_gamepad_global_get(void);

bool* input_suppress_global_get(void);

void __cdecl input_set_gamepad_rumbler_state(int16 gamepad_index, uint16 left, uint16 right);

