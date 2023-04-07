#include "stdafx.h"

#include "Config.h"

#include "H2MOD\Modules\Shell\Shell.h"
#include "H2MOD\Modules\CustomMenu\CustomMenu.h"
#include "H2MOD\Modules\Updater\Updater.h"
#include "H2MOD\Modules\OnScreenDebug\OnscreenDebug.h"
#include "H2MOD\Modules\Shell\Startup\Startup.h"
#include "H2MOD\Utils\Utils.h"
#include "Util\SimpleIni.h"
#include "JsonConfig/JsonConfig.cpp"


#pragma region Config IO
const wchar_t* H2ConfigFilenames[] = { L"%wshalo2config%d.ini", L"%wsh2serverconfig%d.ini" };
const wchar_t* H2ConfigJsonFilenames[] = { L"%wshalo2config%d.json", L"%wsh2serverconfig%d.json" };

std::string H2ConfigVersionNumber("1");
std::string H2ConfigVersionSection("H2ConfigurationVersion:" + H2ConfigVersionNumber);

unsigned long H2Config_master_ip = inet_addr("149.56.81.89");
unsigned short H2Config_master_port_login = 27020;
unsigned short H2Config_master_port_relay = 1001;

//config variables
bool H2Portable = false;//TODO

std::string cartographerURL = "https://cartographer.online";
std::string cartographerMapRepoURL = "http://www.h2maps.net/Cartographer/CustomMaps";

unsigned short H2Config_base_port = 2000;
char H2Config_str_wan[16] = { "" };
char H2Config_str_lan[16] = { "" };
unsigned long H2Config_ip_wan = 0;
unsigned long H2Config_ip_lan = 0;
_H2Config_language H2Config_language = { -1, 0 };
bool H2Config_custom_labels_capture_missing = false;
bool H2Config_skip_intro = false;
bool H2Config_raw_input = false;
bool H2Config_discord_enable = true;
//bool H2Config_controller_aim_assist = true;
int H2Config_fps_limit = 60;
int H2Config_static_lod_state = e_static_lod::disable;
int H2Config_field_of_view = 70;
int H2Config_vehicle_field_of_view = 70;
__int16 H2Config_refresh_rate = 60;
bool H2Config_static_first_person = false;
float H2Config_mouse_sens = 0;
bool H2Config_mouse_uniform = false;
float H2Config_controller_sens = 0;
bool H2Config_controller_modern = false;
H2Config_Deadzone_Type H2Config_Controller_Deadzone = H2Config_Deadzone_Type::Axial;
float H2Config_Deadzone_A_X = 26.0f;
float H2Config_Deadzone_A_Y = 26.0f;
float H2Config_Deadzone_Radial = 1.0f;
float H2Config_crosshair_offset = 0.138f;
bool H2Config_disable_ingame_keyboard = false;
bool H2Config_hide_ingame_chat = false;
bool H2Config_xDelay = true;
bool H2Config_hiresfix = false;
bool H2Config_shader_lod_max = false;
bool H2Config_light_suppressor = false;
bool H2Config_d3dex = false;
bool H2Config_voice_chat = false;
char H2Config_dedi_server_name[XUSER_NAME_SIZE] = { "" };
char H2Config_dedi_server_playlist[256] = { "" };
int H2Config_additional_pcr_time = 25;
bool H2Config_debug_log = false;
int H2Config_debug_log_level = 2;
bool H2Config_debug_log_console = false;
char H2Config_login_identifier[255] = { "" };
char H2Config_login_password[255] = { "" };
int H2Config_minimum_player_start = 0;
char H2Config_team_bit_flags_str[] = "1-1-1-1-1-1-1-1";
bool H2Config_team_flag_array[8];
short H2Config_team_bit_flags = 0xFF;
char H2Config_stats_authkey[32 + 1] = { "" };
bool H2Config_vip_lock = false;
bool H2Config_even_shuffle_teams = false;
bool H2Config_koth_random = true;
H2Config_Experimental_Rendering_Mode H2Config_experimental_fps = _rendering_mode_none;
bool H2Config_anti_cheat_enabled = true;

float H2Config_crosshair_scale = 1.0f;
float H2Config_raw_mouse_scale = 25.0f;

e_override_texture_resolution H2Config_Override_Shadows;
e_override_texture_resolution H2Config_Override_Water;

ControllerInput::CustomControllerLayout H2Config_CustomLayout;

bool H2Config_upnp_enable = true;
bool H2Config_melee_fix = true;
bool H2Config_no_events = false;
bool H2Config_spooky_boy = true;
#ifndef NDEBUG
int H2Config_forced_event = 0;
#endif

int H2Config_hotkeyIdHelp = VK_F2;
int H2Config_hotkeyIdAlignWindow = VK_F7;
int H2Config_hotkeyIdWindowMode = VK_F8;
int H2Config_hotkeyIdToggleHideIngameChat = VK_F9;
int H2Config_hotkeyIdGuide = VK_HOME;
int H2Config_hotkeyIdConsole = VK_F10;

bool ownsConfigFile = false;
bool H2Config_isConfigFileAppDataLocal = false;

void SaveH2Config() {
	addDebugText("Saving H2Configuration File...");

	if (!H2IsDediServer) {
		extern int current_language_main;
		extern int current_language_sub;
		H2Config_language.code_main = current_language_main;
		H2Config_language.code_variant = current_language_sub;
	}

	wchar_t fileConfigPath[1024];
	if (FlagFilePathConfig) {
		swprintf(fileConfigPath, ARRAYSIZE(fileConfigPath), FlagFilePathConfig);
	}
	else if (H2Portable || !H2Config_isConfigFileAppDataLocal) {
		swprintf(fileConfigPath, ARRAYSIZE(fileConfigPath), H2ConfigJsonFilenames[H2IsDediServer], H2ProcessFilePath, _Shell::GetInstanceId());
	}
	else {
		swprintf(fileConfigPath, ARRAYSIZE(fileConfigPath), H2ConfigJsonFilenames[H2IsDediServer], H2AppDataLocal, _Shell::GetInstanceId());
	}

	addDebugText(L"Saving config: \"%ws\"", fileConfigPath);

	json_config json(fileConfigPath);

	auto rc = json.load();

	if (rc < 0) {
		addDebugText("json.load() failed with error: %d while trying to read configuration file!", (int)rc);
	}
	else
	{
		json["cartographer"].set("h2portable", H2Portable);
		json["cartographer"].set("base_port", H2Config_base_port);
		json["cartographer"].set("upnp", H2Config_upnp_enable);
		json["cartographer"].set("enable_xdelay", H2Config_xDelay);
		json["cartographer"].set("debug_log", H2Config_debug_log);
		json["cartographer"].set("debug_log_level", H2Config_debug_log_level);
		json["cartographer"].set("debug_log_console", H2Config_debug_log_console);
		json["cartographer"].set("language_label_capture", H2Config_custom_labels_capture_missing);
		json["cartographer"].set("discord_enable", H2Config_discord_enable);
		std::string lang_str(std::to_string(H2Config_language.code_main) + "x" + std::to_string(H2Config_language.code_variant));
		json["cartographer"].set("language_code", lang_str);

		if (!H2IsDediServer)
		{
			json["game"].set("skip_intro", H2Config_skip_intro);
			json["game"].set("melee_fix", H2Config_melee_fix);
			json["game"].set("no_events", H2Config_no_events);
			json["game"].set("skeleton_biped", H2Config_spooky_boy);

			json["game"]["video"].set("fps_limit", H2Config_fps_limit);
			json["game"]["video"].set("static_lod_scale", H2Config_static_lod_state);
			json["game"]["video"].set("field_of_view", H2Config_field_of_view);
			json["game"]["video"].set("vehicle_field_of_view", H2Config_vehicle_field_of_view);
			json["game"]["video"].set("static_fp_fov", H2Config_static_first_person);
			json["game"]["video"].set("experimental_rendering", (int)H2Config_experimental_fps);
			json["game"]["video"].set("refresh_rate", H2Config_refresh_rate);
			json["game"]["video"].set("shader_lod_max", H2Config_shader_lod_max);
			json["game"]["video"].set("light_suppressor", H2Config_light_suppressor);
			json["game"]["video"].set("hires_fix", H2Config_hiresfix);
			json["game"]["video"].set("d3dex", H2Config_d3dex);
			json["game"]["video"].set("override_shadows", (int)H2Config_Override_Shadows);
			json["game"]["video"].set("override_water", (int)H2Config_Override_Water);

			json["game"]["hud"].set("crosshair_offset", H2Config_crosshair_offset);
			json["game"]["hud"].set("crosshair_scale", H2Config_crosshair_scale);
			json["game"]["hud"].set("hide_ingame_chat", H2Config_hide_ingame_chat);

			json["game"]["input"].set("raw_mouse_input", H2Config_raw_input);
			json["game"]["input"].set("mouse_raw_scale", H2Config_raw_mouse_scale);
			json["game"]["input"].set("mouse_uniform_sens", H2Config_mouse_uniform);
			json["game"]["input"].set("disalbe_ingame_keyboard", H2Config_disable_ingame_keyboard);
			json["game"]["input"].set("hotkey_help", H2Config_hotkeyIdHelp);
			json["game"]["input"].set("hotkey_align_window", H2Config_hotkeyIdAlignWindow);
			json["game"]["input"].set("hotkey_window_mode", H2Config_hotkeyIdWindowMode);
			json["game"]["input"].set("hotkey_hide_ingame_chat", H2Config_hotkeyIdToggleHideIngameChat);
			json["game"]["input"].set("hotkey_guide", H2Config_hotkeyIdGuide);
			json["game"]["input"].set("hotkey_console", H2Config_hotkeyIdConsole);
			json["game"]["input"].set("controller_sens", H2Config_controller_sens);
			json["game"]["input"].set("contoller_modern", H2Config_controller_modern);
			json["game"]["input"].set("deadzone_type", (int)H2Config_Controller_Deadzone);
			json["game"]["input"].set("deadzone_axial_x", H2Config_Deadzone_A_X);
			json["game"]["input"].set("deadzone_axial_y", H2Config_Deadzone_A_Y);
			json["game"]["input"].set("deadzone_radial", H2Config_Deadzone_Radial);
			json["game"]["input"].set("controller_layout", H2Config_CustomLayout.ToString());
		}
		if (H2IsDediServer)
		{
			json["server"].set("server_name", H2Config_dedi_server_name);
			json["server"].set("server_playlist", H2Config_dedi_server_playlist);
			json["server"].set("login_identifier", H2Config_login_identifier);
			json["server"].set("login_password", H2Config_login_password);
			json["server"].set("additional_pcr_time", H2Config_additional_pcr_time);
			json["server"].set("minimum_player_start", H2Config_minimum_player_start);
			json["server"].set("vip_lock", H2Config_vip_lock);
			json["server"].set("shuffle_even_teams", H2Config_even_shuffle_teams);
			json["server"].set("koth_random", H2Config_koth_random);
			json["server"].set("enable_anti_cheat", H2Config_anti_cheat_enabled);
			json["server"].set("teams_enabled_bit_flags", H2Config_team_bit_flags_str);
		}
#ifndef NDEBUG
		json["development"].set("forced_event", H2Config_forced_event);
#endif
		json.save();
	}

	addDebugText("End saving H2Configuration file.");
}

void ReadH2Config() {
	addDebugText("Reading H2Configuration file...");

	int readInstanceIdFile = _Shell::GetInstanceId();
	wchar_t local[1024];
	wcscpy_s(local, ARRAYSIZE(local), H2AppDataLocal);
	H2Config_isConfigFileAppDataLocal = false;

	errno_t err = 0;
	FILE* fileConfig = nullptr;
	wchar_t fileConfigPath[1024];

	if (FlagFilePathConfig) {
		wcscpy_s(fileConfigPath, ARRAYSIZE(fileConfigPath), FlagFilePathConfig);
		addDebugText(L"Reading flag config: \"%ws\"", fileConfigPath);
		err = _wfopen_s(&fileConfig, fileConfigPath, L"rb");
	}
	else {
		do {
			wchar_t* checkFilePath = H2ProcessFilePath;
			if (H2Config_isConfigFileAppDataLocal) {
				checkFilePath = local;
			}

			swprintf(fileConfigPath, ARRAYSIZE(fileConfigPath), H2ConfigJsonFilenames[H2IsDediServer], checkFilePath, readInstanceIdFile);
			addDebugText(L"Reading config: \"%ws\"", fileConfigPath);
			err = _wfopen_s(&fileConfig, fileConfigPath, L"rb");

			if (err) {
				addDebugText("H2Configuration file does not exist, error code: 0x%x", err);
			}
			H2Config_isConfigFileAppDataLocal = !H2Config_isConfigFileAppDataLocal;
			if (err && !H2Config_isConfigFileAppDataLocal) {
				--readInstanceIdFile;
			}
		} while (err && readInstanceIdFile > 0);
		H2Config_isConfigFileAppDataLocal = !H2Config_isConfigFileAppDataLocal;
	}

	if (err) {
		addDebugText("ERROR: No H2Configuration files could be found!");
		CMForce_Update = true;
		H2Config_isConfigFileAppDataLocal = true;
	}
	else {
		ownsConfigFile = (readInstanceIdFile == _Shell::GetInstanceId());

		if (!H2IsDediServer) {
			extern int current_language_main;
			extern int current_language_sub;
			H2Config_language.code_main = current_language_main;
			H2Config_language.code_variant = current_language_sub;
		}
		fclose(fileConfig);

		json_config json(fileConfigPath);

		auto rc = json.load();
		if (rc < 0)
		{
			addDebugText("json.load() failed with error: %d while trying to read configuration file!", (int)rc);
		}
		else
		{
			json["cartographer"].get("h2portable", &H2Portable, H2Portable);
			json["cartographer"].get("base_port", &H2Config_base_port, H2Config_base_port);
			json["cartographer"].get("upnp", &H2Config_upnp_enable, true);
			json["cartographer"].get("enable_xdelay", &H2Config_xDelay, H2Config_xDelay);
			json["cartographer"].get("debug_log", &H2Config_debug_log, H2Config_debug_log);
			json["cartographer"].get("debug_log_level", &H2Config_debug_log_level, H2Config_debug_log_level);
			json["cartographer"].get("debug_log_console", &H2Config_debug_log_console, H2Config_debug_log_console);
			json["cartographer"].get("language_label_capture", &H2Config_custom_labels_capture_missing, H2Config_custom_labels_capture_missing);
			json["cartographer"].get("discord_enable", &H2Config_disable_ingame_keyboard, H2Config_discord_enable);

			auto language_code = json["cartographer"].get<std::string>("language_code", "-1x0");
			if (!language_code.empty())
			{
				auto delim_offset = language_code.find("x");
				if (delim_offset != std::string::npos)
				{
					std::string code_main_substr = language_code.substr(0, delim_offset);
					std::string code_variant_substr = language_code.substr(delim_offset + 1, language_code.size());
					H2Config_language.code_main = stol(code_main_substr);
					H2Config_language.code_variant = stol(code_variant_substr);
				}
			}
			if (!H2IsDediServer)
			{
				json["game"].get("skip_intro", &H2Config_skip_intro, H2Config_skip_intro);
				json["game"].get("melee_fix", &H2Config_melee_fix, H2Config_melee_fix);
				json["game"].get("no_events", &H2Config_no_events, H2Config_no_events);
				json["game"].get("skeleton_biped", &H2Config_spooky_boy, H2Config_spooky_boy);

				json["game"]["video"].get("fps_limit", &H2Config_fps_limit, H2Config_fps_limit);
				json["game"]["video"].get("static_lod_scale", &H2Config_static_lod_state, H2Config_static_lod_state);
				json["game"]["video"].get("field_of_view", &H2Config_field_of_view, H2Config_field_of_view);
				json["game"]["video"].get("vehicle_field_of_view", &H2Config_vehicle_field_of_view, H2Config_vehicle_field_of_view);
				json["game"]["video"].get("static_fp_fov", &H2Config_static_first_person, false);

				switch (json["game"]["video"].get<int>("experimental_rendering", 0))
				{
				default:
					//Incase any of the old rendering modes were used for a higher fps, set it back to 60.
					H2Config_fps_limit = 60;
				case 0:
					H2Config_experimental_fps = _rendering_mode_none;
					break;
				case 1:
					H2Config_experimental_fps = _rendering_mode_original_game_frame_limit;
					break;
				}

				json["game"]["video"].get("refresh_rate", &H2Config_refresh_rate, H2Config_refresh_rate);
				json["game"]["video"].get("shader_lod_max", &H2Config_shader_lod_max, H2Config_shader_lod_max);
				json["game"]["video"].get("light_suppressor", &H2Config_light_suppressor, H2Config_light_suppressor);
				json["game"]["video"].get("hires_fix", &H2Config_hiresfix, H2Config_hiresfix);
				json["game"]["video"].get("d3dex", &H2Config_d3dex, H2Config_d3dex);

				switch (json["game"]["video"].get<int>("override_shadows", 1))
				{
				case 0:
					H2Config_Override_Shadows = e_override_texture_resolution::tex_low;
					break;
				default:
				case 1:
					H2Config_Override_Shadows = e_override_texture_resolution::tex_default;
					break;
				case 2:
					H2Config_Override_Shadows = e_override_texture_resolution::tex_high;
					break;
				case 3:
					H2Config_Override_Shadows = e_override_texture_resolution::tex_ultra;
					break;
				}
				switch (json["game"]["video"].get<int>("override_water", 1))
				{
				case 0:
					H2Config_Override_Water = e_override_texture_resolution::tex_low;
					break;
				default:
				case 1:
					H2Config_Override_Water = e_override_texture_resolution::tex_default;
					break;
				case 2:
					H2Config_Override_Water = e_override_texture_resolution::tex_high;
					break;
				case 3:
					H2Config_Override_Water = e_override_texture_resolution::tex_ultra;
					break;
				}

				json["game"]["hud"].get("crosshair_offset", &H2Config_crosshair_offset, 0.138f);
				json["game"]["hud"].get("crosshair_scale", &H2Config_crosshair_scale, 1.f);
				json["game"]["hud"].get("hide_ingame_chat", &H2Config_hide_ingame_chat, H2Config_hide_ingame_chat);

				json["game"]["input"].get("raw_mouse_input", &H2Config_raw_input, H2Config_raw_input);
				json["game"]["input"].get("mouse_raw_scale", &H2Config_raw_mouse_scale, H2Config_raw_mouse_scale);
				json["game"]["input"].get("mouse_uniform_sens", &H2Config_mouse_uniform, H2Config_mouse_uniform);
				json["game"]["input"].get("disable_ingame_keyboard", &H2Config_disable_ingame_keyboard, H2Config_disable_ingame_keyboard);
				json["game"]["input"].get("hotkey_help", &H2Config_hotkeyIdHelp, H2Config_hotkeyIdHelp);
				json["game"]["input"].get("hotkey_align_window", &H2Config_hotkeyIdAlignWindow, H2Config_hotkeyIdAlignWindow);
				json["game"]["input"].get("hotkey_window_mode", &H2Config_hotkeyIdWindowMode, H2Config_hotkeyIdWindowMode);
				json["game"]["input"].get("hotkey_hide_ingame_chat", &H2Config_hotkeyIdToggleHideIngameChat, H2Config_hotkeyIdToggleHideIngameChat);
				json["game"]["input"].get("hotkey_guide", &H2Config_hotkeyIdGuide, H2Config_hotkeyIdGuide);
				json["game"]["input"].get("hotkey_console", &H2Config_hotkeyIdConsole, H2Config_hotkeyIdConsole);
				json["game"]["input"].get("controller_sens", &H2Config_controller_sens, H2Config_controller_sens);
				json["game"]["input"].get("controller_modern", &H2Config_controller_modern, H2Config_controller_modern);

				switch (json["game"]["input"].get<int>("deadzone_type", 0))
				{
				default:
				case 0:
					H2Config_Controller_Deadzone = Axial;
					break;
				case 1:
					H2Config_Controller_Deadzone = Radial;
					break;
				case 2:
					H2Config_Controller_Deadzone = Both;
					break;
				}

				json["game"]["input"].get("deadzone_axial_x", &H2Config_Deadzone_A_X, H2Config_Deadzone_A_X);
				json["game"]["input"].get("deadzone_axial_y", &H2Config_Deadzone_A_Y, H2Config_Deadzone_A_Y);
				json["game"]["input"].get("deadzone_radial", &H2Config_Deadzone_Radial, H2Config_Deadzone_Radial);
				H2Config_CustomLayout.FromString(json["game"]["input"].get<std::string>("controller_layout", "1-2-4-8-16-32-64-128-256-512-4096-8192-16384-32768"));
			}

			if (H2IsDediServer)
			{
				auto server_name = json["server"].get<const char*>("server_name", "Halo 2 Server");
				auto server_playlist = json["server"].get<const char*>("server_playlist", "");
				auto login_identifier = json["server"].get<const char*>("login_identifier", "");
				auto login_password = json["server"].get<const char*>("login_password", "");
				if (server_name)
					strncpy(H2Config_dedi_server_name, server_name, XUSER_MAX_NAME_LENGTH);
				if (server_playlist)
					strncpy(H2Config_dedi_server_playlist, server_playlist, sizeof(H2Config_dedi_server_playlist));
				if (login_identifier)
					strncpy(H2Config_login_identifier, login_identifier, sizeof(H2Config_login_identifier));
				if (login_password)
					strncpy(H2Config_login_password, login_password, sizeof(H2Config_login_password));

				json["server"].get("additional_pcr_time", &H2Config_additional_pcr_time, H2Config_additional_pcr_time);
				json["server"].get("minimum_player_start", &H2Config_minimum_player_start, H2Config_minimum_player_start);
				json["server"].get("vip_lock", &H2Config_vip_lock, H2Config_vip_lock);
				json["server"].get("shuffle_even_teams", &H2Config_even_shuffle_teams, H2Config_even_shuffle_teams);
				json["server"].get("koth_random", &H2Config_koth_random, H2Config_koth_random);
				json["server"].get("enable_anti_cheat", &H2Config_anti_cheat_enabled, H2Config_anti_cheat_enabled);

				auto team_bit_mask = json["server"].get<std::string>("teams_enabled_bit_flags", H2Config_team_bit_flags_str);
				if (!team_bit_mask.empty())
				{
					strncpy_s(H2Config_team_bit_flags_str, sizeof(H2Config_team_bit_flags_str), team_bit_mask.c_str(), 15);
					H2Config_team_bit_flags = 0;
					memset(H2Config_team_flag_array, 0, sizeof(H2Config_team_flag_array));

					size_t true_bit_value_count = std::count(team_bit_mask.begin(), team_bit_mask.end(), '1');
					size_t false_bit_value_count = std::count(team_bit_mask.begin(), team_bit_mask.end(), '0');

					const char team_bit_to_find[] = "01";
					size_t occurance_offset;
					occurance_offset = team_bit_mask.find_first_of(team_bit_to_find, 0);

					// TODO move to function
					// validate first
					if (true_bit_value_count + false_bit_value_count == 8
						&& occurance_offset != std::string::npos)
					{
						// then loop
						for (int i = 0; i < 8; i++)
						{
							if (team_bit_mask.at(occurance_offset) == '1') // check if the team is enabled
							{
								H2Config_team_bit_flags |= FLAG(i); // if so, enable the flag
								H2Config_team_flag_array[i] = true;
							}

							occurance_offset = team_bit_mask.find_first_of(team_bit_to_find, occurance_offset + 1);
							if (occurance_offset == std::string::npos)
								break;
						}
					}
				}
			}
#ifndef NDEBUG
			json["development"].get("forced_event", &H2Config_forced_event, H2Config_forced_event);
#endif

			LOG_INFO_GAME("ASDF");
		}

		if (!ownsConfigFile) {
			if (H2Config_base_port < 64000 + 1)
				H2Config_base_port += 1000;
			else if (H2Config_base_port < 65535 - 10 + 1)
				H2Config_base_port += 10;
		}
	}

	addDebugText("End reading H2Configuration file.");
}

void UpgradeConfig()
{
	addDebugText("Checking upgrade status of H2Configuration file...");

	int readInstanceIdFile = _Shell::GetInstanceId();
	wchar_t local[1024];
	wcscpy_s(local, ARRAYSIZE(local), H2AppDataLocal);
	H2Config_isConfigFileAppDataLocal = false;

	errno_t err = 0;
	FILE* fileConfig = nullptr;
	wchar_t fileConfigPath[1024];
	wchar_t jsonPath[1024];

	if (FlagFilePathConfig) {
		wcscpy_s(fileConfigPath, ARRAYSIZE(fileConfigPath), FlagFilePathConfig);
		addDebugText(L"Reading flag config: \"%ws\"", fileConfigPath);
		err = _wfopen_s(&fileConfig, fileConfigPath, L"rb");
	}
	else {
		do {
			wchar_t* checkFilePath = H2ProcessFilePath;
			if (H2Config_isConfigFileAppDataLocal) {
				checkFilePath = local;
			}
			swprintf(fileConfigPath, ARRAYSIZE(fileConfigPath), H2ConfigFilenames[H2IsDediServer], checkFilePath, readInstanceIdFile);
			swprintf(jsonPath, ARRAYSIZE(jsonPath), H2ConfigJsonFilenames[H2IsDediServer], checkFilePath, readInstanceIdFile);
			addDebugText(L"Reading config: \"%ws\"", fileConfigPath);
			err = _wfopen_s(&fileConfig, fileConfigPath, L"rb");

			if (err) {
				addDebugText("H2Configuration file does not exist, error code: 0x%x", err);
			}
			H2Config_isConfigFileAppDataLocal = !H2Config_isConfigFileAppDataLocal;
			if (err && !H2Config_isConfigFileAppDataLocal) {
				--readInstanceIdFile;
			}
		} while (err && readInstanceIdFile > 0);
		H2Config_isConfigFileAppDataLocal = !H2Config_isConfigFileAppDataLocal;
	}
	if (err) {
		addDebugText("ERROR: No H2Configuration files could be found!");
		CMForce_Update = true;
		H2Config_isConfigFileAppDataLocal = true;
	}
	else
	{
		ownsConfigFile = (readInstanceIdFile == _Shell::GetInstanceId());

		if (!H2IsDediServer) {
			extern int current_language_main;
			extern int current_language_sub;
			H2Config_language.code_main = current_language_main;
			H2Config_language.code_variant = current_language_sub;
		}

		CSimpleIniA ini;
		ini.SetUnicode();
		SI_Error rc = ini.LoadFile(fileConfig);
		if (rc < 0)
		{
			addDebugText("ini.LoadFile() failed with error: %d while trying to read configuration file!", (int)rc);
		}
		else
		{
			bool upgrade_check = ini.GetBoolValue(H2ConfigVersionSection.c_str(), "json_upgraded", false);
			if (!upgrade_check)
			{
				addDebugText("ini file has not been upgraded.. upgrading now");
				json_config json(jsonPath);
				json.load();
				json["cartographer"].set("h2portable", ini.GetBoolValue(H2ConfigVersionSection.c_str(), "h2portable", false));
				json["cartographer"].set("base_port", ini.GetLongValue(H2ConfigVersionSection.c_str(), "base_port", H2Config_base_port));
				json["cartographer"].set("upnp", ini.GetBoolValue(H2ConfigVersionSection.c_str(), "upnp", true));
				json["cartographer"].set("debug_log", ini.GetBoolValue(H2ConfigVersionSection.c_str(), "debug_log", H2Config_debug_log));
				json["cartographer"].set("debug_log_level", ini.GetLongValue(H2ConfigVersionSection.c_str(), "debug_log_level", H2Config_debug_log_level));
				json["cartographer"].set("debug_log_console", ini.GetBoolValue(H2ConfigVersionSection.c_str(), "debug_log_console", H2Config_debug_log_console));
				//json["cartographer"].set("wan_ip", ini.GetValue(H2ConfigVersionSection.c_str(), "wan_ip"));
				//json["cartographer"].set("lan_ip", ini.GetValue(H2ConfigVersionSection.c_str(), "lan_ip"));
				json["cartographer"].set("discord_enable", ini.GetBoolValue(H2ConfigVersionSection.c_str(), "discord_enable", H2Config_discord_enable));
				json["cartographer"].set("language_label_capture", ini.GetBoolValue(H2ConfigVersionSection.c_str(), "language_label_capture", H2Config_custom_labels_capture_missing));
				json["cartographer"].set("enable_xdelay", ini.GetBoolValue(H2ConfigVersionSection.c_str(), "enable_xdelay", H2Config_xDelay));
				json["cartographer"].set("language_code", ini.GetValue(H2ConfigVersionSection.c_str(), "language_code", "-1x0"));

				json["game"].set("skip_intro", ini.GetBoolValue(H2ConfigVersionSection.c_str(), "skip_intro", H2Config_skip_intro));
				json["game"].set("melee_fix", ini.GetBoolValue(H2ConfigVersionSection.c_str(), "melee_fix", H2Config_melee_fix));
				json["game"].set("no_events", ini.GetBoolValue(H2ConfigVersionSection.c_str(), "no_events", H2Config_no_events));
				json["game"].set("skeleton_biped", ini.GetBoolValue(H2ConfigVersionSection.c_str(), "skeleton_biped", H2Config_spooky_boy));


				json["game"]["video"].set("fps_limit", ini.GetLongValue(H2ConfigVersionSection.c_str(), "fps_limit", H2Config_fps_limit));
				json["game"]["video"].set("static_lod_scale", ini.GetLongValue(H2ConfigVersionSection.c_str(), "static_lod_state", H2Config_static_lod_state));
				json["game"]["video"].set("field_of_view", ini.GetLongValue(H2ConfigVersionSection.c_str(), "field_of_view", H2Config_field_of_view));
				json["game"]["video"].set("vehicle_field_of_view", ini.GetLongValue(H2ConfigVersionSection.c_str(), "vehicle_field_of_view", H2Config_vehicle_field_of_view));
				json["game"]["video"].set("static_fp_fov", ini.GetBoolValue(H2ConfigVersionSection.c_str(), "static_fp_fov", false));
				json["game"]["video"].set("experimental_rendering", std::stoi(ini.GetValue(H2ConfigVersionSection.c_str(), "experimental_rendering", "0")));
				json["game"]["video"].set("refresh_rate", ini.GetLongValue(H2ConfigVersionSection.c_str(), "refresh_rate", H2Config_refresh_rate));
				json["game"]["video"].set("shader_lod_max", ini.GetBoolValue(H2ConfigVersionSection.c_str(), "shader_lod_max", H2Config_shader_lod_max));
				json["game"]["video"].set("light_suppressor", ini.GetBoolValue(H2ConfigVersionSection.c_str(), "light_suppressor", H2Config_light_suppressor));
				json["game"]["video"].set("hires_fix", ini.GetBoolValue(H2ConfigVersionSection.c_str(), "hires_fix", H2Config_hiresfix));
				json["game"]["video"].set("d3dex", ini.GetBoolValue(H2ConfigVersionSection.c_str(), "d3dex", H2Config_d3dex));
				json["game"]["video"].set("override_shadows", std::stoi(ini.GetValue(H2ConfigVersionSection.c_str(), "override_shadows", "1")));
				json["game"]["video"].set("override_water", std::stoi(ini.GetValue(H2ConfigVersionSection.c_str(), "override_water", "1")));


				std::string crosshair_offset_str(ini.GetValue(H2ConfigVersionSection.c_str(), "crosshair_offset", "NaN"));
				if (crosshair_offset_str != "NaN")
					json["game"]["hud"].set("crosshair_offset", std::stof(crosshair_offset_str));
				else
					json["game"]["hud"].set("crosshair_offset", 0.138f);

				std::string crosshair_scale_str(ini.GetValue(H2ConfigVersionSection.c_str(), "crosshair_scale", "NaN"));
				if (crosshair_scale_str != "NaN")
					json["game"]["hud"].set("crosshair_scale", std::stof(crosshair_scale_str));
				else
					json["game"]["hud"].set("crosshair_scale", 1.0f);

				json["game"]["hud"].set("hide_ingame_chat", ini.GetBoolValue(H2ConfigVersionSection.c_str(), "hide_ingame_chat", H2Config_hide_ingame_chat));


				std::string raw_mouse_scale_str(ini.GetValue(H2ConfigVersionSection.c_str(), "mouse_raw_scale", "25"));
				std::string mouse_sens_str(ini.GetValue(H2ConfigVersionSection.c_str(), "mouse_sens", "0"));
				json["game"]["input"].set("raw_mouse_input", ini.GetBoolValue(H2ConfigVersionSection.c_str(), "raw_mouse_input", H2Config_raw_input));
				json["game"]["input"].set("mouse_raw_scale", std::stof(raw_mouse_scale_str));
				json["game"]["input"].set("mouse_uniform_sens", ini.GetBoolValue(H2ConfigVersionSection.c_str(), "mouse_uniform_sens", H2Config_mouse_uniform));
				json["game"]["input"].set("mouse_sens", std::stof(mouse_sens_str));
				json["game"]["input"].set("disable_ingame_keyboard", ini.GetBoolValue(H2ConfigVersionSection.c_str(), "disable_ingame_keyboard", H2Config_disable_ingame_keyboard));
				json["game"]["input"].set("hotkey_help", ini.GetLongValue(H2ConfigVersionSection.c_str(), "hotkey_help", H2Config_hotkeyIdHelp));
				json["game"]["input"].set("hotkey_align_window", ini.GetLongValue(H2ConfigVersionSection.c_str(), "hotkey_align_window", H2Config_hotkeyIdAlignWindow));
				json["game"]["input"].set("hotkey_window_mode", ini.GetLongValue(H2ConfigVersionSection.c_str(), "hotkey_window_mode", H2Config_hotkeyIdWindowMode));
				json["game"]["input"].set("hotkey_hide_ingame_chat", ini.GetLongValue(H2ConfigVersionSection.c_str(), "hotkey_hide_ingame_chat", H2Config_hotkeyIdToggleHideIngameChat));
				json["game"]["input"].set("hotkey_guide", ini.GetLongValue(H2ConfigVersionSection.c_str(), "hotkey_guide", H2Config_hotkeyIdGuide));
				json["game"]["input"].set("hotkey_console", ini.GetLongValue(H2ConfigVersionSection.c_str(), "hotkey_console", H2Config_hotkeyIdConsole));


				std::string controller_sens_str(ini.GetValue(H2ConfigVersionSection.c_str(), "controller_sens", "0"));
				std::string deadzone_axial_x(ini.GetValue(H2ConfigVersionSection.c_str(), "deadzone_axial_x", "26.518"));
				std::string deadzone_axial_y(ini.GetValue(H2ConfigVersionSection.c_str(), "deadzone_axial_y", "26.518"));
				std::string deadzone_radial(ini.GetValue(H2ConfigVersionSection.c_str(), "deadzone_radial", "26.518"));
				json["game"]["input"].set("controller_sens", std::stof(controller_sens_str));
				json["game"]["input"].set("controller_modern", ini.GetBoolValue(H2ConfigVersionSection.c_str(), "controller_modern", H2Config_controller_modern));
				json["game"]["input"].set("deadzone_type", std::stoi(ini.GetValue(H2ConfigVersionSection.c_str(), "deadzone_type", "0")));
				json["game"]["input"].set("deadzone_axial_x", std::stof(deadzone_axial_x));
				json["game"]["input"].set("deadzone_axial_y", std::stof(deadzone_axial_y));
				json["game"]["input"].set("deadzone_radial", std::stof(deadzone_radial));
				json["game"]["input"].set("controller_layout", std::string(ini.GetValue(H2ConfigVersionSection.c_str(), "controller_layout", "1-2-4-8-16-32-64-128-256-512-4096-8192-16384-32768")));


				const char* server_name = ini.GetValue(H2ConfigVersionSection.c_str(), "server_name", H2Config_dedi_server_name);
				const char* server_playlist = ini.GetValue(H2ConfigVersionSection.c_str(), "server_playlist", H2Config_dedi_server_playlist);
				const char* login_identifier = ini.GetValue(H2ConfigVersionSection.c_str(), "login_identifier", H2Config_login_identifier);
				const char* login_password = ini.GetValue(H2ConfigVersionSection.c_str(), "login_password", H2Config_login_password);
				std::string team_bit_mask(ini.GetValue(H2ConfigVersionSection.c_str(), "teams_enabled_bit_flags", H2Config_team_bit_flags_str));
				if (server_name)
					strncpy(H2Config_dedi_server_name, server_name, XUSER_MAX_NAME_LENGTH);
				if (server_playlist)
					strncpy(H2Config_dedi_server_playlist, server_playlist, sizeof(H2Config_dedi_server_playlist));
				if (login_identifier)
					strncpy(H2Config_login_identifier, login_identifier, sizeof(H2Config_login_identifier));
				if (login_password)
					strncpy(H2Config_login_password, login_password, sizeof(H2Config_login_password));


				json["server"].set("server_name", H2Config_dedi_server_name);
				json["server"].set("server_playlist", H2Config_dedi_server_playlist);
				json["server"].set("additional_pcr_time", ini.GetLongValue(H2ConfigVersionSection.c_str(), "additional_pcr_time", H2Config_additional_pcr_time));
				json["server"].set("minimum_player_start", ini.GetLongValue(H2ConfigVersionSection.c_str(), "minimum_player_start", H2Config_minimum_player_start));
				json["server"].set("vip_lock", ini.GetBoolValue(H2ConfigVersionSection.c_str(), "vip_lock", H2Config_vip_lock));
				json["server"].set("shuffle_even_teams", ini.GetBoolValue(H2ConfigVersionSection.c_str(), "shuffle_even_teams", H2Config_even_shuffle_teams));
				json["server"].set("koth_random", ini.GetBoolValue(H2ConfigVersionSection.c_str(), "koth_random", H2Config_koth_random));
				json["server"].set("login_identifier", H2Config_login_identifier);
				json["server"].set("login_password", H2Config_login_password);
				json["server"].set("teams_enabled_bit_flags", team_bit_mask);
				json["server"].set("enable_anti_cheat", ini.GetBoolValue(H2ConfigVersionSection.c_str(), "enable_anti_cheat", H2Config_anti_cheat_enabled));

#ifndef NDEBUG
				json["development"].set("forced_event", ini.GetLongValue(H2ConfigVersionSection.c_str(), "forced_event", H2Config_forced_event));
#endif
				json.save();

				//ini.SetBoolValue(H2ConfigVersionSection.c_str(), "json_upgraded", true);
				//ini.SaveFile(fileConfig);
				fclose(fileConfig);
				_wfopen_s(&fileConfig, fileConfigPath, L"a+b");
				fputs("json_upgraded = true\n", fileConfig);
				fclose(fileConfig);
				addDebugText("ini file has been upgraded!");
			}
			else
			{
				addDebugText("ini file has already been upgraded.");
			}
		}
	}

}

#pragma region Config Init/Deinit
void InitH2Config() {
	H2Config_disable_ingame_keyboard = _Shell::GetInstanceId() > 1 ? true : false;
	UpgradeConfig();
	ReadH2Config();
}
void DeinitH2Config() {
	SaveH2Config();
}
#pragma endregion
