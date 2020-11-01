#pragma once
#include "imgui.h"

namespace imgui_handler
{
	struct s_imgui_window
	{
		std::string name;
		bool DoRender;
		std::function<void(bool*)> RenderFunc;
		std::function<void()> OpenFunc;
		std::function<void()> CloseFunc;
		s_imgui_window(std::string name, bool doRender, std::function<void(bool*)> renderFunc, std::function<void()> openFunc, std::function<void()> closeFunc)
		{
			this->name = name;
			this->DoRender = doRender;
			this->RenderFunc = renderFunc;
			this->OpenFunc = openFunc;
			this->CloseFunc = closeFunc;
		}
	};
	HWND get_HWND();
	bool CanDrawImgui();
	void DrawImgui();
	void ToggleWindow(std::string);
	static bool ImGui_ImplWin32_UpdateMouseCursor();
	static void ImGui_ImplWin32_UpdateMousePos();
	IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
	void ImGuiWin32Frame();
	void Initalize(LPDIRECT3DDEVICE9 pDevice, HWND hWnd);
	float WidthPercentage(float percent);
	void TextVerticalPad(char* label, float amount);


	namespace MOTD {
		void Render(bool* p_open);
		void Open();
		void Close();
	}
	namespace AdvancedSettings
	{
		void Render(bool* p_open);
		void Open();
		void Close();
	}
}
