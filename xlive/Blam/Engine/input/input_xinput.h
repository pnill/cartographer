#pragma once
#include "input_windows.h"

class xinput_device : public input_device
{
protected:
	uint32 dwUserIndex;
	uint32 error_level;
	XINPUT_STATE xinput_state;
public:
	virtual void XInputOpen(void) { return; };
	virtual void XInputClose(void) { return; };
	virtual void XUpdateState(void) override;
	virtual uint32 XGetState(XINPUT_STATE* state) override;
	virtual void XSetState(XINPUT_VIBRATION* state) override;
	virtual void XUpdateImmediate(void) override;
};
ASSERT_STRUCT_SIZE(xinput_device, 0x1C);

bool input_xinput_update_gamepad(uint32 gamepad_index, uint32 duration_ms, struct s_gamepad_input_button_state* gamepad_state);

void xinput_apply_patches(void);
