#pragma once
#include "imgui.h"
#include <D3dx9tex.h>

namespace imgui_handler
{
	enum s_imgui_images
	{
		patch_notes
	};
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
	enum s_aspect_ratio : byte
	{
		four_three,
		sixten_nine
	};
	HWND get_HWND();
	bool CanDrawImgui();
	void DrawImgui();
	void ToggleWindow(std::string name);
	bool IsWindowActive(std::string name);
	static bool ImGui_ImplWin32_UpdateMouseCursor();
	static void ImGui_ImplWin32_UpdateMousePos();
	IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
	void ImGuiWin32Frame();
	void Initalize(LPDIRECT3DDEVICE9 pDevice, HWND hWnd);
	float WidthPercentage(float percent);
	void TextVerticalPad(char* label, float amount);
	bool LoadTextureFromFile(const char* filename, s_imgui_images image, int* out_width, int* out_height);
	PDIRECT3DTEXTURE9 GetImage(s_imgui_images image);
	s_aspect_ratio getAspectRatio(float width, float height);
	void preloadImages();
	namespace MOTD {
		bool GetMOTD(s_aspect_ratio ratio);
		void Render(bool* p_open);
		void Open();
		void Close();
	}
	namespace AdvancedSettings
	{
		enum e_advanced_string : int
		{
			title,
			hud_title,
			player_field_of_view,
			reset,
			vehicle_field_of_view,
			crosshair_offset,
			crosshair_size,
			hide_ingame_chat,
			show_hud,
			show_first_person,
			video_title,
			fps_limit,
			fps_limit_tooltip,
			experimental_rendering_changes,
			experimental_rendering_tooltip,
			refresh_rate,
			refresh_rate_tooltip,
			lod,
			e_default,
			lod_1,
			lod_2,
			lod_3,
			lod_4,
			lod_5,
			lod_6,
			lod_tooltip,
			hires_fix,
			hires_fix_tooltip,
			m_k_title,
			raw_mouse,
			raw_mouse_tooltip,
			uniform_sensitivity,
			uniform_sensitivity_tooltip,
			raw_mouse_sensitivity,
			mouse_sensitivity,
			controller_title,
			controller_sensitivity,
			aiming_type,
			modern,
			aiming_type_tooltip,
			deadzone_type,
			axial,
			radial,
			both,
			deadzone_type_tooltip,
			axial_deadzone_X,
			axial_deadzone_Y,
			radial_deadzone_radius,
			host_campagin_settings,
			anti_cheat,
			anti_cheat_tooltip,
			disable_x_delay,
			skull_anger,
			skull_anger_tooltip,
			skull_assassins,
			skull_assassins_tooltip,
			skull_black_eye,
			skull_black_eye_tooltip,
			skull_blind,
			skull_blind_tooltip,
			skull_catch,
			skull_catch_tooltip,
			skull_envy,
			skull_envy_tooltip,
			skull_famine,
			skull_famine_tooltip,
			skull_ghost,
			skull_ghost_tooltip,
			skull_grunt,
			skull_grunt_tooltip,
			skull_iron,
			skull_iron_tooltip,
			skull_iwbyd,
			skull_iwbyd_tooltip,
			skull_mythic,
			skull_mythic_tooltip,
			skull_sputnik,
			skull_sputnik_tooltip,
			skull_thunderstorm,
			skull_thunderstorm_tooltip,
			skull_whuppopotamus,
			skull_whuppopotamus_tooltip,
			game_title,
			discord_presence,
			disable_intro_videos,
			language,
			lang_english,
			lang_japanese,
			lang_german,
			lang_french,
			lang_spanish,
			lang_italian,
			lang_korean,
			lang_chinese,
			lang_native,
			static_fp,
			static_fp_tooltip
		};
		void BuildStringsTable();
		char* GetString(e_advanced_string string, std::string id = "");
		void Render(bool* p_open);
		void Open();
		void Close();
	}
}
