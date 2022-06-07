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
		
		struct s_weapon_custom_offset {
			const real_vector3d DefaultOffsets[16] = 
			{
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
			real_vector3d ModifiedOffsets[16];
			s_weapon_group_definition* tags[16];
		};
		s_weapon_custom_offset customOffsets;

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
					OffsetMenu(customOffsets.tags[BattleRifle], "##BattleRifle_OffsetX", weapon_offset_x,
						customOffsets.ModifiedOffsets[BattleRifle].i, customOffsets.DefaultOffsets[BattleRifle].i);
					OffsetMenu(customOffsets.tags[BattleRifle], "##BattleRifle_OffsetY", weapon_offset_y,
						customOffsets.ModifiedOffsets[BattleRifle].j, customOffsets.DefaultOffsets[BattleRifle].j);
					OffsetMenu(customOffsets.tags[BattleRifle], "##BattleRifle_OffsetZ", weapon_offset_z,
						customOffsets.ModifiedOffsets[BattleRifle].k, customOffsets.DefaultOffsets[BattleRifle].k);
					break;
				case BeamRifle:
					OffsetMenu
					(customOffsets.tags[BeamRifle], "##BeamRifle_OffsetX", weapon_offset_x,
						customOffsets.ModifiedOffsets[BeamRifle].i, customOffsets.DefaultOffsets[BeamRifle].i);
					OffsetMenu(customOffsets.tags[BeamRifle], "##BeamRifle_OffsetY", weapon_offset_y,
						customOffsets.ModifiedOffsets[BeamRifle].j, customOffsets.DefaultOffsets[BeamRifle].j);
					OffsetMenu(customOffsets.tags[BeamRifle], "##BeamRifle_OffsetZ", weapon_offset_z,
						customOffsets.ModifiedOffsets[BeamRifle].k, customOffsets.DefaultOffsets[BeamRifle].k);
					break;
				case BrutePlasmaRifle:
					OffsetMenu(customOffsets.tags[BrutePlasmaRifle], "##BrutePlasmaRifle_OffsetX", weapon_offset_x,
						customOffsets.ModifiedOffsets[BrutePlasmaRifle].i, customOffsets.DefaultOffsets[BrutePlasmaRifle].i);
					OffsetMenu(customOffsets.tags[BrutePlasmaRifle], "##BrutePlasmaRifle_OffsetY", weapon_offset_y,
						customOffsets.ModifiedOffsets[BrutePlasmaRifle].j, customOffsets.DefaultOffsets[BrutePlasmaRifle].j);
					OffsetMenu(customOffsets.tags[BrutePlasmaRifle], "##BrutePlasmaRifle_OffsetZ", weapon_offset_z,
						customOffsets.ModifiedOffsets[BrutePlasmaRifle].k, customOffsets.DefaultOffsets[BrutePlasmaRifle].k);
					break;
				case BruteShot:
					OffsetMenu(customOffsets.tags[BruteShot], "##BruteShot_OffsetX", weapon_offset_x,
						customOffsets.ModifiedOffsets[BruteShot].i, customOffsets.DefaultOffsets[BruteShot].i);
					OffsetMenu(customOffsets.tags[BruteShot], "##BruteShot_OffsetY", weapon_offset_y,
						customOffsets.ModifiedOffsets[BruteShot].j, customOffsets.DefaultOffsets[BruteShot].j);
					OffsetMenu(customOffsets.tags[BruteShot], "##BruteShot_OffsetZ", weapon_offset_z,
						customOffsets.ModifiedOffsets[BruteShot].k, customOffsets.DefaultOffsets[BruteShot].k);
					break;
				case Carbine:
					OffsetMenu(customOffsets.tags[Carbine], "##Carbine_OffsetX", weapon_offset_x,
						customOffsets.ModifiedOffsets[Carbine].i, customOffsets.DefaultOffsets[Carbine].i);
					OffsetMenu(customOffsets.tags[Carbine], "##Carbine_OffsetY", weapon_offset_y,
						customOffsets.ModifiedOffsets[Carbine].j, customOffsets.DefaultOffsets[Carbine].j);
					OffsetMenu(customOffsets.tags[Carbine], "##Carbine_OffsetZ", weapon_offset_z,
						customOffsets.ModifiedOffsets[Carbine].k, customOffsets.DefaultOffsets[Carbine].k);
					break;
				case EnergySword:
					OffsetMenu(customOffsets.tags[EnergySword], "##EnergySword_OffsetX", weapon_offset_x,
						customOffsets.ModifiedOffsets[EnergySword].i, customOffsets.DefaultOffsets[EnergySword].i);
					OffsetMenu(customOffsets.tags[EnergySword], "##EnergySword_OffsetY", weapon_offset_y,
						customOffsets.ModifiedOffsets[EnergySword].j, customOffsets.DefaultOffsets[EnergySword].j);
					OffsetMenu(customOffsets.tags[EnergySword], "##EnergySword_OffsetZ", weapon_offset_z,
						customOffsets.ModifiedOffsets[EnergySword].k, customOffsets.DefaultOffsets[EnergySword].k);
					break;
				case FuelRod:
					OffsetMenu(customOffsets.tags[FuelRod], "##FuelRod_OffsetX", weapon_offset_x,
						customOffsets.ModifiedOffsets[FuelRod].i, customOffsets.DefaultOffsets[FuelRod].i);
					OffsetMenu(customOffsets.tags[FuelRod], "##FuelRod_OffsetY", weapon_offset_y,
						customOffsets.ModifiedOffsets[FuelRod].j, customOffsets.DefaultOffsets[FuelRod].j);
					OffsetMenu(customOffsets.tags[FuelRod], "##FuelRod_OffsetZ", weapon_offset_z,
						customOffsets.ModifiedOffsets[FuelRod].k, customOffsets.DefaultOffsets[FuelRod].k);
					break;
				case Magnum:
					OffsetMenu(customOffsets.tags[Magnum], "##Magnum_OffsetX", weapon_offset_x,
						customOffsets.ModifiedOffsets[Magnum].i, customOffsets.DefaultOffsets[Magnum].i);
					OffsetMenu(customOffsets.tags[Magnum], "##Magnum_OffsetY", weapon_offset_y,
						customOffsets.ModifiedOffsets[Magnum].j, customOffsets.DefaultOffsets[Magnum].j);
					OffsetMenu(customOffsets.tags[Magnum], "##Magnum_OffsetZ", weapon_offset_z,
						customOffsets.ModifiedOffsets[Magnum].k, customOffsets.DefaultOffsets[Magnum].k);
					break;
				case Needler:
					OffsetMenu(customOffsets.tags[Needler], "##Needler_OffsetX", weapon_offset_x,
						customOffsets.ModifiedOffsets[Needler].i, customOffsets.DefaultOffsets[Needler].i);
					OffsetMenu(customOffsets.tags[Needler], "##Needler_OffsetY", weapon_offset_y,
						customOffsets.ModifiedOffsets[Needler].j, customOffsets.DefaultOffsets[Needler].j);
					OffsetMenu(customOffsets.tags[Needler], "##Needler_OffsetZ", weapon_offset_z,
						customOffsets.ModifiedOffsets[Needler].k, customOffsets.DefaultOffsets[Needler].k);
					break;
				case PlasmaPistol:
					OffsetMenu(customOffsets.tags[PlasmaPistol], "##PlasmaPistol_OffsetX", weapon_offset_x,
						customOffsets.ModifiedOffsets[PlasmaPistol].i, customOffsets.DefaultOffsets[PlasmaPistol].i);
					OffsetMenu(customOffsets.tags[PlasmaPistol], "##PlasmaPistol_OffsetY", weapon_offset_y,
						customOffsets.ModifiedOffsets[PlasmaPistol].j, customOffsets.DefaultOffsets[PlasmaPistol].j);
					OffsetMenu(customOffsets.tags[PlasmaPistol], "##PlasmaPistol_OffsetZ", weapon_offset_z,
						customOffsets.ModifiedOffsets[PlasmaPistol].k, customOffsets.DefaultOffsets[PlasmaPistol].k);
					break;
				case PlasmaRifle:
					OffsetMenu(customOffsets.tags[PlasmaRifle], "##PlasmaRifle_OffsetX", weapon_offset_x,
						customOffsets.ModifiedOffsets[PlasmaRifle].i, customOffsets.DefaultOffsets[PlasmaRifle].i);
					OffsetMenu(customOffsets.tags[PlasmaRifle], "##PlasmaRifle_OffsetY", weapon_offset_y,
						customOffsets.ModifiedOffsets[PlasmaRifle].j, customOffsets.DefaultOffsets[PlasmaRifle].j);
					OffsetMenu(customOffsets.tags[PlasmaRifle], "##PlasmaRifle_OffsetZ", weapon_offset_z,
						customOffsets.ModifiedOffsets[PlasmaRifle].k, customOffsets.DefaultOffsets[PlasmaRifle].k);
					break;
				case RocketLauncher:
					OffsetMenu(customOffsets.tags[RocketLauncher], "##RocketLauncher_OffsetX", weapon_offset_x,
						customOffsets.ModifiedOffsets[RocketLauncher].i, customOffsets.DefaultOffsets[RocketLauncher].i);
					OffsetMenu(customOffsets.tags[RocketLauncher], "##RocketLauncher_OffsetY", weapon_offset_y,
						customOffsets.ModifiedOffsets[RocketLauncher].j, customOffsets.DefaultOffsets[RocketLauncher].j);
					OffsetMenu(customOffsets.tags[RocketLauncher], "##RocketLauncher_OffsetZ", weapon_offset_z,
						customOffsets.ModifiedOffsets[RocketLauncher].k, customOffsets.DefaultOffsets[RocketLauncher].k);
					break;
				case SentinelBeam:
					OffsetMenu(customOffsets.tags[SentinelBeam], "##SentinelBeam_OffsetX", weapon_offset_x,
						customOffsets.ModifiedOffsets[SentinelBeam].i, customOffsets.DefaultOffsets[SentinelBeam].i);
					OffsetMenu(customOffsets.tags[SentinelBeam], "##SentinelBeam_OffsetY", weapon_offset_y,
						customOffsets.ModifiedOffsets[SentinelBeam].j, customOffsets.DefaultOffsets[SentinelBeam].j);
					OffsetMenu(customOffsets.tags[SentinelBeam], "##SentinelBeam_OffsetZ", weapon_offset_z,
						customOffsets.ModifiedOffsets[SentinelBeam].k, customOffsets.DefaultOffsets[SentinelBeam].k);
					break;
				case Shotgun:
					OffsetMenu(customOffsets.tags[Shotgun], "##Shotgun_OffsetX", weapon_offset_x,
						customOffsets.ModifiedOffsets[Shotgun].i, customOffsets.DefaultOffsets[Shotgun].i);
					OffsetMenu(customOffsets.tags[Shotgun], "##Shotgun_OffsetY", weapon_offset_y,
						customOffsets.ModifiedOffsets[Shotgun].j, customOffsets.DefaultOffsets[Shotgun].j);
					OffsetMenu(customOffsets.tags[Shotgun], "##Shotgun_OffsetZ", weapon_offset_z,
						customOffsets.ModifiedOffsets[Shotgun].k, customOffsets.DefaultOffsets[Shotgun].k);
					break;
				case SMG:
					OffsetMenu(customOffsets.tags[SMG], "##SMG_OffsetX", weapon_offset_x,
						customOffsets.ModifiedOffsets[SMG].i, customOffsets.DefaultOffsets[SMG].i);
					OffsetMenu(customOffsets.tags[SMG], "##SMG_OffsetY", weapon_offset_y,
						customOffsets.ModifiedOffsets[SMG].j, customOffsets.DefaultOffsets[SMG].j);
					OffsetMenu(customOffsets.tags[SMG], "##SMG_OffsetZ", weapon_offset_z,
						customOffsets.ModifiedOffsets[SMG].k, customOffsets.DefaultOffsets[SMG].k);
					break;
				case Sniper:
					OffsetMenu(customOffsets.tags[Sniper], "##Sniper_OffsetX", weapon_offset_x,
						customOffsets.ModifiedOffsets[Sniper].i, customOffsets.DefaultOffsets[Sniper].i);
					OffsetMenu(customOffsets.tags[Sniper], "##Sniper_OffsetY", weapon_offset_y,
						customOffsets.ModifiedOffsets[Sniper].j, customOffsets.DefaultOffsets[Sniper].j);
					OffsetMenu(customOffsets.tags[Sniper], "##Sniper_OffsetZ", weapon_offset_z,
						customOffsets.ModifiedOffsets[Sniper].k, customOffsets.DefaultOffsets[Sniper].k);
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
			ReadWeaponOffsetConfig(customOffsets.ModifiedOffsets);
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
			SaveWeaponOffsetConfig(customOffsets.ModifiedOffsets);
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
			if (tag != nullptr)
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
			WriteDefaultFile(customOffsets.DefaultOffsets);

			ReadWeaponOffsetConfig(customOffsets.ModifiedOffsets);

			for (byte i = BattleRifle; i != Sniper + 1; i++)
			{
				customOffsets.tags[i] = tags::get_tag < blam_tag::tag_group_type::weapon, s_weapon_group_definition>
					(tags::find_tag(blam_tag::tag_group_type::weapon, customOffsets.WeaponPaths[i]));
				ApplyOffset(customOffsets.tags[i], customOffsets.ModifiedOffsets[i]);
			}
		}
	}
}