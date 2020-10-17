#include "KeyboardInput.h"
#include "H2MOD/Modules/Startup/Startup.h"
#include "Util/Hooks/Hook.h"
#include "H2MOD/Modules/Config/Config.h"
static BYTE enableKeyboard3[] = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };

void KeyboardInput::ToggleKeyboardInput()
{
	if (H2Config_disable_ingame_keyboard) {
		//Allows to repeat last movement when lose focus in mp, unlocks METHOD E from point after intro vid
		BYTE getFocusB[] = { 0x00 };
		WriteBytes(H2BaseAddr + 0x2E3C5, getFocusB, 1);
		//Allows input when not in focus.
		BYTE getFocusE[] = { 0x90, 0x90, 0x90, 0x90, 0x90, 0x90 };
		WriteBytes(H2BaseAddr + 0x2F9EA, getFocusE, 6);
		WriteBytes(H2BaseAddr + 0x2F9FC, getFocusE, 6);
		WriteBytes(H2BaseAddr + 0x2FA09, getFocusE, 6);
		//Disables the keyboard only when in-game and not in a menu.
		BYTE disableKeyboard1[] = { 0x90, 0x90, 0x90 };
		WriteBytes(H2BaseAddr + 0x2FA8A, disableKeyboard1, 3);
		BYTE disableKeyboard2[] = { 0x00 };
		WriteBytes(H2BaseAddr + 0x2FA92, disableKeyboard2, 1);
		BYTE disableKeyboard3[] = { 0x90, 0x90, 0x90, 0x90, 0x90, 0x90 };
		WriteBytes(H2BaseAddr + 0x2FA67, disableKeyboard3, 6);
	}
	else {
		//Reset them all back.
		BYTE getFocusB[] = { 0x01 };
		WriteBytes(H2BaseAddr + 0x2E3C5, getFocusB, 1);

		BYTE getFocusE[] = { 0x0F, 0x85, 0x02, 0x02, 0x00, 0x00 };
		WriteBytes(H2BaseAddr + 0x2F9EA, getFocusE, 6);
		getFocusE[2] = 0xF0;
		getFocusE[3] = 0x01;
		WriteBytes(H2BaseAddr + 0x2F9FC, getFocusE, 6);
		getFocusE[2] = 0xE3;
		WriteBytes(H2BaseAddr + 0x2FA09, getFocusE, 6);

		BYTE disableKeyboard1[] = { 0x56, 0xFF, 0xD3 };
		WriteBytes(H2BaseAddr + 0x2FA8A, disableKeyboard1, 3);
		BYTE disableKeyboard2[] = { 0x01 };
		WriteBytes(H2BaseAddr + 0x2FA92, disableKeyboard2, 1);
		WriteBytes(H2BaseAddr + 0x2FA67, enableKeyboard3, 6);
	}
}

void KeyboardInput::Init()
{
	if (!enableKeyboard3[0]) {
		for (int i = 0; i < 6; i++) {
			enableKeyboard3[i] = *((BYTE*)H2BaseAddr + 0x2FA67 + i);
		}
	}
	ToggleKeyboardInput();
}
