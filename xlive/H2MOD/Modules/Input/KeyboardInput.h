#pragma once

namespace KeyboardInput
{
	void ToggleKeyboardInput();
	void Initialize();
	void RegisterHotkey(int* hotkey, std::function<void()> callback);
	void RemoveHotkey(int* hotkey);
	void ExecuteHotkey(WPARAM message);
}