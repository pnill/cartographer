#include "stdafx.h"
#include "imgui_handler.h"

#ifndef IMGUI_DISABLE

#include "game/player_control.h"
#include "interface/user_interface_guide.h"

#include "imgui.h"
#include "backends/imgui_impl_dx9.h"
#include "backends/imgui_impl_win32.h"

#include "ImGui_CartographerStyle.h"
#include "Console/ImGui_ConsoleImpl.h"
#include "ImGui_NetworkStatsOverlay.h"

#include "imgui_ProdigyCleanTTF.h"

#include <d3dx9.h>
#include <XLive/XAM/uithread.h>

const char* k_advanced_settings_window_name = "advanced_settings";
const char* k_weapon_offsets_window_name = "Weapon Offsets";
const char* k_motd_window_name = "motd";
const char* k_debug_overlay_window_name = "debug_overlay";
const char* k_message_box_window_name = "messagebox";

c_static_flags_no_init<k_imgui_window_type_count> g_imgui_window_should_render = {};

namespace ImGuiHandler
{
	enum e_im_window_handler_flags
	{
		_im_window_no_input_bit = 0,
	};

	struct s_imgui_window
	{
		const char *const name;
		void(*const renderFunc)(bool*);
		void(*const openFunc)(void);
		void(*const closeFunc)(void);
		const e_im_window_handler_flags flags;
	};

	const s_imgui_window imgui_windows[k_imgui_window_type_count] =
	{
		{ k_weapon_offsets_window_name, WeaponOffsets::Render, WeaponOffsets::Open, WeaponOffsets::Close, (e_im_window_handler_flags)0},
		{ k_motd_window_name, ImMOTD::Render, ImMOTD::Open, ImMOTD::Close, (e_im_window_handler_flags)0},
		{ k_message_box_window_name, ImMessageBox::Render, ImMessageBox::Open, ImMessageBox::Close, (e_im_window_handler_flags)0},
		{ k_advanced_settings_window_name, ImAdvancedSettings::Render, ImAdvancedSettings::Open, ImAdvancedSettings::Close, (e_im_window_handler_flags)0},
#ifdef TERMINAL_ENABLED
		{ k_cartographer_console_window_name, CartographerConsole::Render, CartographerConsole::Open, CartographerConsole::Close, (e_im_window_handler_flags)0 }
#endif
	};

	PDIRECT3DTEXTURE9			g_patch_notes_texture = NULL;

	namespace {
		bool					handle_window_input = false;

		// need to update ImGui state at least one more tick
		// otherwise the enter key gets stuck when ImGui input is disabled, breaking the console
		bool					last_frame_update = true;
	}

	int						g_network_stats_overlay = _network_stats_display_none;

	ImFont*					g_im_font_ProdigyClean = NULL;

	bool ImGuiShouldHandleInput()
	{
		return handle_window_input;
	}
	void ImGuiToggleInput(bool state)
	{
		handle_window_input = state;
	}
	void SetGameInputState(bool enable)
	{
		// ### TODO move this function somewhere else
		user_interface_guide_state_manager_get()->m_block_game_input = !enable;
	}

	void DrawImgui()
	{
		// clear keyboard/mouse input state if we are about to close the ImGui windows
		if (last_frame_update)
		{
			ImGuiIO& io = ImGui::GetIO();
			io.ClearInputKeys();
			io.ClearInputMouse();
			io.ClearInputCharacters();
			release_motd_texture();
			last_frame_update = false;
		}

		ImGui_ImplDX9_NewFrame();
		ImGui_ImplWin32_NewFrame();
		ImGui::NewFrame();

		ImGui::PushFont(g_im_font_ProdigyClean);

		static bool display_network_stats = false;
		display_network_stats = g_network_stats_overlay == _network_stats_display_complete;
		ShowNetworkStatsOverlay(&display_network_stats);
		for (int8 i = 0; i < k_imgui_window_type_count; ++i)
		{
			bool should_render = g_imgui_window_should_render.test(i);
			if (should_render)
			{
				const s_imgui_window *const window = &imgui_windows[i];
				window->renderFunc(&should_render);
			}
		}

		ImGui::PopFont();

		// Rendering
		ImGui::Render();
		ImGui_ImplDX9_RenderDrawData(ImGui::GetDrawData());
	}

	void ToggleWindow(const char* name)
	{
		bool keep_game_input_blocked = false;

		for (int8 i = 0; i < k_imgui_window_type_count; ++i)
		{
			const s_imgui_window *const window = &imgui_windows[i];
			if (strncmp(window->name, name, 256) == 0)
			{
				// Toggle render
				g_imgui_window_should_render.set(i, !g_imgui_window_should_render.test(i));
				
				if (g_imgui_window_should_render.test(i))
				{
					window->openFunc();
				}
				else
				{
					window->closeFunc();
				}
			}

			// check if we still need to block the input of the game
			if (g_imgui_window_should_render.test(i) && !TEST_BIT(window->flags, _im_window_no_input_bit))
			{
				keep_game_input_blocked = true;
			}
		}

		last_frame_update = !keep_game_input_blocked;
		SetGameInputState(!keep_game_input_blocked);
		ImGuiToggleInput(keep_game_input_blocked);
		player_control_disable_local_camera(keep_game_input_blocked);
	}

	bool IsWindowActive(const char* name)
	{
		if (strncmp(name, "net_metrics", 16) == 0 && g_network_stats_overlay)
			return true;

		for (int8 i = 0; i < k_imgui_window_type_count; ++i)
		{
			const s_imgui_window *const window = &imgui_windows[i];
			if (strncmp(window->name, name, 256) == 0)
			{
				return g_imgui_window_should_render.test(i);
			}
		}
		return false;
	}

	void Initalize(HWND hWnd)
	{
		IMGUI_CHECKVERSION();
		ImGui::CreateContext();
		ImGuiIO& io = ImGui::GetIO();
		ImGui_ImplWin32_Init((void*)hWnd);

		io.ConfigFlags |= ImGuiConfigFlags_NoMouseCursorChange;
		imgui::Cartographer::StyleSetAsDefault();

		ImFontConfig fontConfig;
		fontConfig.SizePixels = 13.0f * 1.5f;
		g_im_font_ProdigyClean = io.Fonts->AddFontFromMemoryCompressedBase85TTF(
			proggy_clean_ttf_compressed_data_base85,
			fontConfig.SizePixels,
			&fontConfig);

		ImGui_ImplDX9_Init(g_xlive_d3d_device);

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
		const HRESULT hr = D3DXCreateTextureFromFileEx(g_xlive_d3d_device, filename, D3DX_DEFAULT_NONPOW2, D3DX_DEFAULT_NONPOW2, D3DX_FROM_FILE, 0,
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


	void* GetTexture(s_imgui_images image)
	{
		switch (image) {
		case patch_notes:
			return g_patch_notes_texture;
		default: 
			return NULL;
		}
	}

	void release_motd_texture(void)
	{
		if (g_patch_notes_texture)
		{
			g_patch_notes_texture->Release();
			g_patch_notes_texture = NULL;
		}
		return;
	}

	e_imgui_aspect_ratio GetAspectRatio(const real_point2d* display_size)
	{
		if (display_size->x / display_size->y >= 1.6f)
		{
			return sixten_nine;
		}
		else
		{
			return four_three;
		}
	}
}

#endif
