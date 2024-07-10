#pragma once
#include "imgui.h"

/* constants */

#define k_advanced_settings_window_name "advanced_settings"

namespace ImGuiHandler
{
	extern bool g_network_stats_overlay;
	extern PDIRECT3DTEXTURE9 g_patch_notes_texture;

	enum s_imgui_images
	{
		patch_notes
	};

	typedef int ImWWindowHandlerFlags;
	enum ImWWindowHandlerFlags_
	{
		_ImWindow_no_input = 1 << 0,
	};

	struct s_imgui_window
	{
		std::string name;
		bool doRender;
		std::function<void(bool*)> renderFunc;
		std::function<void()> openFunc;
		std::function<void()> closeFunc;
		ImWWindowHandlerFlags flags;

		s_imgui_window(const std::string& _name, bool _doRender, std::function<void(bool*)> _renderFunc, std::function<void()> _openFunc, std::function<void()> _closeFunc, ImWWindowHandlerFlags _flags = 0)
		{
			name = _name;
			doRender = _doRender;
			renderFunc = _renderFunc;
			openFunc = _openFunc;
			closeFunc = _closeFunc;
			flags = _flags;
		}

		bool NoImInput()
		{
			return (flags & _ImWindow_no_input) != 0;
		}
	};

	enum s_aspect_ratio : byte
	{
		four_three,
		sixten_nine
	};

	bool ImGuiShouldHandleInput();
	void ImGuiToggleInput(bool state);
	void SetGameInputState(bool enable);
	bool CanDrawImgui();
	void DrawImgui();
	bool IsWindowActive(const std::string& name);
	void ToggleWindow(const std::string& name);
	void Initalize(LPDIRECT3DDEVICE9 pDevice, HWND hWnd);
	float WidthPercentage(float percent);
	void TextVerticalPad(const char* label);
	bool LoadTextureFromFile(const wchar_t* filename, s_imgui_images image, int* out_width, int* out_height);
	PDIRECT3DTEXTURE9 GetTexture(s_imgui_images image);
	void ReleaseTextures();
	s_aspect_ratio GetAspectRatio(const ImVec2 displaySize);
	namespace ImMOTD {
		bool DownloadMOTD(const std::wstring& motd_path, s_aspect_ratio ratio);
		void Render(bool* p_open);
		void Open();
		void Close();

		extern std::string windowName;
	}
	namespace ImAdvancedSettings
	{
		void Render(bool* p_open);
		void Open();
		void Close();
	}
	namespace ImDebugOverlay
	{
		void Render(bool* p_open);
		void AddWatchItem(std::string Key, std::string Description);
		void UpdateWatchItem(std::string Key, std::string Value);
		void Open();
		void Close();

		extern std::string windowName;
	}
	namespace ImMessageBox
	{
		void Render(bool* p_open);
		void SetMessage(std::string message);
		void Open();
		void Close();

		extern std::string windowName;
	}
	namespace WeaponOffsets
	{
		enum e_weapon_offsets_string : int
		{
			title,
			combo_title,
			battle_rifle_title,
			beam_rifle_title,
			brute_plasma_rifle_title,
			brute_shot_title,
			carbine_title,
			energy_sword_title,
			fuel_rod_title,
			magnum_title,
			needler_title,
			plasma_pistol_title,
			plasma_rifle_title,
			rocket_launcher_title,
			sentinel_beam_title,
			shotgun_title,
			sniper_title,
			smg_title,
			weapon_offset_x,
			weapon_offset_y,
			weapon_offset_z,
			reset
		};
		void BuildStringsTable();
		const char* GetString(e_weapon_offsets_string string, const std::string& id = "");
		void Render(bool* p_open);
		void Open();
		void Close();
		void MapLoad();
		void Initialize();
	}
}