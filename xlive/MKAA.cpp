#include <Windows.h>
#include <iostream>
#include <xinput.h>
#include "MKAA.h"
#include "Hook.h"
#pragma comment(lib, "XInput9_1_0.lib")

int isPressed;

void Xinput()
{
	XINPUT_STATE state;
	DWORD dwUserIndex = 0;
	ZeroMemory(&state, sizeof(XINPUT_STATE));
	XInputGetState(0, &state);

	if (state.Gamepad.wButtons)
	{
		isPressed = 1;
	}
	else
		isPressed = 0;

}

static BYTE enableKeyboard3[] = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };
extern DWORD H2BaseAddr;

void DisableKeyboard()
{
	if (!enableKeyboard3[0]) {

		for (int i = 0; i < 6; i++) {

			enableKeyboard3[i] = *((BYTE*)H2BaseAddr + 0x2FA67 + i);

		}

	}

	if (isPressed == 1)
	{
		BYTE disableKeyboard1[] = { 0x90, 0x90, 0x90 };

		WriteBytes(H2BaseAddr + 0x2FA8A, disableKeyboard1, 3);

		BYTE disableKeyboard2[] = { 0x00 };

		WriteBytes(H2BaseAddr + 0x2FA92, disableKeyboard2, 1);

		BYTE disableKeyboard3[] = { 0x90, 0x90, 0x90, 0x90, 0x90, 0x90 };

		WriteBytes(H2BaseAddr + 0x2FA67, disableKeyboard3, 6);
	}
	else
	{
		BYTE disableKeyboard1[] = { 0x56, 0xFF, 0xD3 };

		WriteBytes(H2BaseAddr + 0x2FA8A, disableKeyboard1, 3);

		BYTE disableKeyboard2[] = { 0x01 };

		WriteBytes(H2BaseAddr + 0x2FA92, disableKeyboard2, 1);

		WriteBytes(H2BaseAddr + 0x2FA67, enableKeyboard3, 6);
	}

}
