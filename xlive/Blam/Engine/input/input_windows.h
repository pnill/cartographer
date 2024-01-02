#pragma once

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

struct rumble_state
{
	int16 left_rumble;
	int16 right_rumble;
};

rumble_state* controller_rumble_state_get(int32 controller_index);

int32* hs_debug_simulate_gamepad_global_get(void);

bool* input_suppress_global_get(void);
