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
CHECK_STRUCT_SIZE(xinput_device, 0x1C);

void xinput_apply_patches(void);
