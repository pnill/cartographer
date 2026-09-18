#include "stdafx.h"
#include "KeyboardInput.h"

#include "input/controllers.h"

#include "H2MOD/Modules/Shell/Config.h"
#include "H2MOD/GUI/ImGui_Integration/ImGui_Handler.h"
#include "H2MOD/GUI/ImGui_Integration/Console/ImGui_ConsoleImpl.h"
#include "H2MOD/Modules/OnScreenDebug/OnscreenDebug.h"
#include "H2MOD/Utils/Utils.h"

/* enums */
enum
{
	k_number_of_global_hotkeys = 2
};


/* structures */

struct s_keyboard_hotkey_data
{
	WPARAM* message;
	void(__cdecl* callback)();
};

/* constants */

static BYTE enableKeyboard3[] = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };

/* globals */

s_keyboard_hotkey_data g_keyboard_hotkey_data[k_number_of_global_hotkeys] = {};

void KeyboardInput::ToggleKeyboardInput()
{
	if (H2Config_disable_ingame_keyboard) {
		//Allows to repeat last movement when lose focus in mp, unlocks METHOD E from point after intro vid
		BYTE getFocusB[] = { 0x00 };
		WriteBytes(Memory::GetAddress() + 0x2E3C5, getFocusB, 1);

		//Allows input when not in focus.
		BYTE getFocusE[] = { 0x90, 0x90, 0x90, 0x90, 0x90, 0x90 };
		WriteBytes(Memory::GetAddress() + 0x2F9EA, getFocusE, 6);
		WriteBytes(Memory::GetAddress() + 0x2F9FC, getFocusE, 6);
		WriteBytes(Memory::GetAddress() + 0x2FA09, getFocusE, 6);
	}
	else {
		//Reset them all back.
		BYTE getFocusB[] = { 0x01 };
		WriteBytes(Memory::GetAddress() + 0x2E3C5, getFocusB, 1);

		//dont allow input when not in focus.
		BYTE getFocusE[] = { 0x0F, 0x85, 0x02, 0x02, 0x00, 0x00 };
		WriteBytes(Memory::GetAddress() + 0x2F9EA, getFocusE, 6);
		getFocusE[2] = 0xF0;
		getFocusE[3] = 0x01;
		WriteBytes(Memory::GetAddress() + 0x2F9FC, getFocusE, 6);
		getFocusE[2] = 0xE3;
		WriteBytes(Memory::GetAddress() + 0x2FA09, getFocusE, 6);
	}
}

void KeyboardInput::ExecuteHotkey(WPARAM message)
{
	for(auto &hk : g_keyboard_hotkey_data)
	{
		if (*hk.message == message)
		{
			hk.callback();
		}
	}
	return;
}

void hotkeyFuncGuide() {
#ifndef IMGUI_DISABLE
	ImGuiHandler::ImAdvancedSettings::set_controller_index(_controller0);
	ImGuiHandler::ToggleWindow(k_advanced_settings_window_name);
#endif
}

void hotkeyFuncConsole() {
#ifdef TERMINAL_ENABLED
	ImGuiHandler::ToggleWindow(k_cartographer_console_window_name);
#endif
}

void KeyboardInput::Initialize()
{
	if (!enableKeyboard3[0]) {
		for (int i = 0; i < 6; i++) {
			enableKeyboard3[i] = *((BYTE*)Memory::GetAddress() + 0x2FA67 + i);
		}
	}
	ToggleKeyboardInput();

	g_keyboard_hotkey_data[0] = { &H2Config_hotkeyIdGuide, hotkeyFuncGuide };
	g_keyboard_hotkey_data[1] = { &H2Config_hotkeyIdConsole, hotkeyFuncConsole };
	return;
}
