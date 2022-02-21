#pragma once
#include "imgui.h"
#include "Blam\Cache\TagGroups\weapon_definition.hpp"

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
	bool ImGuiShouldHandleInput();
	void ImGuiToggleInput(bool state);
	bool CanDrawImgui();
	void DrawImgui();
	void ToggleWindow(const std::string& name);
	bool IsWindowActive(const std::string& name);
	void Initalize(LPDIRECT3DDEVICE9 pDevice, HWND hWnd);
	float WidthPercentage(float percent);
	void TextVerticalPad(char* label);
	bool LoadTextureFromFile(const char* filename, s_imgui_images image, int* out_width, int* out_height);
	PDIRECT3DTEXTURE9 GetTexture(s_imgui_images image);
	void ReleaseTextures();
	s_aspect_ratio getAspectRatio(const ImVec2 displaySize);
	void preloadImages();
	extern short g_NumWindowsOpen;
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
			weaponoffsets,
			video_title,
			fps_limit,
			fps_limit_tooltip,
			experimental_rendering_changes,
			render_none,
			render_cinematic,
			render_engine,
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
			shader_lod_max,
			shader_lod_max_tooltip,
			light_suppressor,
			light_suppressor_tooltip,
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
			static_fp_tooltip,
			tex_L1,
			tex_L2,
			tex_L3,
			shadow_title,
			water_title,
			upnp_title,
			upnp_tooltip,
			melee_fix_title,
			melee_fix_tooltip,
			no_events_title,
			no_events_tooltip,
			render_patch,
			skeleton_biped,
			skeleton_biped_tooltip,
			event_music_title,
			event_music_tooltip
		};
		void BuildStringsTable();
		char* GetString(e_advanced_string string, const std::string& id = "");
		void Render(bool* p_open);
		void Open();
		void Close();
	}
	namespace DebugOverlay
	{

		void Render(bool* p_open);
		void AddWatchItem(std::string Key, std::string Description);
		void UpdateWatchItem(std::string Key, std::string Value);
		void Open();
		void Close();
	}
	namespace iMessageBox
	{
		void Render(bool* p_open);
		void SetMessage(std::string message);
		void Open();
		void Close();
	}
	namespace WeaponOffsets
	{
		extern int battle_rifle_datum;
		extern s_weapon_group_definition* battle_rifle_tag;
		extern int beam_rifle_datum;
		extern s_weapon_group_definition* beam_rifle_tag;
		extern int brute_plasma_rifle_datum;
		extern s_weapon_group_definition* brute_plasma_rifle_tag;
		extern int brute_shot_datum;
		extern s_weapon_group_definition* brute_shot_tag;
		extern int carbine_datum;
		extern s_weapon_group_definition* carbine_tag;
		extern int energy_sword_datum;
		extern s_weapon_group_definition* energy_sword_tag;
		extern int fuel_rod_datum;
		extern s_weapon_group_definition* fuel_rod_tag;
		extern int magnum_datum;
		extern s_weapon_group_definition* magnum_tag;
		extern int needler_datum;
		extern s_weapon_group_definition* needler_tag;
		extern int plasma_pistol_datum;
		extern s_weapon_group_definition* plasma_pistol_tag;
		extern int plasma_rifle_datum;
		extern s_weapon_group_definition* plasma_rifle_tag;
		extern int rocket_launcher_datum;
		extern s_weapon_group_definition* rocket_launcher_tag;
		extern int sentinel_beam_datum;
		extern s_weapon_group_definition* sentinel_beam_tag;
		extern int shotgun_datum;
		extern s_weapon_group_definition* shotgun_tag;
		extern int smg_datum;
		extern s_weapon_group_definition* smg_tag;
		extern int sniper_datum;
		extern s_weapon_group_definition* sniper_tag;

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
		char* GetString(e_weapon_offsets_string string, const std::string& id = "");
		void Render(bool* p_open);
		void Open();
		void Close();
		void ApplyOffset(s_weapon_group_definition* tag, float i, float j, float k);
		void ApplyOffsetImgui(s_weapon_group_definition* tag, char* slider, float& offset);
		void Initialize();
	}
}