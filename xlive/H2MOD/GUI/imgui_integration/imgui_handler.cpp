#include "imgui_handler.h"
#include "H2MOD.h"
#include "H2MOD\Modules\Input\PlayerControl.h"
#include "Util\Hooks\Hook.h"

#include "imgui.h"
#include "backends\imgui_impl_dx9.h"
#include "backends\imgui_impl_win32.h"

#include "ImGui_Cartographer_Style.h"

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
	static bool					g_take_input = false;

	HWND get_HWND()
	{
		return g_hWnd;
	}

	bool ImGuiShouldHandleInput()
	{
		return g_take_input;
	}
	void ImGuiToggleInput(bool state)
	{
		g_take_input = state;
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
		if (!imgui_handler::CanDrawImgui()) return;

		ImGui_ImplDX9_NewFrame();
		ImGui_ImplWin32_NewFrame();
		ImGui::NewFrame();
		for (auto& window : windows)
		{
			if (window.DoRender)
			{
				window.RenderFunc(&window.DoRender);
			}
		}
		ImGui::Render();
		ImGui_ImplDX9_RenderDrawData(ImGui::GetDrawData());
	}

	void ToggleWindow(const std::string& name)
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

	bool IsWindowActive(const std::string& name)
	{
		for (auto& window : windows)
		{
			if (window.name == name)
				return window.DoRender;
		}
		return false;
	}

	void Initalize(LPDIRECT3DDEVICE9 pDevice, HWND hWnd)
	{
		windows.emplace_back("Advanced Settings", false, AdvancedSettings::Render, AdvancedSettings::Open, AdvancedSettings::Close);
		windows.emplace_back("motd", false, MOTD::Render, MOTD::Open, MOTD::Close);
		windows.emplace_back("debug_overlay", false, DebugOverlay::Render, DebugOverlay::Open, DebugOverlay::Close);
		windows.emplace_back("messagebox", false, iMessageBox::Render, iMessageBox::Open, iMessageBox::Close);
		
		IMGUI_CHECKVERSION();
		ImGui::CreateContext();
		ImGuiIO& io = ImGui::GetIO();
		ImGui_ImplWin32_Init((void*)hWnd);

		g_hWnd = hWnd;
		g_pDevice = pDevice;
		io.ConfigFlags |= ImGuiConfigFlags_NoMouseCursorChange;
		imgui::Cartographer::StyleSetAsDefault();

		ImFontConfig fontConfig;
		fontConfig.SizePixels = 13.0f * 1.5f;
		ImFont* font1 = io.Fonts->AddFontDefault(&fontConfig);

		ImGui_ImplDX9_Init(pDevice);

		AdvancedSettings::BuildStringsTable();
	}
	float WidthPercentage(float percent)
	{
		auto Width = ImGui::GetWindowContentRegionMax().x - ImGui::GetWindowContentRegionMin().x;
		if (ImGui::GetColumnsCount() > 1)
			Width = ImGui::GetColumnWidth();

		return Width * (percent / 100.0f);
	}

	void TextVerticalPad(char* label)
	{
		ImGui::AlignTextToFramePadding();
		ImGui::Text(label);
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

	s_aspect_ratio getAspectRatio(const ImVec2 displaySize)
	{
		if (displaySize.x / displaySize.y >= 1.6f)
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
			const ImGuiViewport* viewport = ImGui::GetMainViewport();
			MOTD::GetMOTD(getAspectRatio(viewport->WorkSize));
		};
		std::thread(grab_thread).detach();
	}
}