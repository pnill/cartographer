#include "stdafx.h"
#include "H2MOD.h"
#include "H2MOD\Modules\Input\PlayerControl.h"
#include "H2MOD\GUI\imgui_integration\imgui_handler.h"
#include "Util\Hooks\Hook.h"
#include "H2MOD\Tags\MetaLoader\tag_loader.h"
#include "H2MOD\Modules\WeaponOffsets\WeaponOffsetConfig\WeaponOffsetConfig.h"

namespace imgui_handler {
	namespace WeaponOffsets {
		s_weapon_custom_offset customOffsets[16]
		{
			{{0.02, 0, -0.004}, "objects\\weapons\\rifle\\battle_rifle\\battle_rifle", {0,0,0}, NULL},
			{{0.02, 0, 0}, "objects\\weapons\\rifle\\beam_rifle\\beam_rifle", {0,0,0}, NULL},
			{{0.02, 0, 0}, "objects\\weapons\\rifle\\brute_plasma_rifle\\brute_plasma_rifle", {0,0,0}, NULL},
			{{0.02, -0.01, 0.001}, "objects\\weapons\\support_low\\brute_shot\\brute_shot", {0,0,0}, NULL},
			{{0.02, 0, -0.004}, "objects\\weapons\\rifle\\covenant_carbine\\covenant_carbine", {0,0,0}, NULL},
			{{0.05, 0, 0}, "objects\\weapons\\melee\\energy_blade\\energy_blade", {0,0,0}, NULL},
			{{0.02, 0, -0.005}, "objects\\weapons\\support_high\\flak_cannon\\flak_cannon", {0,0,0}, NULL},
			{{0.03, 0, -0.005}, "objects\\weapons\\pistol\\magnum\\magnum", {0,0,0}, NULL},
			{{0.03, 0, -0.005}, "objects\\weapons\\pistol\\needler\\needler", {0,0,0}, NULL},
			{{0.03, 0, -0.005}, "objects\\weapons\\pistol\\plasma_pistol\\plasma_pistol", {0,0,0}, NULL},
			{{0.02, 0, 0}, "objects\\weapons\\rifle\\plasma_rifle\\plasma_rifle", {0,0,0}, NULL},
			{{0.02, 0, -0.005}, "objects\\weapons\\support_high\\rocket_launcher\\rocket_launcher", {0,0,0}, NULL},
			{{0.02, 0.03, 0}, "objects\\characters\\sentinel_aggressor\\weapons\\beam\\sentinel_aggressor_beam", {0,0,0}, NULL},
			{{0.02, -0.001, -0.015}, "objects\\weapons\\rifle\\shotgun\\shotgun", {0,0,0}, NULL},
			{{0.02, 0, 0}, "objects\\weapons\\rifle\\smg\\smg", {0,0,0}, NULL},
			{{0.01, 0, 0}, "objects\\weapons\\rifle\\sniper_rifle\\sniper_rifle", {0,0,0}, NULL}
		};

		namespace
		{
			std::map<int, std::map<e_weapon_offsets_string, char*>> string_table;
			// Used for controls that use the same string, A identifier has to be appended to them
			// I.E Reset##1... Reset##20
			std::map<std::string, std::string> string_cache;
			void OffsetMenu(Weapons weapon, char* slider, e_weapon_offsets_string text, float& offset, const float default_value)
			{
				// Bullshit for unique widget ids
				char item2[32], item3[32];
				strcpy(item2, slider);
				strcat(item2, "1");
				strcpy(item3, slider);
				strcat(item3, "2");

				ImVec2 item_size = ImGui::GetItemRectSize();
				ImVec2 b2_size = ImVec2(WidthPercentage(20), item_size.y);

				ImGui::Text(GetString(text));
				ImGui::PushItemWidth(WidthPercentage(60));
				ImGui::SliderFloat(slider, &offset, -0.15, 0.15, ""); ImGui::SameLine();
				if (ImGui::IsItemEdited() && customOffsets[weapon].tag != nullptr) { ApplyOffset(weapon); }

				ImGui::PushItemWidth(WidthPercentage(20));
				ImGui::InputFloat(item2, &offset, -0.15, 0.15, "%.3f"); ImGui::SameLine();
				if (ImGui::IsItemEdited() && customOffsets[weapon].tag != nullptr) { ApplyOffset(weapon); }

				ImGui::PushItemWidth(WidthPercentage(20));
				if (ImGui::Button(GetString(reset, item3), b2_size))
				{
					offset = default_value;
					if (customOffsets[weapon].tag != nullptr) { ApplyOffset(weapon); }
				}
				ImGui::PopItemWidth();
			}
			void OffsetSettings()
			{

				static const char* weapons[]
				{ GetString(battle_rifle_title), GetString(beam_rifle_title),GetString(brute_plasma_rifle_title), GetString(brute_shot_title),
				GetString(carbine_title), GetString(energy_sword_title), GetString(fuel_rod_title), GetString(magnum_title),
				GetString(needler_title), GetString(plasma_pistol_title), GetString(plasma_rifle_title), GetString(rocket_launcher_title),
				GetString(sentinel_beam_title), GetString(shotgun_title), GetString(smg_title), GetString(sniper_title) };
				static int selectedOption = 0;

				// Setup combo box menus for each weapon
				ImGui::Combo(GetString(combo_title), &selectedOption, weapons, IM_ARRAYSIZE(weapons));
				OffsetMenu(static_cast<Weapons>(selectedOption), "##OffsetX", weapon_offset_x,
					customOffsets[selectedOption].ModifiedOffset.i, customOffsets[selectedOption].DefaultOffset.i);
				OffsetMenu(static_cast<Weapons>(selectedOption), "##OffsetY", weapon_offset_y,
					customOffsets[selectedOption].ModifiedOffset.j, customOffsets[selectedOption].DefaultOffset.j);
				OffsetMenu(static_cast<Weapons>(selectedOption), "##OffsetZ", weapon_offset_z,
					customOffsets[selectedOption].ModifiedOffset.k, customOffsets[selectedOption].DefaultOffset.k);
			}
		}

		char* GetString(e_weapon_offsets_string string, const std::string& id)
		{
			if (id.empty()) {
				return const_cast<char*>(string_table.at(0).at(string));
			}
			if (!string_cache.count(id))
			{
				std::string temp_str(const_cast<char*>(string_table.at(0).at(string)));
				temp_str.append("##");
				temp_str.append(id);
				string_cache[id] = temp_str;
			}
			return (char*)string_cache[id].c_str();
		}
		void Render(bool* p_open)
		{
			ImGuiIO& io = ImGui::GetIO();
			RECT clientRect;
			::GetClientRect(get_HWND(), &clientRect);
			io.DisplaySize = ImVec2((float)(clientRect.right - clientRect.left), (float)(clientRect.bottom - clientRect.top));
			ImGuiWindowFlags window_flags = 0;
			window_flags |= ImGuiWindowFlags_NoCollapse;
			window_flags |= ImGuiWindowFlags_AlwaysVerticalScrollbar;
			ImGui::SetNextWindowPos(ImVec2(io.DisplaySize.x * 0.5f, io.DisplaySize.y * 0.5f), ImGuiCond_::ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));
			ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(4, 8));
			ImGui::SetNextWindowSize(ImVec2(450, 320), ImGuiCond_Appearing);
			ImGui::SetNextWindowSizeConstraints(ImVec2(410, 320), ImVec2(1920, 1080));
			if (h2mod->GetEngineType() == _main_menu)
				ImGui::SetNextWindowBgAlpha(1);
			if (ImGui::Begin(GetString(e_weapon_offsets_string::title), p_open, window_flags))
			{
				OffsetSettings();
			}

			ImGui::PopStyleVar();
			ImGui::End();
			if (!*p_open)
				Close();
		}
		void Open()
		{
			g_NumWindowsOpen++;
			WriteValue<byte>(Memory::GetAddress(0x9712cC), 1);		// Enable Cursor visibility
			ImGuiToggleInput(true);
			PlayerControl::DisableLocalCamera(true);
			ReadWeaponOffsetConfig(customOffsets);
		}
		void Close()
		{
			g_NumWindowsOpen--;
			if (g_NumWindowsOpen == 0)
			{
				WriteValue<byte>(Memory::GetAddress(0x9712cC), 0);		// Disable Cursor visibility
				ImGuiToggleInput(false);
				PlayerControl::DisableLocalCamera(false);
			}
			SaveWeaponOffsetConfig(customOffsets, false);
		}
		void BuildStringsTable()
		{
			string_table[0][e_weapon_offsets_string::title] = "        Weapon Offsets";
			string_table[0][e_weapon_offsets_string::combo_title] = "Weapon";
			string_table[0][e_weapon_offsets_string::battle_rifle_title] = "Battle Rifle";
			string_table[0][e_weapon_offsets_string::beam_rifle_title] = "Beam Rifle";
			string_table[0][e_weapon_offsets_string::brute_plasma_rifle_title] = "Brute Plasma Rifle";
			string_table[0][e_weapon_offsets_string::brute_shot_title] = "Brute Shot";
			string_table[0][e_weapon_offsets_string::carbine_title] = "Carbine";
			string_table[0][e_weapon_offsets_string::energy_sword_title] = "Energy Sword";
			string_table[0][e_weapon_offsets_string::fuel_rod_title] = "Fuel Rod";
			string_table[0][e_weapon_offsets_string::magnum_title] = "Magnum";
			string_table[0][e_weapon_offsets_string::needler_title] = "Needler";
			string_table[0][e_weapon_offsets_string::plasma_pistol_title] = "Plasma Pistol";
			string_table[0][e_weapon_offsets_string::plasma_rifle_title] = "Plasma Rifle";
			string_table[0][e_weapon_offsets_string::rocket_launcher_title] = "Rocket Launcher";
			string_table[0][e_weapon_offsets_string::sentinel_beam_title] = "Sentinel Beam";
			string_table[0][e_weapon_offsets_string::shotgun_title] = "Shotgun";
			string_table[0][e_weapon_offsets_string::smg_title] = "SMG";
			string_table[0][e_weapon_offsets_string::sniper_title] = "Sniper";
			string_table[0][e_weapon_offsets_string::weapon_offset_x] = "Weapon Offset X";
			string_table[0][e_weapon_offsets_string::weapon_offset_y] = "Weapon Offset Y";
			string_table[0][e_weapon_offsets_string::weapon_offset_z] = "Weapon Offset Z";
			string_table[0][e_weapon_offsets_string::reset] = "Reset";

			//Spanish.
			string_table[4][e_weapon_offsets_string::title] = "        Compensaciones de armas";
			string_table[4][e_weapon_offsets_string::combo_title] = "Arma";
			string_table[4][e_weapon_offsets_string::battle_rifle_title] = "Fusil de Batalla";
			string_table[4][e_weapon_offsets_string::beam_rifle_title] = "Fusil de Haz";
			string_table[4][e_weapon_offsets_string::brute_plasma_rifle_title] = "Rifle de Plasma Bruto";
			string_table[4][e_weapon_offsets_string::brute_shot_title] = "Disparo Bruto";
			string_table[4][e_weapon_offsets_string::carbine_title] = "Carabina";
			string_table[4][e_weapon_offsets_string::energy_sword_title] = "Espada de Energía";
			string_table[4][e_weapon_offsets_string::fuel_rod_title] = "Varilla de Combustible";
			string_table[4][e_weapon_offsets_string::magnum_title] = "Botella doble";
			string_table[4][e_weapon_offsets_string::needler_title] = "Aguijoneador";
			string_table[4][e_weapon_offsets_string::plasma_pistol_title] = "Pistola de Plasma";
			string_table[4][e_weapon_offsets_string::plasma_rifle_title] = "Fusil de Plasma";
			string_table[4][e_weapon_offsets_string::rocket_launcher_title] = "Lanzacohetes";
			string_table[4][e_weapon_offsets_string::sentinel_beam_title] = "Rayo Centinela";
			string_table[4][e_weapon_offsets_string::shotgun_title] = "Escopeta";
			string_table[4][e_weapon_offsets_string::smg_title] = "Subfusil";
			string_table[4][e_weapon_offsets_string::sniper_title] = "Francotirador";
			string_table[4][e_weapon_offsets_string::weapon_offset_x] = "Compensación de Armas X";
			string_table[4][e_weapon_offsets_string::weapon_offset_y] = "Compensación de Armas Y";
			string_table[4][e_weapon_offsets_string::weapon_offset_z] = "Compensación de Armas Z";
			string_table[4][e_weapon_offsets_string::reset] = "Reiniciar";
		}
		void ApplyOffset(Weapons weapon)
		{
			if (customOffsets[weapon].tag != nullptr)
			{
				customOffsets[weapon].tag->first_person_weapon_offset = customOffsets[weapon].ModifiedOffset;
			}
		}
		void MapLoad()
		{
			for (byte i = BattleRifle; i != Sniper + 1; i++)
			{
				customOffsets[i].tag = tags::get_tag < blam_tag::tag_group_type::weapon, s_weapon_group_definition>
					(tags::find_tag(blam_tag::tag_group_type::weapon, customOffsets[i].WeaponPath));

				ApplyOffset(static_cast<Weapons>(i));
			}
		}
		void Initialize() 
		{
			WriteDefaultFile(customOffsets);

			ReadWeaponOffsetConfig(customOffsets);
		}
	}
}