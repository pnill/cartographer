#include "stdafx.h"

#include "game/game.h"
#include "H2MOD/Modules/Input/PlayerControl.h"
#include "H2MOD/GUI/ImGui_Integration/ImGui_Handler.h"
#include "H2MOD/Tags/MetaLoader/tag_loader.h"
#include "H2MOD/Modules/WeaponOffsets/WeaponOffsetConfig.h"


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
			std::map<int, std::map<e_weapon_offsets_string, const char*>> string_table;
			// Used for controls that use the same string, A identifier has to be appended to them
			// I.E Reset##1... Reset##20
			std::map<std::string, std::string> string_cache;
			void OffsetMenu(int weapon, const char* slider, e_weapon_offsets_string text, float& offset, float default_value)
			{
				ImVec2 item_size = ImGui::GetItemRectSize();
				ImVec2 b2_size = ImVec2(WidthPercentage(20), item_size.y);

				std::string sliderId(slider); sliderId += "slider";
				std::string buttonId(slider); buttonId += "button";

				ImGui::Text(GetString(text));
				ImGui::PushItemWidth(WidthPercentage(60));
				ImGui::SliderFloat(slider, &offset, -0.15, 0.15, ""); ImGui::SameLine();
				if (ImGui::IsItemEdited() && weapOffsets[weapon].tag != nullptr) { ApplyOffset(weapon); }

				ImGui::PushItemWidth(WidthPercentage(20));
				ImGui::InputFloat(sliderId.c_str(), &offset, -0.15, 0.15, "%.3f"); ImGui::SameLine();
				if (ImGui::IsItemEdited() && weapOffsets[weapon].tag != nullptr) { ApplyOffset(weapon); }

				ImGui::PushItemWidth(WidthPercentage(20));
				if (ImGui::Button(GetString(reset, buttonId.c_str()), b2_size))
				{
					offset = default_value;
					if (weapOffsets[weapon].tag != nullptr) { ApplyOffset(weapon); }
				}
				ImGui::PopItemWidth();
			}
			void OffsetSettings()
			{
				static int selectedOption = 0;

				// TODO cleanup
				const char* weapons[] = { 
					GetString(battle_rifle_title), GetString(beam_rifle_title),GetString(brute_plasma_rifle_title), GetString(brute_shot_title),
					GetString(carbine_title), GetString(energy_sword_title), GetString(fuel_rod_title), GetString(magnum_title),
					GetString(needler_title), GetString(plasma_pistol_title), GetString(plasma_rifle_title), GetString(rocket_launcher_title),
					GetString(sentinel_beam_title), GetString(shotgun_title), GetString(smg_title), GetString(sniper_title) 
				};

				// Setup combo box menus for each weapon
				ImGui::Combo(GetString(combo_title), &selectedOption, weapons, ARRAYSIZE(weapons));
				OffsetMenu(selectedOption, "##OffsetX", weapon_offset_x,
					weapOffsets[selectedOption].modifiedOffset.i, weapOffsets[selectedOption].defaultOffset.i);
				OffsetMenu(selectedOption, "##OffsetY", weapon_offset_y,
					weapOffsets[selectedOption].modifiedOffset.j, weapOffsets[selectedOption].defaultOffset.j);
				OffsetMenu(selectedOption, "##OffsetZ", weapon_offset_z,
					weapOffsets[selectedOption].modifiedOffset.k, weapOffsets[selectedOption].defaultOffset.k);
			}
		}

		const char* GetString(e_weapon_offsets_string string, const std::string& id)
		{
			if (id.empty()) {
				return string_table.at(0).at(string);
			}
			if (!string_cache.count(id))
			{
				std::string temp_str(string_table.at(0).at(string));
				temp_str.append("##");
				temp_str.append(id);
				string_cache[id] = temp_str;
			}
			return string_cache[id].c_str();
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
			if (ImGui::Begin(GetString(e_weapon_offsets_string::title), &open, window_flags))
			{
				OffsetSettings();
			}

			ImGui::PopStyleVar();
			ImGui::End();
			if (!open)
			{
				ImGuiHandler::ToggleWindow("Weapon Offsets");
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
		void MapLoad()
		{
			for (int i = 0; i < ARRAYSIZE(weapOffsets); i++)
			{
				datum weap_datum = tags::find_tag(_tag_group_weapon, weapOffsets[i].weaponPath);
				if (!DATUM_IS_NONE(weap_datum))
				{
					weapOffsets[i].tag = tags::get_tag<_tag_group_weapon, _weapon_definition>(weap_datum);

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
