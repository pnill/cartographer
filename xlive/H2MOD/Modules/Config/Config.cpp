
#include "Config.h"

#include "H2MOD\Modules\Utils\Utils.h"
#include "H2MOD\Modules\OnScreenDebug\OnScreenDebug.h"
#include "H2MOD\Modules\Startup\Startup.h"
#include "H2MOD\Modules\AdvLobbySettings\AdvLobbySettings.h"

#include "Util\SimpleIni.h"

static void HandleFileError(int fpErrNo) {//TODO
	if (fpErrNo == EACCES || fpErrNo == EIO || fpErrNo == EPERM) {
		MessageBoxA(NULL, "Cannot write a file. Please restart Halo 2 in Administrator mode!", "Permission Error!", MB_OK);
	}
	else if (fpErrNo == ESRCH) {
		MessageBoxA(NULL, "Probably a missing folder issue if file writing related. Please restart Halo 2 in Administrator mode!", "Permission Error!", MB_OK);
	}
	else {
		char NotificationPlayerText[20];
		sprintf(NotificationPlayerText, "Error 0x%x", fpErrNo);
		addDebugText(NotificationPlayerText);
		MessageBoxA(NULL, NotificationPlayerText, "Unknown File Failure!", MB_OK);
	}
}

#pragma region Config IO
const wchar_t H2ConfigFilenames[][25] = { L"%wshalo2config%d.ini", L"%wsh2serverconfig%d.ini" };

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
unsigned long H2Config_ip_wan = inet_addr("127.0.0.1");
unsigned long H2Config_ip_lan = inet_addr("127.0.0.1");
_H2Config_language H2Config_language = { -1, 0 };
bool H2Config_custom_labels_capture_missing = false;
bool H2Config_skip_intro = false;
bool H2Config_raw_input = false;
bool H2Config_discord_enable = true;
//bool H2Config_controller_aim_assist = true;
int H2Config_fps_limit = 60;
bool H2Config_experimental_game_main_loop_patches = false;
int H2Config_static_lod_state = static_lod::disable;
int H2Config_field_of_view = 70;
int H2Config_vehicle_field_of_view = 70;
int H2Config_refresh_rate = 60;
int H2Config_mouse_sens = 0;
int H2Config_controller_sens = 0;
float H2Config_crosshair_offset = NAN;
bool H2Config_disable_ingame_keyboard = false;
bool H2Config_hide_ingame_chat = false;
bool H2Config_xDelay = true;
bool H2Config_hiresfix = false;
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
char H2Config_team_bit_flags_str[] = "1-1-1-1-1-1-1-1";
short H2Config_team_bit_flags = 0xFF;

//weapon crosshair sizes
point2d	H2Config_BATRIF = { 1 , 1 };
point2d	H2Config_SMG = { 1 , 1 };
point2d	H2Config_CRBN = { 1 , 1 };
point2d	H2Config_BEAMRIF = { 1 , 1 };
point2d	H2Config_MAG = { 1 , 1 };
point2d	H2Config_PLASRIF = { 1 , 1 };
point2d	H2Config_SHTGN = { 1 , 1 };
point2d	H2Config_SNIP = { 1 , 1 };
point2d	H2Config_SWRD = { 1 , 1 };
point2d	H2Config_ROCKLAUN = { 1 , 1 };
point2d	H2Config_PLASPI = { 1 , 1 };
point2d	H2Config_BRUTESHOT = { 1 , 1 };
point2d	H2Config_NEED = { 1 , 1 };
point2d	H2Config_SENTBEAM = { 1 , 1 };

int H2Config_hotkeyIdHelp = VK_F3;
int H2Config_hotkeyIdToggleDebug = VK_F2;
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
		swprintf(fileConfigPath, ARRAYSIZE(fileConfigPath), H2ConfigFilenames[H2IsDediServer], H2ProcessFilePath, H2GetInstanceId());
	}
	else {
		swprintf(fileConfigPath, ARRAYSIZE(fileConfigPath), H2ConfigFilenames[H2IsDediServer], H2AppDataLocal, H2GetInstanceId());
	}

	wchar_t fileConfigPathLog[1124];
	swprintf(fileConfigPathLog, 1024, L"Saving Config: \"%ws\"", fileConfigPath);
	addDebugText(fileConfigPathLog);
	FILE* fileConfig = nullptr;
	errno_t err = _wfopen_s(&fileConfig, fileConfigPath, L"wb");


	if (err != 0) {
		HandleFileError(GetLastError());
		addDebugText("ERROR: Unable to write H2Configuration File!");
	}
	else {
#pragma region Put Data To File
		CSimpleIniA ini;

		std::fstream iniStringBuffer(fileConfig);
		iniStringBuffer <<
			"#--- Halo 2 Project Cartographer Configuration File ---"
			"\n\n"
			"# h2portable Options:"
			"\n# 0 - Config files are read from executable's launch directory then AppDataLocal if missing. Will write to AppDataLocal if not read from the execution directory."
			"\n# 1 - All config files are read and written to the executable's launch directory (however will still scan and read from AppDataLocal if missing)."
			"\n\n"
			"# base_port Options:"
			"\n# <1 - 65526> - The port the game binds to including any of the nine (9) afterward: UDP and/or TCP (Upper limit: 65535 - 9 = 65526)."
			"\n\n"

			"# wan_ip Options:"
			"\n# lan_ip Options:"
			"\n# This option is used for when you cannot join games hosted on the same local network due to NAT issues."
			"\n# Configuring these settings for an internal network address avoids the requirement for that host user to port forward."
			"\n# <IPv4> - External IP Address of the local / internal network user you are trying to connect to. If blank, the External IP returned from the Master Login is used."
			"\n# <IPv4> - Internal IP Address of the local / internal network user you are trying to connect to."
			"\n\n"
			;

		if (!H2IsDediServer) {
			iniStringBuffer <<
				"# language_code Options (Client):"
				"\n# <main>x<variant> - Sets the main/custom language for the game."
				"\n# --- <main> ---"
				"\n# -1 - System Default"
				"\n# 0  - English"
				"\n# 1  - Japanese"
				"\n# 2  - German"
				"\n# 3  - French"
				"\n# 4  - Spanish"
				"\n# 5  - Italian"
				"\n# 6  - Korean"
				"\n# 7  - Chinese"
				"\n# --- <variant> ---"
				"\n# 0  - Default"
				"\n\n"

				"# language_label_capture Options (Client):"
				"\n# Capture new labels not in the custom language file under the language currently in use."
				"\n# 0 - Ignore listening for unrecorded labels."
				"\n# 1 - Listen for and record any labels/strings not seen before."
				"\n\n"

				"# skip_intro Options (Client):"
				"\n# 0 - Normal Intro."
				"\n# 1 - No Intro."
				"\n\n"

				"# raw_mouse_input Options (Client):"
				"\n# 0 - Default mouse input handling (includes mouse acceleration)."
				"\n# 1 - Mouse input does not have input acceleration."
				"\n\n"

				"# discord_enable Options (Client):"
				"\n# 0 - Disables Discord Rich Presence."
				"\n# 1 - Enables Discord Rich Presence."
				"\n\n"

				/*
				"# controller_aim_assist Options (Client):"
				"\n# 0 - Disables aim assist for controllers."
				"\n# 1 - Enables aim assist for controllers."
				"\n\n"
				*/

				"# fps_limit Options (Client):"
				"\n# <uint> - 0 disables the built in frame limiter. >0 is the fps limit of the game."
				"\n\n"

				"# experimental_game_main_loop_patches Options (Client):"
				"\n# <uint> - 0 - keep game main loop patches that Hired Gun added when porting Halo 2 to PC from Xbox"
				"\n# <uint> - 1 - disable game main loop patches that Hired Gun added when porting Halo 2 to PC from Xbox"
				"\n# NOTE: enabling this will improve game/network synchronization between dedicated servers and the game"
				"\n# at the cost of having the game locked at 60 FPS (or at which tickrate the game is set at), because at unlocked FPS (or even when using external frame limiters) the game is not in sync with game ticks,"
				"\n# forcing the game to render a stale state of the game causing heavy stuttering and jagged movement because prior frame rendering no game tick has been executed."
				"\n# Enabling this will force the game to render frames only if a game tick has been executed."
				"\n\n"

				"# static_lod_state Options (Client):"
				"\n# 0 - Disables the Level of Detail level enforcement for models."
				"\n# The following describes each Level of Detail setting:"
				"\n# 1 - L1 - Very Low"
				"\n# 2 - L2 - Low"
				"\n# 3 - L3 - Medium"
				"\n# 4 - L4 - High"
				"\n# 5 - L5 - Very High"
				"\n# 6 - L6 - Cinematic"
				"\n\n"

				"# field_of_view Options (Client):"
				"\n# <uint 0 to 110> - 0 disables the built in FoV adjustment. >0 is the FoV set value."
				"\n\n"

				"# refresh_rates Options (Client):"
				"\n# <uint 0 to 240> - 0 disables the built in refresh rate adjustment. >0 is the refresh rate set value."
				"\n\n"

				"# mouse_sens Options (Client):"
				"\n# <uint 0 to inf> - 0 uses the default sensitivity."
				"\n\n"

				"# hiresfix Options (Client):"
				"\n# 0 - Disable hiresfix. User is not running the game at a resolution above 1920x1200"
				"\n# 1 - Enable hiresfix. User is running the game at a resolution above 1920x1200"
				"\n\n"

				"# d3dex Options (Client):"
				"\n# 0 - NOTE: If your game crashes on startup, consider disabling this. If it's not enabled, seek for help."
				"\n# 0 - Disable D3D9Ex version of D3D9."
				"\n# 1 - Enable D3D9Ex version of D3D9."
				"\n\n"

				"# controller_sens Options (Client):"
				"\n# <uint 0 to inf> - 0 uses the default sensitivity."
				"\n\n"

				"# crosshair_offset Options (Client):"
				"\n# <0 to 0.53> - NaN disables the built in Crosshair adjustment."
				"\n\n"

				"# disable_ingame_keyboard Options (Client):"
				"\n# 0 - Normal Game Controls."
				"\n# 1 - Disables ONLY Keyboard when in-game & allows controllers when game is not in focus."
				"\n\n"

				"# hide_ingame_chat Options (Client):"
				"\n# 0 - In-game chat is displayed normally."
				"\n# 1 - In-game chat is hidden."
				"\n\n";
		}

		iniStringBuffer <<
			"# enable_xdelay Options:"
			"\n# 0 - Non-host players cannot delay the game start countdown timer."
			"\n# 1 - Non-host players can delay the game start countdown timer (native default)."
			"\n\n";

		/*
		if (H2IsDediServer) {
			iniStringBuffer <<
				"# mp_explosion_physics Options (Server):"
				"\n# 0 - Explosions do not push players or vehicles they drive."
				"\n# 1 - Enables explosion physics for players in the game."
				"\n\n"

				"# mp_sputnik Options (Server):"
				"\n# 0 - Sputnik skull is off."
				"\n# 1 - Sputnik skull is on for all players."
				"\n\n"

				"# mp_grunt_bday_party Options (Server):"
				"\n# 0 - Grunt Birthday Party skull is off."
				"\n# 1 - Grunt Birthday Party skull is on for all players."
				"\n\n"

				"# grenade_chain_react Options (Server):"
				"\n# 0 - Grenades do not chain react in multiplayer."
				"\n# 1 - Grenades chain react in multiplayer."
				"\n\n"

				"# banshee_bomb Options (Server):"
				"\n# 0 - Players cannot use the Banshee Bomb in multiplayer."
				"\n# 1 - Players can use the Banshee Bomb in multiplayer."
				"\n\n",

				"# mp_blind Options (Server):"
				"\n# 0 - Players do not have missing HUD or First Person elements."
				"\n# 1 - Players cannot see their HUD."
				"\n# 2 - Players cannot see their First Person Model."
				"\n# 3 - Players cannot see their HUD or First Person Model."
				"\n\n"

				"# banshee_bomb Options (Server):"
				"\n# 0 - Players cannot use their Flashlight in multiplayer."
				"\n# 1 - Players can use their Flashlight in multiplayer."
				"\n\n";
		}
		*/

		iniStringBuffer <<
			"# debug_log Options:"
			"\n# 0 - Disables logging."
			"\n# 1 - Enables logging."
			"\n\n"

			"# debug_log_level Options:"
			"\n# 0 - Trace, tell me *everything*."
			"\n# 1 - Debug, give me the dirty details."
			"\n# 2 - Info, occasionally helpful information."
			"\n# 3 - Warning, what probably shouldn't be happening."
			"\n# 4 - Error, bad news only, please."
			"\n# 5 - Critical, I only want to see death and destruction."
			"\n\n"

			"# debug_log_console Options:"
			"\n# 0 - Disables console window logging."
			"\n# 1 - Enables console window logging, will display all output from all loggers."
			"\n\n";

		if (H2IsDediServer) {
			iniStringBuffer <<
				"# server_name Options (Server):"
				"\n# Sets the name of the server up to 15 characters long."
				"\n# Leave blank/empty for no effect."
				"\n\n"

				"# server_playlist Options (Server):"
				"\n# Sets the playlist of the server up to 255 characters long."
				"\n# Leave blank/empty for no effect."
				"\n\n"

				"# login_identifier Options (Server):"
				"\n# The email or username used to login to an account."
				"\n# Note: Server accounts *should not* be signed into multiple times concurrently *unless* it is all on the same computer (i.e. only exempt when running multiple server instances)."
				"\n\n"

				"# login_password Options (Server):"
				"\n# The password used to login to the defined account."
				"\n\n"

				"# additional_pcr_time Options (Server):"
				"\n# By default, 25 seconds are added to post game carnage time from the playlist setting."
				"\n# Now you have the possibility to change it to your preference."
				"\n\n"
				;
		}

		if (!H2IsDediServer) {
			iniStringBuffer <<
				"# hotkey_... Options (Client):"
				"\n# The number used is the keyboard Virtual-Key (VK) Code in base-10 integer form."
				"\n# The codes in hexadecimal (base-16) form can be found here:"
				"\n# https://msdn.microsoft.com/en-us/library/windows/desktop/dd375731(v=vs.85).aspx"
				"\n\n"

				"# crosshair_... size Options (Client):"
				"\n# 0 - Disables the crosshair width/height from being displayed."
				"\n# 1 - Default width/height size "
				"\n# Anything above 1 will change the width/height to the indicated size"
				"\n# The size can range from a minimum of 0 to a maximum of 65535"
				"\n\n";
		}

		ini.SetBoolValue(H2ConfigVersionSection.c_str(), "h2portable", H2Portable);
		ini.SetLongValue(H2ConfigVersionSection.c_str(), "base_port", H2Config_base_port);

		ini.SetValue(H2ConfigVersionSection.c_str(), "wan_ip", H2Config_str_wan);

		ini.SetValue(H2ConfigVersionSection.c_str(), "lan_ip", H2Config_str_lan);

		if (!H2IsDediServer) {
			std::string lang_str(std::to_string(H2Config_language.code_main) + "x" + std::to_string(H2Config_language.code_variant));
			ini.SetValue(H2ConfigVersionSection.c_str(), "language_code", lang_str.c_str());
		}

		if (!H2IsDediServer) {
			ini.SetBoolValue(H2ConfigVersionSection.c_str(), "language_label_capture", H2Config_custom_labels_capture_missing);

			ini.SetBoolValue(H2ConfigVersionSection.c_str(), "skip_intro", H2Config_skip_intro);

			ini.SetBoolValue(H2ConfigVersionSection.c_str(), "raw_mouse_input", H2Config_raw_input);

			ini.SetBoolValue(H2ConfigVersionSection.c_str(), "discord_enable", H2Config_discord_enable);

			//ini.SetBoolValue(H2ConfigVersionSection.c_str(), "controller_aim_assist", H2Config_controller_aim_assist);

			ini.SetLongValue(H2ConfigVersionSection.c_str(), "fps_limit", H2Config_fps_limit);

			ini.SetBoolValue(H2ConfigVersionSection.c_str(), "experimental_game_main_loop_patches", H2Config_experimental_game_main_loop_patches);

			ini.SetLongValue(H2ConfigVersionSection.c_str(), "static_lod_state", H2Config_static_lod_state);

			ini.SetLongValue(H2ConfigVersionSection.c_str(), "field_of_view", H2Config_field_of_view);

			ini.SetLongValue(H2ConfigVersionSection.c_str(), "vehicle_field_of_view", H2Config_vehicle_field_of_view);

			ini.SetLongValue(H2ConfigVersionSection.c_str(), "refresh_rate", H2Config_refresh_rate);

			ini.SetLongValue(H2ConfigVersionSection.c_str(), "mouse_sens", H2Config_mouse_sens);

			ini.SetBoolValue(H2ConfigVersionSection.c_str(), "hires_fix", H2Config_hiresfix);

			ini.SetBoolValue(H2ConfigVersionSection.c_str(), "d3dex", H2Config_d3dex);

			ini.SetLongValue(H2ConfigVersionSection.c_str(), "controller_sens", H2Config_controller_sens);

			if (FloatIsNaN(H2Config_crosshair_offset)) {
				ini.SetValue(H2ConfigVersionSection.c_str(), "crosshair_offset", "NaN");
			}
			else {
				ini.SetValue(H2ConfigVersionSection.c_str(), "crosshair_offset", std::to_string(H2Config_crosshair_offset).c_str());
			}
			ini.SetBoolValue(H2ConfigVersionSection.c_str(), "disable_ingame_keyboard", H2Config_disable_ingame_keyboard);

			ini.SetBoolValue(H2ConfigVersionSection.c_str(), "hide_ingame_chat", H2Config_hide_ingame_chat);
		}

		ini.SetBoolValue(H2ConfigVersionSection.c_str(), "enable_xdelay", H2Config_xDelay);

		/*
		if (H2IsDediServer) {
			fputs("\nmp_explosion_physics = ", fileConfig); fputs(AdvLobbySettings_mp_explosion_physics ? "1" : "0", fileConfig);

			fputs("\nmp_sputnik = ", fileConfig); fputs(AdvLobbySettings_mp_sputnik ? "1" : "0", fileConfig);

			fputs("\nmp_grunt_bday_party = ", fileConfig); fputs(AdvLobbySettings_mp_grunt_bday_party ? "1" : "0", fileConfig);

			fputs("\ngrenade_chain_react = ", fileConfig); fputs(AdvLobbySettings_grenade_chain_react ? "1" : "0", fileConfig);

			fputs("\nbanshee_bomb = ", fileConfig); fputs(AdvLobbySettings_banshee_bomb ? "1" : "0", fileConfig);

			char tmpChar[2] = { '0' + AdvLobbySettings_mp_blind, 0 };
			fputs("\nmp_blind = ", fileConfig); fputs(tmpChar, fileConfig);

			fputs("\nflashlight = ", fileConfig); fputs(AdvLobbySettings_flashlight ? "1" : "0", fileConfig);
		}*/

		ini.SetBoolValue(H2ConfigVersionSection.c_str(), "debug_log", H2Config_debug_log);

		ini.SetLongValue(H2ConfigVersionSection.c_str(), "debug_log_level", H2Config_debug_log_level);

		ini.SetBoolValue(H2ConfigVersionSection.c_str(), "debug_log_console", H2Config_debug_log_console);

		if (H2IsDediServer) {
			ini.SetValue(H2ConfigVersionSection.c_str(), "server_name", H2Config_dedi_server_name);

			ini.SetValue(H2ConfigVersionSection.c_str(), "server_playlist", H2Config_dedi_server_playlist);

			ini.SetLongValue(H2ConfigVersionSection.c_str(), "additional_pcr_time", H2Config_additional_pcr_time);

			ini.SetValue(H2ConfigVersionSection.c_str(), "login_identifier", H2Config_login_identifier);

			ini.SetValue(H2ConfigVersionSection.c_str(), "login_password", H2Config_login_password);

			ini.SetValue(H2ConfigVersionSection.c_str(), "teams_enabled_bit_flags", H2Config_team_bit_flags_str, 
				"# teams_enabled_bit_flags (Server)"
				"\n# By default, the game reads team bitflags from the current map."
				"\n# With this option, you can enable which teams are enabled."
				"\n# Each bit corresponds to a team. Example bellow where we disable Blue, Green and Pink teams:"
				"\n# Teams:   Red  Blue  Yellow  Green  Purple  Orange  Brown  Pink"
				"\n#           |     |     |       |       |       |      |      |  "
				"\n#          \\/    \\/    \\/      \\/      \\/      \\/     \\/     \\/  "
				"\n#           1  -  0  -  1   -   0   -   1   -   1  -   1  -  0"
				"\n");
		}

		if (!H2IsDediServer) {

			ini.SetLongValue(H2ConfigVersionSection.c_str(), "hotkey_help", H2Config_hotkeyIdHelp, std::string("# " + GetVKeyCodeString(H2Config_hotkeyIdHelp)).c_str());
			ini.SetLongValue(H2ConfigVersionSection.c_str(), "hotkey_toggle_debug", H2Config_hotkeyIdToggleDebug, std::string("# " + GetVKeyCodeString(H2Config_hotkeyIdToggleDebug)).c_str());
			ini.SetLongValue(H2ConfigVersionSection.c_str(), "hotkey_align_window", H2Config_hotkeyIdAlignWindow, std::string("# " + GetVKeyCodeString(H2Config_hotkeyIdAlignWindow)).c_str());
			ini.SetLongValue(H2ConfigVersionSection.c_str(), "hotkey_window_mode", H2Config_hotkeyIdWindowMode, std::string("# " + GetVKeyCodeString(H2Config_hotkeyIdWindowMode)).c_str());
			ini.SetLongValue(H2ConfigVersionSection.c_str(), "hotkey_hide_ingame_chat", H2Config_hotkeyIdToggleHideIngameChat, std::string("# " + GetVKeyCodeString(H2Config_hotkeyIdToggleHideIngameChat)).c_str());
			ini.SetLongValue(H2ConfigVersionSection.c_str(), "hotkey_guide", H2Config_hotkeyIdGuide, std::string("# " + GetVKeyCodeString(H2Config_hotkeyIdGuide)).c_str());
			ini.SetLongValue(H2ConfigVersionSection.c_str(), "hotkey_console", H2Config_hotkeyIdConsole, std::string("# " + GetVKeyCodeString(H2Config_hotkeyIdConsole)).c_str());

			ini.SetLongValue(H2ConfigVersionSection.c_str(), "\ncrosshair_battle_rifle_width", H2Config_BATRIF.x);
			ini.SetLongValue(H2ConfigVersionSection.c_str(), "crosshair_battle_rifle_height", H2Config_BATRIF.y);
			ini.SetLongValue(H2ConfigVersionSection.c_str(), "crosshair_smg_width",	H2Config_SMG.x);
			ini.SetLongValue(H2ConfigVersionSection.c_str(), "crosshair_smg_height", H2Config_SMG.y);
			ini.SetLongValue(H2ConfigVersionSection.c_str(), "crosshair_carbine_width", H2Config_CRBN.x);
			ini.SetLongValue(H2ConfigVersionSection.c_str(), "crosshair_carbine_height", H2Config_CRBN.y);
			ini.SetLongValue(H2ConfigVersionSection.c_str(), "crosshair_beam_rifle_width", H2Config_BEAMRIF.x);
			ini.SetLongValue(H2ConfigVersionSection.c_str(), "crosshair_beam_rifle_height", H2Config_BEAMRIF.y);
			ini.SetLongValue(H2ConfigVersionSection.c_str(), "crosshair_magnum_width", H2Config_MAG.x);
			ini.SetLongValue(H2ConfigVersionSection.c_str(), "crosshair_magnum_height", H2Config_MAG.y);
			ini.SetLongValue(H2ConfigVersionSection.c_str(), "crosshair_plasma_rifle_width", H2Config_PLASRIF.x);
			ini.SetLongValue(H2ConfigVersionSection.c_str(), "crosshair_plasma_rifle_height", H2Config_PLASRIF.y);
			ini.SetLongValue(H2ConfigVersionSection.c_str(), "crosshair_shotgun_width", H2Config_SHTGN.x);
			ini.SetLongValue(H2ConfigVersionSection.c_str(), "crosshair_shotgun_height", H2Config_SHTGN.y);
			ini.SetLongValue(H2ConfigVersionSection.c_str(), "crosshair_sniper_width", H2Config_SNIP.x);
			ini.SetLongValue(H2ConfigVersionSection.c_str(), "crosshair_sniper_height", H2Config_SNIP.y);
			ini.SetLongValue(H2ConfigVersionSection.c_str(), "crosshair_sword_width", H2Config_SWRD.x);
			ini.SetLongValue(H2ConfigVersionSection.c_str(), "crosshair_sword_height", H2Config_SWRD.y);
			ini.SetLongValue(H2ConfigVersionSection.c_str(), "crosshair_rocket_launcher_width", H2Config_ROCKLAUN.x);
			ini.SetLongValue(H2ConfigVersionSection.c_str(), "crosshair_rocket_launcher_height", H2Config_ROCKLAUN.y);
			ini.SetLongValue(H2ConfigVersionSection.c_str(), "crosshair_plasma_pistol_width", H2Config_PLASPI.x);
			ini.SetLongValue(H2ConfigVersionSection.c_str(), "crosshair_plasma_pistol_height", H2Config_PLASPI.y);
			ini.SetLongValue(H2ConfigVersionSection.c_str(), "crosshair_bruteshot_width", H2Config_BRUTESHOT.x);
			ini.SetLongValue(H2ConfigVersionSection.c_str(), "crosshair_bruteshot_height", H2Config_BRUTESHOT.y);
			ini.SetLongValue(H2ConfigVersionSection.c_str(), "crosshair_needler_width", H2Config_NEED.x);
			ini.SetLongValue(H2ConfigVersionSection.c_str(), "crosshair_needler_height", H2Config_NEED.y);
			ini.SetLongValue(H2ConfigVersionSection.c_str(), "crosshair_sentinal_beam_width", H2Config_SENTBEAM.x);
			ini.SetLongValue(H2ConfigVersionSection.c_str(), "crosshair_sentinal_beam_height", H2Config_SENTBEAM.y);
		}

		ini.SaveFile(fileConfig);

		fputs("\n", fileConfig);
#pragma endregion
		fclose(fileConfig);
	}

	addDebugText("End Saving H2Configuration File.");
}

#pragma endregion

void ReadH2Config() {
	addDebugText("Reading H2Configuration File...");

	int readInstanceIdFile = H2GetInstanceId();
	wchar_t local[1024];
	swprintf(local, ARRAYSIZE(local), L"%ws", H2AppDataLocal);
	H2Config_isConfigFileAppDataLocal = false;

	errno_t err = 0;
	FILE* fileConfig = nullptr;
	wchar_t fileConfigPath[1024];
	wchar_t fileConfigPathLog[1124];

	if (FlagFilePathConfig) {
		swprintf(fileConfigPath, ARRAYSIZE(fileConfigPath), FlagFilePathConfig);
		swprintf(fileConfigPathLog, 1124, L"Reading Flag Config: \"%ws\"", fileConfigPath);
		addDebugText(fileConfigPathLog);
		err = _wfopen_s(&fileConfig, fileConfigPath, L"rb");
	}
	else {
		do {
			wchar_t* checkFilePath = H2ProcessFilePath;
			if (H2Config_isConfigFileAppDataLocal) {
				checkFilePath = local;
			}
			swprintf(fileConfigPath, ARRAYSIZE(fileConfigPath), H2ConfigFilenames[H2IsDediServer], checkFilePath, readInstanceIdFile);
			swprintf(fileConfigPathLog, 1124, L"Reading Config: \"%ws\"", fileConfigPath);
			addDebugText(fileConfigPathLog);
			err = _wfopen_s(&fileConfig, fileConfigPath, L"rb");

			if (err) {
				addDebugText("H2Configuration File does not exist.");
			}
			H2Config_isConfigFileAppDataLocal = !H2Config_isConfigFileAppDataLocal;
			if (err && !H2Config_isConfigFileAppDataLocal) {
				--readInstanceIdFile;
			}
		} while (err && readInstanceIdFile > 0);
		H2Config_isConfigFileAppDataLocal = !H2Config_isConfigFileAppDataLocal;
	}

	if (err) {
		addDebugText("ERROR: No H2Configuration Files Could Be Found!");
		H2Config_isConfigFileAppDataLocal = true;
	}
	else {
		ownsConfigFile = (readInstanceIdFile == H2GetInstanceId());

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
			addDebugText(std::string("ini.LoadFile() failed with error: " + std::to_string(rc) + "while trying to read configuration file!").c_str());
		}
		else
		{
			// global
			H2Portable = ini.GetBoolValue(H2ConfigVersionSection.c_str(), "h2portable", false);
			H2Config_base_port = ini.GetLongValue(H2ConfigVersionSection.c_str(), "base_port", H2Config_base_port);

			H2Config_xDelay = ini.GetBoolValue(H2ConfigVersionSection.c_str(), "enable_xdelay", H2Config_xDelay);

			H2Config_debug_log = ini.GetBoolValue(H2ConfigVersionSection.c_str(), "debug_log", H2Config_debug_log);
			H2Config_debug_log_level = ini.GetLongValue(H2ConfigVersionSection.c_str(), "debug_log_level", H2Config_debug_log_level);
			H2Config_debug_log_console = ini.GetBoolValue(H2ConfigVersionSection.c_str(), "debug_log_console", H2Config_debug_log_console);

			const char* ip_wan = ini.GetValue(H2ConfigVersionSection.c_str(), "wan_ip");
			if (ip_wan)
			{
				strncpy(H2Config_str_wan, ip_wan, 15);
				H2Config_ip_lan = inet_addr(H2Config_str_wan);
			}

			const char* ip_lan = ini.GetValue(H2ConfigVersionSection.c_str(), "lan_ip");
			if (ip_lan)
			{
				strncpy(H2Config_str_lan, ip_lan, 15);
				H2Config_ip_lan = inet_addr(H2Config_str_lan);
			}

			// client only
			if (!H2IsDediServer)
			{
				std::string language_code(ini.GetValue(H2ConfigVersionSection.c_str(), "language_code", "-1x0"));
				if (!language_code.empty())
				{
					size_t delimiter_offset = language_code.find("x");
					if (delimiter_offset != std::string::npos)
					{
						std::string code_main_substr = language_code.substr(0, delimiter_offset);
						std::string code_variant_substr = language_code.substr(delimiter_offset + 1, language_code.size());

						H2Config_language.code_main = stol(code_main_substr);
						H2Config_language.code_variant = stol(code_variant_substr);
					}
				}

				H2Config_custom_labels_capture_missing = ini.GetBoolValue(H2ConfigVersionSection.c_str(), "language_label_capture", H2Config_custom_labels_capture_missing);
				H2Config_skip_intro = ini.GetBoolValue(H2ConfigVersionSection.c_str(), "skip_intro", H2Config_skip_intro);
				H2Config_raw_input = ini.GetBoolValue(H2ConfigVersionSection.c_str(), "raw_mouse_input", H2Config_raw_input);
				H2Config_discord_enable = ini.GetBoolValue(H2ConfigVersionSection.c_str(), "discord_enable", H2Config_discord_enable);
				H2Config_fps_limit = ini.GetLongValue(H2ConfigVersionSection.c_str(), "fps_limit", H2Config_fps_limit);
				H2Config_experimental_game_main_loop_patches = ini.GetBoolValue(H2ConfigVersionSection.c_str(), "experimental_game_main_loop_patches", H2Config_experimental_game_main_loop_patches);
				H2Config_static_lod_state = ini.GetLongValue(H2ConfigVersionSection.c_str(), "static_lod_state", H2Config_static_lod_state);

				H2Config_field_of_view = ini.GetLongValue(H2ConfigVersionSection.c_str(), "field_of_view", H2Config_field_of_view);
				H2Config_vehicle_field_of_view = ini.GetLongValue(H2ConfigVersionSection.c_str(), "vehicle_field_of_view", H2Config_vehicle_field_of_view);

				std::string crosshair_offset_str(ini.GetValue(H2ConfigVersionSection.c_str(), "crosshair_offset", "NaN"));
				if (crosshair_offset_str != "NaN")
					H2Config_crosshair_offset = std::stof(crosshair_offset_str);
				else
					H2Config_crosshair_offset = NAN;

				H2Config_refresh_rate = ini.GetLongValue(H2ConfigVersionSection.c_str(), "refresh_rate", H2Config_refresh_rate);
				H2Config_mouse_sens = ini.GetLongValue(H2ConfigVersionSection.c_str(), "mouse_sens", H2Config_mouse_sens);
				H2Config_controller_sens = ini.GetLongValue(H2ConfigVersionSection.c_str(), "controller_sens", H2Config_controller_sens);
				H2Config_hiresfix = ini.GetBoolValue(H2ConfigVersionSection.c_str(), "hires_fix", H2Config_hiresfix);
				H2Config_d3dex = ini.GetBoolValue(H2ConfigVersionSection.c_str(), "d3dex", H2Config_d3dex);
				H2Config_disable_ingame_keyboard = ini.GetBoolValue(H2ConfigVersionSection.c_str(), "disable_ingame_keyboard", H2Config_disable_ingame_keyboard);
				H2Config_hide_ingame_chat = ini.GetBoolValue(H2ConfigVersionSection.c_str(), "hide_ingame_chat", H2Config_hide_ingame_chat);
			
				H2Config_hotkeyIdHelp = ini.GetLongValue(H2ConfigVersionSection.c_str(), "hotkey_help", H2Config_hotkeyIdHelp);
				H2Config_hotkeyIdToggleDebug = ini.GetLongValue(H2ConfigVersionSection.c_str(), "hotkey_toggle_debug", H2Config_hotkeyIdToggleDebug);
				H2Config_hotkeyIdAlignWindow = ini.GetLongValue(H2ConfigVersionSection.c_str(), "hotkey_align_window", H2Config_hotkeyIdAlignWindow);
				H2Config_hotkeyIdWindowMode = ini.GetLongValue(H2ConfigVersionSection.c_str(), "hotkey_window_mode", H2Config_hotkeyIdWindowMode);
				H2Config_hotkeyIdToggleHideIngameChat = ini.GetLongValue(H2ConfigVersionSection.c_str(), "hotkey_hide_ingame_chat", H2Config_hotkeyIdToggleHideIngameChat);
				H2Config_hotkeyIdGuide = ini.GetLongValue(H2ConfigVersionSection.c_str(), "hotkey_guide", H2Config_hotkeyIdGuide);
				H2Config_hotkeyIdConsole = ini.GetLongValue(H2ConfigVersionSection.c_str(), "hotkey_console", H2Config_hotkeyIdConsole);
			
				H2Config_BATRIF.x = ini.GetLongValue(H2ConfigVersionSection.c_str(), "crosshair_battle_rifle_width", 1);
				H2Config_BATRIF.y = ini.GetLongValue(H2ConfigVersionSection.c_str(), "crosshair_battle_rifle_height", 1);
				H2Config_SMG.x = ini.GetLongValue(H2ConfigVersionSection.c_str(), "crosshair_smg_width", 1);
				H2Config_SMG.y = ini.GetLongValue(H2ConfigVersionSection.c_str(), "crosshair_smg_height", 1);
				H2Config_CRBN.x = ini.GetLongValue(H2ConfigVersionSection.c_str(), "crosshair_carbine_width", 1);
				H2Config_CRBN.y = ini.GetLongValue(H2ConfigVersionSection.c_str(), "crosshair_carbine_height", 1);
				H2Config_BEAMRIF.x = ini.GetLongValue(H2ConfigVersionSection.c_str(), "crosshair_beam_rifle_width", 1);
				H2Config_BEAMRIF.y = ini.GetLongValue(H2ConfigVersionSection.c_str(), "crosshair_beam_rifle_height", 1);
				H2Config_MAG.x = ini.GetLongValue(H2ConfigVersionSection.c_str(), "crosshair_magnum_width", 1);
				H2Config_MAG.y = ini.GetLongValue(H2ConfigVersionSection.c_str(), "crosshair_magnum_height", 1);
				H2Config_PLASRIF.x = ini.GetLongValue(H2ConfigVersionSection.c_str(), "crosshair_plasma_rifle_width", 1);
				H2Config_PLASRIF.y = ini.GetLongValue(H2ConfigVersionSection.c_str(), "crosshair_plasma_rifle_height", 1);
				H2Config_SHTGN.x = ini.GetLongValue(H2ConfigVersionSection.c_str(), "crosshair_shotgun_width", 1);
				H2Config_SHTGN.y = ini.GetLongValue(H2ConfigVersionSection.c_str(), "crosshair_shotgun_height", 1);
				H2Config_SNIP.x = ini.GetLongValue(H2ConfigVersionSection.c_str(), "crosshair_sniper_width", 1);
				H2Config_SNIP.y = ini.GetLongValue(H2ConfigVersionSection.c_str(), "crosshair_sniper_height", 1);
				H2Config_SWRD.x = ini.GetLongValue(H2ConfigVersionSection.c_str(), "crosshair_sword_width", 1);
				H2Config_SWRD.y = ini.GetLongValue(H2ConfigVersionSection.c_str(), "crosshair_sword_height", 1);
				H2Config_ROCKLAUN.x = ini.GetLongValue(H2ConfigVersionSection.c_str(), "crosshair_rocket_launcher_width", 1);
				H2Config_ROCKLAUN.y = ini.GetLongValue(H2ConfigVersionSection.c_str(), "crosshair_rocket_launcher_height", 1);
				H2Config_PLASPI.x = ini.GetLongValue(H2ConfigVersionSection.c_str(), "crosshair_plasma_pistol_width", 1);
				H2Config_PLASPI.y = ini.GetLongValue(H2ConfigVersionSection.c_str(), "crosshair_plasma_pistol_height", 1);
				H2Config_BRUTESHOT.x = ini.GetLongValue(H2ConfigVersionSection.c_str(), "crosshair_bruteshot_width", 1);
				H2Config_BRUTESHOT.y = ini.GetLongValue(H2ConfigVersionSection.c_str(), "crosshair_bruteshot_height", 1);
				H2Config_NEED.x = ini.GetLongValue(H2ConfigVersionSection.c_str(), "crosshair_needler_width", 1);
				H2Config_NEED.y = ini.GetLongValue(H2ConfigVersionSection.c_str(), "crosshair_needler_height", 1);
				H2Config_SENTBEAM.x = ini.GetLongValue(H2ConfigVersionSection.c_str(), "crosshair_sentinal_beam_width", 1);
				H2Config_SENTBEAM.y = ini.GetLongValue(H2ConfigVersionSection.c_str(), "crosshair_sentinal_beam_height", 1);
			}

			// dedicated server only
			if (H2IsDediServer)
			{
				const char* server_name = ini.GetValue(H2ConfigVersionSection.c_str(), "server_name", H2Config_dedi_server_name);
				if (server_name) {
					strncpy(H2Config_dedi_server_name, server_name, XUSER_MAX_NAME_LENGTH);
				}

				const char* server_playlist = ini.GetValue(H2ConfigVersionSection.c_str(), "server_playlist", H2Config_dedi_server_playlist);
				if (server_playlist) {
					strncpy(H2Config_dedi_server_playlist, server_playlist, sizeof(H2Config_dedi_server_playlist));
				}

				H2Config_additional_pcr_time = ini.GetLongValue(H2ConfigVersionSection.c_str(), "additional_pcr_time", H2Config_additional_pcr_time);

				const char* login_identifier = ini.GetValue(H2ConfigVersionSection.c_str(), "login_identifier", H2Config_login_identifier);
				if (login_identifier) {
					strncpy(H2Config_login_identifier, login_identifier, sizeof(H2Config_login_identifier));
				}

				const char* login_password = ini.GetValue(H2ConfigVersionSection.c_str(), "login_password", H2Config_login_password);
				if (login_password) {
					strncpy(H2Config_login_password, login_password, sizeof(H2Config_login_password));
				}

				std::string team_bit_mask(ini.GetValue(H2ConfigVersionSection.c_str(), "teams_enabled_bit_flags", H2Config_team_bit_flags_str));
				if (!team_bit_mask.empty())
				{
					strncpy_s(H2Config_team_bit_flags_str, sizeof(H2Config_team_bit_flags_str), team_bit_mask.c_str(), 15);
					size_t last_offset = 0;
					size_t occurance_offset = 0;
					H2Config_team_bit_flags = 0;
					for (int i = 0; i < 8; i++)
					{
						occurance_offset = team_bit_mask.find_first_not_of("-", last_offset);
						last_offset += 2;
						if (occurance_offset != std::string::npos 
							&& team_bit_mask.substr(occurance_offset, 1) == "1") // check if the team is enabled
						{
							H2Config_team_bit_flags |= FLAG(i); // if so, enable the flag
						}

					}
				}
			}
		}

		fclose(fileConfig);

		if (!ownsConfigFile) {
			if (H2Config_base_port < 64000 + 1)
				H2Config_base_port += 1000;
			else if (H2Config_base_port < 65535 - 10 + 1)
				H2Config_base_port += 10;
		}
	}

	addDebugText("End Reading H2Configuration File.");
}
#pragma endregion

#pragma region Config Init/Deinit
void InitH2Config() {
	H2Config_disable_ingame_keyboard = H2GetInstanceId() > 1 ? true : false;
	ReadH2Config();
}
void DeinitH2Config() {
	SaveH2Config();
}
#pragma endregion
