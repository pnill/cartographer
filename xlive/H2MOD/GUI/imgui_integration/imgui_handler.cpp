#include "stdafx.h"

#include "imgui_handler.h"
#include "H2MOD.h"
#include "H2MOD\Modules\Input\PlayerControl.h"
#include "Util\Hooks\Hook.h"

#include "imgui.h"
#include "backends\imgui_impl_dx9.h"
#include "backends\imgui_impl_win32.h"

#include "ImGui_CartographerStyle.h"
#include "Console\ImGui_ConsoleImpl.h"
#include "ImGui_NetworkStatsOverlay.h"

namespace ImGuiHandler
{
	std::vector<s_imgui_window> imgui_windows =
	{
		{"motd", false, ImMOTD::Render, ImMOTD::Open, ImMOTD::Close},
		{"debug_overlay", false, ImDebugOverlay::Render, ImDebugOverlay::Open, ImDebugOverlay::Close},
		{"messagebox", false, ImMessageBox::Render, ImMessageBox::Open, ImMessageBox::Close},
		{"advanced_settings", false, ImAdvancedSettings::Render, ImAdvancedSettings::Open, ImAdvancedSettings::Close},
		{"console", false, Console::Render, Console::Open, Console::Close}
	};

	static HWND                 g_hWnd = NULL;
	static INT64                g_Time = 0;
	static INT64                g_TicksPerSecond = 0;
	static ImGuiMouseCursor     g_LastMouseCursor = ImGuiMouseCursor_COUNT;
	static bool                 g_HasGamepad = false;
	static bool                 g_WantUpdateHasGamepad = true;
	static LPDIRECT3DDEVICE9	g_pDevice;
	static PDIRECT3DTEXTURE9	g_patchNotes_Image = NULL;
	static bool					g_take_input = false;

	bool						g_network_stats_overlay = false;

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
		// TODO add these to some container
		if (g_network_stats_overlay)
			return true;
		for (auto& window : imgui_windows)
		{
			if (window.doRender)
				return true;
		}
		return false;
	}

	void SetGameInputState(bool enable)
	{
		// TODO move this function somewhere else
		*Memory::GetAddress<bool*>(0x9712C8 + 4) = !enable;
	}

	void DrawImgui()
	{
		if (!ImGuiHandler::CanDrawImgui()) return;

		ImGui_ImplDX9_NewFrame();
		ImGui_ImplWin32_NewFrame();
		ImGui::NewFrame();
		ShowNetworkStatsOverlay(&g_network_stats_overlay);
		for (auto& window : imgui_windows)
		{
			if (window.doRender)
			{
				window.renderFunc(&window.doRender);
			}
		}
		ImGui::Render();
		ImGui_ImplDX9_RenderDrawData(ImGui::GetDrawData());
	}

	void ToggleWindow(const std::string& name)
	{
		bool keep_game_input_blocked = false;

		for (auto& window : imgui_windows)
		{
			if (window.name == name)
			{
				window.doRender = !window.doRender;
				if (window.doRender)
				{
					if (window.openFunc != nullptr)
						window.openFunc();
				}
				else
				{
					if (window.closeFunc != nullptr)
						window.closeFunc();
				}
			}

			// check if we still need to block the input of the game
			if (window.doRender && !keep_game_input_blocked)
			{
				keep_game_input_blocked = true;
			}
		}

		SetGameInputState(!keep_game_input_blocked);
		ImGuiToggleInput(keep_game_input_blocked);
		PlayerControl::DisableLocalCamera(keep_game_input_blocked);
	}

	bool IsWindowActive(const std::string& name)
	{
		if (name == "net_metrics" && g_network_stats_overlay)
			return true;
		for (auto& window : imgui_windows)
		{
			if (window.name == name)
				return window.doRender;
		}
		return false;
	}

	void Initalize(LPDIRECT3DDEVICE9 pDevice, HWND hWnd)
	{
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

		ImAdvancedSettings::BuildStringsTable();
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
			ImMOTD::GetMOTD(getAspectRatio(viewport->WorkSize));
		};
		std::thread(grab_thread).detach();
	}
}