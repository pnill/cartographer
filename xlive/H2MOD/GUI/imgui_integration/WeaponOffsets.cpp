#include "stdafx.h"

#ifndef IMGUI_DISABLE

#include "WeaponOffsetsStringTable.h"

#include "game/game.h"
#include "saved_games/cartographer_player_profile/cartographer_player_profile.h"
#include "H2MOD/GUI/ImGui_Integration/ImGui_Handler.h"

#include "imgui.h"

/* globals */

extern s_saved_game_cartographer_player_profile* g_advanced_settings_current_cartographer_profile;

static c_static_string<256> g_weapon_offsets_temp_string;

/* prototypes */

static const char* weapon_offsets_get_string(e_weapon_offsets_string string, const char* id = NULL);

namespace ImGuiHandler {
	namespace WeaponOffsets
	{
		namespace
		{
			void OffsetMenu(e_weapon_offset_weapon weapon, const char* slider, e_weapon_offsets_string text, float& offset, float default_value)
			{
				ImVec2 item_size = ImGui::GetItemRectSize();
				ImVec2 b2_size = ImVec2(WidthPercentage(20), item_size.y);

				std::string sliderId(slider); sliderId += "slider";
				std::string buttonId(slider); buttonId += "button";

				ImGui::Text(weapon_offsets_get_string(text));
				ImGui::PushItemWidth(WidthPercentage(60));
				ImGui::SliderFloat(slider, &offset, -0.15f, 0.15f, ""); ImGui::SameLine();

				ImGui::PushItemWidth(WidthPercentage(20));
				ImGui::InputFloat(sliderId.c_str(), &offset, -0.15f, 0.15f, "%.3f"); ImGui::SameLine();

				ImGui::PushItemWidth(WidthPercentage(20));
				if (ImGui::Button(weapon_offsets_get_string(_weapon_offsets_string_reset, buttonId.c_str()), b2_size))
				{
					offset = default_value;
				}
				ImGui::PopItemWidth();
			}
			void OffsetSettings()
			{
				static e_weapon_offset_weapon selected_weapon = _weapon_offset_weapon_battle_rifle;

				// Populate weapon string list
				const char* weapons[k_weapon_offsets_weapon_title_count];
				uint32 i = 0;
				for (uint8 string = k_weapon_offsets_weapon_title_first; string <= k_weapon_offsets_weapon_title_last; string++)
				{
					weapons[i] = weapon_offsets_get_string((e_weapon_offsets_string)string);
					++i;
				}

				// Setup combo box menus for each weapon
				ImGui::Combo(weapon_offsets_get_string(_weapon_offsets_string_combo_title), (int*)&selected_weapon, weapons, k_weapon_offset_weapon_count);
				OffsetMenu(selected_weapon, "##OffsetX", _weapon_offsets_string_weapon_offset_x, g_advanced_settings_current_cartographer_profile->weapon_offsets[selected_weapon].x, k_weapon_offset_constant_data[selected_weapon].default_offset.x);
				OffsetMenu(selected_weapon, "##OffsetY", _weapon_offsets_string_weapon_offset_y, g_advanced_settings_current_cartographer_profile->weapon_offsets[selected_weapon].y, k_weapon_offset_constant_data[selected_weapon].default_offset.y);
				OffsetMenu(selected_weapon, "##OffsetZ", _weapon_offsets_string_weapon_offset_z, g_advanced_settings_current_cartographer_profile->weapon_offsets[selected_weapon].z, k_weapon_offset_constant_data[selected_weapon].default_offset.z);
			}
		}

		void Render(bool* p_open)
		{
			bool open = *p_open;
			
			ImGuiIO& io = ImGui::GetIO();
			ImGuiWindowFlags window_flags = 0 | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_AlwaysVerticalScrollbar;
			ImGui::SetNextWindowPos(ImVec2(io.DisplaySize.x * 0.5f, io.DisplaySize.y * 0.5f), ImGuiCond_::ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));
			ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(4, 8));
			ImGui::SetNextWindowSize(ImVec2(450, 320), ImGuiCond_Appearing);
			ImGui::SetNextWindowSizeConstraints(ImVec2(410, 320), ImVec2(1920, 1080));
			if (game_is_ui_shell())
			{
				ImGui::SetNextWindowBgAlpha(1);
			}
			if (ImGui::Begin(weapon_offsets_get_string(_weapon_offsets_string_title), &open, window_flags))
			{
				OffsetSettings();
			}

			ImGui::PopStyleVar();
			ImGui::End();
			if (!open)
			{
				ImGuiHandler::ToggleWindow(k_weapon_offsets_window_name);
			}
		}

		void Open()
		{
		}

		void Close()
		{
			if (cartographer_player_profile_is_signed_in(g_advanced_settings_current_controller_index))
			{
				cartographer_player_profile_save(g_advanced_settings_current_controller_index);
			}
		}
	}
}

/* private code */

static const char* weapon_offsets_get_string(e_weapon_offsets_string string, const char* id)
{
	const e_language language = get_current_language();
	const char* result = k_weapon_offsets_string_table[language][string];

	if (id != NULL)
	{
		g_weapon_offsets_temp_string.set(result);
		g_weapon_offsets_temp_string.append("##");
		g_weapon_offsets_temp_string.append(id);
		result = g_weapon_offsets_temp_string.get_string();
	}

	return result;
}

#endif
