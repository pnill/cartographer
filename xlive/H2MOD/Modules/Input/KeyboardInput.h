#pragma once

namespace KeyboardInput
{
	void ToggleKeyboardInput();
	void Initialize();
	void RemoveHotkey(int* hotkey);
	void ExecuteHotkey(WPARAM message);
	void RegisterHotkey(int* hotkey, std::function<void()> callback);	
}

void hotkeyFuncToggleHideIngameChat();