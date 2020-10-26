#pragma once

namespace KeyboardInput
{
	void ToggleKeyboardInput();
	void Initialize();
	void RegisterHotkey(int* hotkey, std::function<void()> callback);
	void ExecuteHotkey(WPARAM message);
}