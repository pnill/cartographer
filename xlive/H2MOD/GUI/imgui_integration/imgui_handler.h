#pragma once
#include "imgui.h"

namespace imgui_handler
{
	HWND get_HWND();
	bool CanDrawImgui();
	void DrawImgui();
	void ToggleWindow(std::string);
	static bool ImGui_ImplWin32_UpdateMouseCursor();
	static void ImGui_ImplWin32_UpdateMousePos();
	IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
	void ImGuiWin32Frame();
	void AdvancedSettings(bool* p_open);
	void Initalize(LPDIRECT3DDEVICE9 pDevice, HWND hWnd);
	float WidthPercentage(float percent);
	void TextVerticalPad(char* label, float amount);
}
