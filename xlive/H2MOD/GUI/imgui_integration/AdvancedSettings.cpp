#include "stdafx.h"

#include "AdvancedSettingsStringTable.h"

#include "cartographer/twizzler/twizzler.h"
#include "game/cheats.h"
#include "game/game.h"
#include "input/input_abstraction.h"
#include "interface/hud.h"
#include "interface/new_hud.h"
#include "interface/first_person_camera.h"
#include "interface/first_person_weapons.h"
#include "networking/NetworkMessageTypeCollection.h"
#include "rasterizer/rasterizer_globals.h"

#include "H2MOD/Modules/CustomMenu/CustomLanguage.h"
#include "H2MOD/Modules/RenderHooks/RenderHooks.h"
#include "H2MOD/Modules/Shell/Config.h"
#include "H2MOD/Modules/SpecialEvents/SpecialEvents.h"
#include "H2MOD/Modules/Tweaks/Tweaks.h"

#ifndef NDEBUG
#include "H2MOD/Modules/CustomMenu/CustomMenu.h"
#include "H2MOD/Modules/DirectorHooks/DirectorHooks.h"
#include "H2MOD/Modules/ObserverMode/ObserverMode.h"
#include "H2MOD/Utils/Utils.h"
#endif

#include "imgui.h"
#include "imgui_handler.h"

/* constants */

const char* k_button_items[] = { "Dpad Up","Dpad Down","Dpad Left","Dpad Right","Start","Back","Left Thumb","Right Thumb","Left Bumper","Right Bumper","A","B","X","Y" };
const char* k_action_items[] = { "Dpad Up","Dpad Down","Dpad Left","Dpad Right","Start","Back","Crouch","Zoom","Flashlight","Switch Grenades","Jump","Melee","Reload","Switch Weapons" };
const uint16 k_button_values[] = { 1, 2, 4, 8, 16, 32, 64, 128, 256, 512, 4096, 8192, 16384, 32768 };

/* prototypes */

const char* advanced_settings_get_string(e_advanced_string string, const char* id = NULL);

/* globals */

e_language g_language;

c_static_string<256> g_advanced_settings_temp_string;

int g_button_placeholders[14];

namespace ImGuiHandler {
	namespace ImAdvancedSettings {

		namespace
		{
			bool g_twizzler_checkbox = g_twizzler_status;
			bool should_show_hud = true;
			bool g_showFP = true;
			int g_deadzone = 0;
			int g_aiming = 0;
			int g_shadows = 0;
			int g_water = 0;
			bool g_init = false;



			void DrawDeadzones()
			{
				ImDrawList* draw_list = ImGui::GetForegroundDrawList();
				const ImGuiViewport* viewport = ImGui::GetMainViewport();
				ImVec2 Center(
					viewport->WorkSize.x - 200,
					viewport->WorkSize.y - 200
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

				s_gamepad_input_button_state* state = input_get_gamepad_state(_controller_index_0);
				ImVec2 Thumb_Pos(
					Center.x + (100 * (state->thumb_right.x / (float)MAXSHORT)),
					Center.y - (100 * (state->thumb_right.y / (float)MAXSHORT)));
				int axial_invalid = 0;
				if (abs(state->thumb_right.x) <= ((float)MAXSHORT * (H2Config_Deadzone_A_X / 100)))
					axial_invalid++;
				if (abs(state->thumb_right.y) <= ((float)MAXSHORT * (H2Config_Deadzone_A_Y / 100)))
					axial_invalid++;
				bool radial_invalid = false;
				unsigned int ar = pow((short)((float)MAXSHORT * (H2Config_Deadzone_Radial / 100)), 2);
				unsigned int arx = pow(state->thumb_right.x, 2);
				unsigned int ary = pow(state->thumb_right.y, 2);
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

				int8 red = (valid ? 0 : 255);
				int8 green = (valid ? 255 : 0);
				draw_list->AddCircleFilled(Thumb_Pos, 5, ImColor(red, green, 0), 60);
			}
			void HudSettings()
			{
				ImVec2 item_size = ImGui::GetItemRectSize();
				if (ImGui::CollapsingHeader(advanced_settings_get_string(_advanced_string_hud_title)))
				{
					ImVec2 b2_size = ImVec2(WidthPercentage(10), item_size.y);
					//Player FOV
					ImGui::Text(advanced_settings_get_string(_advanced_string_player_field_of_view));
					ImGui::PushItemWidth(WidthPercentage(80));
					ImGui::SliderInt("##PlayerFOV1", &H2Config_field_of_view, 45, 110, ""); ImGui::SameLine();
					if (ImGui::IsItemEdited())
						player_control_set_field_of_view(H2Config_field_of_view);

					ImGui::PushItemWidth(WidthPercentage(10));
					ImGui::InputInt("##PlayerFOV2", &H2Config_field_of_view, 0, 110, ImGuiInputTextFlags_::ImGuiInputTextFlags_AutoSelectAll); ImGui::SameLine();
					if (ImGui::IsItemEdited()) {
						H2Config_field_of_view = PIN(H2Config_field_of_view, 45, 110);
						player_control_set_field_of_view(H2Config_field_of_view);
					}
					ImGui::PushItemWidth(WidthPercentage(10));
					if (ImGui::Button(advanced_settings_get_string(_advanced_string_reset, "PlayerFov3"), b2_size))
					{
						H2Config_field_of_view = 78;
						player_control_set_field_of_view(H2Config_field_of_view);
					}
					ImGui::PopItemWidth();


					//Vehicle FOV
					ImGui::Text(advanced_settings_get_string(_advanced_string_vehicle_field_of_view));
					ImGui::PushItemWidth(WidthPercentage(80));
					ImGui::SliderInt("##VehicleFOV1", &H2Config_vehicle_field_of_view, 45, 110, ""); ImGui::SameLine();
					if (ImGui::IsItemEdited())
						observer_set_suggested_field_of_view(H2Config_vehicle_field_of_view);

					ImGui::PushItemWidth(WidthPercentage(10));
					ImGui::InputInt("##VehicleFOV2", &H2Config_vehicle_field_of_view, 0, 110, ImGuiInputTextFlags_::ImGuiInputTextFlags_AutoSelectAll); ImGui::SameLine();
					if (ImGui::IsItemEdited()) {
						if (H2Config_vehicle_field_of_view > 110)
							H2Config_vehicle_field_of_view = 110;
						if (H2Config_vehicle_field_of_view < 45)
							H2Config_vehicle_field_of_view = 45;

						observer_set_suggested_field_of_view(H2Config_vehicle_field_of_view);
					}
					ImGui::PushItemWidth(WidthPercentage(10));
					if (ImGui::Button(advanced_settings_get_string(_advanced_string_reset, "VehicleFOV3"), b2_size))
					{
						H2Config_vehicle_field_of_view = 78;
						observer_set_suggested_field_of_view(H2Config_vehicle_field_of_view);
					}
					ImGui::PopItemWidth();

					//Crosshair Offset
					ImGui::Text(advanced_settings_get_string(_advanced_string_crosshair_offset));
					ImGui::PushItemWidth(WidthPercentage(80));
					ImGui::SliderFloat("##Crosshair1", &H2Config_crosshair_offset, 0.0f, 0.5f, ""); ImGui::SameLine();
					if (ImGui::IsItemEdited())
						set_crosshair_offset(H2Config_crosshair_offset);
					ImGui::PushItemWidth(WidthPercentage(10));
					ImGui::InputFloat("##Crosshair2", &H2Config_crosshair_offset, 0, 110, "%.3f"); ImGui::SameLine();
					if (ImGui::IsItemEdited()) {
						if (H2Config_crosshair_offset > 0.5)
							H2Config_crosshair_offset = 0.5;
						if (H2Config_crosshair_offset < 0)
							H2Config_crosshair_offset = 0;

						set_crosshair_offset(H2Config_crosshair_offset);
					}
					ImGui::PushItemWidth(WidthPercentage(10));
					if (ImGui::Button(advanced_settings_get_string(_advanced_string_reset, "Crosshair3"), b2_size))
					{
						H2Config_crosshair_offset = 0.138f;
						set_crosshair_offset(H2Config_crosshair_offset);
					}
					ImGui::PopItemWidth();

					//Crosshair Size
					ImGui::Text(advanced_settings_get_string(_advanced_string_crosshair_size));
					ImGui::PushItemWidth(WidthPercentage(80));
					ImGui::SliderFloat("##CrosshairSize1", &H2Config_crosshair_scale, 0.0f, 2.0f, "");  ImGui::SameLine();
					if (ImGui::IsItemEdited())
						set_crosshair_scale(H2Config_crosshair_scale);
					ImGui::PushItemWidth(WidthPercentage(10));
					ImGui::InputFloat("##CrosshairSize2", &H2Config_crosshair_scale, 0, 110, "%.3f"); ImGui::SameLine();
					if (ImGui::IsItemEdited()) {
						if (H2Config_crosshair_scale > 2)
							H2Config_crosshair_scale = 2;
						if (H2Config_crosshair_scale < 0)
							H2Config_crosshair_scale = 0;
						set_crosshair_scale(H2Config_crosshair_scale);
					}
					ImGui::PushItemWidth(WidthPercentage(10));
					if (ImGui::Button(advanced_settings_get_string(_advanced_string_reset, "CrosshairSize3"), b2_size))
					{
						H2Config_crosshair_scale = 1;
						set_crosshair_scale(H2Config_crosshair_scale);
					}
					ImGui::PopItemWidth();

					ImVec2 b3_size = ImVec2(WidthPercentage(33.3333333333f), item_size.y);
					ImGui::NewLine();
					//Ingame Change Display
					if (ImGui::Button(advanced_settings_get_string(_advanced_string_weaponoffsets, "WeaponOffsets"), b3_size))
					{
						ImGuiHandler::ToggleWindow("Weapon Offsets");
					}

					ImGui::Columns(2, NULL, false);

					ImGui::Checkbox(advanced_settings_get_string(_advanced_string_hide_ingame_chat), &H2Config_hide_ingame_chat);
					ImGui::NextColumn();
					ImGui::Checkbox(advanced_settings_get_string(_advanced_string_static_fp), &H2Config_static_first_person);
					if (ImGui::IsItemHovered())
						ImGui::SetTooltip(advanced_settings_get_string(_advanced_string_static_fp_tooltip));
					ImGui::NextColumn();
					ImGui::Checkbox(advanced_settings_get_string(_advanced_string_show_hud), &should_show_hud);
					if (ImGui::IsItemEdited())
						should_draw_hud_override_set(should_show_hud);
					ImGui::NextColumn();
					ImGui::Checkbox(advanced_settings_get_string(_advanced_string_show_first_person), &g_showFP);
					if (ImGui::IsItemEdited())
						toggle_first_person(g_showFP);
					ImGui::Columns(1);
					ImGui::NewLine();
				}
			}
			void VideoSettings()
			{
				s_rasterizer_globals* rasterizer_globals = rasterizer_globals_get();
				ImVec2 item_size = ImGui::GetItemRectSize();
				if (ImGui::CollapsingHeader(advanced_settings_get_string(_advanced_string_video_title)))
				{
					//FPS Limit
					ImGui::Columns(2, NULL, false);
					ImGui::Text(advanced_settings_get_string(_advanced_string_fps_limit));
					ImGui::PushItemWidth(WidthPercentage(50));
					int fps_limit = H2Config_fps_limit;
					ImGui::InputInt("##FPS1", &fps_limit, 0, 110);
					if (ImGui::IsItemHovered())
						ImGui::SetTooltip(advanced_settings_get_string(_advanced_string_fps_limit_tooltip));
					if (ImGui::IsItemDeactivatedAfterEdit())
					{
						if (fps_limit > 0)
						{
							fps_limit = MAX(fps_limit, 25);
						}
						else
						{
							fps_limit = 0;
						}

						H2Config_fps_limit = fps_limit;
					}

					ImGui::SameLine();
					if (ImGui::Button(advanced_settings_get_string(_advanced_string_reset, "FPS2"), ImVec2(WidthPercentage(50), item_size.y)))
					{
						H2Config_fps_limit = 60;
					}

					// from the old refresh button
					ImGui::NextColumn();
					ImGui::NextColumn();
					//LOD
					ImGui::Text(advanced_settings_get_string(_advanced_string_lod));
					const char* items[] = { advanced_settings_get_string(_advanced_string_default), advanced_settings_get_string(_advanced_string_lod_1), advanced_settings_get_string(_advanced_string_lod_2), advanced_settings_get_string(_advanced_string_lod_3), advanced_settings_get_string(_advanced_string_lod_4), advanced_settings_get_string(_advanced_string_lod_5), advanced_settings_get_string(_advanced_string_lod_6) };
					ImGui::PushItemWidth(WidthPercentage(100));
					ImGui::Combo("##LOD", &H2Config_static_lod_state, items, 7);
					if (ImGui::IsItemHovered())
						ImGui::SetTooltip(advanced_settings_get_string(_advanced_string_lod_tooltip));

					ImGui::NextColumn();
					ImGui::Text(advanced_settings_get_string(_advanced_string_shadow_title));
					

					const char* s_items[]
					{ 
						advanced_settings_get_string(_advanced_string_tex_L1),
						advanced_settings_get_string(_advanced_string_default),
						advanced_settings_get_string(_advanced_string_tex_L2),
						advanced_settings_get_string(_advanced_string_tex_L3)
					};
					ImGui::PushItemWidth(WidthPercentage(100));
					if (ImGui::Combo("##Shadows", &g_shadows, s_items, 4))
					{
						H2Config_Override_Shadows = (e_override_texture_resolution)g_shadows;
						rasterizer_globals->reset_screen = true;
					}
					ImGui::NextColumn();
					ImGui::Text(advanced_settings_get_string(_advanced_string_water_title));
					ImGui::PushItemWidth(WidthPercentage(100));
					if (ImGui::Combo("##Water", &g_water, s_items, 4))
					{
						H2Config_Override_Water = (e_override_texture_resolution)g_water;
						rasterizer_globals->reset_screen = true;
					}

					ImGui::Columns(1);

					//Force max shader LOD

					ImGui::Checkbox(advanced_settings_get_string(_advanced_string_shader_lod_max), &H2Config_shader_lod_max);
					if (ImGui::IsItemHovered())
						ImGui::SetTooltip(advanced_settings_get_string(_advanced_string_shader_lod_max_tooltip));

					//Disable Light Suppressor

					ImGui::Checkbox(advanced_settings_get_string(_advanced_string_light_suppressor), &H2Config_light_suppressor);
					if (ImGui::IsItemHovered())
						ImGui::SetTooltip(advanced_settings_get_string(_advanced_string_light_suppressor_tooltip));
				}
			}
			void MouseKeyboardSettings()
			{
				ImVec2 item_size = ImGui::GetItemRectSize();
				if (ImGui::CollapsingHeader(advanced_settings_get_string(_advanced_string_m_k_title)))
				{
					ImGui::Columns(2, NULL, false);

					//Raw Input
					TextVerticalPad(advanced_settings_get_string(_advanced_string_raw_mouse));
					ImGui::SameLine(ImGui::GetColumnWidth() - 35);
					ImGui::Checkbox("##RawMouse", &H2Config_raw_input);
					if (ImGui::IsItemHovered())
					{
						ImGui::SetTooltip(advanced_settings_get_string(_advanced_string_raw_mouse_tooltip));
					}
					//Uniform Sensitivity
					ImGui::NextColumn();
					TextVerticalPad(advanced_settings_get_string(_advanced_string_uniform_sensitivity));
					ImGui::SameLine(ImGui::GetColumnWidth() - 35);
					ImGui::Checkbox("##MK_Sep", &H2Config_mouse_uniform);
					if (ImGui::IsItemEdited())
					{
						input_abstraction_set_mouse_look_sensitivity(_controller_index_0, H2Config_mouse_sens);
					}
					if (ImGui::IsItemHovered())
					{
						ImGui::SetTooltip(advanced_settings_get_string(_advanced_string_uniform_sensitivity_tooltip));
					}
					ImGui::Columns(1);
					if (H2Config_raw_input) {
						ImGui::Text(advanced_settings_get_string(_advanced_string_raw_mouse_sensitivity));
						ImGui::PushItemWidth(WidthPercentage(75));
						int g_raw_scale = (int)H2Config_raw_mouse_scale;
						ImGui::SliderInt("##RawMouseScale1", &g_raw_scale, 1, 100, ""); ImGui::SameLine();
						if (ImGui::IsItemEdited())
						{
							H2Config_raw_mouse_scale = (float)g_raw_scale;
						}
						ImGui::PushItemWidth(WidthPercentage(15));
						ImGui::InputFloat("##RawMouseScale2", &H2Config_raw_mouse_scale, 0, 110, "%.5f", ImGuiInputTextFlags_::ImGuiInputTextFlags_AutoSelectAll); ImGui::SameLine();
						if (ImGui::IsItemEdited()) {
							if (g_raw_scale > 100)
								g_raw_scale = 100;
							if (g_raw_scale < 1)
								g_raw_scale = 1;
							g_raw_scale = (int)H2Config_raw_mouse_scale;
						}
						ImGui::PushItemWidth(WidthPercentage(10));
						if (ImGui::Button(advanced_settings_get_string(_advanced_string_reset, "RawMouseScale2"), ImVec2(WidthPercentage(10), item_size.y)))
						{
							g_raw_scale = 25;
							H2Config_raw_mouse_scale = 25.0f;
						}
					}
					else
					{
						ImGui::Text(advanced_settings_get_string(_advanced_string_mouse_sensitivity));
						ImGui::PushItemWidth(WidthPercentage(75));
						int g_mouse_sens = (int)H2Config_mouse_sens;
						ImGui::SliderInt("##Mousesens1", &g_mouse_sens, 1, 100, ""); ImGui::SameLine();
						if (ImGui::IsItemEdited())
						{
							H2Config_mouse_sens = (float)g_mouse_sens;
							input_abstraction_set_mouse_look_sensitivity(_controller_index_0, H2Config_mouse_sens);
						}
						ImGui::PushItemWidth(WidthPercentage(15));
						ImGui::InputFloat("##Mousesens2", &H2Config_mouse_sens, 0, 110, "%.5f", ImGuiInputTextFlags_::ImGuiInputTextFlags_AutoSelectAll); ImGui::SameLine();
						if (ImGui::IsItemEdited()) {
							if (g_mouse_sens > 100)
								g_mouse_sens = 100;
							if (g_mouse_sens < 1)
								g_mouse_sens = 1;
							g_mouse_sens = (int)H2Config_mouse_sens;
							input_abstraction_set_mouse_look_sensitivity(_controller_index_0, H2Config_mouse_sens);
						}
						ImGui::PushItemWidth(WidthPercentage(10));
						if (ImGui::Button(advanced_settings_get_string(_advanced_string_reset, "Mousesens3"), ImVec2(WidthPercentage(10), item_size.y)))
						{
							g_mouse_sens = 3;
							H2Config_mouse_sens = 3.0f;
							input_abstraction_set_mouse_look_sensitivity(_controller_index_0, H2Config_mouse_sens);
						}
					}
				}
			}
			void ControllerSettings()
			{
				ImVec2 item_size = ImGui::GetItemRectSize();
				if (ImGui::CollapsingHeader(advanced_settings_get_string(_advanced_string_controller_title)))
				{
					DrawDeadzones();
					ImGui::Columns(2, NULL, false);
					//Uniform Sensitivity
					TextVerticalPad(advanced_settings_get_string(_advanced_string_uniform_sensitivity));
					ImGui::SameLine(ImGui::GetColumnWidth() - 35);
					ImGui::Checkbox("##C_Sep", &H2Config_mouse_uniform);
					if (ImGui::IsItemEdited())
					{
						input_abstraction_set_mouse_look_sensitivity(_controller_index_0, H2Config_mouse_sens);
					}
					if (ImGui::IsItemHovered())
					{
						ImGui::SetTooltip(advanced_settings_get_string(_advanced_string_uniform_sensitivity_tooltip));
					}
					ImGui::Columns(1);


					ImGui::Text(advanced_settings_get_string(_advanced_string_controller_sensitivity));
					ImGui::PushItemWidth(WidthPercentage(75));
					int g_controller_sens = (int)H2Config_controller_sens;
					ImGui::SliderInt("##Controllersens1", &g_controller_sens, 1, 100, ""); ImGui::SameLine();
					if (ImGui::IsItemEdited())
					{
						H2Config_controller_sens = (float)g_controller_sens;
						input_abstraction_set_controller_look_sensitivity(_controller_index_0, H2Config_controller_sens);
					}
					ImGui::PushItemWidth(WidthPercentage(15));
					ImGui::InputFloat("##Controllersens2", &H2Config_controller_sens, 0, 110, "%.5f", ImGuiInputTextFlags_::ImGuiInputTextFlags_AutoSelectAll); ImGui::SameLine();
					if (ImGui::IsItemEdited()) {
						if (g_controller_sens > 100)
							g_controller_sens = 100;
						if (g_controller_sens < 1)
							g_controller_sens = 1;
						g_controller_sens = (int)H2Config_controller_sens;
						input_abstraction_set_controller_look_sensitivity(_controller_index_0, H2Config_controller_sens);
					}
					ImGui::PushItemWidth(WidthPercentage(10));
					if (ImGui::Button(advanced_settings_get_string(_advanced_string_reset, "Controllersens3"), ImVec2(WidthPercentage(10), item_size.y)))
					{
						g_controller_sens = 3;
						H2Config_controller_sens = 3.0f;
						input_abstraction_set_controller_look_sensitivity(_controller_index_0, H2Config_controller_sens);
					}
					ImGui::PopItemWidth();

					ImGui::Columns(2, NULL, false);
					ImGui::Text(advanced_settings_get_string(_advanced_string_aiming_type));
					const char* a_items[] = { advanced_settings_get_string(_advanced_string_default), advanced_settings_get_string(_advanced_string_modern) };
					ImGui::PushItemWidth(ImGui::GetColumnWidth());
					if (ImGui::Combo("##C_Aiming_Style", &g_aiming, a_items, 2))
					{
						H2Config_controller_modern = g_aiming != 0;
					}
					if (ImGui::IsItemHovered())
					{
						ImGui::SetTooltip(advanced_settings_get_string(_advanced_string_aiming_type_tooltip));
					}
					ImGui::PopItemWidth();

					ImGui::NextColumn();

					ImGui::Text(advanced_settings_get_string(_advanced_string_deadzone_type));
					const char* items[] = { advanced_settings_get_string(_advanced_string_axial), advanced_settings_get_string(_advanced_string_radial), advanced_settings_get_string(_advanced_string_both) };
					ImGui::PushItemWidth(ImGui::GetColumnWidth());
					if (ImGui::Combo("##C_Deadzone_Type", &g_deadzone, items, 3))
					{
						H2Config_Controller_Deadzone = (H2Config_Deadzone_Type)(byte)g_deadzone;
						input_abstraction_set_controller_thumb_deadzone(_controller_index_0);
					}
					if (ImGui::IsItemHovered())
					{
						ImGui::SetTooltip(advanced_settings_get_string(_advanced_string_deadzone_type_tooltip));
					}
					ImGui::PopItemWidth();
					ImGui::Columns(1);

					if (H2Config_Controller_Deadzone == Axial || H2Config_Controller_Deadzone == Both) {
						ImGui::Text(advanced_settings_get_string(_advanced_string_axial_deadzone_X));
						ImGui::PushItemWidth(WidthPercentage(75));
						ImGui::SliderFloat("##C_Deadzone_A_X_1", &H2Config_Deadzone_A_X, 0, 100, "");
						if (ImGui::IsItemEdited())
						{
							input_abstraction_set_controller_thumb_deadzone(_controller_index_0);
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
							input_abstraction_set_controller_thumb_deadzone(_controller_index_0);
						}
						ImGui::SameLine();
						ImGui::PushItemWidth(WidthPercentage(15));
						if (ImGui::Button(advanced_settings_get_string(_advanced_string_default, "C_Deadzone_A_X_3"), ImVec2(WidthPercentage(12), item_size.y)))
						{
							H2Config_Deadzone_A_X = (8689.0f / (float)MAXSHORT) * 100;
							input_abstraction_set_controller_thumb_deadzone(_controller_index_0);
						}
						ImGui::PopItemWidth();
						ImGui::Text(advanced_settings_get_string(_advanced_string_axial_deadzone_Y));
						ImGui::PushItemWidth(WidthPercentage(75));
						ImGui::SliderFloat("##C_Deadzone_A_Y_1", &H2Config_Deadzone_A_Y, 0, 100, "");
						if (ImGui::IsItemEdited())
						{
							input_abstraction_set_controller_thumb_deadzone(_controller_index_0);
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
							input_abstraction_set_controller_thumb_deadzone(_controller_index_0);
						}
						ImGui::SameLine();
						ImGui::PushItemWidth(WidthPercentage(12));
						if (ImGui::Button(advanced_settings_get_string(_advanced_string_default, "C_Deadzone_A_Y_3"), ImVec2(WidthPercentage(12), item_size.y)))
						{
							H2Config_Deadzone_A_Y = (8689.0f / (float)MAXSHORT) * 100;
							input_abstraction_set_controller_thumb_deadzone(_controller_index_0);
						}
						ImGui::PopItemWidth();
					}
					if (H2Config_Controller_Deadzone == Radial || H2Config_Controller_Deadzone == Both) {
						ImGui::Text(advanced_settings_get_string(_advanced_string_radial_deadzone_radius));
						ImGui::PushItemWidth(WidthPercentage(75));
						ImGui::SliderFloat("##C_Deadzone_R_1", &H2Config_Deadzone_Radial, 0, 100, "");
						if (ImGui::IsItemEdited())
						{
							input_abstraction_set_controller_thumb_deadzone(_controller_index_0);
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
							input_abstraction_set_controller_thumb_deadzone(_controller_index_0);
						}
						ImGui::SameLine();
						ImGui::PushItemWidth(WidthPercentage(12));
						if (ImGui::Button(advanced_settings_get_string(_advanced_string_default, "C_Deadzone_R_R_3"), ImVec2(WidthPercentage(12), item_size.y)))
						{
							H2Config_Deadzone_Radial = (8689.0f / (float)MAXSHORT) * 100;
							input_abstraction_set_controller_thumb_deadzone(_controller_index_0);
						}
						ImGui::PopItemWidth();
					}
					ImGui::NewLine();
					ImGui::Text("Controller Layout");
					ImGui::NewLine();
					ImGui::TextWrapped("To use this you must have your games controller layout SET TO DEFAULT. Changing the drop down for the specific action will remap the button to the new one");
					ImGui::NewLine();
					ImGui::Columns(3, NULL, false);
					for (auto i = 0; i < 14; i++)
					{
						ImGui::Text(k_button_items[i]);
						ImGui::PushItemWidth(ImGui::GetColumnWidth());

						// 2 digit number + null terminator
						char index_string[3];
						sprintf(index_string, "%d", i);

						c_static_string<8> label("##C_L");
						label.append(index_string);
						if (ImGui::Combo(label.get_string(), &g_button_placeholders[i], k_action_items, 14))
						{
							switch (k_button_values[i])
							{
							case XINPUT_GAMEPAD_DPAD_UP:
								H2Config_CustomLayout.DPAD_UP = k_button_values[g_button_placeholders[i]];
								break;
							case XINPUT_GAMEPAD_DPAD_DOWN:
								H2Config_CustomLayout.DPAD_DOWN = k_button_values[g_button_placeholders[i]];
								break;
							case XINPUT_GAMEPAD_DPAD_LEFT:
								H2Config_CustomLayout.DPAD_LEFT = k_button_values[g_button_placeholders[i]];
								break;
							case XINPUT_GAMEPAD_DPAD_RIGHT:
								H2Config_CustomLayout.DPAD_RIGHT = k_button_values[g_button_placeholders[i]];
								break;
							case XINPUT_GAMEPAD_START:
								H2Config_CustomLayout.START = k_button_values[g_button_placeholders[i]];
								break;
							case XINPUT_GAMEPAD_BACK:
								H2Config_CustomLayout.BACK = k_button_values[g_button_placeholders[i]];
								break;
							case XINPUT_GAMEPAD_LEFT_THUMB:
								H2Config_CustomLayout.LEFT_THUMB = k_button_values[g_button_placeholders[i]];
								break;
							case XINPUT_GAMEPAD_RIGHT_THUMB:
								H2Config_CustomLayout.RIGHT_THUMB = k_button_values[g_button_placeholders[i]];
								break;
							case XINPUT_GAMEPAD_LEFT_SHOULDER:
								H2Config_CustomLayout.LEFT_SHOULDER = k_button_values[g_button_placeholders[i]];
								break;
							case XINPUT_GAMEPAD_RIGHT_SHOULDER:
								H2Config_CustomLayout.RIGHT_SHOULDER = k_button_values[g_button_placeholders[i]];
								break;
							case XINPUT_GAMEPAD_A:
								H2Config_CustomLayout.A = k_button_values[g_button_placeholders[i]];
								break;
							case XINPUT_GAMEPAD_B:
								H2Config_CustomLayout.B = k_button_values[g_button_placeholders[i]];
								break;
							case XINPUT_GAMEPAD_X:
								H2Config_CustomLayout.X = k_button_values[g_button_placeholders[i]];
								break;
							case XINPUT_GAMEPAD_Y:
								H2Config_CustomLayout.Y = k_button_values[g_button_placeholders[i]];
								break;
							}
						}
						ImGui::PopItemWidth();
						ImGui::NextColumn();
					}
					ImGui::Columns(1);
				}
			}
			void HostSettings()
			{
				if (NetworkSession::LocalPeerIsSessionHost() || game_is_campaign()) {
					if (ImGui::CollapsingHeader(advanced_settings_get_string(_advanced_string_host_campagin_settings)))
					{
						ImGui::Columns(2, NULL, false);
						TextVerticalPad(advanced_settings_get_string(_advanced_string_anti_cheat));
						ImGui::SameLine(ImGui::GetColumnWidth() - 35);
						if (ImGui::Checkbox("##Anti-Cheat", &g_twizzler_checkbox))
						{
							twizzler_set_status(g_twizzler_checkbox);
							for (int i = 0; i < NetworkSession::GetPeerCount(); i++)
							{
								NetworkMessage::SendAntiCheat(i);
							}
						}
						if (ImGui::IsItemHovered())
						{
							ImGui::SetTooltip(advanced_settings_get_string(_advanced_string_anti_cheat_tooltip));
						}

						ImGui::NextColumn();

						//XDelay
						TextVerticalPad(advanced_settings_get_string(_advanced_string_disable_x_delay));
						ImGui::SameLine(ImGui::GetColumnWidth() - 35);
						if (ImGui::Checkbox("##XDelay", &H2Config_xDelay))
						{
							H2Tweaks::RefreshTogglexDelay();
						}
						ImGui::Columns(1);
						ImGui::Separator();
						ImGui::Columns(3, NULL, false);

						// TODO Remove this and replace with proper menu when selecting a map (WIP)
						TextVerticalPad(advanced_settings_get_string(_advanced_string_skull_anger));
						ImGui::SameLine(ImGui::GetColumnWidth() - 35);
						ImGui::Checkbox("##SkullAnger", &get_ice_cream_activation()[_skull_type_anger]);
						if (ImGui::IsItemHovered())
							ImGui::SetTooltip(advanced_settings_get_string(_advanced_string_skull_anger_tooltip));

						ImGui::NextColumn();

						TextVerticalPad(advanced_settings_get_string(_advanced_string_skull_assassins));
						ImGui::SameLine(ImGui::GetColumnWidth() - 35);
						ImGui::Checkbox("##SkullAssassins", &get_ice_cream_activation()[_skull_type_assassians]);
						if (ImGui::IsItemHovered())
							ImGui::SetTooltip(advanced_settings_get_string(_advanced_string_skull_assassins_tooltip));

						ImGui::NextColumn();

						TextVerticalPad(advanced_settings_get_string(_advanced_string_skull_black_eye));
						ImGui::SameLine(ImGui::GetColumnWidth() - 35);
						ImGui::Checkbox("##SkullBlackEye", &get_ice_cream_activation()[_skull_type_black_eye]);
						if (ImGui::IsItemHovered())
							ImGui::SetTooltip(advanced_settings_get_string(_advanced_string_skull_black_eye_tooltip));

						ImGui::NextColumn();

						TextVerticalPad(advanced_settings_get_string(_advanced_string_skull_blind));
						ImGui::SameLine(ImGui::GetColumnWidth() - 35);
						ImGui::Checkbox("##SkullBlind", &get_ice_cream_activation()[_skull_type_blind]);
						if (ImGui::IsItemHovered())
							ImGui::SetTooltip(advanced_settings_get_string(_advanced_string_skull_blind_tooltip));

						ImGui::NextColumn();

						TextVerticalPad(advanced_settings_get_string(_advanced_string_skull_catch));
						ImGui::SameLine(ImGui::GetColumnWidth() - 35);
						ImGui::Checkbox("##SkullCatch", &get_ice_cream_activation()[_skull_type_catch]);
						if (ImGui::IsItemHovered())
							ImGui::SetTooltip(advanced_settings_get_string(_advanced_string_skull_catch_tooltip));

						ImGui::NextColumn();

						TextVerticalPad(advanced_settings_get_string(_advanced_string_skull_envy));
						ImGui::SameLine(ImGui::GetColumnWidth() - 35);
						ImGui::Checkbox("##SkullEnvy", &get_ice_cream_activation()[_skull_type_envy]);
						if (ImGui::IsItemHovered())
							ImGui::SetTooltip(advanced_settings_get_string(_advanced_string_skull_envy_tooltip));

						ImGui::NextColumn();

						TextVerticalPad(advanced_settings_get_string(_advanced_string_skull_famine));
						ImGui::SameLine(ImGui::GetColumnWidth() - 35);
						ImGui::Checkbox("##SkullFamine", &get_ice_cream_activation()[_skull_type_famine]);
						if (ImGui::IsItemHovered())
							ImGui::SetTooltip(advanced_settings_get_string(_advanced_string_skull_famine_tooltip));

						ImGui::NextColumn();

						TextVerticalPad(advanced_settings_get_string(_advanced_string_skull_ghost));
						ImGui::SameLine(ImGui::GetColumnWidth() - 35);
						ImGui::Checkbox("##SkullGhost", &get_ice_cream_activation()[_skull_type_ghost]);
						if (ImGui::IsItemHovered())
							ImGui::SetTooltip(advanced_settings_get_string(_advanced_string_skull_ghost_tooltip));

						ImGui::NextColumn();

						TextVerticalPad(advanced_settings_get_string(_advanced_string_skull_grunt));
						ImGui::SameLine(ImGui::GetColumnWidth() - 35);
						ImGui::Checkbox("##SkullGBP", &get_ice_cream_activation()[_skull_type_grunt_birthday_party]);
						if (ImGui::IsItemHovered())
							ImGui::SetTooltip(advanced_settings_get_string(_advanced_string_skull_grunt_tooltip));

						ImGui::NextColumn();

						TextVerticalPad(advanced_settings_get_string(_advanced_string_skull_iron));
						ImGui::SameLine(ImGui::GetColumnWidth() - 35);
						ImGui::Checkbox("##SkullIron", &get_ice_cream_activation()[_skull_type_iron]);
						if (ImGui::IsItemHovered())
							ImGui::SetTooltip(advanced_settings_get_string(_advanced_string_skull_iron_tooltip));

						ImGui::NextColumn();

						TextVerticalPad(advanced_settings_get_string(_advanced_string_skull_iwbyd));
						ImGui::SameLine(ImGui::GetColumnWidth() - 35);
						ImGui::Checkbox("##SkullIWHBYD", &get_ice_cream_activation()[_skull_type_iwhbyd]);
						if (ImGui::IsItemHovered())
							ImGui::SetTooltip(advanced_settings_get_string(_advanced_string_skull_iwbyd_tooltip));

						ImGui::NextColumn();

						TextVerticalPad(advanced_settings_get_string(_advanced_string_skull_mythic));
						ImGui::SameLine(ImGui::GetColumnWidth() - 35);
						ImGui::Checkbox("##SkullMythic", &get_ice_cream_activation()[_skull_type_mythic]);
						if (ImGui::IsItemHovered())
							ImGui::SetTooltip(advanced_settings_get_string(_advanced_string_skull_mythic_tooltip));

						ImGui::NextColumn();

						TextVerticalPad(advanced_settings_get_string(_advanced_string_skull_sputnik));
						ImGui::SameLine(ImGui::GetColumnWidth() - 35);
						ImGui::Checkbox("##SkullSputnik", &get_ice_cream_activation()[_skull_type_sputnik]);
						if (ImGui::IsItemHovered())
							ImGui::SetTooltip(advanced_settings_get_string(_advanced_string_skull_sputnik_tooltip));

						ImGui::NextColumn();

						TextVerticalPad(advanced_settings_get_string(_advanced_string_skull_thunderstorm));
						ImGui::SameLine(ImGui::GetColumnWidth() - 35);
						ImGui::Checkbox("##SkullThunderstorm", &get_ice_cream_activation()[_skull_type_thunderstorm]);
						if (ImGui::IsItemHovered())
							ImGui::SetTooltip(advanced_settings_get_string(_advanced_string_skull_thunderstorm_tooltip));

						ImGui::NextColumn();

						TextVerticalPad(advanced_settings_get_string(_advanced_string_skull_whuppopotamus));
						ImGui::SameLine(ImGui::GetColumnWidth() - 35);
						ImGui::Checkbox("##SkullWhuppopatamus", &get_ice_cream_activation()[_skull_type_whuppopotamus]);
						if (ImGui::IsItemHovered())
							ImGui::SetTooltip(advanced_settings_get_string(_advanced_string_skull_whuppopotamus_tooltip));

						ImGui::Columns(1);
					}
				}
			}
			void GameSettings()
			{
				if (ImGui::CollapsingHeader(advanced_settings_get_string(_advanced_string_game_title)))
				{
					ImGui::Columns(2, NULL, false);

					TextVerticalPad(advanced_settings_get_string(_advanced_string_discord_presence));
					ImGui::SameLine(ImGui::GetColumnWidth() - 35);
					ImGui::Checkbox("##DRP", &H2Config_discord_enable);

					TextVerticalPad(advanced_settings_get_string(_advanced_string_upnp_title));
					ImGui::SameLine(ImGui::GetColumnWidth() - 35);
					ImGui::Checkbox("##upnp", &H2Config_upnp_enable);
					if (ImGui::IsItemHovered())
					{
						ImGui::SetTooltip(advanced_settings_get_string(_advanced_string_upnp_tooltip));
					}

					TextVerticalPad(advanced_settings_get_string(_advanced_string_no_events_title));
					ImGui::SameLine(ImGui::GetColumnWidth() - 35);
					ImGui::Checkbox("##no_events", &H2Config_no_events);
					if (ImGui::IsItemHovered())
						ImGui::SetTooltip(advanced_settings_get_string(_advanced_string_no_events_tooltip));

					if (get_current_special_event() == _special_event_halloween) {
						TextVerticalPad(advanced_settings_get_string(_advanced_string_skeleton_biped));
						ImGui::SameLine(ImGui::GetColumnWidth() - 35);
						ImGui::Checkbox("##spooky_scary", &H2Config_spooky_boy);
						if (ImGui::IsItemHovered())
							ImGui::SetTooltip(advanced_settings_get_string(_advanced_string_skeleton_biped_tooltip));
					}

					// next column elements now
					//Skip Intro
					ImGui::NextColumn();
					TextVerticalPad(advanced_settings_get_string(_advanced_string_disable_intro_videos));
					ImGui::SameLine(ImGui::GetColumnWidth() - 35);
					ImGui::Checkbox("##Intro", &H2Config_skip_intro);

					ImGui::Columns(1);

					ImGui::Text(advanced_settings_get_string(_advanced_string_language));

					const char* lang_items[]
					{
						advanced_settings_get_string(_advanced_string_lang_english),
						advanced_settings_get_string(_advanced_string_lang_japanese),
						advanced_settings_get_string(_advanced_string_lang_german),
						advanced_settings_get_string(_advanced_string_lang_french),
						advanced_settings_get_string(_advanced_string_lang_spanish),
						advanced_settings_get_string(_advanced_string_lang_italian),
						advanced_settings_get_string(_advanced_string_lang_korean),
						advanced_settings_get_string(_advanced_string_lang_chinese),
						advanced_settings_get_string(_advanced_string_lang_native) 
					};

					ImGui::PushItemWidth(ImGui::GetColumnWidth());
					if (ImGui::Combo("##Language_Selection", (int*)&g_language, lang_items, k_language_count))
					{
						H2Config_language.code_main = (g_language == 8 ? -1 : g_language);
						setCustomLanguage(H2Config_language.code_main, H2Config_language.code_variant);
					}
					ImGui::PopItemWidth();


					ImGui::Columns(1);
					ImGui::NewLine();
				}
			}
		}

		
		void Render(bool* p_open)
		{
			if (!g_init)
			{
				g_deadzone = (int)H2Config_Controller_Deadzone;
				g_aiming = (int)H2Config_controller_modern;
				g_shadows = (int)H2Config_Override_Shadows;
				g_water = (int)H2Config_Override_Water;
				g_language = (H2Config_language.code_main == -1 ? _language_portuguese : (e_language)H2Config_language.code_main);
				g_init = true;
			}

			bool open = *p_open;
			ImGuiIO& io = ImGui::GetIO();
			const ImGuiViewport* viewport = ImGui::GetMainViewport();
			ImGuiWindowFlags window_flags = 0;
			window_flags |= ImGuiWindowFlags_NoCollapse;
			window_flags |= ImGuiWindowFlags_AlwaysVerticalScrollbar;
			//window_flags |= ImGuiWindowFlags_MenuBar;
			ImGui::SetNextWindowPos(ImVec2(viewport->WorkSize.x * 0.5f, viewport->WorkSize.y * 0.5f), ImGuiCond_::ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));
			ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(4, 8));
			//ImGui::PushFont(font2);
			ImGui::SetNextWindowSize(ImVec2(650, 530), ImGuiCond_Appearing);
			ImGui::SetNextWindowSizeConstraints(ImVec2(610, 530), ImVec2(1920, 1080));
			if (game_is_ui_shell())
				ImGui::SetNextWindowBgAlpha(1);
			if (ImGui::Begin(advanced_settings_get_string(_advanced_string_title), &open, window_flags))
			{
				HudSettings();
				VideoSettings();
				MouseKeyboardSettings();
				ControllerSettings();
				HostSettings();
				GameSettings();


#ifndef NDEBUG
				if (ImGui::CollapsingHeader("Dev Testing"))
				{
					s_rasterizer_globals* rasterizer_globals = rasterizer_globals_get();
					
					ImGui::Indent();
					if (ImGui::CollapsingHeader("Director Mode"))
					{
						if (ImGui::Button("Set Observer Team"))
						{
							WriteValue(Memory::GetAddress(0x51A6B4), 255);
						}
						if (ImGui::Button("Game"))
						{
							DirectorHooks::SetDirectorMode(DirectorHooks::e_game);
							ObserverMode::SwitchObserverMode(ObserverMode::observer_none);
						}
						if (ImGui::Button("Editor"))
						{
							ObserverMode::SwitchObserverMode(ObserverMode::observer_freecam);
						}
						ImGui::SameLine();
						if (ImGui::Button("Editor Follow"))
						{
							ObserverMode::NextPlayer();
							ObserverMode::SwitchObserverMode(ObserverMode::observer_followcam);
						}
						ImGui::SameLine();
						if (ImGui::Button("Editor First Person"))
						{
							ObserverMode::NextPlayer();
							ObserverMode::SwitchObserverMode(ObserverMode::observer_firstperson);
						}
						if (ImGui::Button("Player"))
						{
							DirectorHooks::SetDirectorMode(DirectorHooks::e_game);
							ObserverMode::SwitchObserverMode(ObserverMode::observer_none);
						}
						if (ImGui::Button("N"))
						{
							ObserverMode::NextPlayer();
						}
					}
					if (ImGui::CollapsingHeader("Raster Layers")) {
						ImGui::Columns(4, NULL, false);
						for (auto i = 0; i < 25; i++)
						{
							if (ImGui::Checkbox(IntToString<int>(i).c_str(), &ras_layer_overrides[i]))
							{
								rasterizer_globals->reset_screen = true;
							}
							ImGui::NextColumn();
						}
						ImGui::Columns(1);
					}
					if (ImGui::CollapsingHeader("Render Geometries"))
					{
						ImGui::Columns(4, NULL, false);
						for (auto i = 0; i < 24; i++)
						{
							ImGui::Checkbox(IntToString<int>(i).c_str(), &geo_render_overrides[i]);
							ImGui::NextColumn();
						}
						ImGui::Columns(1);
					}
					static int event_type = H2Config_forced_event;
					if (ImGui::CollapsingHeader("Events"))
					{
						if (ImGui::RadioButton("None", &event_type, _special_event_none))
						{
							H2Config_forced_event = _special_event_none;
						} ImGui::SameLine();
						if (ImGui::RadioButton("Christmas", &event_type, _special_event_christmas))
						{
							H2Config_forced_event = _special_event_christmas;
						} ImGui::SameLine();
						if (ImGui::RadioButton("St Paddys", &event_type, _special_event_st_paddys))
						{
							H2Config_forced_event = _special_event_st_paddys;
						} ImGui::SameLine();
						if (ImGui::RadioButton("Mook Madness", &event_type, _special_event_mook_maddness))
						{
							H2Config_forced_event = _special_event_mook_maddness;
						}

						if (ImGui::RadioButton("Halloween", &event_type, _special_event_halloween))
						{
							H2Config_forced_event = _special_event_halloween;
						}ImGui::SameLine();
						if (ImGui::RadioButton("Birthday", &event_type, _special_event_birthday))
						{
							H2Config_forced_event = _special_event_birthday;
						}

					}
					if (ImGui::CollapsingHeader("WGIT Testing"))
					{
						if (ImGui::Button("Custom Languages"))
						{
							GSCustomMenuCall_Language();
						}
					}
				}
#endif
			}

			ImGui::PopStyleVar();
			ImGui::End();

			if (!open)
			{
				ImGuiHandler::ToggleWindow(k_advanced_settings_window_name);
			}
		}
		void Open()
		{
			uint16 Buttons[14];
			H2Config_CustomLayout.ToArray(Buttons);
			for (auto i = 0; i < 14; i++)
			{
				for (auto j = 0; j < 14; j++)
				{
					if (k_button_values[j] == Buttons[i])
						g_button_placeholders[i] = j;
				}
			}
		}
		void Close()
		{
			SaveH2Config();
		}
	}
}

/* private code */

const char* advanced_settings_get_string(e_advanced_string string, const char* id)
{
	e_language language = get_current_language();
	const char* result;

	if (g_advanced_settings_string_table[language] != NULL && 
		g_advanced_settings_string_table[language][string] != NULL)
	{
		result = g_advanced_settings_string_table[language][string];
	}
	else
	{
		result = g_advanced_settings_string_table[_language_english][string];
	}
	

	if (id != NULL)
	{
		g_advanced_settings_temp_string.set(result);
		g_advanced_settings_temp_string.append("##");
		g_advanced_settings_temp_string.append(id);
		result = g_advanced_settings_temp_string.get_string();
	}

	return result;
}
