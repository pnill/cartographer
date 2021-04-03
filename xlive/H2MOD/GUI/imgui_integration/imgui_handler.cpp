#include "imgui_handler.h"
#include "imgui.h"
#include "imgui_impl_dx9.h"
#include "H2MOD/Modules/Input/PlayerControl.h"
#include "Util/Hooks/Hook.h"
#include "H2MOD.h"

namespace imgui_handler
{
	//Window Name, DrawState, RenderFunc, OpenFunc, CloseFunc
	/*std::vector<std::tuple<std::string, bool, std::function<void(bool*)>,
		std::function<void()>, std::function<void()>>> windows;*/
	std::vector<s_imgui_window> windows;
	static HWND                 g_hWnd = NULL;
	static INT64                g_Time = 0;
	static INT64                g_TicksPerSecond = 0;
	static ImGuiMouseCursor     g_LastMouseCursor = ImGuiMouseCursor_COUNT;
	static bool                 g_HasGamepad = false;
	static bool                 g_WantUpdateHasGamepad = true;
	static LPDIRECT3DDEVICE9	g_pDevice;
	static PDIRECT3DTEXTURE9	g_patchNotes_Image = NULL;
	static bool					g_block_input = false;

	HWND get_HWND()
	{
		return g_hWnd;
	}

	bool ImGuiShoulBlockInput()
	{
		return g_block_input;
	}
	void ImGuiToggleInput(bool state)
	{
		g_block_input = state;
	}
	bool CanDrawImgui()
	{
		for (auto& window : windows)
		{
			if (window.DoRender)
				return true;
		}
		return false;
	}

	void DrawImgui()
	{
		ImGui_ImplDX9_NewFrame();
		ImGuiWin32Frame();
		ImGui::NewFrame();
		for (auto& window : windows)
		{
			if (window.DoRender)
			{
				window.RenderFunc(&window.DoRender);
			}
		}
		ImGui::EndFrame();
		ImGui::Render();
		ImGui_ImplDX9_RenderDrawData(ImGui::GetDrawData());
	}

	void ToggleWindow(std::string name)
	{
		for (auto& window : windows)
		{
			if (window.name == name)
			{
				window.DoRender = !window.DoRender;
				if (window.DoRender)
				{
					if (window.OpenFunc != nullptr)
						window.OpenFunc();
				}
				else
				{
					if (window.CloseFunc != nullptr)
						window.CloseFunc();
				}
			}
		}
	}

	bool IsWindowActive(std::string name)
	{
		for (auto& window : windows)
		{
			if (window.name == name)
				return window.DoRender;
		}
		return false;
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
		windows.emplace_back("Advanced Settings", false, AdvancedSettings::Render, AdvancedSettings::Open, AdvancedSettings::Close);
		windows.emplace_back("motd", false, MOTD::Render, MOTD::Open, MOTD::Close);
		windows.emplace_back("debug_overlay", false, DebugOverlay::Render, DebugOverlay::Open, DebugOverlay::Close);
		windows.emplace_back("messagebox", false, iMessageBox::Render, iMessageBox::Open, iMessageBox::Close);
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
		g_pDevice = pDevice;
		//	ImGuiIO& io = ImGui::GetIO();
		io.BackendFlags |= ImGuiBackendFlags_HasMouseCursors;         // We can honor GetMouseCursor() values (optional)
		io.BackendFlags |= ImGuiBackendFlags_HasSetMousePos;          // We can honor io.WantSetMousePos requests (optional, rarely used)
		io.ConfigFlags |= ImGuiConfigFlags_NoMouseCursorChange;
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

		//int my_image_width = 1786;
		//int my_image_height = 745;

		//bool ret = LoadTextureFromFile("patchnotes.png", , &my_image_width, &my_image_height);
		//IM_ASSERT(ret);
		preloadImages();
		AdvancedSettings::BuildStringsTable();
	}
	float WidthPercentage(float percent)
	{
		auto Width = ImGui::GetWindowContentRegionWidth();
		if (ImGui::GetColumnsCount() > 1)
			Width = ImGui::GetColumnWidth();

		return Width * (percent / 100.0f);
	}

	void TextVerticalPad(char* label, float amount = 8.5)
	{
		ImGui::GetFont()->DisplayOffset.y += amount;
		ImGui::Text(label);
		ImGui::GetFont()->DisplayOffset.y -= amount;
		//ImGui::SetCursorPosY(cursor_pos.y);
	}

	bool LoadTextureFromFile(const char* filename, s_imgui_images image, int* out_width, int* out_height)
	{
		// Load texture from disk
		D3DXIMAGE_INFO imgInfo;
		PDIRECT3DTEXTURE9 texture = nullptr;
		//HRESULT hr = D3DXCreateTextureFromFileA(g_pDevice, filename, &texture);
		HRESULT hr = D3DXCreateTextureFromFileExA(g_pDevice, filename, D3DX_DEFAULT_NONPOW2, D3DX_DEFAULT_NONPOW2, D3DX_FROM_FILE, 0,
			D3DFMT_FROM_FILE, D3DPOOL_DEFAULT, D3DX_FILTER_NONE, D3DX_FILTER_NONE, 0, &imgInfo, NULL, &texture);

		if (hr != S_OK)
			return false;

		// Retrieve description of the texture surface so we can access its size
		switch (image)
		{
		case patch_notes:
			if (g_patchNotes_Image)
				g_patchNotes_Image->Release(); // release the texture if we already have one

			g_patchNotes_Image = texture;
			break;
		default:
			return false;
		}

		//texture->GetLevelDesc(0, &my_image_desc);
		//*out_width = my_image_desc.Width;
		//*out_height = my_image_desc.Height;
		
		*out_width = imgInfo.Width;
		*out_height = imgInfo.Height;
		return true;
	}


	PDIRECT3DTEXTURE9 GetTexture(s_imgui_images image)
	{
		switch (image) {
		case patch_notes:
			return g_patchNotes_Image;
		default: 
			return NULL;
		}
	}

	void ReleaseTextures()
	{
		if (g_patchNotes_Image) g_patchNotes_Image->Release();
		g_patchNotes_Image = nullptr;
	}

	s_aspect_ratio getAspectRatio(float width, float height)
	{
		if (width / height >= 1.6f)
		{
			return sixten_nine;
		}
		else
		{
			return four_three;
		}
	}

	void preloadImages()
	{

		auto grab_thread = []()
		{
			ImGuiIO& io = ImGui::GetIO();
			RECT rect;
			::GetClientRect(get_HWND(), &rect);
			io.DisplaySize = ImVec2((float)(rect.right - rect.left), (float)(rect.bottom - rect.top));
			MOTD::GetMOTD(getAspectRatio(
				ImGui::GetIO().DisplaySize.x,
				ImGui::GetIO().DisplaySize.y));
		};
		std::thread(grab_thread).detach();
	}
}