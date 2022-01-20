#include "H2MOD\Modules\Config\Config.h"
#include "H2MOD\Modules\CustomMenu\CustomLanguage.h"
#include "H2MOD\Modules\Input\Mouseinput.h"
#include "H2MOD\Modules\Input\PlayerControl.h"
#include "imgui.h"
#include "imgui_handler.h"
#include "Util\Hooks\Hook.h"
#include "H2MOD\Tags\TagInterface.h"
#include "Blam\Cache\TagGroups\weapon_definition.hpp"
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
					ImVec2 b2_size = ImVec2(WidthPercentage(10), item_size.y);

					//Weapon Offset X
					ImGui::Text(GetString(weapon_offset_x));
					ImGui::PushItemWidth(WidthPercentage(80));
					ImGui::SliderFloat("##MagnumOffsetX1", &H2Config_Magnum_WeaponOffsetX, -0.15, 0.15, ""); ImGui::SameLine();
					if (ImGui::IsItemEdited())
						magnum_tag->first_person_weapon_offset_i = H2Config_Magnum_WeaponOffsetX;

					ImGui::PushItemWidth(WidthPercentage(10));
					ImGui::InputFloat("##MagnumOffsetX2", &H2Config_Magnum_WeaponOffsetX, -0.15, 0.15, "%.3f"); ImGui::SameLine();
					if (ImGui::IsItemEdited()) {
						if (H2Config_Magnum_WeaponOffsetX > 0.15)
							H2Config_Magnum_WeaponOffsetX = 0.15;
						if (H2Config_Magnum_WeaponOffsetX < -0.15)
							H2Config_Magnum_WeaponOffsetX = -0.15;

						magnum_tag->first_person_weapon_offset_i = H2Config_Magnum_WeaponOffsetX;
					}
					ImGui::PushItemWidth(WidthPercentage(10));
					if (ImGui::Button(GetString(reset, "MagnumOffsetX3"), b2_size))
					{
						H2Config_Magnum_WeaponOffsetX = 0.03;
						magnum_tag->first_person_weapon_offset_i = H2Config_Magnum_WeaponOffsetX;
					}
					ImGui::PopItemWidth();


					//Weapon Offset Y
					ImGui::Text(GetString(weapon_offset_y));
					ImGui::PushItemWidth(WidthPercentage(80));
					ImGui::SliderFloat("##MagnumOffsetY1", &H2Config_Magnum_WeaponOffsetY, -0.15, 0.15, ""); ImGui::SameLine();
					if (ImGui::IsItemEdited())
						magnum_tag->first_person_weapon_offset_j = H2Config_Magnum_WeaponOffsetY;

					ImGui::PushItemWidth(WidthPercentage(10));
					ImGui::InputFloat("##MagnumOffsetY2", &H2Config_Magnum_WeaponOffsetY, -0.15, 0.15, "%.3f"); ImGui::SameLine();
					if (ImGui::IsItemEdited()) {
						if (H2Config_Magnum_WeaponOffsetY > 0.15)
							H2Config_Magnum_WeaponOffsetY = 0.15;
						if (H2Config_Magnum_WeaponOffsetY < -0.15)
							H2Config_Magnum_WeaponOffsetY = -0.15;

						magnum_tag->first_person_weapon_offset_j = H2Config_Magnum_WeaponOffsetY;
					}
					ImGui::PushItemWidth(WidthPercentage(10));
					if (ImGui::Button(GetString(reset, "MagnumOffsetY3"), b2_size))
					{
						H2Config_Magnum_WeaponOffsetY = 0;
						magnum_tag->first_person_weapon_offset_j = H2Config_Magnum_WeaponOffsetY;
					}
					ImGui::PopItemWidth();

					//Weapon Offset Z
					ImGui::Text(GetString(weapon_offset_z));
					ImGui::PushItemWidth(WidthPercentage(80));
					ImGui::SliderFloat("##MagnumOffsetZ1", &H2Config_Magnum_WeaponOffsetZ, -0.15, 0.15, ""); ImGui::SameLine();
					if (ImGui::IsItemEdited())
						magnum_tag->first_person_weapon_offset_k = H2Config_Magnum_WeaponOffsetZ;

					ImGui::PushItemWidth(WidthPercentage(10));
					ImGui::InputFloat("##MagnumOffsetZ2", &H2Config_Magnum_WeaponOffsetZ, -0.15, 0.15, "%.3f"); ImGui::SameLine();
					if (ImGui::IsItemEdited()) {
						if (H2Config_Magnum_WeaponOffsetZ > 0.15)
							H2Config_Magnum_WeaponOffsetZ = 0.15;
						if (H2Config_Magnum_WeaponOffsetZ < -0.15)
							H2Config_Magnum_WeaponOffsetZ = -0.15;

						magnum_tag->first_person_weapon_offset_k = H2Config_Magnum_WeaponOffsetZ;
					}
					ImGui::PushItemWidth(WidthPercentage(10));
					if (ImGui::Button(GetString(reset, "MagnumOffsetZ3"), b2_size))
					{
						H2Config_Magnum_WeaponOffsetZ = -0.005;
						magnum_tag->first_person_weapon_offset_k = H2Config_Magnum_WeaponOffsetZ;
					}
					ImGui::PopItemWidth();

					ImVec2 b3_size = ImVec2(WidthPercentage(33.3333333333f), item_size.y);
				}
				if (ImGui::CollapsingHeader(GetString(smg_title)))
				{
					ImVec2 b2_size = ImVec2(WidthPercentage(10), item_size.y);

					//Weapon Offset X
					ImGui::Text(GetString(weapon_offset_x));
					ImGui::PushItemWidth(WidthPercentage(80));
					ImGui::SliderFloat("##SMGOffsetX1", &H2Config_SMG_WeaponOffsetX, -0.15, 0.15, ""); ImGui::SameLine();
					if (ImGui::IsItemEdited())
						smg_tag->first_person_weapon_offset_i = H2Config_SMG_WeaponOffsetX;

					ImGui::PushItemWidth(WidthPercentage(10));
					ImGui::InputFloat("##SMGOffsetX2", &H2Config_SMG_WeaponOffsetX, -0.15, 0.15, "%.3f"); ImGui::SameLine();
					if (ImGui::IsItemEdited()) {
						if (H2Config_SMG_WeaponOffsetX > 0.15)
							H2Config_SMG_WeaponOffsetX = 0.15;
						if (H2Config_SMG_WeaponOffsetX < -0.15)
							H2Config_SMG_WeaponOffsetX = -0.15;

						smg_tag->first_person_weapon_offset_i = H2Config_SMG_WeaponOffsetX;
					}
					ImGui::PushItemWidth(WidthPercentage(10));
					if (ImGui::Button(GetString(reset, "SMGOffsetX3"), b2_size))
					{
						H2Config_SMG_WeaponOffsetX = 0.02;
						smg_tag->first_person_weapon_offset_i = H2Config_SMG_WeaponOffsetX;
					}
					ImGui::PopItemWidth();


					//Weapon Offset Y
					ImGui::Text(GetString(weapon_offset_y));
					ImGui::PushItemWidth(WidthPercentage(80));
					ImGui::SliderFloat("##SMGOffsetY1", &H2Config_SMG_WeaponOffsetY, -0.15, 0.15, ""); ImGui::SameLine();
					if (ImGui::IsItemEdited())
						smg_tag->first_person_weapon_offset_j = H2Config_SMG_WeaponOffsetY;

					ImGui::PushItemWidth(WidthPercentage(10));
					ImGui::InputFloat("##SMGOffsetY2", &H2Config_SMG_WeaponOffsetY, -0.15, 0.15, "%.3f"); ImGui::SameLine();
					if (ImGui::IsItemEdited()) {
						if (H2Config_SMG_WeaponOffsetY > 0.15)
							H2Config_SMG_WeaponOffsetY = 0.15;
						if (H2Config_SMG_WeaponOffsetY < -0.15)
							H2Config_SMG_WeaponOffsetY = -0.15;

						smg_tag->first_person_weapon_offset_j = H2Config_SMG_WeaponOffsetY;
					}
					ImGui::PushItemWidth(WidthPercentage(10));
					if (ImGui::Button(GetString(reset, "SMGOffsetY3"), b2_size))
					{
						H2Config_SMG_WeaponOffsetY = 0;
						smg_tag->first_person_weapon_offset_j = H2Config_SMG_WeaponOffsetY;
					}
					ImGui::PopItemWidth();

					//Weapon Offset Z
					ImGui::Text(GetString(weapon_offset_z));
					ImGui::PushItemWidth(WidthPercentage(80));
					ImGui::SliderFloat("##SMGOffsetZ1", &H2Config_SMG_WeaponOffsetZ, -0.15, 0.15, ""); ImGui::SameLine();
					if (ImGui::IsItemEdited())
						smg_tag->first_person_weapon_offset_k = H2Config_SMG_WeaponOffsetZ;

					ImGui::PushItemWidth(WidthPercentage(10));
					ImGui::InputFloat("##SMGOffsetZ2", &H2Config_SMG_WeaponOffsetZ, -0.15, 0.15, "%.3f"); ImGui::SameLine();
					if (ImGui::IsItemEdited()) {
						if (H2Config_SMG_WeaponOffsetZ > 0.15)
							H2Config_SMG_WeaponOffsetZ = 0.15;
						if (H2Config_SMG_WeaponOffsetZ < -0.15)
							H2Config_SMG_WeaponOffsetZ = -0.15;

						smg_tag->first_person_weapon_offset_k = H2Config_SMG_WeaponOffsetZ;
					}
					ImGui::PushItemWidth(WidthPercentage(10));
					if (ImGui::Button(GetString(reset, "SMGOffsetZ3"), b2_size))
					{
						H2Config_SMG_WeaponOffsetZ = 0;
						smg_tag->first_person_weapon_offset_k = H2Config_SMG_WeaponOffsetZ;
					}
					ImGui::PopItemWidth();

					ImVec2 b3_size = ImVec2(WidthPercentage(33.3333333333f), item_size.y);
				}
				if (ImGui::CollapsingHeader(GetString(sniper_title)))
				{
					ImVec2 b2_size = ImVec2(WidthPercentage(10), item_size.y);

					//Weapon Offset X
					ImGui::Text(GetString(weapon_offset_x));
					ImGui::PushItemWidth(WidthPercentage(80));
					ImGui::SliderFloat("##SniperOffsetX1", &H2Config_Sniper_WeaponOffsetX, -0.15, 0.15, ""); ImGui::SameLine();
					if (ImGui::IsItemEdited())
						sniper_tag->first_person_weapon_offset_i = H2Config_Sniper_WeaponOffsetX;

					ImGui::PushItemWidth(WidthPercentage(10));
					ImGui::InputFloat("##SniperOffsetX2", &H2Config_Sniper_WeaponOffsetX, -0.15, 0.15, "%.3f"); ImGui::SameLine();
					if (ImGui::IsItemEdited()) {
						if (H2Config_Sniper_WeaponOffsetX > 0.15)
							H2Config_Sniper_WeaponOffsetX = 0.15;
						if (H2Config_Sniper_WeaponOffsetX < -0.15)
							H2Config_Sniper_WeaponOffsetX = -0.15;

						sniper_tag->first_person_weapon_offset_i = H2Config_Sniper_WeaponOffsetX;
					}
					ImGui::PushItemWidth(WidthPercentage(10));
					if (ImGui::Button(GetString(reset, "SniperOffsetX3"), b2_size))
					{
						H2Config_Sniper_WeaponOffsetX = 0.01;
						sniper_tag->first_person_weapon_offset_i = H2Config_Sniper_WeaponOffsetX;
					}
					ImGui::PopItemWidth();


					//Weapon Offset Y
					ImGui::Text(GetString(weapon_offset_y));
					ImGui::PushItemWidth(WidthPercentage(80));
					ImGui::SliderFloat("##SniperOffsetY1", &H2Config_Sniper_WeaponOffsetY, -0.15, 0.15, ""); ImGui::SameLine();
					if (ImGui::IsItemEdited())
						sniper_tag->first_person_weapon_offset_j = H2Config_Sniper_WeaponOffsetY;

					ImGui::PushItemWidth(WidthPercentage(10));
					ImGui::InputFloat("##SniperOffsetY2", &H2Config_Sniper_WeaponOffsetY, -0.15, 0.15, "%.3f"); ImGui::SameLine();
					if (ImGui::IsItemEdited()) {
						if (H2Config_Sniper_WeaponOffsetY > 0.15)
							H2Config_Sniper_WeaponOffsetY = 0.15;
						if (H2Config_Sniper_WeaponOffsetY < -0.15)
							H2Config_Sniper_WeaponOffsetY = -0.15;

						sniper_tag->first_person_weapon_offset_j = H2Config_Sniper_WeaponOffsetY;
					}
					ImGui::PushItemWidth(WidthPercentage(10));
					if (ImGui::Button(GetString(reset, "SniperOffsetY3"), b2_size))
					{
						H2Config_Sniper_WeaponOffsetY = 0;
						sniper_tag->first_person_weapon_offset_j = H2Config_Sniper_WeaponOffsetY;
					}
					ImGui::PopItemWidth();

					//Weapon Offset Z
					ImGui::Text(GetString(weapon_offset_z));
					ImGui::PushItemWidth(WidthPercentage(80));
					ImGui::SliderFloat("##SniperOffsetZ1", &H2Config_Sniper_WeaponOffsetZ, -0.15, 0.15, ""); ImGui::SameLine();
					if (ImGui::IsItemEdited())
						sniper_tag->first_person_weapon_offset_k = H2Config_Sniper_WeaponOffsetZ;

					ImGui::PushItemWidth(WidthPercentage(10));
					ImGui::InputFloat("##SniperOffsetZ2", &H2Config_Sniper_WeaponOffsetZ, -0.15, 0.15, "%.3f"); ImGui::SameLine();
					if (ImGui::IsItemEdited()) {
						if (H2Config_Sniper_WeaponOffsetZ > 0.15)
							H2Config_Sniper_WeaponOffsetZ = 0.15;
						if (H2Config_Sniper_WeaponOffsetZ < -0.15)
							H2Config_Sniper_WeaponOffsetZ = -0.15;

						sniper_tag->first_person_weapon_offset_k = H2Config_Sniper_WeaponOffsetZ;
					}
					ImGui::PushItemWidth(WidthPercentage(10));
					if (ImGui::Button(GetString(reset, "SniperOffsetZ3"), b2_size))
					{
						H2Config_Sniper_WeaponOffsetZ = 0;
						sniper_tag->first_person_weapon_offset_k = H2Config_Sniper_WeaponOffsetZ;
					}
					ImGui::PopItemWidth();

					ImVec2 b3_size = ImVec2(WidthPercentage(33.3333333333f), item_size.y);
				}
			}
		}
		char* GetString(e_weapon_offsets_string string, const std::string& id)
		{
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
			}
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
			ImGui::SetNextWindowSize(ImVec2(650, 530), ImGuiCond_Appearing);
			ImGui::SetNextWindowSizeConstraints(ImVec2(610, 530), ImVec2(1920, 1080));
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
			WriteValue<byte>(Memory::GetAddress(0x9712cC), 1);
			ImGuiToggleInput(true);
			PlayerControl::DisableLocalCamera(true);
		}
		void Close()
		{
			WriteValue<byte>(Memory::GetAddress(0x9712cC), 0);
			ImGuiToggleInput(false);
			PlayerControl::DisableLocalCamera(false);
			SaveH2Config();
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
