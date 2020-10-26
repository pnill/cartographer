#include "imgui_handler.h"
#include "imgui.h"
#include "imgui_impl_dx9.h"
#include "H2MOD/Modules/Input/PlayerControl.h"
#include "Util/Hooks/Hook.h"
#include "H2MOD.h"

namespace imgui_handler
{
	std::vector<std::tuple<std::string, bool, std::function<void(bool*)>>> windows;
	static HWND                 g_hWnd = NULL;
	static INT64                g_Time = 0;
	static INT64                g_TicksPerSecond = 0;
	static ImGuiMouseCursor     g_LastMouseCursor = ImGuiMouseCursor_COUNT;
	static bool                 g_HasGamepad = false;
	static bool                 g_WantUpdateHasGamepad = true;

	HWND get_HWND()
	{
		return g_hWnd;
	}

	bool CanDrawImgui()
	{
		for(auto &window : windows)
		{
			if (std::get<1>(window))
				return true;
		}
		return false;
	}

	void DrawImgui()
	{
		ImGui_ImplDX9_NewFrame();
		ImGuiWin32Frame();
		ImGui::NewFrame();
		for (auto &window : windows)
		{
			if (std::get<1>(window))
			{
				std::get<2>(window)(&std::get<1>(window));
			}
		}
		//ImGui::PopFont();
		ImGui::EndFrame();
		ImGui::Render();
		ImGui_ImplDX9_RenderDrawData(ImGui::GetDrawData());
	}

	void ToggleWindow(std::string name)
	{
		for(auto &window : windows)
		{
			if(std::get<0>(window) == name)
			{
				std::get<1>(window) = !std::get<1>(window);
				WriteValue<byte>(h2mod->GetAddress(0x9712cC), std::get<1>(window) ? 1 : 0);
				PlayerControl::GetControls(0)->DisableCamera = std::get<1>(window);
			}
		}
	}

	static bool ImGui_ImplWin32_UpdateMouseCursor()
	{
		ImGuiIO& io = ImGui::GetIO();
		if (io.ConfigFlags & ImGuiConfigFlags_NoMouseCursorChange)
			return false;

		ImGuiMouseCursor imgui_cursor = ImGui::GetMouseCursor();
		if (imgui_cursor == ImGuiMouseCursor_None || io.MouseDrawCursor)
		{
			// Hide OS mouse cursor if imgui is drawing it or if it wants no cursor
			::SetCursor(NULL);
		}
		else
		{
			// Show OS mouse cursor
			LPTSTR win32_cursor = IDC_ARROW;
			switch (imgui_cursor)
			{
			case ImGuiMouseCursor_Arrow:        win32_cursor = IDC_ARROW; break;
			case ImGuiMouseCursor_TextInput:    win32_cursor = IDC_IBEAM; break;
			case ImGuiMouseCursor_ResizeAll:    win32_cursor = IDC_SIZEALL; break;
			case ImGuiMouseCursor_ResizeEW:     win32_cursor = IDC_SIZEWE; break;
			case ImGuiMouseCursor_ResizeNS:     win32_cursor = IDC_SIZENS; break;
			case ImGuiMouseCursor_ResizeNESW:   win32_cursor = IDC_SIZENESW; break;
			case ImGuiMouseCursor_ResizeNWSE:   win32_cursor = IDC_SIZENWSE; break;
			case ImGuiMouseCursor_Hand:         win32_cursor = IDC_HAND; break;
			case ImGuiMouseCursor_NotAllowed:   win32_cursor = IDC_NO; break;
			}
			::SetCursor(::LoadCursor(NULL, win32_cursor));
		}
		return true;
	}

	static void ImGui_ImplWin32_UpdateMousePos()
	{
		ImGuiIO& io = ImGui::GetIO();

		// Set OS mouse position if requested (rarely used, only when ImGuiConfigFlags_NavEnableSetMousePos is enabled by user)
		if (io.WantSetMousePos)
		{
			POINT pos = { (int)io.MousePos.x, (int)io.MousePos.y };
			::ClientToScreen(g_hWnd, &pos);
			::SetCursorPos(pos.x, pos.y);
		}

		// Set mouse position
		io.MousePos = ImVec2(-FLT_MAX, -FLT_MAX);
		POINT pos;
		if (HWND active_window = ::GetForegroundWindow())
			if (active_window == g_hWnd || ::IsChild(active_window, g_hWnd))
				if (::GetCursorPos(&pos) && ::ScreenToClient(g_hWnd, &pos))
					io.MousePos = ImVec2((float)pos.x, (float)pos.y);
	}

	IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
	{
		if (ImGui::GetCurrentContext() == NULL)
			return 0;

		ImGuiIO& io = ImGui::GetIO();
		switch (msg)
		{
		case WM_LBUTTONDOWN: case WM_LBUTTONDBLCLK:
		case WM_RBUTTONDOWN: case WM_RBUTTONDBLCLK:
		case WM_MBUTTONDOWN: case WM_MBUTTONDBLCLK:
		case WM_XBUTTONDOWN: case WM_XBUTTONDBLCLK:
		{
			int button = 0;
			if (msg == WM_LBUTTONDOWN || msg == WM_LBUTTONDBLCLK) { button = 0; }
			if (msg == WM_RBUTTONDOWN || msg == WM_RBUTTONDBLCLK) { button = 1; }
			if (msg == WM_MBUTTONDOWN || msg == WM_MBUTTONDBLCLK) { button = 2; }
			if (msg == WM_XBUTTONDOWN || msg == WM_XBUTTONDBLCLK) { button = (GET_XBUTTON_WPARAM(wParam) == XBUTTON1) ? 3 : 4; }
			if (!ImGui::IsAnyMouseDown() && ::GetCapture() == NULL)
				::SetCapture(hwnd);
			io.MouseDown[button] = true;
			return 0;
		}
		case WM_LBUTTONUP:
		case WM_RBUTTONUP:
		case WM_MBUTTONUP:
		case WM_XBUTTONUP:
		{
			int button = 0;
			if (msg == WM_LBUTTONUP) { button = 0; }
			if (msg == WM_RBUTTONUP) { button = 1; }
			if (msg == WM_MBUTTONUP) { button = 2; }
			if (msg == WM_XBUTTONUP) { button = (GET_XBUTTON_WPARAM(wParam) == XBUTTON1) ? 3 : 4; }
			io.MouseDown[button] = false;
			if (!ImGui::IsAnyMouseDown() && ::GetCapture() == hwnd)
				::ReleaseCapture();
			return 0;
		}
		case WM_MOUSEWHEEL:
			io.MouseWheel += (float)GET_WHEEL_DELTA_WPARAM(wParam) / (float)WHEEL_DELTA;
			return 0;
		case WM_MOUSEHWHEEL:
			io.MouseWheelH += (float)GET_WHEEL_DELTA_WPARAM(wParam) / (float)WHEEL_DELTA;
			return 0;
		case WM_KEYDOWN:
		case WM_SYSKEYDOWN:
			if (wParam < 256)
				io.KeysDown[wParam] = 1;
			return 0;
		case WM_KEYUP:
		case WM_SYSKEYUP:
			if (wParam < 256)
				io.KeysDown[wParam] = 0;
			return 0;
		case WM_CHAR:
			// You can also use ToAscii()+GetKeyboardState() to retrieve characters.
			if (wParam > 0 && wParam < 0x10000)
				io.AddInputCharacterUTF16((unsigned short)wParam);
			return 0;
		case WM_SETCURSOR:
			if (LOWORD(lParam) == HTCLIENT && ImGui_ImplWin32_UpdateMouseCursor())
				return 1;
			return 0;
		}
		return 0;
	}

	void ImGuiWin32Frame()
	{
		ImGuiIO& io = ImGui::GetIO();

		ImGui_ImplWin32_UpdateMousePos();
		ImGuiMouseCursor mouse_cursor = io.MouseDrawCursor ? ImGuiMouseCursor_None : ImGui::GetMouseCursor();
		if (g_LastMouseCursor != mouse_cursor)
		{
			g_LastMouseCursor = mouse_cursor;
			ImGui_ImplWin32_UpdateMouseCursor();
		}
	}
	ImFont* font2;

	void Initalize(LPDIRECT3DDEVICE9 pDevice, HWND hWnd)
	{
		windows.emplace_back("Advanced Settings", false, imgui_handler::AdvancedSettings);
		
		IMGUI_CHECKVERSION();
		ImGui::CreateContext();
		ImGuiIO& io = ImGui::GetIO(); (void)io;
		ImGui::StyleColorsMaik();
		const ImVec2 wPadding(20, 10);
		const ImVec2 fPadding(8, 3);
		const ImVec2 iSpacing(6, 6);
		const ImVec2 iiSpacing(6, 6);
		const float indentSpacing = 20.0f;
		const float ScrollBarSize = 16.0f;
		const float GrabMinSize = 13.0f;

		const float WindowBorderSize = 0.0f;
		const float ChildBorderSize = 0.0f;
		const float PopupBorderSize = 0.0f;
		const float FrameBorderSize = 0.0f;
		const float TabBorderSize = 0.0f;

		const float WindowRound = 0.0f;
		const float ChildRounding = 0.0f;
		const float FrameRounding = 0.0f;
		const float PopupRounding = 0.0f;
		const float ScrollbarRounding = 0.0f;
		const float GrabRounding = 0.0f;
		const float TabRounding = 0.0f;

		const ImVec2 WindowTitleAlign(0.0, 0.50);
		const ImVec2 ButtonTextAlign(0.50, 0.50);
		const ImVec2 SelectabletextAlign(0.0, 0.50);

		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, wPadding);
		ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, fPadding);
		ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, iSpacing);
		ImGui::PushStyleVar(ImGuiStyleVar_ItemInnerSpacing, iiSpacing);
		ImGui::PushStyleVar(ImGuiStyleVar_IndentSpacing, indentSpacing);
		ImGui::PushStyleVar(ImGuiStyleVar_ScrollbarSize, ScrollBarSize);
		ImGui::PushStyleVar(ImGuiStyleVar_GrabMinSize, GrabMinSize);

		ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, WindowBorderSize);
		ImGui::PushStyleVar(ImGuiStyleVar_ChildBorderSize, ChildBorderSize);
		ImGui::PushStyleVar(ImGuiStyleVar_PopupBorderSize, PopupBorderSize);
		ImGui::PushStyleVar(ImGuiStyleVar_FrameBorderSize, FrameBorderSize);

		ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, WindowRound);
		ImGui::PushStyleVar(ImGuiStyleVar_ChildRounding, ChildRounding);
		ImGui::PushStyleVar(ImGuiStyleVar_PopupRounding, PopupRounding);
		ImGui::PushStyleVar(ImGuiStyleVar_ScrollbarRounding, ScrollbarRounding);
		ImGui::PushStyleVar(ImGuiStyleVar_GrabRounding, GrabRounding);
		ImGui::PushStyleVar(ImGuiStyleVar_TabRounding, TabRounding);

		ImGui::PushStyleVar(ImGuiStyleVar_WindowTitleAlign, WindowTitleAlign);
		ImGui::PushStyleVar(ImGuiStyleVar_ButtonTextAlign, ButtonTextAlign);
		ImGui::PushStyleVar(ImGuiStyleVar_SelectableTextAlign, SelectabletextAlign);
		g_hWnd = hWnd;
		//	ImGuiIO& io = ImGui::GetIO();
		io.BackendFlags |= ImGuiBackendFlags_HasMouseCursors;         // We can honor GetMouseCursor() values (optional)
		io.BackendFlags |= ImGuiBackendFlags_HasSetMousePos;          // We can honor io.WantSetMousePos requests (optional, rarely used)
		io.BackendPlatformName = "imgui_impl_win32";
		io.ImeWindowHandle = hWnd;

		// Keyboard mapping. ImGui will use those indices to peek into the io.KeysDown[] array that we will update during the application lifetime.
		io.KeyMap[ImGuiKey_Tab] = VK_TAB;
		io.KeyMap[ImGuiKey_LeftArrow] = VK_LEFT;
		io.KeyMap[ImGuiKey_RightArrow] = VK_RIGHT;
		io.KeyMap[ImGuiKey_UpArrow] = VK_UP;
		io.KeyMap[ImGuiKey_DownArrow] = VK_DOWN;
		io.KeyMap[ImGuiKey_PageUp] = VK_PRIOR;
		io.KeyMap[ImGuiKey_PageDown] = VK_NEXT;
		io.KeyMap[ImGuiKey_Home] = VK_HOME;
		io.KeyMap[ImGuiKey_End] = VK_END;
		io.KeyMap[ImGuiKey_Insert] = VK_INSERT;
		io.KeyMap[ImGuiKey_Delete] = VK_DELETE;
		io.KeyMap[ImGuiKey_Backspace] = VK_BACK;
		io.KeyMap[ImGuiKey_Space] = VK_SPACE;
		io.KeyMap[ImGuiKey_Enter] = VK_RETURN;
		io.KeyMap[ImGuiKey_Escape] = VK_ESCAPE;
		io.KeyMap[ImGuiKey_KeyPadEnter] = VK_RETURN;
		io.KeyMap[ImGuiKey_A] = 'A';
		io.KeyMap[ImGuiKey_C] = 'C';
		io.KeyMap[ImGuiKey_V] = 'V';
		io.KeyMap[ImGuiKey_X] = 'X';
		io.KeyMap[ImGuiKey_Y] = 'Y';
		io.KeyMap[ImGuiKey_Z] = 'Z';

		ImFont* font1 = io.Fonts->AddFontDefault();

		ImGui_ImplDX9_Init(pDevice);
	}
	float WidthPercentage(float percent)
	{
		auto Width = ImGui::GetWindowContentRegionWidth();
		if (ImGui::GetColumnsCount() > 1)
			Width = ImGui::GetColumnWidth();

		return Width * (percent / 100.0f);
	}

	void TextVerticalPad(char* label, float amount)
	{
		ImGui::GetFont()->DisplayOffset.y += 8.5;
		ImGui::Text(label);
		ImGui::GetFont()->DisplayOffset.y -= 8.5;
		//ImGui::SetCursorPosY(cursor_pos.y);
	}
}
