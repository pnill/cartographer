#include "stdafx.h"

#include "imgui_handler.h"

#include "game/player_control.h"

#include "imgui.h"
#include "backends/imgui_impl_dx9.h"
#include "backends/imgui_impl_win32.h"

#include "ImGui_CartographerStyle.h"
#include "Console/ImGui_ConsoleImpl.h"
#include "ImGui_NetworkStatsOverlay.h"

namespace ImGuiHandler
{
	std::vector<s_imgui_window> imgui_windows;
	PDIRECT3DTEXTURE9			g_patch_notes_texture = NULL;

	namespace {
		bool					handle_window_input = false;

		// need to update ImGui state at least one more tick
		// otherwise the enter key gets stuck when ImGui input is disabled, breaking the console
		bool					last_frame_update = true;

		LPDIRECT3DDEVICE9		p_d3d_device = nullptr;
	}

	bool						g_network_stats_overlay = false;

	bool ImGuiShouldHandleInput()
	{
		return handle_window_input;
	}
	void ImGuiToggleInput(bool state)
	{
		handle_window_input = state;
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
		if (!ImGuiHandler::CanDrawImgui() && 
			!last_frame_update)
			return;

		// clear keyboard/mouse input state if we are about to close the ImGui windows
		if (last_frame_update)
		{
			ImGuiIO& io = ImGui::GetIO();
			io.ClearInputKeys();
			io.ClearMouseInput();
			io.ClearInputCharacters();
			ReleaseTextures();
			last_frame_update = false;
		}

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

		// Rendering
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
			if (window.doRender && !window.NoImInput())
			{
				keep_game_input_blocked = true;
			}
		}

		last_frame_update = !keep_game_input_blocked;
		SetGameInputState(!keep_game_input_blocked);
		ImGuiToggleInput(keep_game_input_blocked);
		player_control_disable_local_camera(keep_game_input_blocked);
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

		io.ConfigFlags |= ImGuiConfigFlags_NoMouseCursorChange;
		imgui::Cartographer::StyleSetAsDefault();

		ImFontConfig fontConfig;
		fontConfig.SizePixels = 13.0f * 1.5f;
		ImFont* font1 = io.Fonts->AddFontDefault(&fontConfig);

		ImGui_ImplDX9_Init(pDevice);
		p_d3d_device = pDevice;

		ImAdvancedSettings::BuildStringsTable();
		WeaponOffsets::BuildStringsTable();
		imgui_windows.emplace_back("Weapon Offsets", false, WeaponOffsets::Render, WeaponOffsets::Open, WeaponOffsets::Close);
		imgui_windows.emplace_back(ImMOTD::windowName, false, ImMOTD::Render, ImMOTD::Open, ImMOTD::Close);
		imgui_windows.emplace_back(ImDebugOverlay::windowName, false, ImDebugOverlay::Render, ImDebugOverlay::Open, ImDebugOverlay::Close);
		imgui_windows.emplace_back(ImMessageBox::windowName, false, ImMessageBox::Render, ImMessageBox::Open, ImMessageBox::Close);
		imgui_windows.emplace_back(ImAdvancedSettings::windowName, false, ImAdvancedSettings::Render, ImAdvancedSettings::Open, ImAdvancedSettings::Close);
		imgui_windows.emplace_back(Console::windowName, false, Console::Render, Console::Open, Console::Close);

		atexit([]() {
			ImGui_ImplDX9_Shutdown();
			ImGui_ImplWin32_Shutdown();
			ImGui::DestroyContext();
			});
	}
	float WidthPercentage(float percent)
	{
		auto Width = ImGui::GetWindowContentRegionMax().x - ImGui::GetWindowContentRegionMin().x;
		if (ImGui::GetColumnsCount() > 1)
			Width = ImGui::GetColumnWidth();

		return Width * (percent / 100.0f);
	}

	void TextVerticalPad(const char* label)
	{
		ImGui::AlignTextToFramePadding();
		ImGui::Text(label);
	}

	bool LoadTextureFromFile(const wchar_t* filename, s_imgui_images image, int* out_width, int* out_height)
	{
		// Load texture from disk
		D3DXIMAGE_INFO imgInfo;
		PDIRECT3DTEXTURE9 texture = nullptr;
		//HRESULT hr = D3DXCreateTextureFromFileA(g_pDevice, filename, &texture);
		HRESULT hr = D3DXCreateTextureFromFileEx(p_d3d_device, filename, D3DX_DEFAULT_NONPOW2, D3DX_DEFAULT_NONPOW2, D3DX_FROM_FILE, 0,
			D3DFMT_FROM_FILE, D3DPOOL_DEFAULT, D3DX_FILTER_NONE, D3DX_FILTER_NONE, 0, &imgInfo, NULL, &texture);

		if (hr != S_OK)
			return false;

		// Retrieve description of the texture surface so we can access its size
		switch (image)
		{
		case patch_notes:
			if (g_patch_notes_texture)
				g_patch_notes_texture->Release(); // release the texture if we already have one

			g_patch_notes_texture = texture;
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
			return g_patch_notes_texture;
		default: 
			return NULL;
		}
	}

	void ReleaseTextures()
	{
		if (g_patch_notes_texture) g_patch_notes_texture->Release();
		g_patch_notes_texture = nullptr;
	}

	s_aspect_ratio GetAspectRatio(const ImVec2 displaySize)
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
}