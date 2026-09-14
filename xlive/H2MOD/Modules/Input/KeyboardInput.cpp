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
	k_number_of_global_hotkeys = 4
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

void hotkeyFuncHelp() {
	addDebugText("------------------------------");
	addDebugText("Options:");

	c_static_string<64> vkstring;
	GetVKeyCodeString(H2Config_hotkeyIdHelp, &vkstring);
	addDebugText("%s - Print and show this help text.", vkstring.get_string());
	vkstring.clear();

	GetVKeyCodeString(H2Config_hotkeyIdToggleHideIngameChat, &vkstring);
	addDebugText("%s - Toggles hiding the in-game chat menu.", vkstring.get_string());
	vkstring.clear();

	GetVKeyCodeString(H2Config_hotkeyIdConsole, &vkstring);
	addDebugText("%s - Toggles hiding the Console Menu.", vkstring.get_string());

	addDebugText("------------------------------");

#ifdef TERMINAL_ENABLED
	if (!ImGuiHandler::IsWindowActive(k_cartographer_console_window_name))
	{
		ImGuiHandler::ToggleWindow(k_cartographer_console_window_name);
	}
	GetMainConsoleInstance()->SwitchToTab(_console_tab_logs);
#endif
}

void hotkeyFuncToggleHideIngameChat() {
	H2Config_hide_ingame_chat = !H2Config_hide_ingame_chat;
	if (H2Config_hide_ingame_chat) {
		addDebugText("Hiding in-game chat menu.");
	}
	else {
		addDebugText("Showing in-game chat menu.");
	}
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
	GetMainConsoleInstance()->SwitchToTab(_console_tab_commands);
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

	g_keyboard_hotkey_data[0] = { &H2Config_hotkeyIdHelp, hotkeyFuncHelp };
	g_keyboard_hotkey_data[1] = { &H2Config_hotkeyIdGuide, hotkeyFuncGuide };
	g_keyboard_hotkey_data[2] = { &H2Config_hotkeyIdConsole, hotkeyFuncConsole };
	g_keyboard_hotkey_data[3] = { &H2Config_hotkeyIdToggleHideIngameChat, []() { H2Config_hide_ingame_chat = !H2Config_hide_ingame_chat; } };
	return;
}
