#include "3rdparty/imgui/imgui.h"
#include "H2MOD/Modules/Config/Config.h"
#include "H2MOD/Modules/Tweaks/Tweaks.h"
#include "H2MOD/Modules/Startup/Startup.h"
#include "H2MOD/Modules/HudElements/HudElements.h"
#include "3rdparty/imgui/imgui_internal.h"
#include "H2MOD/Modules/AdvLobbySettings/AdvLobbySettings.h"
#include "H2MOD/Modules/HitFix/HitFix.h"
#include "H2MOD/Modules/Input/KeyboardInput.h"
#include "H2MOD/Modules/Networking/NetworkSession/NetworkSession.h"
#include "H2MOD/Modules/Stats/StatsHandler.h"
#include "H2MOD/Modules/Input/PlayerControl.h"
#include "Util/Hooks/Hook.h"
#include "H2MOD/Modules/Input/Mouseinput.h"
#include "H2MOD/Modules/Input/ControllerInput.h"
#include "imgui_handler.h"
#include "H2MOD/GUI/GUI.h"
#include "H2MOD/Modules/CustomMenu/CustomLanguage.h"


namespace imgui_handler {
	namespace AdvancedSettings {
		namespace
		{
			float crosshairSize = 1.0f;
			bool g_showHud = true;
			bool g_showFP = true;
			bool g_UncappedFPS = false;
			bool g_experimentalFPSPatch = false;
			int g_fpsLimit = 60;
			bool g_hitfix = true;
			int g_deadzone = 0;
			int g_aiming = 0;
			bool g_init = false;
			void DrawDeadzones()
			{
				ImDrawList* draw_list = ImGui::GetOverlayDrawList();
				ImVec2 Center(
					ImGui::GetIO().DisplaySize.x - 200,
					ImGui::GetIO().DisplaySize.y - 200
				);
				
				draw_list->AddRectFilled(ImVec2(Center.x - 100, Center.y - 100), ImVec2(Center.x + 100, Center.y + 100), ImColor(20, 20, 20));
				draw_list->AddCircleFilled(Center, 100, ImColor(255, 255, 255), 120);
				if (H2Config_Controller_Deadzone == Axial || H2Config_Controller_Deadzone == Both) {
					int X_Axis = (int)(100 * (H2Config_Deadzone_A_X / 100));
					int Y_Axis = (int)(100 * (H2Config_Deadzone_A_Y / 100));
					ImVec2 Y_TopLeft(
						Center.x - 100,
						Center.y - Y_Axis);
					ImVec2 Y_BottomRight(
						Center.x + 100,
						Center.y + Y_Axis);
					draw_list->AddRectFilled(Y_TopLeft, Y_BottomRight, ImColor(20, 20, 20, 125));
					ImVec2 X_TopLeft(Center.x - X_Axis, Center.y - 100);
					ImVec2 X_BottomRight(Center.x + X_Axis, Center.y + 100);
					draw_list->AddRectFilled(X_TopLeft, X_BottomRight, ImColor(20, 20, 20, 125));
				}
				if (H2Config_Controller_Deadzone == Radial || H2Config_Controller_Deadzone == Both) {
					draw_list->AddCircleFilled(Center, H2Config_Deadzone_Radial, ImColor(20, 20, 20, 125), 120);
				}

				short* C_Input = (short*)ControllerInput::get_controller_input(0);
				ImVec2 Thumb_Pos(
					Center.x + (100 * (C_Input[28] / (float)MAXSHORT)),
					Center.y - (100 * (C_Input[29] / (float)MAXSHORT)));
				int axial_invalid = 0;
				if (abs(C_Input[28]) <= ((float)MAXSHORT * (H2Config_Deadzone_A_X / 100)))
					axial_invalid++;
				if (abs(C_Input[29]) <= ((float)MAXSHORT * (H2Config_Deadzone_A_Y / 100)))
					axial_invalid++;
				bool radial_invalid = false;
				unsigned int ar = pow((short)((float)MAXSHORT * (H2Config_Deadzone_Radial / 100)), 2);
				unsigned int arx = pow(C_Input[28], 2);
				unsigned int ary = pow(C_Input[29], 2);
				unsigned int rh = arx + ary;
				if (rh <= ar)
				{
					radial_invalid = true;
				}
				bool valid = true;
				if (H2Config_Controller_Deadzone == Axial || H2Config_Controller_Deadzone == Both)
					if (axial_invalid == 2)
						valid = false;
				if (H2Config_Controller_Deadzone == Radial || H2Config_Controller_Deadzone == Both)
					if (radial_invalid)
						valid = false;

				if(valid)
				{
					draw_list->AddCircleFilled(Thumb_Pos, 5, ImColor(0, 255, 0), 60);
				}
				else
				{
					draw_list->AddCircleFilled(Thumb_Pos, 5, ImColor(255, 0, 0), 60);
				}
			}
		}
		void Render(bool* p_open)
		{
			if(!g_init)
			{
				g_deadzone = (int)H2Config_Controller_Deadzone;
				g_aiming = (int)H2Config_controller_modern;
				g_init = true;
			}
			ImGuiIO& io = ImGui::GetIO();

			RECT rect;
			::GetClientRect(get_HWND(), &rect);
			io.DisplaySize = ImVec2((float)(rect.right - rect.left), (float)(rect.bottom - rect.top));
			ImGuiWindowFlags window_flags = 0;
			window_flags |= ImGuiWindowFlags_NoCollapse;
			window_flags |= ImGuiWindowFlags_AlwaysVerticalScrollbar;
			//window_flags |= ImGuiWindowFlags_MenuBar;
			ImGui::SetNextWindowPos(ImVec2(io.DisplaySize.x * 0.5f, io.DisplaySize.y * 0.5f), ImGuiCond_::ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));
			ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(4, 8));
			//ImGui::PushFont(font2);
			ImGui::SetNextWindowSize(ImVec2(650, 530), ImGuiCond_Appearing);
			ImGui::SetNextWindowSizeConstraints(ImVec2(610, 530), ImVec2(1920, 1080));
			if (h2mod->GetMapType() == MainMenu)
				ImGui::SetNextWindowBgAlpha(1);
			std::string AcStatus = "  Advanced Settings";
			if (ImGui::Begin(AcStatus.c_str(), p_open, window_flags))
			{
				ImVec2 item_size = ImGui::GetItemRectSize();
				if (ImGui::CollapsingHeader("HUD Settings"))
				{
					ImVec2 b2_size = ImVec2(WidthPercentage(10), item_size.y);
					//Player FOV
					ImGui::Text("Player Field of View");
					ImGui::PushItemWidth(WidthPercentage(80));
					ImGui::SliderInt("##PlayerFOV1", &H2Config_field_of_view, 45, 110, ""); ImGui::SameLine();
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
					if (ImGui::Button("Reset##PlayerFov3", b2_size))
					{
						H2Config_field_of_view = 78.0f;
						HudElements::setFOV();
					}
					ImGui::PopItemWidth();


					//Vehicle FOV
					ImGui::Text("Vehicle Field of View");
					ImGui::PushItemWidth(WidthPercentage(80));
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
					ImGui::PushItemWidth(WidthPercentage(80));
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
					ImGui::PushItemWidth(WidthPercentage(80));
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
					if (!H2Config_hide_ingame_chat)
					{
						if (ImGui::Button("Show Ingame Chat", b3_size))
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
					if (g_showFP)
					{
						if (ImGui::Button("Disable First Person", b3_size))
						{
							HudElements::ToggleFirstPerson(false);
							g_showFP = false;
						}
					}
					else
					{
						if (ImGui::Button("Enable First Person", b3_size))
						{
							HudElements::ToggleFirstPerson(true);
							g_showFP = true;
						}
					}
					ImGui::NewLine();
				}
				if (ImGui::CollapsingHeader("Video Settings"))
				{
					ImVec2 LargestText = ImGui::CalcTextSize("High Resolution Fix", NULL, true);
					float float_offset = ImGui::GetCursorPosX() + LargestText.x + (LargestText.x * 0.075);
					//FPS Limit
					TextVerticalPad("FPS Limit", 8.5);
					ImGui::SameLine();
					ImGui::SetCursorPosX(float_offset);
					ImGui::PushItemWidth(WidthPercentage(10.0f));
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
					ImGui::Checkbox("Experimental Rendering Changes", &H2Config_experimental_fps);
					if (ImGui::IsItemHovered())
						ImGui::SetTooltip("This will enabled experimental changes to the way the games engine renders.\nA restart is required for the changes to take effect.\n\nThis will cause controller Vibrations not to work.");
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
					const char* items[] = { "Default", "L1 - Very Low", "L2 - Low", "L3 - Medium", "L4 - High", "L5 - Very High", "L6 - Cinematic" };
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
				if(ImGui::CollapsingHeader("Mouse and Keyboard Input"))
				{
					ImGui::Columns(2, "", false);
					//Disable Ingame Keyboard
					TextVerticalPad("Disable Keyboard Input", 8.5);
					ImGui::SameLine(ImGui::GetColumnWidth() - 35);
					if (ImGui::Checkbox("##KeyboardInput", &H2Config_disable_ingame_keyboard))
					{
						KeyboardInput::ToggleKeyboardInput();
					}
					else
					{
						KeyboardInput::ToggleKeyboardInput();
					}

					ImGui::NextColumn();

					//Raw Input
					TextVerticalPad("Raw Mouse Input", 8.5);
					ImGui::SameLine(ImGui::GetColumnWidth() - 35);
					ImGui::Checkbox("##RawMouse", &H2Config_raw_input);

					//Uniform Sensitivity
					ImGui::NextColumn();
					TextVerticalPad("Uniform Sensitivity", 8.5);
					ImGui::SameLine(ImGui::GetColumnWidth() - 35);
					ImGui::Checkbox("##MK_Sep", &H2Config_mouse_uniform);
					if (ImGui::IsItemEdited())
					{
						MouseInput::SetSensitivity(H2Config_mouse_sens);
					}
					if (ImGui::IsItemHovered())
					{
						ImGui::SetTooltip("By default the game has the horizontal sensitivity half of the vertical.\nEnabling this option will make these match.\n\nNOTE: Controller Modern Aiming will make this not work.");
					}
					ImGui::Columns(1);
					if (H2Config_raw_input) {
						ImGui::Text("Raw Mouse Sensitivity");
						ImGui::PushItemWidth(WidthPercentage(75));
						int g_raw_scale = (int)H2Config_raw_mouse_scale;
						ImGui::SliderInt("##RawMouseScale1", &g_raw_scale, 1, 100, ""); ImGui::SameLine();
						if (ImGui::IsItemEdited())
						{
							H2Config_raw_mouse_scale = (float)g_raw_scale;
						}
						ImGui::PushItemWidth(WidthPercentage(15));
						ImGui::InputFloat("##RawMouseScale2", &H2Config_raw_mouse_scale, 0, 110, 5, ImGuiInputTextFlags_::ImGuiInputTextFlags_AutoSelectAll); ImGui::SameLine();
						if (ImGui::IsItemEdited()) {
							if (g_raw_scale > 100)
								g_raw_scale = 100;
							if (g_raw_scale < 1)
								g_raw_scale = 1;
							g_raw_scale = (int)H2Config_raw_mouse_scale;
						}
						ImGui::PushItemWidth(WidthPercentage(10));
						if (ImGui::Button("Reset##RawMouseScale2", ImVec2(WidthPercentage(10), item_size.y)))
						{
							g_raw_scale = 25;
							H2Config_raw_mouse_scale = 25.0f;
						}
					}
					else
					{
						ImGui::Text("Mouse Sensitivity");
						ImGui::PushItemWidth(WidthPercentage(75));
						int g_mouse_sens = (int)H2Config_mouse_sens;
						ImGui::SliderInt("##Mousesens1", &g_mouse_sens, 1, 100, ""); ImGui::SameLine();
						if (ImGui::IsItemEdited())
						{
							H2Config_mouse_sens = (float)g_mouse_sens;
							MouseInput::SetSensitivity(H2Config_mouse_sens);
						}
						ImGui::PushItemWidth(WidthPercentage(15));
						ImGui::InputFloat("##Mousesens2", &H2Config_mouse_sens, 0, 110, 5, ImGuiInputTextFlags_::ImGuiInputTextFlags_AutoSelectAll); ImGui::SameLine();
						if (ImGui::IsItemEdited()) {
							if (g_mouse_sens > 100)
								g_mouse_sens = 100;
							if (g_mouse_sens < 1)
								g_mouse_sens = 1;
							g_mouse_sens = (int)H2Config_mouse_sens;
							MouseInput::SetSensitivity(H2Config_mouse_sens);
						}
						ImGui::PushItemWidth(WidthPercentage(10));
						if (ImGui::Button("Reset##Mousesens3", ImVec2(WidthPercentage(10), item_size.y)))
						{
							g_mouse_sens = 3;
							H2Config_mouse_sens = 3.0f;
							MouseInput::SetSensitivity(H2Config_mouse_sens);
						}
					}
				}
				if(ImGui::CollapsingHeader("Controller Input"))
				{
					DrawDeadzones();
					ImGui::Columns(2, "", false);
					//Uniform Sensitivity
					TextVerticalPad("Uniform Sensitivity", 8.5);
					ImGui::SameLine(ImGui::GetColumnWidth() - 35);
					ImGui::Checkbox("##C_Sep", &H2Config_mouse_uniform);
					if (ImGui::IsItemEdited())
					{
						MouseInput::SetSensitivity(H2Config_mouse_sens);
					}
					if (ImGui::IsItemHovered())
					{
						ImGui::SetTooltip("By default the game has the horizontal sensitivity half of the vertical.\nEnabling this option will make these match.");
					}
					ImGui::Columns(1);
					
					
					ImGui::Text("Controller Sensitivity");
					ImGui::PushItemWidth(WidthPercentage(75));
					int g_controller_sens = (int)H2Config_controller_sens;
					ImGui::SliderInt("##Controllersens1", &g_controller_sens, 1, 100, ""); ImGui::SameLine();
					if (ImGui::IsItemEdited())
					{
						H2Config_controller_sens = (float)g_controller_sens;
						ControllerInput::SetSensitiviy(H2Config_controller_sens);
					}
					ImGui::PushItemWidth(WidthPercentage(15));
					ImGui::InputFloat("##Controllersens2", &H2Config_controller_sens, 0, 110, 5, ImGuiInputTextFlags_::ImGuiInputTextFlags_AutoSelectAll); ImGui::SameLine();
					if (ImGui::IsItemEdited()) {
						if (g_controller_sens > 100)
							g_controller_sens = 100;
						if (g_controller_sens < 1)
							g_controller_sens = 1;
						g_controller_sens = (int)H2Config_controller_sens;
						ControllerInput::SetSensitiviy(H2Config_controller_sens);
					}
					ImGui::PushItemWidth(WidthPercentage(10));
					if (ImGui::Button("Reset##Controllersens3", ImVec2(WidthPercentage(10), item_size.y)))
					{
						g_controller_sens = 3;
						H2Config_controller_sens = 3.0f;
						ControllerInput::SetSensitiviy(H2Config_controller_sens);
					}
					ImGui::PopItemWidth();

					ImGui::Columns(2, "", false);
					ImGui::Text("Aiming Type");
					const char* a_items[] = { "Default", "Modern" };
					ImGui::PushItemWidth(ImGui::GetColumnWidth());
					if (ImGui::Combo("##C_Aiming_Style", &g_aiming, a_items, 2))
					{
						H2Config_controller_modern = g_aiming != 0;
						ControllerInput::ToggleModern();
					}
					if (ImGui::IsItemHovered())
					{
						ImGui::SetTooltip("Mordern Aiming will remove the native acceleration zones from a controller while aiming, allowing for more percise aim.\n\nNOTE: Modern Aiming will make MK not function correctly.");
					}
					ImGui::PopItemWidth();

					ImGui::NextColumn();

					ImGui::Text("Deadzone Type");
					const char* items[] = { "Axial", "Radial", "Both" };
					ImGui::PushItemWidth(ImGui::GetColumnWidth());;
					if(ImGui::Combo("##C_Deadzone_Type", &g_deadzone, items, 3))
					{
						H2Config_Controller_Deadzone = (H2Config_Deadzone_Type)(byte)g_deadzone;
						ControllerInput::SetDeadzones();
					}
					if(ImGui::IsItemHovered())
					{
						ImGui::SetTooltip("Halo 2 by default uses axial deadzones, radial deadzones have been added as another option for players.");
					}
					ImGui::PopItemWidth();
					ImGui::Columns(1);

					if (H2Config_Controller_Deadzone == Axial || H2Config_Controller_Deadzone == Both) {
						ImGui::Text("Axial Deadzone X");
						ImGui::PushItemWidth(WidthPercentage(75));
						ImGui::SliderFloat("##C_Deadzone_A_X_1", &H2Config_Deadzone_A_X, 0, 100, "");
						if (ImGui::IsItemEdited())
						{
							ControllerInput::SetDeadzones();
						}
						ImGui::SameLine();
						ImGui::PushItemWidth(WidthPercentage(13));
						ImGui::InputFloat("##C_Deadzone_A_X_2", &H2Config_Deadzone_A_X, 0, 3);
						if (ImGui::IsItemEdited())
						{
							if (H2Config_Deadzone_A_X < 0)
								H2Config_Deadzone_A_X = 0;
							if (H2Config_Deadzone_A_X > 100)
								H2Config_Deadzone_A_X = 100;
							ControllerInput::SetDeadzones();
						}
						ImGui::SameLine();
						ImGui::PushItemWidth(WidthPercentage(15));
						if (ImGui::Button("Default##C_Deadzone_A_X_3", ImVec2(WidthPercentage(12), item_size.y)))
						{
							H2Config_Deadzone_A_X = (8689.0f / (float)MAXSHORT) * 100;
							ControllerInput::SetDeadzones();
						}
						ImGui::PopItemWidth();
						ImGui::Text("Axial Deadzone Y");
						ImGui::PushItemWidth(WidthPercentage(75));
						ImGui::SliderFloat("##C_Deadzone_A_Y_1", &H2Config_Deadzone_A_Y, 0, 100, "");
						if (ImGui::IsItemEdited())
						{
							ControllerInput::SetDeadzones();
						}
						ImGui::SameLine();
						ImGui::PushItemWidth(WidthPercentage(13));
						ImGui::InputFloat("##C_Deadzone_A_Y_2", &H2Config_Deadzone_A_Y, 0, 3);
						if (ImGui::IsItemEdited())
						{
							if (H2Config_Deadzone_A_Y < 0)
								H2Config_Deadzone_A_Y = 0;
							if (H2Config_Deadzone_A_Y > 100)
								H2Config_Deadzone_A_Y = 100;
							ControllerInput::SetDeadzones();
						}
						ImGui::SameLine();
						ImGui::PushItemWidth(WidthPercentage(12));
						if (ImGui::Button("Default##C_Deadzone_A_Y_3", ImVec2(WidthPercentage(12), item_size.y)))
						{
							H2Config_Deadzone_A_Y = (8689.0f / (float)MAXSHORT) * 100;
							ControllerInput::SetDeadzones();
						}
						ImGui::PopItemWidth();
					}
					if (H2Config_Controller_Deadzone == Radial || H2Config_Controller_Deadzone == Both) {
						ImGui::Text("Radial Deadzone Radius");
						ImGui::PushItemWidth(WidthPercentage(75));
						ImGui::SliderFloat("##C_Deadzone_R_1", &H2Config_Deadzone_Radial, 0, 100, "");
						if (ImGui::IsItemEdited())
						{
							ControllerInput::SetDeadzones();
						}
						ImGui::SameLine();
						ImGui::PushItemWidth(WidthPercentage(13));
						ImGui::InputFloat("##C_Deadzone_R_2", &H2Config_Deadzone_Radial, 0, 3);
						if (ImGui::IsItemEdited())
						{
							if (H2Config_Deadzone_Radial < 0)
								H2Config_Deadzone_Radial = 0;
							if (H2Config_Deadzone_Radial > 100)
								H2Config_Deadzone_Radial = 100;
							ControllerInput::SetDeadzones();
						}
						ImGui::SameLine();
						ImGui::PushItemWidth(WidthPercentage(12));
						if (ImGui::Button("Default##C_Deadzone_R_R_3", ImVec2(WidthPercentage(12), item_size.y)))
						{
							H2Config_Deadzone_Radial = (8689.0f / (float)MAXSHORT) * 100;
							ControllerInput::SetDeadzones();
						}
						ImGui::PopItemWidth();
					}
					ImGui::NewLine();
				}

				if (NetworkSession::localPeerIsSessionHost() || h2mod->GetMapType() == scnr_type::SinglePlayer) {
					if (ImGui::CollapsingHeader("Host & Campaign Settings"))
					{
						ImGui::Columns(2, "", false);
						TextVerticalPad("Anti-Cheat", 8.5);
						ImGui::SameLine(ImGui::GetColumnWidth() - 35);
						if (ImGui::Checkbox("##Anti-Cheat", &H2Config_anti_cheat_enabled))
						{
							for (auto i = 0; i < NetworkSession::getCurrentNetworkSession()->membership.peer_count; i++)
							{
								CustomPackets::sendAntiCheat(i);
							}
						}
						if (ImGui::IsItemHovered())
						{
							ImGui::SetTooltip("Allows you to disable the Anti-Cheat for your lobby.");
						}

						ImGui::NextColumn();

						//XDelay
						TextVerticalPad("Disable X to Delay", 8.5);
						ImGui::SameLine(ImGui::GetColumnWidth() - 35);
						ImGui::Checkbox("##XDelay", &H2Config_xDelay);

						ImGui::Columns(1);
						ImGui::Separator();
						auto Skulls = reinterpret_cast<skull_enabled_flags*>(h2mod->GetAddress(0x4D8320));
						ImGui::Columns(3, "", false);

						TextVerticalPad("Anger", 8.5);
						ImGui::SameLine(ImGui::GetColumnWidth() - 35);
						ImGui::Checkbox("##SkullAnger", &Skulls->Anger);
						if (ImGui::IsItemHovered())
							ImGui::SetTooltip("Enemies and allies fire their weapons faster and more frequently.");

						ImGui::NextColumn();

						TextVerticalPad("Assassins", 8.5);
						ImGui::SameLine(ImGui::GetColumnWidth() - 35);
						ImGui::Checkbox("##SkullAssassins", &Skulls->Assassians);
						if (ImGui::IsItemHovered())
							ImGui::SetTooltip("All enemies in game are permanently cloaked. Allies can sometimes\nsee them but mostly they can't, so they can't help much.");

						ImGui::NextColumn();

						TextVerticalPad("Black Eye", 8.5);
						ImGui::SameLine(ImGui::GetColumnWidth() - 35);
						ImGui::Checkbox("##SkullBlackEye", &Skulls->Black_Eye);
						if (ImGui::IsItemHovered())
							ImGui::SetTooltip("Your shield does not charge normally. To charge your shields you\nmust kill something with a melee attack");

						ImGui::NextColumn();

						TextVerticalPad("Blind", 8.5);
						ImGui::SameLine(ImGui::GetColumnWidth() - 35);
						ImGui::Checkbox("##SkullBlind", &Skulls->Blind);
						if (ImGui::IsItemHovered())
							ImGui::SetTooltip("Your heads-up display becomes invisible. In other words, you cannot\nsee your weapon, body, shields, ammunition, motion tracker,\n or use your flashlight.");

						ImGui::NextColumn();

						TextVerticalPad("Catch", 8.5);
						ImGui::SameLine(ImGui::GetColumnWidth() - 35);
						ImGui::Checkbox("##SkullCatch", &Skulls->Catch);
						if (ImGui::IsItemHovered())
							ImGui::SetTooltip("A.I. will throw more grenades. Also, everybody will drop two grenades\n of their kind Flood will drop grenades depending on whether\n they're human or Covenant.");

						ImGui::NextColumn();

						TextVerticalPad("Envy", 8.5);
						ImGui::SameLine(ImGui::GetColumnWidth() - 35);
						ImGui::Checkbox("##SkullEnvy", &Skulls->Envy);
						if (ImGui::IsItemHovered())
							ImGui::SetTooltip(" The Master Chief now has an Active camouflage just like the Arbiter's.\nHowever, there is no visible timer, so remember: five second\n cloak with ten second recharge on Legendary");

						ImGui::NextColumn();

						TextVerticalPad("Famine", 8.5);
						ImGui::SameLine(ImGui::GetColumnWidth() - 35);
						ImGui::Checkbox("##SkullFamine", &Skulls->Famine);
						if (ImGui::IsItemHovered())
							ImGui::SetTooltip("All dropped weapons have half ammo. Weapons that spawned on the floor or\nspawned with are unaffected.");

						ImGui::NextColumn();

						TextVerticalPad("Ghost", 8.5);
						ImGui::SameLine(ImGui::GetColumnWidth() - 35);
						ImGui::Checkbox("##SkullGhost", &Skulls->Ghost);
						if (ImGui::IsItemHovered())
							ImGui::SetTooltip("A.I. characters will not flinch from attacks, melee or otherwise.");

						ImGui::NextColumn();

						TextVerticalPad("Grunt Birthday", 8.5);
						ImGui::SameLine(ImGui::GetColumnWidth() - 35);
						ImGui::Checkbox("##SkullGBP", &Skulls->Grunt_Birthday_Party);
						if (ImGui::IsItemHovered())
							ImGui::SetTooltip("Headshots turn into Plasma Grenade explosions.");

						ImGui::NextColumn();

						TextVerticalPad("Iron", 8.5);
						ImGui::SameLine(ImGui::GetColumnWidth() - 35);
						ImGui::Checkbox("##SkullIron", &Skulls->Iron);
						if (ImGui::IsItemHovered())
							ImGui::SetTooltip("When playing co-op, if either player dies the game restarts you at your\nlast checkpoint.");

						ImGui::NextColumn();

						TextVerticalPad("IWHBYD", 8.5);
						ImGui::SameLine(ImGui::GetColumnWidth() - 35);
						ImGui::Checkbox("##SkullIWHBYD", &Skulls->IWHBYD);
						if (ImGui::IsItemHovered())
							ImGui::SetTooltip("The rarity of combat dialog is changed, rare lines become far more common\nbut common lines are still present at their normal rate");

						ImGui::NextColumn();

						TextVerticalPad("Mythic", 8.5);
						ImGui::SameLine(ImGui::GetColumnWidth() - 35);
						ImGui::Checkbox("##SkullMythic", &Skulls->Mythic);
						if (ImGui::IsItemHovered())
							ImGui::SetTooltip("Enemies have more health and shielding, and are therefore harder to kill.");

						ImGui::NextColumn();

						TextVerticalPad("Sputnik", 8.5);
						ImGui::SameLine(ImGui::GetColumnWidth() - 35);
						ImGui::Checkbox("##SkullSputnik", &Skulls->Sputnik);
						if (ImGui::IsItemHovered())
							ImGui::SetTooltip("The mass of certain objects is severely reduced, making them fly further\nwhen smacked with a melee hit, or when they are near an explosion");

						ImGui::NextColumn();

						TextVerticalPad("Thunderstorm", 8.5);
						ImGui::SameLine(ImGui::GetColumnWidth() - 35);
						ImGui::Checkbox("##SkullThunderstorm", &Skulls->Thunderstorm);
						if (ImGui::IsItemHovered())
							ImGui::SetTooltip("Causes most enemy and ally units to be their highest rank.");

						ImGui::NextColumn();

						TextVerticalPad("Whuppopotamus", 8.5);
						ImGui::SameLine(ImGui::GetColumnWidth() - 35);
						ImGui::Checkbox("##SkullWhuppopatamus", &Skulls->Whuppopotamus);
						if (ImGui::IsItemHovered())
							ImGui::SetTooltip("Strengthens the hearing of both allies and enemies");

						ImGui::Columns(1);
					}
				}

				if (ImGui::CollapsingHeader("Game Settings"))
				{
					ImGui::Columns(2, "", false);

					TextVerticalPad("Discord Rich Presence", 8.5);
					ImGui::SameLine(ImGui::GetColumnWidth() - 35);
					ImGui::Checkbox("##DRP", &H2Config_discord_enable);
					
					ImGui::NextColumn();

					//Skip Intro
					TextVerticalPad("Disable Intro videos", 8.5);
					ImGui::SameLine(ImGui::GetColumnWidth() - 35);
					ImGui::Checkbox("##Intro", &H2Config_skip_intro);

					ImGui::NextColumn();

					ImGui::Text("Language");
					const char* l_items[]{ "English", "Japanese", "German", "French", "Spanish", "Italian", "Korean", "Chinese" };
					ImGui::PushItemWidth(ImGui::GetColumnWidth());
					if (ImGui::Combo("##Language_Selection", &H2Config_language.code_main, l_items, 8))
					{
						setCustomLanguage(H2Config_language.code_main, H2Config_language.code_variant);
					}
					ImGui::PopItemWidth();
					ImGui::Columns(1);
					ImGui::NewLine();
				}
#if DISPLAY_DEV_TESTING_MENU
				if (ImGui::CollapsingHeader("Dev Testing"))
				{
					ImGui::Checkbox("Projectile Update Tickrate Patch", &g_hitfix);
					if (g_hitfix)
						HitFix_Projectile_Tick_Rate = 30.0f;
					else
						HitFix_Projectile_Tick_Rate = 60.0f;
					if (ImGui::Button("Dump Game Globals"))
					{
						auto game_globals = Blam::EngineDefinitions::game_globals(**h2mod->GetAddress<Blam::EngineDefinitions::game_globals**>(0x482D3C));
						LOG_INFO_GAME(game_globals.engine_settings.game_variant.variant_name);
					}
					ImGui::Checkbox("Anti-Cheat", &H2Config_anti_cheat_enabled);
				}
#endif
				ImGui::End();
			}
			if (!*p_open)
				Close();
		}
		void Open()
		{
			WriteValue<byte>(h2mod->GetAddress(0x9712cC), 1);
			PlayerControl::GetControls(0)->DisableCamera = true;
		}
		void Close()
		{
			WriteValue<byte>(h2mod->GetAddress(0x9712cC), 0);
			PlayerControl::GetControls(0)->DisableCamera = false;
			SaveH2Config();
		}
	}
}
