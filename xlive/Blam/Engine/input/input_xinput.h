#pragma once
#include "input_windows.h"

class xinput_device : public input_device
{
public:
	virtual void nullsub_0(void) { return; };
	virtual void nullsub_1(void) { return; };
	virtual void update_state_error_checking(void) override;
	virtual uint32 get_state(XINPUT_STATE* state) override;
	virtual void set_state(XINPUT_VIBRATION* state) override;
	virtual void update_state(void) override;
};

void xinput_apply_patches(void);
