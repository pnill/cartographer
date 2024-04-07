#pragma once
#include "controllers.h"

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
CHECK_STRUCT_SIZE(dinput_device, 0x40);


extern XINPUT_VIBRATION g_vibration_state[k_number_of_controllers];

int32* hs_debug_simulate_gamepad_global_get(void);

bool* input_suppress_global_get(void);

void __cdecl input_set_gamepad_rumbler_state(int16 gamepad_index, uint16 left, uint16 right);

void input_windows_apply_patches(void);
