#include "H2MOD.h"
#include "H2MOD\Modules\Input\PlayerControl.h"
#include "imgui_handler.h"
#include "Util\Hooks\Hook.h"
#include "H2MOD\Tags\TagInterface.h"
#include "H2MOD\Modules\Config\WeaponOffsetConfig\WeaponOffsetConfig.h"

namespace imgui_handler {
	namespace WeaponOffsets {
		namespace
		{
			std::map<int, std::map<e_weapon_offsets_string, char*>> string_table;
			// Used for controls that use the same string, A identifier has to be appended to them
			// I.E Reset##1... Reset##20
			std::map<std::string, std::string> string_cache;
			void HudSettings()
			{	
				// Tag references
				auto battle_rifle_datum = tags::find_tag(blam_tag::tag_group_type::weapon, "objects\\weapons\\rifle\\battle_rifle\\battle_rifle");
				auto battle_rifle_tag = tags::get_tag < blam_tag::tag_group_type::weapon, s_weapon_group_definition>(battle_rifle_datum);
				auto beam_rifle_datum = tags::find_tag(blam_tag::tag_group_type::weapon, "objects\\weapons\\rifle\\beam_rifle\\beam_rifle");
				auto beam_rifle_tag = tags::get_tag < blam_tag::tag_group_type::weapon, s_weapon_group_definition>(beam_rifle_datum);
				auto brute_plasma_rifle_datum = tags::find_tag(blam_tag::tag_group_type::weapon, "objects\\weapons\\rifle\\brute_plasma_rifle\\brute_plasma_rifle");
				auto brute_plasma_rifle_tag = tags::get_tag < blam_tag::tag_group_type::weapon, s_weapon_group_definition>(brute_plasma_rifle_datum);
				auto brute_shot_datum = tags::find_tag(blam_tag::tag_group_type::weapon, "objects\\weapons\\support_low\\brute_shot\\brute_shot");
				auto brute_shot_tag = tags::get_tag < blam_tag::tag_group_type::weapon, s_weapon_group_definition>(brute_shot_datum);
				auto carbine_datum = tags::find_tag(blam_tag::tag_group_type::weapon, "objects\\weapons\\rifle\\covenant_carbine\\covenant_carbine");
				auto carbine_tag = tags::get_tag < blam_tag::tag_group_type::weapon, s_weapon_group_definition>(carbine_datum);
				auto energy_sword_datum = tags::find_tag(blam_tag::tag_group_type::weapon, "objects\\weapons\\melee\\energy_blade\\energy_blade");
				auto energy_sword_tag = tags::get_tag < blam_tag::tag_group_type::weapon, s_weapon_group_definition>(energy_sword_datum);
				auto fuel_rod_datum = tags::find_tag(blam_tag::tag_group_type::weapon, "objects\\weapons\\support_high\\flak_cannon\\flak_cannon");
				auto fuel_rod_tag = tags::get_tag < blam_tag::tag_group_type::weapon, s_weapon_group_definition>(fuel_rod_datum);
				auto magnum_datum = tags::find_tag(blam_tag::tag_group_type::weapon, "objects\\weapons\\pistol\\magnum\\magnum");
				auto magnum_tag = tags::get_tag < blam_tag::tag_group_type::weapon, s_weapon_group_definition>(magnum_datum);
				auto needler_datum = tags::find_tag(blam_tag::tag_group_type::weapon, "objects\\weapons\\pistol\\needler\\needler");
				auto needler_tag = tags::get_tag < blam_tag::tag_group_type::weapon, s_weapon_group_definition>(needler_datum);
				auto plasma_pistol_datum = tags::find_tag(blam_tag::tag_group_type::weapon, "objects\\weapons\\pistol\\plasma_pistol\\plasma_pistol");
				auto plasma_pistol_tag = tags::get_tag < blam_tag::tag_group_type::weapon, s_weapon_group_definition>(plasma_pistol_datum);
				auto plasma_rifle_datum = tags::find_tag(blam_tag::tag_group_type::weapon, "objects\\weapons\\rifle\\plasma_rifle\\plasma_rifle");
				auto plasma_rifle_tag = tags::get_tag < blam_tag::tag_group_type::weapon, s_weapon_group_definition>(plasma_rifle_datum);
				auto rocket_launcher_datum = tags::find_tag(blam_tag::tag_group_type::weapon, "objects\\weapons\\support_high\\rocket_launcher\\rocket_launcher");
				auto rocket_launcher_tag = tags::get_tag < blam_tag::tag_group_type::weapon, s_weapon_group_definition>(rocket_launcher_datum);
				auto sentinel_beam_datum = tags::find_tag(blam_tag::tag_group_type::weapon, "objects\\characters\\sentinel_aggressor\\weapons\\beam\\sentinel_aggressor_beam");
				auto sentinel_beam_tag = tags::get_tag < blam_tag::tag_group_type::weapon, s_weapon_group_definition>(sentinel_beam_datum);
				auto shotgun_datum = tags::find_tag(blam_tag::tag_group_type::weapon, "objects\\weapons\\rifle\\shotgun\\shotgun");
				auto shotgun_tag = tags::get_tag < blam_tag::tag_group_type::weapon, s_weapon_group_definition>(shotgun_datum);
				auto smg_datum = tags::find_tag(blam_tag::tag_group_type::weapon, "objects\\weapons\\rifle\\smg\\smg");
				auto smg_tag = tags::get_tag < blam_tag::tag_group_type::weapon, s_weapon_group_definition>(smg_datum);
				auto sniper_datum = tags::find_tag(blam_tag::tag_group_type::weapon, "objects\\weapons\\rifle\\sniper_rifle\\sniper_rifle");
				auto sniper_tag = tags::get_tag < blam_tag::tag_group_type::weapon, s_weapon_group_definition>(sniper_datum);

				// Setup drop-down menus for each weapon
				if (ImGui::CollapsingHeader(GetString(battle_rifle_title)))
				{
					OffsetMenu(battle_rifle_tag, "##BattleRifle_OffsetX", weapon_offset_x, BattleRifle_WeaponOffsetX, 0.02);
					OffsetMenu(battle_rifle_tag, "##BattleRifle_OffsetY", weapon_offset_y, BattleRifle_WeaponOffsetY, 0);
					OffsetMenu(battle_rifle_tag, "##BattleRifle_OffsetZ", weapon_offset_z, BattleRifle_WeaponOffsetZ, -0.004);
				}
				if (ImGui::CollapsingHeader(GetString(beam_rifle_title)))
				{
					OffsetMenu(beam_rifle_tag, "##BeamRifle_OffsetX", weapon_offset_x, BeamRifle_WeaponOffsetX, 0.02);
					OffsetMenu(beam_rifle_tag, "##BeamRifle_OffsetY", weapon_offset_y, BeamRifle_WeaponOffsetY, 0);
					OffsetMenu(beam_rifle_tag, "##BeamRifle_OffsetZ", weapon_offset_z, BeamRifle_WeaponOffsetZ, 0);
				}
				if (ImGui::CollapsingHeader(GetString(brute_plasma_rifle_title)))
				{
					OffsetMenu(brute_plasma_rifle_tag, "##BrutePlasmaRifle_OffsetX", weapon_offset_x, BrutePlasmaRifle_WeaponOffsetX, 0.02);
					OffsetMenu(brute_plasma_rifle_tag, "##BrutePlasmaRifle_OffsetY", weapon_offset_y, BrutePlasmaRifle_WeaponOffsetY, 0);
					OffsetMenu(brute_plasma_rifle_tag, "##BrutePlasmaRifle_OffsetZ", weapon_offset_z, BrutePlasmaRifle_WeaponOffsetZ, 0);
				}
				if (ImGui::CollapsingHeader(GetString(brute_shot_title)))
				{
					OffsetMenu(brute_shot_tag, "##BruteShot_OffsetX", weapon_offset_x, BruteShot_WeaponOffsetX, 0.02);
					OffsetMenu(brute_shot_tag, "##BruteShot_OffsetY", weapon_offset_y, BruteShot_WeaponOffsetY, -0.01);
					OffsetMenu(brute_shot_tag, "##BruteShot_OffsetZ", weapon_offset_z, BruteShot_WeaponOffsetZ, 0.001);
				}
				if (ImGui::CollapsingHeader(GetString(carbine_title)))
				{
					OffsetMenu(carbine_tag, "##Carbine_OffsetX", weapon_offset_x, Carbine_WeaponOffsetX, 0.02);
					OffsetMenu(carbine_tag, "##Carbine_OffsetY", weapon_offset_y, Carbine_WeaponOffsetY, 0);
					OffsetMenu(carbine_tag, "##Carbine_OffsetZ", weapon_offset_z, Carbine_WeaponOffsetZ, -0.004);
				}
				if (ImGui::CollapsingHeader(GetString(energy_sword_title)))
				{
					OffsetMenu(energy_sword_tag, "##EnergySword_OffsetX", weapon_offset_x, EnergySword_WeaponOffsetX, 0.05);
					OffsetMenu(energy_sword_tag, "##EnergySword_OffsetY", weapon_offset_y, EnergySword_WeaponOffsetY, 0);
					OffsetMenu(energy_sword_tag, "##EnergySword_OffsetZ", weapon_offset_z, EnergySword_WeaponOffsetZ, 0);
				}
				if (ImGui::CollapsingHeader(GetString(fuel_rod_title)))
				{
					OffsetMenu(fuel_rod_tag, "##FuelRod_OffsetX", weapon_offset_x, FuelRod_WeaponOffsetX, 0.02);
					OffsetMenu(fuel_rod_tag, "##FuelRod_OffsetY", weapon_offset_y, FuelRod_WeaponOffsetY, 0);
					OffsetMenu(fuel_rod_tag, "##FuelRod_OffsetZ", weapon_offset_z, FuelRod_WeaponOffsetZ, -0.005);
				}
				if (ImGui::CollapsingHeader(GetString(magnum_title)))
				{
					OffsetMenu(magnum_tag, "##Magnum_OffsetX", weapon_offset_x, Magnum_WeaponOffsetX, 0.03);
					OffsetMenu(magnum_tag, "##Magnum_OffsetY", weapon_offset_y, Magnum_WeaponOffsetY, 0);
					OffsetMenu(magnum_tag, "##Magnum_OffsetZ", weapon_offset_z, Magnum_WeaponOffsetZ, -0.005);
				}
				if (ImGui::CollapsingHeader(GetString(needler_title)))
				{
					OffsetMenu(needler_tag, "##Needler_OffsetX", weapon_offset_x, Needler_WeaponOffsetX, 0.03);
					OffsetMenu(needler_tag, "##Needler_OffsetY", weapon_offset_y, Needler_WeaponOffsetY, 0);
					OffsetMenu(needler_tag, "##Needler_OffsetZ", weapon_offset_z, Needler_WeaponOffsetZ, -0.005);
				}
				if (ImGui::CollapsingHeader(GetString(plasma_pistol_title)))
				{
					OffsetMenu(plasma_pistol_tag, "##PlasmaPistol_OffsetX", weapon_offset_x, PlasmaPistol_WeaponOffsetX, 0.03);
					OffsetMenu(plasma_pistol_tag, "##PlasmaPistol_OffsetY", weapon_offset_y, PlasmaPistol_WeaponOffsetY, 0);
					OffsetMenu(plasma_pistol_tag, "##PlasmaPistol_OffsetZ", weapon_offset_z, PlasmaPistol_WeaponOffsetZ, -0.005);
				}
				if (ImGui::CollapsingHeader(GetString(plasma_rifle_title)))
				{
					OffsetMenu(plasma_rifle_tag, "##PlasmaRifle_OffsetX", weapon_offset_x, PlasmaRifle_WeaponOffsetX, 0.02);
					OffsetMenu(plasma_rifle_tag, "##PlasmaRifle_OffsetY", weapon_offset_y, PlasmaRifle_WeaponOffsetY, 0);
					OffsetMenu(plasma_rifle_tag, "##PlasmaRifle_OffsetZ", weapon_offset_z, PlasmaRifle_WeaponOffsetZ, 0);
				}
				if (ImGui::CollapsingHeader(GetString(rocket_launcher_title)))
				{
					OffsetMenu(rocket_launcher_tag, "##RocketLauncher_OffsetX", weapon_offset_x, RocketLauncher_WeaponOffsetX, 0.02);
					OffsetMenu(rocket_launcher_tag, "##RocketLauncher_OffsetY", weapon_offset_y, RocketLauncher_WeaponOffsetY, 0);
					OffsetMenu(rocket_launcher_tag, "##RocketLauncher_OffsetZ", weapon_offset_z, RocketLauncher_WeaponOffsetZ, -0.005);
				}
				if (ImGui::CollapsingHeader(GetString(sentinel_beam_title)))
				{
					OffsetMenu(sentinel_beam_tag, "##SentinelBeam_OffsetX", weapon_offset_x, SentinelBeam_WeaponOffsetX, 0.02);
					OffsetMenu(sentinel_beam_tag, "##SentinelBeam_OffsetY", weapon_offset_y, SentinelBeam_WeaponOffsetY, 0.03);
					OffsetMenu(sentinel_beam_tag, "##SentinelBeam_OffsetZ", weapon_offset_z, SentinelBeam_WeaponOffsetZ, 0);
				}
				if (ImGui::CollapsingHeader(GetString(shotgun_title)))
				{
					OffsetMenu(shotgun_tag, "##Shotgun_OffsetX", weapon_offset_x, Shotgun_WeaponOffsetX, 0.02);
					OffsetMenu(shotgun_tag, "##Shotgun_OffsetY", weapon_offset_y, Shotgun_WeaponOffsetY, -0.001);
					OffsetMenu(shotgun_tag, "##Shotgun_OffsetZ", weapon_offset_z, Shotgun_WeaponOffsetZ, -0.015);
				}
				if (ImGui::CollapsingHeader(GetString(smg_title)))
				{
					OffsetMenu(smg_tag, "##SMG_OffsetX", weapon_offset_x, SMG_WeaponOffsetX, 0.02);
					OffsetMenu(smg_tag, "##SMG_OffsetY", weapon_offset_y, SMG_WeaponOffsetY, 0);
					OffsetMenu(smg_tag, "##SMG_OffsetZ", weapon_offset_z, SMG_WeaponOffsetZ, 0);
				}
				if (ImGui::CollapsingHeader(GetString(sniper_title)))
				{
					OffsetMenu(sniper_tag, "##Sniper_OffsetX", weapon_offset_x, Sniper_WeaponOffsetX, 0.01);
					OffsetMenu(sniper_tag, "##Sniper_OffsetY", weapon_offset_y, Sniper_WeaponOffsetY, 0);
					OffsetMenu(sniper_tag, "##Sniper_OffsetZ", weapon_offset_z, Sniper_WeaponOffsetZ, 0);
				}
			}
		}
		char* GetString(e_weapon_offsets_string string, const std::string& id)
		{
			/*
			if (string_table.count(H2Config_language.code_main))
			{
				if (string_table.at(H2Config_language.code_main).count(string)) {
					if (id.empty()) {
						return const_cast<char*>(string_table.at(H2Config_language.code_main).at(string));
					}

					if (!string_cache.count(id))
					{
						std::string temp_str(const_cast<char*>(string_table.at(H2Config_language.code_main).at(string)));
						temp_str.append("##");
						temp_str.append(id);
						string_cache[id] = temp_str;
					}
					return (char*)string_cache[id].c_str();
				}
			}*/
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
			ImGui::SetNextWindowSize(ImVec2(450, 530), ImGuiCond_Appearing);
			ImGui::SetNextWindowSizeConstraints(ImVec2(410, 530), ImVec2(1920, 1080));
			if (h2mod->GetEngineType() == MainMenu)
				ImGui::SetNextWindowBgAlpha(1);
			if (ImGui::Begin(GetString(e_weapon_offsets_string::title), p_open, window_flags))
			{
				HudSettings();
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
			ReadWeaponOffsetConfig();
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
			SaveWeaponOffsetConfig();
		}
		void BuildStringsTable()
		{
			string_table[0][e_weapon_offsets_string::title] = "        Weapon Offsets";
			string_table[0][e_weapon_offsets_string::battle_rifle_title] = "Battle Rifle Offset";
			string_table[0][e_weapon_offsets_string::beam_rifle_title] = "Beam Rifle Offset";
			string_table[0][e_weapon_offsets_string::brute_plasma_rifle_title] = "Brute Plasma Rifle Offset";
			string_table[0][e_weapon_offsets_string::brute_shot_title] = "Brute Shot Offset";
			string_table[0][e_weapon_offsets_string::carbine_title] = "Carbine Offset";
			string_table[0][e_weapon_offsets_string::energy_sword_title] = "Energy Sword Offset";
			string_table[0][e_weapon_offsets_string::fuel_rod_title] = "Fuel Rod Offset";
			string_table[0][e_weapon_offsets_string::magnum_title] = "Magnum Offset";
			string_table[0][e_weapon_offsets_string::needler_title] = "Needler Offset";
			string_table[0][e_weapon_offsets_string::plasma_pistol_title] = "Plasma Pistol Offset";
			string_table[0][e_weapon_offsets_string::plasma_rifle_title] = "Plasma Rifle Offset";
			string_table[0][e_weapon_offsets_string::rocket_launcher_title] = "Rocket Launcher Offset";
			string_table[0][e_weapon_offsets_string::sentinel_beam_title] = "Sentinel Beam Offset";
			string_table[0][e_weapon_offsets_string::shotgun_title] = "Shotgun Offset";
			string_table[0][e_weapon_offsets_string::smg_title] = "SMG Offset";
			string_table[0][e_weapon_offsets_string::sniper_title] = "Sniper Offset";
			string_table[0][e_weapon_offsets_string::weapon_offset_x] = "Weapon Offset X";
			string_table[0][e_weapon_offsets_string::weapon_offset_y] = "Weapon Offset Y";
			string_table[0][e_weapon_offsets_string::weapon_offset_z] = "Weapon Offset Z";
			string_table[0][e_weapon_offsets_string::reset] = "Reset";

			//Spanish.
			string_table[4][e_weapon_offsets_string::title] = "        Weapon Offsets";
			string_table[4][e_weapon_offsets_string::battle_rifle_title] = "Battle Rifle Offset";
			string_table[4][e_weapon_offsets_string::beam_rifle_title] = "Beam Rifle Offset";
			string_table[4][e_weapon_offsets_string::brute_plasma_rifle_title] = "Brute Plasma Rifle Offset";
			string_table[4][e_weapon_offsets_string::brute_shot_title] = "Brute Shot Offset";
			string_table[4][e_weapon_offsets_string::carbine_title] = "Carbine Offset";
			string_table[4][e_weapon_offsets_string::energy_sword_title] = "Energy Sword Offset";
			string_table[4][e_weapon_offsets_string::fuel_rod_title] = "Fuel Rod Offset";
			string_table[4][e_weapon_offsets_string::magnum_title] = "Magnum Offset";
			string_table[4][e_weapon_offsets_string::needler_title] = "Needler Offset";
			string_table[4][e_weapon_offsets_string::plasma_pistol_title] = "Plasma Pistol Offset";
			string_table[4][e_weapon_offsets_string::plasma_rifle_title] = "Plasma Rifle Offset";
			string_table[4][e_weapon_offsets_string::rocket_launcher_title] = "Rocket Launcher Offset";
			string_table[4][e_weapon_offsets_string::sentinel_beam_title] = "Sentinel Beam Offset";
			string_table[4][e_weapon_offsets_string::shotgun_title] = "Shotgun Offset";
			string_table[4][e_weapon_offsets_string::smg_title] = "SMG Offset";
			string_table[4][e_weapon_offsets_string::sniper_title] = "Sniper Offset";
			string_table[4][e_weapon_offsets_string::weapon_offset_x] = "Weapon Offset X";
			string_table[4][e_weapon_offsets_string::weapon_offset_y] = "Weapon Offset Y";
			string_table[4][e_weapon_offsets_string::weapon_offset_z] = "Weapon Offset Z";
			string_table[4][e_weapon_offsets_string::reset] = "Reset";

		}
		void OffsetMenu(s_weapon_group_definition* tag, char* slider, e_weapon_offsets_string text, float& offset, float default_value)
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
			if (ImGui::IsItemEdited() && tag != nullptr) {
				switch (slider[strlen(slider) - 1])
				{
				case 'X':
					tag->first_person_weapon_offset_i = offset;
					break;
				case 'Y':
					tag->first_person_weapon_offset_j = offset;
					break;
				case 'Z':
					tag->first_person_weapon_offset_k = offset;
					break;
				}
			}

			ImGui::PushItemWidth(WidthPercentage(20));
			ImGui::InputFloat(item2, &offset, -0.15, 0.15, "%.3f"); ImGui::SameLine();
			if (ImGui::IsItemEdited() && tag != nullptr) {
				if (offset > 0.15)
					offset = 0.15;
				if (offset < -0.15)
					offset = -0.15;

				switch (slider[strlen(slider) - 1])
				{
				case 'X':
					tag->first_person_weapon_offset_i = offset;
					break;
				case 'Y':
					tag->first_person_weapon_offset_j = offset;
					break;
				case 'Z':
					tag->first_person_weapon_offset_k = offset;
					break;
				}
			}

			ImGui::PushItemWidth(WidthPercentage(20));
			if (ImGui::Button(GetString(reset, item3), b2_size))
			{
				offset = default_value;
				if (tag != nullptr)
				{
					switch (slider[strlen(slider) - 1])
					{
					case 'X':
						tag->first_person_weapon_offset_i = offset;
						break;
					case 'Y':
						tag->first_person_weapon_offset_j = offset;
						break;
					case 'Z':
						tag->first_person_weapon_offset_k = offset;
						break;
					}
				}
			}
			ImGui::PopItemWidth();
		}
		void ApplyOffset(s_weapon_group_definition* tag, float i, float j, float k)
		{
			if (tag != nullptr)
			{
				tag->first_person_weapon_offset_i = i;
				tag->first_person_weapon_offset_j = j;
				tag->first_person_weapon_offset_k = k;
			}
		}
		void Initialize()
		{
			ReadWeaponOffsetConfig();

			// Tag references again... (apparently it crashes if tag references are global?)
			auto battle_rifle_datum = tags::find_tag(blam_tag::tag_group_type::weapon, "objects\\weapons\\rifle\\battle_rifle\\battle_rifle");
			auto battle_rifle_tag = tags::get_tag < blam_tag::tag_group_type::weapon, s_weapon_group_definition>(battle_rifle_datum);
			auto beam_rifle_datum = tags::find_tag(blam_tag::tag_group_type::weapon, "objects\\weapons\\rifle\\beam_rifle\\beam_rifle");
			auto beam_rifle_tag = tags::get_tag < blam_tag::tag_group_type::weapon, s_weapon_group_definition>(beam_rifle_datum);
			auto brute_plasma_rifle_datum = tags::find_tag(blam_tag::tag_group_type::weapon, "objects\\weapons\\rifle\\brute_plasma_rifle\\brute_plasma_rifle");
			auto brute_plasma_rifle_tag = tags::get_tag < blam_tag::tag_group_type::weapon, s_weapon_group_definition>(brute_plasma_rifle_datum);
			auto brute_shot_datum = tags::find_tag(blam_tag::tag_group_type::weapon, "objects\\weapons\\support_low\\brute_shot\\brute_shot");
			auto brute_shot_tag = tags::get_tag < blam_tag::tag_group_type::weapon, s_weapon_group_definition>(brute_shot_datum);
			auto carbine_datum = tags::find_tag(blam_tag::tag_group_type::weapon, "objects\\weapons\\rifle\\covenant_carbine\\covenant_carbine");
			auto carbine_tag = tags::get_tag < blam_tag::tag_group_type::weapon, s_weapon_group_definition>(carbine_datum);
			auto energy_sword_datum = tags::find_tag(blam_tag::tag_group_type::weapon, "objects\\weapons\\melee\\energy_blade\\energy_blade");
			auto energy_sword_tag = tags::get_tag < blam_tag::tag_group_type::weapon, s_weapon_group_definition>(energy_sword_datum);
			auto fuel_rod_datum = tags::find_tag(blam_tag::tag_group_type::weapon, "objects\\weapons\\support_high\\flak_cannon\\flak_cannon");
			auto fuel_rod_tag = tags::get_tag < blam_tag::tag_group_type::weapon, s_weapon_group_definition>(fuel_rod_datum);
			auto magnum_datum = tags::find_tag(blam_tag::tag_group_type::weapon, "objects\\weapons\\pistol\\magnum\\magnum");
			auto magnum_tag = tags::get_tag < blam_tag::tag_group_type::weapon, s_weapon_group_definition>(magnum_datum);
			auto needler_datum = tags::find_tag(blam_tag::tag_group_type::weapon, "objects\\weapons\\pistol\\needler\\needler");
			auto needler_tag = tags::get_tag < blam_tag::tag_group_type::weapon, s_weapon_group_definition>(needler_datum);
			auto plasma_pistol_datum = tags::find_tag(blam_tag::tag_group_type::weapon, "objects\\weapons\\pistol\\plasma_pistol\\plasma_pistol");
			auto plasma_pistol_tag = tags::get_tag < blam_tag::tag_group_type::weapon, s_weapon_group_definition>(plasma_pistol_datum);
			auto plasma_rifle_datum = tags::find_tag(blam_tag::tag_group_type::weapon, "objects\\weapons\\rifle\\plasma_rifle\\plasma_rifle");
			auto plasma_rifle_tag = tags::get_tag < blam_tag::tag_group_type::weapon, s_weapon_group_definition>(plasma_rifle_datum);
			auto rocket_launcher_datum = tags::find_tag(blam_tag::tag_group_type::weapon, "objects\\weapons\\support_high\\rocket_launcher\\rocket_launcher");
			auto rocket_launcher_tag = tags::get_tag < blam_tag::tag_group_type::weapon, s_weapon_group_definition>(rocket_launcher_datum);
			auto sentinel_beam_datum = tags::find_tag(blam_tag::tag_group_type::weapon, "objects\\characters\\sentinel_aggressor\\weapons\\beam\\sentinel_aggressor_beam");
			auto sentinel_beam_tag = tags::get_tag < blam_tag::tag_group_type::weapon, s_weapon_group_definition>(sentinel_beam_datum);
			auto shotgun_datum = tags::find_tag(blam_tag::tag_group_type::weapon, "objects\\weapons\\rifle\\shotgun\\shotgun");
			auto shotgun_tag = tags::get_tag < blam_tag::tag_group_type::weapon, s_weapon_group_definition>(shotgun_datum);
			auto smg_datum = tags::find_tag(blam_tag::tag_group_type::weapon, "objects\\weapons\\rifle\\smg\\smg");
			auto smg_tag = tags::get_tag < blam_tag::tag_group_type::weapon, s_weapon_group_definition>(smg_datum);
			auto sniper_datum = tags::find_tag(blam_tag::tag_group_type::weapon, "objects\\weapons\\rifle\\sniper_rifle\\sniper_rifle");
			auto sniper_tag = tags::get_tag < blam_tag::tag_group_type::weapon, s_weapon_group_definition>(sniper_datum);

			ApplyOffset(battle_rifle_tag, BattleRifle_WeaponOffsetX, BattleRifle_WeaponOffsetY, BattleRifle_WeaponOffsetZ);
			ApplyOffset(beam_rifle_tag, BeamRifle_WeaponOffsetX, BeamRifle_WeaponOffsetY, BeamRifle_WeaponOffsetZ);
			ApplyOffset(brute_plasma_rifle_tag, BrutePlasmaRifle_WeaponOffsetX, BrutePlasmaRifle_WeaponOffsetY, BrutePlasmaRifle_WeaponOffsetZ);
			ApplyOffset(brute_shot_tag, BruteShot_WeaponOffsetX, BruteShot_WeaponOffsetY, BruteShot_WeaponOffsetZ);
			ApplyOffset(carbine_tag, Carbine_WeaponOffsetX, Carbine_WeaponOffsetY, Carbine_WeaponOffsetZ);
			ApplyOffset(energy_sword_tag, EnergySword_WeaponOffsetX, EnergySword_WeaponOffsetY, EnergySword_WeaponOffsetZ);
			ApplyOffset(fuel_rod_tag, FuelRod_WeaponOffsetX, FuelRod_WeaponOffsetY, FuelRod_WeaponOffsetZ);
			ApplyOffset(magnum_tag, Magnum_WeaponOffsetX, Magnum_WeaponOffsetY, Magnum_WeaponOffsetZ);
			ApplyOffset(needler_tag, Needler_WeaponOffsetX, Needler_WeaponOffsetY, Needler_WeaponOffsetZ);
			ApplyOffset(plasma_pistol_tag, PlasmaPistol_WeaponOffsetX, PlasmaPistol_WeaponOffsetY, PlasmaPistol_WeaponOffsetZ);
			ApplyOffset(plasma_rifle_tag, PlasmaRifle_WeaponOffsetX, PlasmaRifle_WeaponOffsetY, PlasmaRifle_WeaponOffsetZ);
			ApplyOffset(rocket_launcher_tag, RocketLauncher_WeaponOffsetX, RocketLauncher_WeaponOffsetY, RocketLauncher_WeaponOffsetZ);
			ApplyOffset(sentinel_beam_tag, SentinelBeam_WeaponOffsetX, SentinelBeam_WeaponOffsetY, SentinelBeam_WeaponOffsetZ);
			ApplyOffset(shotgun_tag, Shotgun_WeaponOffsetX, Shotgun_WeaponOffsetY, Shotgun_WeaponOffsetZ);
			ApplyOffset(smg_tag, SMG_WeaponOffsetX, SMG_WeaponOffsetY, SMG_WeaponOffsetZ);
			ApplyOffset(sniper_tag, Sniper_WeaponOffsetX, Sniper_WeaponOffsetY, Sniper_WeaponOffsetZ);
		}
	}
}