#include "3rdparty/imgui/imgui.h"
#include "GUI.h"
#include "H2MOD/Modules/Config/Config.h"
#include "H2MOD/Modules/Tweaks/Tweaks.h"
#include "H2MOD/Modules/Startup/Startup.h"
#include "H2MOD/Modules/HudElements/HudElements.h"
#include "imgui_internal.h"
#include "H2MOD/Modules/AdvLobbySettings/AdvLobbySettings.h"

float crosshairSize = 1.0f;
bool g_showHud = true;
bool g_showFP = true;
bool g_UncappedFPS = false;
bool g_experimentalFPSPatch = false;
int g_fpsLimit = 60;
void GUI::ShowAdvancedSettings(bool* p_open)
{
	ImGuiWindowFlags window_flags = 0;
	window_flags |= ImGuiWindowFlags_NoCollapse;
	window_flags |= ImGuiWindowFlags_AlwaysVerticalScrollbar;

	ImGui::SetNextWindowSize(ImVec2(650, 500), ImGuiCond_Appearing);
	ImGui::SetNextWindowSizeConstraints(ImVec2(610, 500), ImVec2(1920, 1080));
	if(h2mod->GetMapType() == MainMenu)
		ImGui::SetNextWindowBgAlpha(1);
	if(ImGui::Begin("  Advanced Settings", p_open, window_flags))
	{
		ImVec2 item_size = ImGui::GetItemRectSize();
		if (ImGui::CollapsingHeader("HUD Settings"))
		{
			ImVec2 b2_size = ImVec2(WidthPercentage(10), item_size.y);
			ImGui::TextWrapped("HUD Settings allow you to adjust specific settings related to the HUD being displayed.");
			ImGui::Separator();
			//Player FOV
			ImGui::Text("Player Field of View");
			ImGui::PushItemWidth(GUI::WidthPercentage(80));
			ImGui::SliderInt("##PlayerFOV1", &H2Config_field_of_view, 45, 110,""); ImGui::SameLine();
			if (ImGui::IsItemEdited())
				HudElements::setFOV();
			
			ImGui::PushItemWidth(WidthPercentage(10));
			ImGui::InputInt("##PlayerFOV2", &H2Config_field_of_view, 0, 110, ImGuiInputTextFlags_::ImGuiInputTextFlags_AutoSelectAll); ImGui::SameLine();
			if (ImGui::IsItemEdited()) {
				if (H2Config_field_of_view > 110)
					H2Config_field_of_view = 110;
				if (H2Config_field_of_view < 45)
					H2Config_field_of_view = 45;

				HudElements::setFOV();
			}
			ImGui::PushItemWidth(WidthPercentage(10));
			if(ImGui::Button("Reset##PlayerFov3", b2_size))
			{
				H2Config_field_of_view = 78.0f;
				HudElements::setFOV();
			}
			ImGui::PopItemWidth();


			//Vehicle FOV
			ImGui::Text("Vehicle Field of View");
			ImGui::PushItemWidth(GUI::WidthPercentage(80));
			ImGui::SliderInt("##VehicleFOV1", &H2Config_vehicle_field_of_view, 45, 110, "");ImGui::SameLine();
			if (ImGui::IsItemEdited())
				HudElements::setVehicleFOV();
			
			ImGui::PushItemWidth(WidthPercentage(10));
			ImGui::InputInt("##VehicleFOV2", &H2Config_vehicle_field_of_view, 0, 110, ImGuiInputTextFlags_::ImGuiInputTextFlags_AutoSelectAll); ImGui::SameLine();
			if (ImGui::IsItemEdited()) {
				if (H2Config_vehicle_field_of_view > 110) 
					H2Config_vehicle_field_of_view = 110;
				if (H2Config_vehicle_field_of_view < 45)
					H2Config_vehicle_field_of_view = 45;

				HudElements::setVehicleFOV();
			}
			ImGui::PushItemWidth(WidthPercentage(10));
			if (ImGui::Button("Reset##VehicleFOV3", b2_size))
			{
				H2Config_vehicle_field_of_view = 78.0f;
				HudElements::setVehicleFOV();
			}
			ImGui::PopItemWidth();

			//Crosshair Offset
			ImGui::Text("Crosshair Offset");
			ImGui::PushItemWidth(GUI::WidthPercentage(80));
			ImGui::SliderFloat("##Crosshair1", &H2Config_crosshair_offset, 0.0f, 0.5f, ""); ImGui::SameLine();
			if (ImGui::IsItemEdited())
				HudElements::setCrosshairPos();
			ImGui::PushItemWidth(WidthPercentage(10));
			ImGui::InputFloat("##Crosshair2", &H2Config_crosshair_offset, 0, 110, 3); ImGui::SameLine();
			if (ImGui::IsItemEdited()) {
				if (H2Config_crosshair_offset > 0.5)
					H2Config_crosshair_offset = 0.5;
				if (H2Config_crosshair_offset < 0)
					H2Config_crosshair_offset = 0;

				HudElements::setCrosshairPos();
			}
			ImGui::PushItemWidth(WidthPercentage(10));
			if (ImGui::Button("Reset##Crosshair3", b2_size))
			{
				H2Config_crosshair_offset = 0.138f;
				HudElements::setCrosshairPos();
			}
			ImGui::PopItemWidth();

			//Crosshair Size
			ImGui::Text("Crosshair Size");
			ImGui::PushItemWidth(GUI::WidthPercentage(80));
			ImGui::SliderFloat("##CrosshairSize1", &H2Config_crosshair_scale, 0.0f, 2.0f, "");  ImGui::SameLine();
			if (ImGui::IsItemEdited())
				HudElements::setCrosshairSize();
			ImGui::PushItemWidth(WidthPercentage(10));
			ImGui::InputFloat("##CrosshairSize2", &H2Config_crosshair_scale, 0, 110, 3); ImGui::SameLine();
			if (ImGui::IsItemEdited()) {
				if (H2Config_crosshair_scale > 2)
					H2Config_crosshair_scale = 2;
				if (H2Config_crosshair_scale < 0)
					H2Config_crosshair_scale = 0;
				HudElements::setCrosshairSize();
			}
			ImGui::PushItemWidth(WidthPercentage(10));
			if (ImGui::Button("Reset##CrosshairSize3", b2_size))
			{
				H2Config_crosshair_scale = 1;
				HudElements::setCrosshairSize();
			}
			ImGui::PopItemWidth();
			
			ImVec2 b3_size = ImVec2(WidthPercentage(33.3333333333f), item_size.y);
			ImGui::NewLine();
			//Ingame Change Display
			if(!H2Config_hide_ingame_chat)
			{
				if(ImGui::Button("Show Ingame Chat", b3_size))
					H2Config_hide_ingame_chat = false;
				ImGui::SameLine();
			}
			else
			{
				if (ImGui::Button("Hide Ingame Chat", b3_size))
					H2Config_hide_ingame_chat = true;
				ImGui::SameLine();
			}

			//Toggle HUD
			if (g_showHud)
			{
				if (ImGui::Button("Disable HUD", b3_size))
				{
					HudElements::ToggleHUD(false);
					g_showHud = false;
				}
				ImGui::SameLine();
			}
			else
			{
				if (ImGui::Button("Enable HUD", b3_size))
				{
					HudElements::ToggleHUD(true);
					g_showHud = true;
				}
				ImGui::SameLine();
			}

			//Toggle First Person
			if(g_showFP)
			{
				if(ImGui::Button("Disable First Person", b3_size))
				{
					HudElements::ToggleFirstPerson(false);
					g_showFP = false;
				}
			}
			else
			{
				if(ImGui::Button("Enable First Person", b3_size))
				{
					HudElements::ToggleFirstPerson(true);
					g_showFP = true;
				}
			}
			ImGui::NewLine();
		}

		if(ImGui::CollapsingHeader("Video Settings"))
		{
			ImVec2 LargestText = ImGui::CalcTextSize("High Resolution Fix", NULL, true);
			float float_offset = ImGui::GetCursorPosX() + LargestText.x + (LargestText.x * 0.075);
			//FPS Limit
			TextVerticalPad("FPS Limit", 8.5);
			ImGui::SameLine();
			ImGui::SetCursorPosX(float_offset);
			ImGui::PushItemWidth(GUI::WidthPercentage(10.0f));
			ImGui::InputInt("##FPS1", &H2Config_fps_limit, 0, 110); ImGui::SameLine();
			if (ImGui::IsItemHovered())
				ImGui::SetTooltip("Setting this to 0 will uncap your games frame rate.\nAnything over 60 may cause performance issues\nUse the Experiment FPS Fix to resolve them");
			if (ImGui::IsItemEdited()) {
				if (H2Config_fps_limit < 10 && H2Config_fps_limit != 0)
					H2Config_fps_limit = 10;
				if (H2Config_fps_limit > 2048)
					H2Config_fps_limit = 2048;
				desiredRenderTime = std::chrono::duration_cast<std::chrono::high_resolution_clock::duration>(std::chrono::duration<double>(1.0 / (double)H2Config_fps_limit));
			}
			ImGui::PopItemWidth();
			if (ImGui::Button("Reset##FPS2", ImVec2(WidthPercentage(10.0f), item_size.y)))
			{
				H2Config_fps_limit = 60;
				desiredRenderTime = std::chrono::duration_cast<std::chrono::high_resolution_clock::duration>(std::chrono::duration<double>(1.0 / (double)H2Config_fps_limit));
			}
			ImGui::SameLine();
			ImGui::Checkbox("Experimental Uncapped FPS Fix", &g_experimentalFPSPatch);
			if (ImGui::IsItemHovered())
				ImGui::SetTooltip("This will enabled experimental changes to the way the games engine renders.");

			//ImGui::Columns(2, "VideoSettings", false);
			//Refresh Rate
			TextVerticalPad("Refresh Rate", 8.5);
			ImGui::SameLine();
			ImGui::SetCursorPosX(float_offset);
			ImGui::PushItemWidth(165);
			ImGui::InputInt("##Refresh1", &H2Config_refresh_rate, 0, 110, ImGuiInputTextFlags_AlwaysInsertMode); //ImGui::SameLine();
			if (ImGui::IsItemHovered())
				ImGui::SetTooltip("This setting requires a restart to take effect.");
			ImGui::PopItemWidth();
			//ImGui::NextColumn();
			

			//LOD
			TextVerticalPad("Level of Detail", 8.5);
			ImGui::SameLine();
			const char* items[] = {"Default", "L1 - Very Low", "L2 - Low", "L3 - Medium", "L4 - High", "L5 - Very High", "L6 - Cinematic"};
			ImGui::PushItemWidth(165);
			ImGui::SetCursorPosX(float_offset);
			ImGui::Combo("##LOD", &H2Config_static_lod_state, items, 7);
			if (ImGui::IsItemHovered())
				ImGui::SetTooltip("Changing this will force the game to use the set Level of Detail for models that have them\nLeaving it at default makes it dynamic which is the games\n default behaviour.");
			ImGui::PopItemWidth();
			//Hires Fix
			TextVerticalPad("High Resolution Fix", 8.5);
			ImGui::SameLine();
			ImGui::SetCursorPosX(ImGui::GetCursorPosX() + 5);
			ImGui::Checkbox("##HighRes", &H2Config_hiresfix);
			if (ImGui::IsItemHovered())
				ImGui::SetTooltip("This will enable fixes for high resolution monitors that will fix text clipping\nA restart is required for these changes to take effect.");
			//ImGui::Columns(0);
			ImGui::NewLine();
		}

		if(ImGui::CollapsingHeader("Game Settings"))
		{
			ImGui::Columns(2, "", false);

			//XDelay
			TextVerticalPad("Disable X to Delay", 8.5);
			ImGui::SameLine(ImGui::GetColumnWidth() - 35);
			ImGui::Checkbox("##XDelay", &H2Config_xDelay);
			
			ImGui::NextColumn();

			//Skip Intro
			TextVerticalPad("Disable Intro videos", 8.5);
			ImGui::SameLine(ImGui::GetColumnWidth() - 35);
			ImGui::Checkbox("##Intro", &H2Config_skip_intro);

			ImGui::NextColumn();

			//Disable Ingame Keyboard
			TextVerticalPad("Disable Keyboard Input", 8.5);
			ImGui::SameLine(ImGui::GetColumnWidth() - 35);
			ImGui::Checkbox("##KeyboardInput", &H2Config_disable_ingame_keyboard);

			ImGui::NextColumn();

			//Raw Input
			TextVerticalPad("Raw Mouse Input", 8.5);
			ImGui::SameLine(ImGui::GetColumnWidth() - 35);
			ImGui::Checkbox("##RawMouse", &H2Config_raw_input);
			if (ImGui::IsItemHovered())
				ImGui::SetTooltip("This setting requires a restart to take effect.");
			ImGui::Columns(1);
			ImGui::NewLine();
		}

		if(ImGui::CollapsingHeader("Host Settings"))
		{
			auto Skulls = reinterpret_cast<skull_enabled_flags*>(h2mod->GetAddress(0x4D8320));
			ImGui::Columns(3, "", true);

			TextVerticalPad("Anger", 8.5);
			ImGui::SameLine(ImGui::GetColumnWidth() - 35);
			ImGui::Checkbox("##SkullAnger", &Skulls->Anger);

			ImGui::NextColumn();

			TextVerticalPad("Assassins", 8.5);
			ImGui::SameLine(ImGui::GetColumnWidth() - 35);
			ImGui::Checkbox("##SkullAssassins", &Skulls->Assassians);

			ImGui::NextColumn();

			TextVerticalPad("Black Eye", 8.5);
			ImGui::SameLine(ImGui::GetColumnWidth() - 35);
			ImGui::Checkbox("##SkullBlackEye", &Skulls->Black_Eye);

			ImGui::NextColumn();

			TextVerticalPad("Blind", 8.5);
			ImGui::SameLine(ImGui::GetColumnWidth() - 35);
			ImGui::Checkbox("##SkullBlind", &Skulls->Blind);

			ImGui::NextColumn();

			TextVerticalPad("Catch", 8.5);
			ImGui::SameLine(ImGui::GetColumnWidth() - 35);
			ImGui::Checkbox("##SkullCatch", &Skulls->Catch);

			ImGui::NextColumn();

			TextVerticalPad("Envy", 8.5);
			ImGui::SameLine(ImGui::GetColumnWidth() - 35);
			ImGui::Checkbox("##SkullEnvy", &Skulls->Envy);

			ImGui::NextColumn();

			TextVerticalPad("Famine", 8.5);
			ImGui::SameLine(ImGui::GetColumnWidth() - 35);
			ImGui::Checkbox("##SkullFamine", &Skulls->Famine);

			ImGui::NextColumn();

			TextVerticalPad("Ghost", 8.5);
			ImGui::SameLine(ImGui::GetColumnWidth() - 35);
			ImGui::Checkbox("##SkullGhost", &Skulls->Ghost);

			ImGui::NextColumn();

			TextVerticalPad("Grunt Birthday Party", 8.5);
			ImGui::SameLine(ImGui::GetColumnWidth() - 35);
			ImGui::Checkbox("##SkullGBP", &Skulls->Grunt_Birthday_Party);

			ImGui::NextColumn();

			TextVerticalPad("Iron", 8.5);
			ImGui::SameLine(ImGui::GetColumnWidth() - 35);
			ImGui::Checkbox("##SkullIron", &Skulls->Iron);

			ImGui::NextColumn();

			TextVerticalPad("IWHBYD", 8.5);
			ImGui::SameLine(ImGui::GetColumnWidth() - 35);
			ImGui::Checkbox("##SkullIWHBYD", &Skulls->IWHBYD);

			ImGui::NextColumn();

			TextVerticalPad("Mythic", 8.5);
			ImGui::SameLine(ImGui::GetColumnWidth() - 35);
			ImGui::Checkbox("##SkullMythic", &Skulls->Mythic);

			ImGui::NextColumn();

			TextVerticalPad("Sputnik", 8.5);
			ImGui::SameLine(ImGui::GetColumnWidth() - 35);
			ImGui::Checkbox("##SkullSputnik", &Skulls->Sputnik);

			ImGui::NextColumn();

			TextVerticalPad("Thunderstorm", 8.5);
			ImGui::SameLine(ImGui::GetColumnWidth() - 35);
			ImGui::Checkbox("##SkullThunderstorm", &Skulls->Thunderstorm);

			ImGui::NextColumn();

			TextVerticalPad("Whuppopotamus", 8.5);
			ImGui::SameLine(ImGui::GetColumnWidth() - 35);
			ImGui::Checkbox("##SkullWhuppopatamus", &Skulls->Whuppopotamus);

			ImGui::Columns(1);
		}

		if(ImGui::CollapsingHeader("Project Settings"))
		{
			TextVerticalPad("Discord Rich Presence", 8.5);
			ImGui::SameLine();
			ImGui::Checkbox("##DRP", &H2Config_discord_enable);
		}
		ImGui::End();
	}
}


