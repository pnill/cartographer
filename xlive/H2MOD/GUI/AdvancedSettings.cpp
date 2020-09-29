#include "3rdparty/imgui/imgui.h"
#include "GUI.h"
#include "H2MOD/Modules/Config/Config.h"
#include "H2MOD/Modules/Tweaks/Tweaks.h"

float crosshairSize = 1.0f;
void GUI::ShowAdvancedSettings(bool* p_open)
{
	ImGuiWindowFlags window_flags = 0;
	window_flags |= ImGuiWindowFlags_NoCollapse;

	ImGui::SetNextWindowSize(ImVec2(550, 500), ImGuiCond_Appearing);
	if(ImGui::Begin("Advanced Settings", p_open, window_flags))
	{
		
		ImGui::Indent(1.0);
		ImGui::TextWrapped("These are advanced settings which project cartographer allows you to control over the game.");
		ImGui::NewLine();
		if (ImGui::CollapsingHeader("HUD Settings"))
		{
			ImGui::TextWrapped("HUD Settings allow you to adjust specific settings related to the HUD being displayed.");
			ImGui::Separator();
			float CursorPos = ImGui::GetCursorPosX();

			//Player FOV
			ImGui::Text("Player Field of View");
			ImGui::PushItemWidth(ImGui::GetWindowContentRegionWidth() - (ImGui::GetWindowContentRegionWidth() * 0.2) - 5);
			ImGui::SliderInt("##PlayerFOV1", &H2Config_field_of_view, 45, 110, "FOV = %.3f"); ImGui::SameLine();
			if(ImGui::IsItemEdited())
				H2Tweaks::setFOV(H2Config_field_of_view);
			
			ImGui::PushItemWidth(ImGui::GetWindowContentRegionWidth() - (ImGui::GetWindowContentRegionWidth() * 0.8) - 5);
			ImGui::InputInt("##PlayerFOV2", &H2Config_field_of_view, 0, 110, ImGuiInputTextFlags_::ImGuiInputTextFlags_AutoSelectAll);
			if (ImGui::IsItemEdited()) {
				if (H2Config_field_of_view > 110)
					H2Config_field_of_view = 110;
				if (H2Config_field_of_view < 45)
					H2Config_field_of_view = 45;

				H2Tweaks::setFOV(H2Config_field_of_view);
			}
			ImGui::PopItemWidth();

			//Vehicle FOV
			ImGui::Text("Vehicle Field of View");
			ImGui::PushItemWidth(ImGui::GetWindowContentRegionWidth() - (ImGui::GetWindowContentRegionWidth() * 0.2) - 5);
			ImGui::SliderInt("##VehicleFOV1", &H2Config_vehicle_field_of_view, 45, 110, "FOV = %.3f");ImGui::SameLine();
			if (ImGui::IsItemEdited()) 
				H2Tweaks::setVehicleFOV(H2Config_vehicle_field_of_view);
			
			ImGui::PushItemWidth(ImGui::GetWindowContentRegionWidth() - (ImGui::GetWindowContentRegionWidth() * 0.8) - 5);
			ImGui::InputInt("##VehicleFOV2", &H2Config_vehicle_field_of_view, 0, 110, ImGuiInputTextFlags_::ImGuiInputTextFlags_AutoSelectAll);
			if (ImGui::IsItemEdited()) {
				if (H2Config_vehicle_field_of_view > 110) 
					H2Config_vehicle_field_of_view = 110;
				if (H2Config_vehicle_field_of_view < 45)
					H2Config_vehicle_field_of_view = 45;

				H2Tweaks::setVehicleFOV(H2Config_vehicle_field_of_view);
			}
			ImGui::PopItemWidth();


			//Crosshair Offset
			ImGui::Text("Crosshair Offset");
			ImGui::PushItemWidth(ImGui::GetWindowContentRegionWidth() - (ImGui::GetWindowContentRegionWidth() * 0.2) - 5);
			ImGui::SliderFloat("##Crosshair1", &H2Config_crosshair_offset, 0.0f, 0.5f, "Offset = %.3f");ImGui::SameLine();
			if (ImGui::IsItemEdited())
				H2Tweaks::setCrosshairPos(H2Config_crosshair_offset);
			ImGui::PushItemWidth(ImGui::GetWindowContentRegionWidth() - (ImGui::GetWindowContentRegionWidth() * 0.8) - 5);
			ImGui::InputFloat("##Crosshair2", &H2Config_crosshair_offset, 0, 110, 3);
			if (ImGui::IsItemEdited()) {
				if (H2Config_crosshair_offset > 0.5)
					H2Config_crosshair_offset = 0.5;
				if (H2Config_crosshair_offset < 0)
					H2Config_crosshair_offset = 0;

				H2Tweaks::setCrosshairPos(H2Config_crosshair_offset);
			}
			ImGui::PopItemWidth();

			//Crosshair Size
			ImGui::Text("Crosshair Size");
			ImGui::SliderFloat("##CrosshairSize", &H2Config_crosshair_scale, 0.1f, 2.0f, "Size = %.3f");
			if(ImGui::IsItemEdited())
			{
				H2Tweaks::setCrosshairSize2();
			}
			/*const char* items[] = { "Default", "Very Small", "Small", "Medium", "Large", "Extra Large" };
			ImGui::Combo("##CrosshairSize", )*/
			
			
			ImGui::Separator();


		}

		ImGui::End();
	}
}
