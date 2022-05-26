#include "stdafx.h"
#include "H2MOD.h"
#include "H2MOD\Modules\Input\PlayerControl.h"
#include "H2MOD\GUI\imgui_integration\imgui_handler.h"
#include "Util\Hooks\Hook.h"
#include "H2MOD\Tags\TagInterface.h"
#include "H2MOD\Tags\MetaLoader\tag_loader.h"
#include "H2MOD\Modules\WeaponOffsets\WeaponOffsetConfig\WeaponOffsetConfig.h"

namespace imgui_handler {
	namespace WeaponOffsets {
		const real_vector3d DefaultOffsets[16] = {
			{0.02, 0, -0.004},
			{0.02, 0, 0},
			{0.02, 0, 0},
			{0.02, -0.01, 0.001},
			{0.02, 0, -0.004},
			{0.05, 0, 0},
			{0.02, 0, -0.005},
			{0.03, 0, -0.005},
			{0.03, 0, -0.005},
			{0.03, 0, -0.005},
			{0.02, 0, 0},
			{0.02, 0, -0.005},
			{0.02, 0.03, 0},
			{0.02, -0.001, -0.015},
			{0.02, 0, 0},
			{0.01, 0, 0}

		};
		real_vector3d ModifiedOffsets[16];
		s_weapon_group_definition* tags[16];
		const char WeaponPaths[16][128] = 
		{
			"objects\\weapons\\rifle\\battle_rifle\\battle_rifle",
			"objects\\weapons\\rifle\\beam_rifle\\beam_rifle",
			"objects\\weapons\\rifle\\brute_plasma_rifle\\brute_plasma_rifle",
			"objects\\weapons\\support_low\\brute_shot\\brute_shot",
			"objects\\weapons\\rifle\\covenant_carbine\\covenant_carbine",
			"objects\\weapons\\melee\\energy_blade\\energy_blade",
			"objects\\weapons\\support_high\\flak_cannon\\flak_cannon",
			"objects\\weapons\\pistol\\magnum\\magnum",
			"objects\\weapons\\pistol\\needler\\needler",
			"objects\\weapons\\pistol\\plasma_pistol\\plasma_pistol",
			"objects\\weapons\\rifle\\plasma_rifle\\plasma_rifle",
			"objects\\weapons\\support_high\\rocket_launcher\\rocket_launcher",
			"objects\\characters\\sentinel_aggressor\\weapons\\beam\\sentinel_aggressor_beam",
			"objects\\weapons\\rifle\\shotgun\\shotgun",
			"objects\\weapons\\rifle\\smg\\smg",
			"objects\\weapons\\rifle\\sniper_rifle\\sniper_rifle"
		};


		namespace
		{
			std::map<int, std::map<e_weapon_offsets_string, char*>> string_table;
			// Used for controls that use the same string, A identifier has to be appended to them
			// I.E Reset##1... Reset##20
			std::map<std::string, std::string> string_cache;
			void OffsetMenu(s_weapon_group_definition* tag, char* slider, e_weapon_offsets_string text, float& offset, const float default_value)
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
				if (ImGui::IsItemEdited() && tag != nullptr) { ApplyOffsetImgui(tag, slider, offset); }

				ImGui::PushItemWidth(WidthPercentage(20));
				ImGui::InputFloat(item2, &offset, -0.15, 0.15, "%.3f"); ImGui::SameLine();
				if (ImGui::IsItemEdited() && tag != nullptr) { ApplyOffsetImgui(tag, slider, offset); }

				ImGui::PushItemWidth(WidthPercentage(20));
				if (ImGui::Button(GetString(reset, item3), b2_size))
				{
					offset = default_value;
					if (tag != nullptr) { ApplyOffsetImgui(tag, slider, offset); }
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
				switch (selectedOption)
				{
				case BattleRifle:
					OffsetMenu(tags[BattleRifle], "##BattleRifle_OffsetX", weapon_offset_x, ModifiedOffsets[BattleRifle].i, DefaultOffsets[BattleRifle].i);
					OffsetMenu(tags[BattleRifle], "##BattleRifle_OffsetY", weapon_offset_y, ModifiedOffsets[BattleRifle].j, DefaultOffsets[BattleRifle].j);
					OffsetMenu(tags[BattleRifle], "##BattleRifle_OffsetZ", weapon_offset_z, ModifiedOffsets[BattleRifle].k, DefaultOffsets[BattleRifle].k);
					break;
				case BeamRifle:
					OffsetMenu(tags[BeamRifle], "##BeamRifle_OffsetX", weapon_offset_x, ModifiedOffsets[BeamRifle].i, DefaultOffsets[BeamRifle].i);
					OffsetMenu(tags[BeamRifle], "##BeamRifle_OffsetY", weapon_offset_y, ModifiedOffsets[BeamRifle].j, DefaultOffsets[BeamRifle].j);
					OffsetMenu(tags[BeamRifle], "##BeamRifle_OffsetZ", weapon_offset_z, ModifiedOffsets[BeamRifle].k, DefaultOffsets[BeamRifle].k);
					break;
				case BrutePlasmaRifle:
					OffsetMenu(tags[BrutePlasmaRifle], "##BrutePlasmaRifle_OffsetX", weapon_offset_x, ModifiedOffsets[BrutePlasmaRifle].i, DefaultOffsets[BrutePlasmaRifle].i);
					OffsetMenu(tags[BrutePlasmaRifle], "##BrutePlasmaRifle_OffsetY", weapon_offset_y, ModifiedOffsets[BrutePlasmaRifle].j, DefaultOffsets[BrutePlasmaRifle].j);
					OffsetMenu(tags[BrutePlasmaRifle], "##BrutePlasmaRifle_OffsetZ", weapon_offset_z, ModifiedOffsets[BrutePlasmaRifle].k, DefaultOffsets[BrutePlasmaRifle].k);
					break;
				case BruteShot:
					OffsetMenu(tags[BruteShot], "##BruteShot_OffsetX", weapon_offset_x, ModifiedOffsets[BruteShot].i, DefaultOffsets[BruteShot].i);
					OffsetMenu(tags[BruteShot], "##BruteShot_OffsetY", weapon_offset_y, ModifiedOffsets[BruteShot].j, DefaultOffsets[BruteShot].j);
					OffsetMenu(tags[BruteShot], "##BruteShot_OffsetZ", weapon_offset_z, ModifiedOffsets[BruteShot].k, DefaultOffsets[BruteShot].k);
					break;
				case Carbine:
					OffsetMenu(tags[Carbine], "##Carbine_OffsetX", weapon_offset_x, ModifiedOffsets[Carbine].i, DefaultOffsets[Carbine].i);
					OffsetMenu(tags[Carbine], "##Carbine_OffsetY", weapon_offset_y, ModifiedOffsets[Carbine].j, DefaultOffsets[Carbine].j);
					OffsetMenu(tags[Carbine], "##Carbine_OffsetZ", weapon_offset_z, ModifiedOffsets[Carbine].k, DefaultOffsets[Carbine].k);
					break;
				case EnergySword:
					OffsetMenu(tags[EnergySword], "##EnergySword_OffsetX", weapon_offset_x, ModifiedOffsets[EnergySword].i, DefaultOffsets[EnergySword].i);
					OffsetMenu(tags[EnergySword], "##EnergySword_OffsetY", weapon_offset_y, ModifiedOffsets[EnergySword].j, DefaultOffsets[EnergySword].j);
					OffsetMenu(tags[EnergySword], "##EnergySword_OffsetZ", weapon_offset_z, ModifiedOffsets[EnergySword].k, DefaultOffsets[EnergySword].k);
					break;
				case FuelRod:
					OffsetMenu(tags[FuelRod], "##FuelRod_OffsetX", weapon_offset_x, ModifiedOffsets[FuelRod].i, DefaultOffsets[FuelRod].i);
					OffsetMenu(tags[FuelRod], "##FuelRod_OffsetY", weapon_offset_y, ModifiedOffsets[FuelRod].j, DefaultOffsets[FuelRod].j);
					OffsetMenu(tags[FuelRod], "##FuelRod_OffsetZ", weapon_offset_z, ModifiedOffsets[FuelRod].k, DefaultOffsets[FuelRod].k);
					break;
				case Magnum:
					OffsetMenu(tags[Magnum], "##Magnum_OffsetX", weapon_offset_x, ModifiedOffsets[Magnum].i, DefaultOffsets[Magnum].i);
					OffsetMenu(tags[Magnum], "##Magnum_OffsetY", weapon_offset_y, ModifiedOffsets[Magnum].j, DefaultOffsets[Magnum].j);
					OffsetMenu(tags[Magnum], "##Magnum_OffsetZ", weapon_offset_z, ModifiedOffsets[Magnum].k, DefaultOffsets[Magnum].k);
					break;
				case Needler:
					OffsetMenu(tags[Needler], "##Needler_OffsetX", weapon_offset_x, ModifiedOffsets[Needler].i, DefaultOffsets[Needler].i);
					OffsetMenu(tags[Needler], "##Needler_OffsetY", weapon_offset_y, ModifiedOffsets[Needler].j, DefaultOffsets[Needler].j);
					OffsetMenu(tags[Needler], "##Needler_OffsetZ", weapon_offset_z, ModifiedOffsets[Needler].k, DefaultOffsets[Needler].k);
					break;
				case PlasmaPistol:
					OffsetMenu(tags[PlasmaPistol], "##PlasmaPistol_OffsetX", weapon_offset_x, ModifiedOffsets[PlasmaPistol].i, DefaultOffsets[PlasmaPistol].i);
					OffsetMenu(tags[PlasmaPistol], "##PlasmaPistol_OffsetY", weapon_offset_y, ModifiedOffsets[PlasmaPistol].j, DefaultOffsets[PlasmaPistol].j);
					OffsetMenu(tags[PlasmaPistol], "##PlasmaPistol_OffsetZ", weapon_offset_z, ModifiedOffsets[PlasmaPistol].k, DefaultOffsets[PlasmaPistol].k);
					break;
				case PlasmaRifle:
					OffsetMenu(tags[PlasmaRifle], "##PlasmaRifle_OffsetX", weapon_offset_x, ModifiedOffsets[PlasmaRifle].i, DefaultOffsets[PlasmaRifle].i);
					OffsetMenu(tags[PlasmaRifle], "##PlasmaRifle_OffsetY", weapon_offset_y, ModifiedOffsets[PlasmaRifle].j, DefaultOffsets[PlasmaRifle].j);
					OffsetMenu(tags[PlasmaRifle], "##PlasmaRifle_OffsetZ", weapon_offset_z, ModifiedOffsets[PlasmaRifle].k, DefaultOffsets[PlasmaRifle].k);
					break;
				case RocketLauncher:
					OffsetMenu(tags[RocketLauncher], "##RocketLauncher_OffsetX", weapon_offset_x, ModifiedOffsets[RocketLauncher].i, DefaultOffsets[RocketLauncher].i);
					OffsetMenu(tags[RocketLauncher], "##RocketLauncher_OffsetY", weapon_offset_y, ModifiedOffsets[RocketLauncher].j, DefaultOffsets[RocketLauncher].j);
					OffsetMenu(tags[RocketLauncher], "##RocketLauncher_OffsetZ", weapon_offset_z, ModifiedOffsets[RocketLauncher].k, DefaultOffsets[RocketLauncher].k);
					break;
				case SentinelBeam:
					OffsetMenu(tags[SentinelBeam], "##SentinelBeam_OffsetX", weapon_offset_x, ModifiedOffsets[SentinelBeam].i, DefaultOffsets[SentinelBeam].i);
					OffsetMenu(tags[SentinelBeam], "##SentinelBeam_OffsetY", weapon_offset_y, ModifiedOffsets[SentinelBeam].j, DefaultOffsets[SentinelBeam].j);
					OffsetMenu(tags[SentinelBeam], "##SentinelBeam_OffsetZ", weapon_offset_z, ModifiedOffsets[SentinelBeam].k, DefaultOffsets[SentinelBeam].k);
					break;
				case Shotgun:
					OffsetMenu(tags[Shotgun], "##Shotgun_OffsetX", weapon_offset_x, ModifiedOffsets[Shotgun].i, DefaultOffsets[Shotgun].i);
					OffsetMenu(tags[Shotgun], "##Shotgun_OffsetY", weapon_offset_y, ModifiedOffsets[Shotgun].j, DefaultOffsets[Shotgun].j);
					OffsetMenu(tags[Shotgun], "##Shotgun_OffsetZ", weapon_offset_z, ModifiedOffsets[Shotgun].k, DefaultOffsets[Shotgun].k);
					break;
				case SMG:
					OffsetMenu(tags[SMG], "##SMG_OffsetX", weapon_offset_x, ModifiedOffsets[SMG].i, DefaultOffsets[SMG].i);
					OffsetMenu(tags[SMG], "##SMG_OffsetY", weapon_offset_y, ModifiedOffsets[SMG].j, DefaultOffsets[SMG].j);
					OffsetMenu(tags[SMG], "##SMG_OffsetZ", weapon_offset_z, ModifiedOffsets[SMG].k, DefaultOffsets[SMG].k);
					break;
				case Sniper:
					OffsetMenu(tags[Sniper], "##Sniper_OffsetX", weapon_offset_x, ModifiedOffsets[Sniper].i, DefaultOffsets[Sniper].i);
					OffsetMenu(tags[Sniper], "##Sniper_OffsetY", weapon_offset_y, ModifiedOffsets[Sniper].j, DefaultOffsets[Sniper].j);
					OffsetMenu(tags[Sniper], "##Sniper_OffsetZ", weapon_offset_z, ModifiedOffsets[Sniper].k, DefaultOffsets[Sniper].k);
					break;
				}
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
			ReadWeaponOffsetConfig(ModifiedOffsets);
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
			SaveWeaponOffsetConfig(ModifiedOffsets);
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
		void ApplyOffset(s_weapon_group_definition* tag, real_vector3d offset)
		{
			if (tag != nullptr && h2mod->GetEngineType() != _main_menu)
			{
				tag->first_person_weapon_offset = offset;
			}
		}
		void ApplyOffsetImgui(s_weapon_group_definition* tag, char* slider, float &offset)
		{
			switch (slider[strlen(slider) - 1])
			{
			case 'X':
				tag->first_person_weapon_offset.i = offset;
				break;
			case 'Y':
				tag->first_person_weapon_offset.j = offset;
				break;
			case 'Z':
				tag->first_person_weapon_offset.k = offset;
				break;
			}
		}
		void Initialize()
		{
			WriteDefaultFile(DefaultOffsets);

			ReadWeaponOffsetConfig(ModifiedOffsets);

			for (byte i = BattleRifle; i != Sniper + 1; i++)
			{
				ApplyOffset(tags[i], ModifiedOffsets[i]);
			}
		}
	}
}