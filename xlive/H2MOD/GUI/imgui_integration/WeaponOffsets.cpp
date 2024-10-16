#include "stdafx.h"

#include "WeaponOffsetsStringTable.h"

#include "cseries/cseries_strings.h"
#include "game/game.h"
#include "H2MOD/GUI/ImGui_Integration/ImGui_Handler.h"
#include "H2MOD/Modules/WeaponOffsets/WeaponOffsetConfig.h"

/* globals */

c_static_string<256> g_weapon_offsets_temp_string;

/* prototypes */

const char* weapon_offsets_get_string(e_weapon_offsets_string string, const char* id = NULL);

namespace ImGuiHandler {
	namespace WeaponOffsets {
		s_weapon_custom_offset weapOffsets[16]
		{
			{{{0.02f, 0.f, -0.004f}},		"objects\\weapons\\rifle\\battle_rifle\\battle_rifle", {{0,0,0}}, NULL},
			{{{0.02f, 0.f, 0.f}},			"objects\\weapons\\rifle\\beam_rifle\\beam_rifle", {{0,0,0}}, NULL},
			{{{0.02f, 0.f, 0.f}},			"objects\\weapons\\rifle\\brute_plasma_rifle\\brute_plasma_rifle", {{0,0,0}}, NULL},
			{{{0.02f, -0.01f, 0.001f}},		"objects\\weapons\\support_low\\brute_shot\\brute_shot", {{0,0,0}}, NULL},
			{{{0.02f, 0.f, -0.004f}},		"objects\\weapons\\rifle\\covenant_carbine\\covenant_carbine", {{0,0,0}}, NULL},
			{{{0.05f, 0.f, 0.f}},			"objects\\weapons\\melee\\energy_blade\\energy_blade", {{0,0,0}}, NULL},
			{{{0.02f, 0.f, -0.005f}},		"objects\\weapons\\support_high\\flak_cannon\\flak_cannon", {{0,0,0}}, NULL},
			{{{0.03f, 0.f, -0.005f}},		"objects\\weapons\\pistol\\magnum\\magnum", {{0,0,0}}, NULL},
			{{{0.03f, 0.f, -0.005f}},		"objects\\weapons\\pistol\\needler\\needler", {{0,0,0}}, NULL},
			{{{0.03f, 0.f, -0.005f}},		"objects\\weapons\\pistol\\plasma_pistol\\plasma_pistol", {{0,0,0}}, NULL},
			{{{0.02f, 0.f, 0.f}},			"objects\\weapons\\rifle\\plasma_rifle\\plasma_rifle", {{0,0,0}}, NULL},
			{{{0.02f, 0.f, -0.005f}},		"objects\\weapons\\support_high\\rocket_launcher\\rocket_launcher", {{0,0,0}}, NULL},
			{{{0.02f, 0.03f, 0.f}},			"objects\\characters\\sentinel_aggressor\\weapons\\beam\\sentinel_aggressor_beam", {{0,0,0}}, NULL},
			{{{0.02f, -0.001f, -0.015f}},	"objects\\weapons\\rifle\\shotgun\\shotgun", {{0,0,0}}, NULL},
			{{{0.02f, 0.f, 0.f}},			"objects\\weapons\\rifle\\smg\\smg", {{0,0,0}}, NULL},
			{{{0.01f, 0.f, 0.f}},			"objects\\weapons\\rifle\\sniper_rifle\\sniper_rifle", {{0,0,0}}, NULL}
		};

		void ApplyOffset(int weapon)
		{
			if (weapOffsets[weapon].tag != nullptr)
			{
				weapOffsets[weapon].tag->first_person_weapon_offset = weapOffsets[weapon].modifiedOffset;
			}
		}
		namespace
		{
			void OffsetMenu(int weapon, const char* slider, e_weapon_offsets_string text, float& offset, float default_value)
			{
				ImVec2 item_size = ImGui::GetItemRectSize();
				ImVec2 b2_size = ImVec2(WidthPercentage(20), item_size.y);

				std::string sliderId(slider); sliderId += "slider";
				std::string buttonId(slider); buttonId += "button";

				ImGui::Text(weapon_offsets_get_string(text));
				ImGui::PushItemWidth(WidthPercentage(60));
				ImGui::SliderFloat(slider, &offset, -0.15f, 0.15f, ""); ImGui::SameLine();
				if (ImGui::IsItemEdited() && weapOffsets[weapon].tag != nullptr) { ApplyOffset(weapon); }

				ImGui::PushItemWidth(WidthPercentage(20));
				ImGui::InputFloat(sliderId.c_str(), &offset, -0.15f, 0.15f, "%.3f"); ImGui::SameLine();
				if (ImGui::IsItemEdited() && weapOffsets[weapon].tag != nullptr) { ApplyOffset(weapon); }

				ImGui::PushItemWidth(WidthPercentage(20));
				if (ImGui::Button(weapon_offsets_get_string(_weapon_offsets_string_reset, buttonId.c_str()), b2_size))
				{
					offset = default_value;
					if (weapOffsets[weapon].tag != nullptr) { ApplyOffset(weapon); }
				}
				ImGui::PopItemWidth();
			}
			void OffsetSettings()
			{
				static int selectedOption = 0;

				// Populate weapon string list
				const char* weapons[k_weapon_offsets_weapon_title_count];
				uint32 i = 0;
				for (uint8 string = k_weapon_offsets_weapon_title_first; string <= k_weapon_offsets_weapon_title_last; string++)
				{
					weapons[i] = weapon_offsets_get_string((e_weapon_offsets_string)string);
					++i;
				}

				// Setup combo box menus for each weapon
				ImGui::Combo(weapon_offsets_get_string(_weapon_offsets_string_combo_title), &selectedOption, weapons, ARRAYSIZE(weapons));
				OffsetMenu(selectedOption, "##OffsetX", _weapon_offsets_string_weapon_offset_x,
					weapOffsets[selectedOption].modifiedOffset.x, weapOffsets[selectedOption].defaultOffset.x);
				OffsetMenu(selectedOption, "##OffsetY", _weapon_offsets_string_weapon_offset_y,
					weapOffsets[selectedOption].modifiedOffset.y, weapOffsets[selectedOption].defaultOffset.y);
				OffsetMenu(selectedOption, "##OffsetZ", _weapon_offsets_string_weapon_offset_z,
					weapOffsets[selectedOption].modifiedOffset.z, weapOffsets[selectedOption].defaultOffset.z);
			}
		}

		void Render(bool* p_open)
		{
			bool open = *p_open;
			
			ImGuiIO& io = ImGui::GetIO();
			const ImGuiViewport* viewport = ImGui::GetMainViewport();
			ImGuiWindowFlags window_flags = 0
				| ImGuiWindowFlags_NoCollapse
				| ImGuiWindowFlags_AlwaysVerticalScrollbar
				;
			ImGui::SetNextWindowPos(ImVec2(io.DisplaySize.x * 0.5f, io.DisplaySize.y * 0.5f), ImGuiCond_::ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));
			ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(4, 8));
			ImGui::SetNextWindowSize(ImVec2(450, 320), ImGuiCond_Appearing);
			ImGui::SetNextWindowSizeConstraints(ImVec2(410, 320), ImVec2(1920, 1080));
			if (game_is_ui_shell())
				ImGui::SetNextWindowBgAlpha(1);
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
			ReadWeaponOffsetConfig(weapOffsets, ARRAYSIZE(weapOffsets));
		}
		void Close()
		{
			SaveWeaponOffsetConfig(weapOffsets, ARRAYSIZE(weapOffsets), false);
		}

		void MapLoad()
		{
			for (int i = 0; i < ARRAYSIZE(weapOffsets); i++)
			{
				datum weap_datum = tag_loaded(_tag_group_weapon, weapOffsets[i].weaponPath);
				if (weap_datum != NONE)
				{
					weapOffsets[i].tag = (_weapon_definition*)tag_get_fast(weap_datum);
					ApplyOffset(i);
				}
			}
		}
		void Initialize()
		{
			WriteDefaultFile(weapOffsets, ARRAYSIZE(weapOffsets));
			ReadWeaponOffsetConfig(weapOffsets, ARRAYSIZE(weapOffsets));
		}
	}
}

/* private code */

const char* weapon_offsets_get_string(e_weapon_offsets_string string, const char* id)
{
	e_language language = get_current_language();
	const char* result;

	if (g_weapon_offsets_string_table[language] != NULL &&
		g_weapon_offsets_string_table[language][string] != NULL)
	{
		result = g_weapon_offsets_string_table[language][string];
	}
	else
	{
		result = g_weapon_offsets_string_table[_language_english][string];
	}


	if (id != NULL)
	{
		g_weapon_offsets_temp_string.set(result);
		g_weapon_offsets_temp_string.append("##");
		g_weapon_offsets_temp_string.append(id);
		result = g_weapon_offsets_temp_string.get_string();
	}

	return result;
}
