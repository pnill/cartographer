#include "H2MOD\Modules\Input\PlayerControl.h"
#include "imgui_handler.h"
#include "H2MOD\Tags\TagInterface.h"
#include "Blam\Cache\TagGroups\weapon_definition.hpp"
#include "H2MOD\Modules\Config\WeaponOffsetConfig\WeaponOffsetConfig.h"

namespace imgui_handler {
	namespace WeaponOffsets {
		namespace
		{
			std::map<int, std::map<e_weapon_offsets_string, char*>> string_table;
			//Used for controls that use the same string, A identifier has to be appended to them
			//I.E Reset##1... Reset##20
			std::map<std::string, std::string> string_cache;
			void HudSettings()
			{
				auto magnum_datum = tags::find_tag(blam_tag::tag_group_type::weapon, "objects\\weapons\\pistol\\magnum\\magnum");
				auto magnum_tag = tags::get_tag < blam_tag::tag_group_type::weapon, s_weapon_group_definition>(magnum_datum);
				auto smg_datum = tags::find_tag(blam_tag::tag_group_type::weapon, "objects\\weapons\\rifle\\smg\\smg");
				auto smg_tag = tags::get_tag < blam_tag::tag_group_type::weapon, s_weapon_group_definition>(smg_datum);
				auto sniper_datum = tags::find_tag(blam_tag::tag_group_type::weapon, "objects\\weapons\\rifle\\sniper_rifle\\sniper_rifle");
				auto sniper_tag = tags::get_tag < blam_tag::tag_group_type::weapon, s_weapon_group_definition>(sniper_datum);

				ImVec2 item_size = ImGui::GetItemRectSize();
				if (ImGui::CollapsingHeader(GetString(magnum_title)))
				{
					ImVec2 b2_size = ImVec2(WidthPercentage(20), item_size.y);

					//Weapon Offset X
					ImGui::Text(GetString(weapon_offset_x));
					ImGui::PushItemWidth(WidthPercentage(60));
					ImGui::SliderFloat("##MagnumOffsetX1", &Magnum_WeaponOffsetX, -0.15, 0.15, ""); ImGui::SameLine();
					if (ImGui::IsItemEdited())
						magnum_tag->first_person_weapon_offset_i = Magnum_WeaponOffsetX;

					ImGui::PushItemWidth(WidthPercentage(20));
					ImGui::InputFloat("##MagnumOffsetX2", &Magnum_WeaponOffsetX, -0.15, 0.15, "%.3f"); ImGui::SameLine();
					if (ImGui::IsItemEdited()) {
						if (Magnum_WeaponOffsetX > 0.15)
							Magnum_WeaponOffsetX = 0.15;
						if (Magnum_WeaponOffsetX < -0.15)
							Magnum_WeaponOffsetX = -0.15;

						magnum_tag->first_person_weapon_offset_i = Magnum_WeaponOffsetX;
					}
					ImGui::PushItemWidth(WidthPercentage(20));
					if (ImGui::Button(GetString(reset, "MagnumOffsetX3"), b2_size))
					{
						Magnum_WeaponOffsetX = 0.03;
						magnum_tag->first_person_weapon_offset_i = Magnum_WeaponOffsetX;
					}
					ImGui::PopItemWidth();


					//Weapon Offset Y
					ImGui::Text(GetString(weapon_offset_y));
					ImGui::PushItemWidth(WidthPercentage(60));
					ImGui::SliderFloat("##MagnumOffsetY1", &Magnum_WeaponOffsetY, -0.15, 0.15, ""); ImGui::SameLine();
					if (ImGui::IsItemEdited())
						magnum_tag->first_person_weapon_offset_j = Magnum_WeaponOffsetY;

					ImGui::PushItemWidth(WidthPercentage(20));
					ImGui::InputFloat("##MagnumOffsetY2", &Magnum_WeaponOffsetY, -0.15, 0.15, "%.3f"); ImGui::SameLine();
					if (ImGui::IsItemEdited()) {
						if (Magnum_WeaponOffsetY > 0.15)
							Magnum_WeaponOffsetY = 0.15;
						if (Magnum_WeaponOffsetY < -0.15)
							Magnum_WeaponOffsetY = -0.15;

						magnum_tag->first_person_weapon_offset_j = Magnum_WeaponOffsetY;
					}
					ImGui::PushItemWidth(WidthPercentage(20));
					if (ImGui::Button(GetString(reset, "MagnumOffsetY3"), b2_size))
					{
						Magnum_WeaponOffsetY = 0;
						magnum_tag->first_person_weapon_offset_j = Magnum_WeaponOffsetY;
					}
					ImGui::PopItemWidth();

					//Weapon Offset Z
					ImGui::Text(GetString(weapon_offset_z));
					ImGui::PushItemWidth(WidthPercentage(60));
					ImGui::SliderFloat("##MagnumOffsetZ1", &Magnum_WeaponOffsetZ, -0.15, 0.15, ""); ImGui::SameLine();
					if (ImGui::IsItemEdited())
						magnum_tag->first_person_weapon_offset_k = Magnum_WeaponOffsetZ;

					ImGui::PushItemWidth(WidthPercentage(20));
					ImGui::InputFloat("##MagnumOffsetZ2", &Magnum_WeaponOffsetZ, -0.15, 0.15, "%.3f"); ImGui::SameLine();
					if (ImGui::IsItemEdited()) {
						if (Magnum_WeaponOffsetZ > 0.15)
							Magnum_WeaponOffsetZ = 0.15;
						if (Magnum_WeaponOffsetZ < -0.15)
							Magnum_WeaponOffsetZ = -0.15;

						magnum_tag->first_person_weapon_offset_k = Magnum_WeaponOffsetZ;
					}
					ImGui::PushItemWidth(WidthPercentage(20));
					if (ImGui::Button(GetString(reset, "MagnumOffsetZ3"), b2_size))
					{
						Magnum_WeaponOffsetZ = -0.005;
						magnum_tag->first_person_weapon_offset_k = Magnum_WeaponOffsetZ;
					}
					ImGui::PopItemWidth();

					ImVec2 b3_size = ImVec2(WidthPercentage(33.3333333333f), item_size.y);
				}
				if (ImGui::CollapsingHeader(GetString(smg_title)))
				{
					ImVec2 b2_size = ImVec2(WidthPercentage(20), item_size.y);

					//Weapon Offset X
					ImGui::Text(GetString(weapon_offset_x));
					ImGui::PushItemWidth(WidthPercentage(60));
					ImGui::SliderFloat("##SMGOffsetX1", &SMG_WeaponOffsetX, -0.15, 0.15, ""); ImGui::SameLine();
					if (ImGui::IsItemEdited())
						smg_tag->first_person_weapon_offset_i = SMG_WeaponOffsetX;

					ImGui::PushItemWidth(WidthPercentage(20));
					ImGui::InputFloat("##SMGOffsetX2", &SMG_WeaponOffsetX, -0.15, 0.15, "%.3f"); ImGui::SameLine();
					if (ImGui::IsItemEdited()) {
						if (SMG_WeaponOffsetX > 0.15)
							SMG_WeaponOffsetX = 0.15;
						if (SMG_WeaponOffsetX < -0.15)
							SMG_WeaponOffsetX = -0.15;

						smg_tag->first_person_weapon_offset_i = SMG_WeaponOffsetX;
					}
					ImGui::PushItemWidth(WidthPercentage(20));
					if (ImGui::Button(GetString(reset, "SMGOffsetX3"), b2_size))
					{
						SMG_WeaponOffsetX = 0.02;
						smg_tag->first_person_weapon_offset_i = SMG_WeaponOffsetX;
					}
					ImGui::PopItemWidth();


					//Weapon Offset Y
					ImGui::Text(GetString(weapon_offset_y));
					ImGui::PushItemWidth(WidthPercentage(60));
					ImGui::SliderFloat("##SMGOffsetY1", &SMG_WeaponOffsetY, -0.15, 0.15, ""); ImGui::SameLine();
					if (ImGui::IsItemEdited())
						smg_tag->first_person_weapon_offset_j = SMG_WeaponOffsetY;

					ImGui::PushItemWidth(WidthPercentage(20));
					ImGui::InputFloat("##SMGOffsetY2", &SMG_WeaponOffsetY, -0.15, 0.15, "%.3f"); ImGui::SameLine();
					if (ImGui::IsItemEdited()) {
						if (SMG_WeaponOffsetY > 0.15)
							SMG_WeaponOffsetY = 0.15;
						if (SMG_WeaponOffsetY < -0.15)
							SMG_WeaponOffsetY = -0.15;

						smg_tag->first_person_weapon_offset_j = SMG_WeaponOffsetY;
					}
					ImGui::PushItemWidth(WidthPercentage(20));
					if (ImGui::Button(GetString(reset, "SMGOffsetY3"), b2_size))
					{
						SMG_WeaponOffsetY = 0;
						smg_tag->first_person_weapon_offset_j = SMG_WeaponOffsetY;
					}
					ImGui::PopItemWidth();

					//Weapon Offset Z
					ImGui::Text(GetString(weapon_offset_z));
					ImGui::PushItemWidth(WidthPercentage(60));
					ImGui::SliderFloat("##SMGOffsetZ1", &SMG_WeaponOffsetZ, -0.15, 0.15, ""); ImGui::SameLine();
					if (ImGui::IsItemEdited())
						smg_tag->first_person_weapon_offset_k = SMG_WeaponOffsetZ;

					ImGui::PushItemWidth(WidthPercentage(20));
					ImGui::InputFloat("##SMGOffsetZ2", &SMG_WeaponOffsetZ, -0.15, 0.15, "%.3f"); ImGui::SameLine();
					if (ImGui::IsItemEdited()) {
						if (SMG_WeaponOffsetZ > 0.15)
							SMG_WeaponOffsetZ = 0.15;
						if (SMG_WeaponOffsetZ < -0.15)
							SMG_WeaponOffsetZ = -0.15;

						smg_tag->first_person_weapon_offset_k = SMG_WeaponOffsetZ;
					}
					ImGui::PushItemWidth(WidthPercentage(20));
					if (ImGui::Button(GetString(reset, "SMGOffsetZ3"), b2_size))
					{
						SMG_WeaponOffsetZ = 0;
						smg_tag->first_person_weapon_offset_k = SMG_WeaponOffsetZ;
					}
					ImGui::PopItemWidth();

					ImVec2 b3_size = ImVec2(WidthPercentage(33.3333333333f), item_size.y);
				}
				if (ImGui::CollapsingHeader(GetString(sniper_title)))
				{
					ImVec2 b2_size = ImVec2(WidthPercentage(20), item_size.y);

					//Weapon Offset X
					ImGui::Text(GetString(weapon_offset_x));
					ImGui::PushItemWidth(WidthPercentage(60));
					ImGui::SliderFloat("##SniperOffsetX1", &Sniper_WeaponOffsetX, -0.15, 0.15, ""); ImGui::SameLine();
					if (ImGui::IsItemEdited())
						sniper_tag->first_person_weapon_offset_i = Sniper_WeaponOffsetX;

					ImGui::PushItemWidth(WidthPercentage(20));
					ImGui::InputFloat("##SniperOffsetX2", &Sniper_WeaponOffsetX, -0.15, 0.15, "%.3f"); ImGui::SameLine();
					if (ImGui::IsItemEdited()) {
						if (Sniper_WeaponOffsetX > 0.15)
							Sniper_WeaponOffsetX = 0.15;
						if (Sniper_WeaponOffsetX < -0.15)
							Sniper_WeaponOffsetX = -0.15;

						sniper_tag->first_person_weapon_offset_i = Sniper_WeaponOffsetX;
					}
					ImGui::PushItemWidth(WidthPercentage(20));
					if (ImGui::Button(GetString(reset, "SniperOffsetX3"), b2_size))
					{
						Sniper_WeaponOffsetX = 0.01;
						sniper_tag->first_person_weapon_offset_i = Sniper_WeaponOffsetX;
					}
					ImGui::PopItemWidth();


					//Weapon Offset Y
					ImGui::Text(GetString(weapon_offset_y));
					ImGui::PushItemWidth(WidthPercentage(60));
					ImGui::SliderFloat("##SniperOffsetY1", &Sniper_WeaponOffsetY, -0.15, 0.15, ""); ImGui::SameLine();
					if (ImGui::IsItemEdited())
						sniper_tag->first_person_weapon_offset_j = Sniper_WeaponOffsetY;

					ImGui::PushItemWidth(WidthPercentage(20));
					ImGui::InputFloat("##SniperOffsetY2", &Sniper_WeaponOffsetY, -0.15, 0.15, "%.3f"); ImGui::SameLine();
					if (ImGui::IsItemEdited()) {
						if (Sniper_WeaponOffsetY > 0.15)
							Sniper_WeaponOffsetY = 0.15;
						if (Sniper_WeaponOffsetY < -0.15)
							Sniper_WeaponOffsetY = -0.15;

						sniper_tag->first_person_weapon_offset_j = Sniper_WeaponOffsetY;
					}
					ImGui::PushItemWidth(WidthPercentage(20));
					if (ImGui::Button(GetString(reset, "SniperOffsetY3"), b2_size))
					{
						Sniper_WeaponOffsetY = 0;
						sniper_tag->first_person_weapon_offset_j = Sniper_WeaponOffsetY;
					}
					ImGui::PopItemWidth();

					//Weapon Offset Z
					ImGui::Text(GetString(weapon_offset_z));
					ImGui::PushItemWidth(WidthPercentage(60));
					ImGui::SliderFloat("##SniperOffsetZ1", &Sniper_WeaponOffsetZ, -0.15, 0.15, ""); ImGui::SameLine();
					if (ImGui::IsItemEdited())
						sniper_tag->first_person_weapon_offset_k = Sniper_WeaponOffsetZ;

					ImGui::PushItemWidth(WidthPercentage(20));
					ImGui::InputFloat("##SniperOffsetZ2", &Sniper_WeaponOffsetZ, -0.15, 0.15, "%.3f"); ImGui::SameLine();
					if (ImGui::IsItemEdited()) {
						if (Sniper_WeaponOffsetZ > 0.15)
							Sniper_WeaponOffsetZ = 0.15;
						if (Sniper_WeaponOffsetZ < -0.15)
							Sniper_WeaponOffsetZ = -0.15;

						sniper_tag->first_person_weapon_offset_k = Sniper_WeaponOffsetZ;
					}
					ImGui::PushItemWidth(WidthPercentage(20));
					if (ImGui::Button(GetString(reset, "SniperOffsetZ3"), b2_size))
					{
						Sniper_WeaponOffsetZ = 0;
						sniper_tag->first_person_weapon_offset_k = Sniper_WeaponOffsetZ;
					}
					ImGui::PopItemWidth();

					ImVec2 b3_size = ImVec2(WidthPercentage(33.3333333333f), item_size.y);
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
			//WriteValue<byte>(Memory::GetAddress(0x9712cC), 1);
			ImGuiToggleInput(true);
			PlayerControl::DisableLocalCamera(true);
			ReadWeaponOffsetConfig();
		}
		void Close()
		{
			//WriteValue<byte>(Memory::GetAddress(0x9712cC), 0);
			ImGuiToggleInput(false);
			PlayerControl::DisableLocalCamera(false);
			SaveWeaponOffsetConfig();
		}
		void BuildStringsTable()
		{
			string_table[0][e_weapon_offsets_string::title] = "        Weapon Offsets";
			string_table[0][e_weapon_offsets_string::magnum_title] = "Magnum Offset";
			string_table[0][e_weapon_offsets_string::smg_title] = "SMG Offset";
			string_table[0][e_weapon_offsets_string::sniper_title] = "Sniper Offset";
			string_table[0][e_weapon_offsets_string::weapon_offset_x] = "Weapon Offset X";
			string_table[0][e_weapon_offsets_string::weapon_offset_y] = "Weapon Offset Y";
			string_table[0][e_weapon_offsets_string::weapon_offset_z] = "Weapon Offset Z";
			string_table[0][e_weapon_offsets_string::reset] = "Reset";

			//Spanish.
			string_table[4][e_weapon_offsets_string::title] = "      Compensaciones de armas";
			string_table[0][e_weapon_offsets_string::magnum_title] = "Magnum Offset";
			string_table[0][e_weapon_offsets_string::smg_title] = "SMG Offset";
			string_table[0][e_weapon_offsets_string::sniper_title] = "Sniper Offset";
			string_table[0][e_weapon_offsets_string::weapon_offset_x] = "Weapon Offset X";
			string_table[0][e_weapon_offsets_string::weapon_offset_y] = "Weapon Offset Y";
			string_table[0][e_weapon_offsets_string::weapon_offset_z] = "Weapon Offset Z";
			string_table[0][e_weapon_offsets_string::reset] = "Reset";
		}
	}
}
