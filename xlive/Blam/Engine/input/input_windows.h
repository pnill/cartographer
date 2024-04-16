#pragma once
#include "controllers.h"

class input_device
{
public:
	virtual void nullsub_0(void) = 0;
	virtual void nullsub_1(void) = 0;
	virtual void update_state_error_checking(void) = 0;
	virtual uint32 get_state(XINPUT_STATE* state) = 0;
	virtual void set_state(XINPUT_VIBRATION* state) = 0;
	virtual void update_state(void) = 0;

protected:
	uint32 controller_index;
	uint32 controller_state;
	XINPUT_STATE state;
};

struct s_vibration_state
{
	uint16 left;
	uint16 right;
};

extern s_vibration_state g_vibration_state[k_number_of_controllers];

void input_windows_apply_patches(void);

int32* hs_debug_simulate_gamepad_global_get(void);

bool* input_suppress_global_get(void);

void __cdecl input_set_gamepad_rumbler_state(int16 gamepad_index, uint16 left, uint16 right);

bool __cdecl input_initialize(void);
