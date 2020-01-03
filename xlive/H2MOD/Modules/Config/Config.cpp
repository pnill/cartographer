#include "stdafx.h"
#include "Config.h"

#include "H2MOD.h"

#include "H2MOD\Modules\Utils\Utils.h"
#include "H2MOD\Modules\OnScreenDebug\OnScreenDebug.h"
#include "H2MOD\Modules\Startup\Startup.h"
#include "H2MOD\Modules\AdvLobbySettings\AdvLobbySettings.h"

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
const wchar_t H2ConfigFilenames[2][24] = { L"%wshalo2config%d.ini", L"%wsh2serverconfig%d.ini" };

const char H2ConfigVersionStr[] = "[H2ConfigurationVersion:%hs]";
const char H2ConfigVersionReadStr[] = "[H2ConfigurationVersion:%[^]]]";
char H2ConfigVersion[] = "1";

static const int bufferIncSize = 10;
static int oldConfigBufferI = 0;
static int oldConfigBufferLen = 0;
static char** oldConfigBuffer = 0;
static int badConfigBufferI = 0;
static int badConfigBufferLen = 0;
static char** badConfigBuffer = 0;

static void oldConfigBufferFree() {
	if (oldConfigBuffer) {
		for (int i = 0; i < oldConfigBufferI; i++) {
			free(oldConfigBuffer[i]);
		}
		oldConfigBufferI = 0;
		oldConfigBufferLen = 0;
		free(oldConfigBuffer);
		oldConfigBuffer = 0;
	}
}
static void badConfigBufferFree() {
	if (badConfigBuffer) {
		for (int i = 0; i < badConfigBufferI; i++) {
			free(badConfigBuffer[i]);
		}
		badConfigBufferI = 0;
		badConfigBufferLen = 0;
		free(badConfigBuffer);
		badConfigBuffer = 0;
	}
}


unsigned long H2Config_master_ip = inet_addr("149.56.81.89");
unsigned short H2Config_master_port_login = 27020;
unsigned short H2Config_master_port_relay = 1001;

//config variables
bool H2Portable = false;//TODO

unsigned short H2Config_base_port = 2000;
char H2Config_str_wan[16] = { "" };
char H2Config_str_lan[16] = { "" };
unsigned long H2Config_ip_wan = inet_addr("127.0.0.1");
unsigned long H2Config_ip_lan = inet_addr("127.0.0.1");

int H2Config_language_code_main = -1;
int H2Config_language_code_variant = 0;
bool H2Config_custom_labels_capture_missing = false;
bool H2Config_skip_intro = false;
bool H2Config_raw_input = false;
bool H2Config_discord_enable = true;
//bool H2Config_controller_aim_assist = true;
int H2Config_fps_limit = 60;
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
//bool H2Config_hitmarker_sound = false;
bool H2Config_voice_chat = false;
char H2Config_dedi_server_name[32] = { "" };
char H2Config_dedi_server_playlist[256] = { "" };
int H2Config_additional_pcr_time = 25;
bool H2Config_debug_log = false;
int H2Config_debug_log_level = 2;
bool H2Config_debug_log_console = false;
char H2Config_login_identifier[255] = { "" };
char H2Config_login_password[255] = { "" };

//weapon crosshair sizes
int H2Config_BATRIF_WIDTH = 1;
int H2Config_BATRIF_HEIGHT = 1;
int H2Config_SMG_WIDTH = 1;
int H2Config_SMG_HEIGHT = 1;
int H2Config_CRBN_WIDTH = 1;
int H2Config_CRBN_HEIGHT = 1;
int H2Config_BEAMRIF_WIDTH = 1;
int H2Config_BEAMRIF_HEIGHT = 1;
int H2Config_MAG_WIDTH = 1;
int H2Config_MAG_HEIGHT = 1;
int H2Config_PLASRIF_WIDTH = 1;
int H2Config_PLASRIF_HEIGHT = 1;
int H2Config_SHTGN_WIDTH = 1;
int H2Config_SHTGN_HEIGHT = 1;
int H2Config_SNIP_WIDTH = 1;
int H2Config_SNIP_HEIGHT = 1;
int H2Config_SWRD_WIDTH = 1;
int H2Config_SWRD_HEIGHT = 1;
int H2Config_ROCKLAUN_WIDTH = 1;
int H2Config_ROCKLAUN_HEIGHT = 1;
int H2Config_PLASPI_WIDTH = 1;
int H2Config_PLASPI_HEIGHT = 1;
int H2Config_BRUTESHOT_WIDTH = 1;
int H2Config_BRUTESHOT_HEIGHT = 1;
int H2Config_NEED_WIDTH = 1;
int H2Config_NEED_HEIGHT = 1;
int H2Config_SENTBEAM_WIDTH = 1;
int H2Config_SENTBEAM_HEIGHT = 1;

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
		H2Config_language_code_main = current_language_main;
		H2Config_language_code_variant = current_language_sub;
	}

	wchar_t fileConfigPath[1024];
	if (FlagFilePathConfig) {
		swprintf(fileConfigPath, 1024, FlagFilePathConfig);
	}
	else if (H2Portable || !H2Config_isConfigFileAppDataLocal) {
		swprintf(fileConfigPath, 1024, H2ConfigFilenames[H2IsDediServer], H2ProcessFilePath, H2GetInstanceId());
	}
	else {
		swprintf(fileConfigPath, 1024, H2ConfigFilenames[H2IsDediServer], H2AppDataLocal, H2GetInstanceId());
	}
	
	wchar_t fileConfigPathLog[1124];
	swprintf(fileConfigPathLog, 1024, L"Saving Config: \"%ws\"", fileConfigPath);
	addDebugText(fileConfigPathLog);
	FILE* fileConfig = nullptr; 
	errno_t err =  _wfopen_s(&fileConfig, fileConfigPath, L"wb");

	if (err != 0) {
		HandleFileError(GetLastError());
		addDebugText("ERROR: Unable to write H2Configuration File!");
	}
	else {
#pragma region Put Data To File
		fputs("#--- Halo 2 Project Cartographer Configuration File ---", fileConfig);
		fputs("\n\n", fileConfig);
		
		fputs("# h2portable Options:", fileConfig);
		fputs("\n# 0 - Config files are read from executable's launch directory then AppDataLocal if missing. Will write to AppDataLocal if not read from the execution directory.", fileConfig);
		fputs("\n# 1 - All config files are read and written to the executable's launch directory (however will still scan and read from AppDataLocal if missing).", fileConfig);
		fputs("\n\n", fileConfig);

		fputs("# base_port Options:", fileConfig);
		fputs("\n# <1 - 65526> - The port the game binds to including any of the nine (9) afterward: UDP and/or TCP (Upper limit: 65535 - 9 = 65526).", fileConfig);
		fputs("\n\n", fileConfig);

		fputs("# wan_ip Options:", fileConfig);
		fputs("\n# lan_ip Options:", fileConfig);
		fputs("\n# This option is used for when you cannot join games hosted on the same local network due to NAT issues.", fileConfig);
		fputs("\n# Configuring these settings for an internal network address avoids the requirement for that host user to port forward.", fileConfig);
		fputs("\n# <IPv4> - External IP Address of the local / internal network user you are trying to connect to. If blank, the External IP returned from the Master Login is used.", fileConfig);
		fputs("\n# <IPv4> - Internal IP Address of the local / internal network user you are trying to connect to.", fileConfig);
		fputs("\n\n", fileConfig);

		if (!H2IsDediServer) {
			fputs("# language_code Options (Client):", fileConfig);
			fputs("\n# <main>x<variant> - Sets the main/custom language for the game.", fileConfig);
			fputs("\n# --- <main> ---", fileConfig);
			fputs("\n# -1 - System Default", fileConfig);
			fputs("\n# 0  - English", fileConfig);
			fputs("\n# 1  - Japanese", fileConfig);
			fputs("\n# 2  - German", fileConfig);
			fputs("\n# 3  - French", fileConfig);
			fputs("\n# 4  - Spanish", fileConfig);
			fputs("\n# 5  - Italian", fileConfig);
			fputs("\n# 6  - Korean", fileConfig);
			fputs("\n# 7  - Chinese", fileConfig);
			fputs("\n# --- <variant> ---", fileConfig);
			fputs("\n# 0  - Default", fileConfig);
			fputs("\n\n", fileConfig);

			fputs("# language_label_capture Options (Client):", fileConfig);
			fputs("\n# Capture new labels not in the custom language file under the language currently in use.", fileConfig);
			fputs("\n# 0 - Ignore listening for unrecorded labels.", fileConfig);
			fputs("\n# 1 - Listen for and record any labels/strings not seen before.", fileConfig);
			fputs("\n\n", fileConfig);

			fputs("# skip_intro Options (Client):", fileConfig);
			fputs("\n# 0 - Normal Intro.", fileConfig);
			fputs("\n# 1 - No Intro.", fileConfig);
			fputs("\n\n", fileConfig);

			fputs("# raw_mouse_input Options (Client):", fileConfig);
			fputs("\n# 0 - Default mouse input handling (includes mouse acceleration).", fileConfig);
			fputs("\n# 1 - Mouse input does not have input acceleration.", fileConfig);
			fputs("\n\n", fileConfig);

			fputs("# discord_enable Options (Client):", fileConfig);
			fputs("\n# 0 - Disables Discord Rich Presence.", fileConfig);
			fputs("\n# 1 - Enables Discord Rich Presence.", fileConfig);
			fputs("\n\n", fileConfig);

			/*fputs("# controller_aim_assist Options (Client):", fileConfig);
			fputs("\n# 0 - Disables aim assist for controllers.", fileConfig);
			fputs("\n# 1 - Enables aim assist for controllers.", fileConfig);
			fputs("\n\n", fileConfig);*/

			fputs("# fps_limit Options (Client):", fileConfig);
			fputs("\n# <uint> - 0 disables the built in frame limiter. >0 is the fps limit of the game.", fileConfig);
			fputs("\n\n", fileConfig);

			fputs("# static_lod_state Options (Client):", fileConfig);
			fputs("\n# 0 - Disables the Level of Detail level enforcement for models.", fileConfig);
			fputs("\n# The following describes each Level of Detail setting:", fileConfig);
			fputs("\n# 1 - L1 - Very Low", fileConfig);
			fputs("\n# 2 - L2 - Low", fileConfig);
			fputs("\n# 3 - L3 - Medium", fileConfig);
			fputs("\n# 4 - L4 - High", fileConfig);
			fputs("\n# 5 - L5 - Very High", fileConfig);
			fputs("\n# 6 - L6 - Cinematic", fileConfig);
			fputs("\n\n", fileConfig);

			fputs("# field_of_view Options (Client):", fileConfig);
			fputs("\n# <uint 0 to 110> - 0 disables the built in FoV adjustment. >0 is the FoV set value.", fileConfig);
			fputs("\n\n", fileConfig);

			fputs("# refresh_rates Options (Client):", fileConfig);
			fputs("\n# <uint 0 to 240> - 0 disables the built in refresh rate adjustment. >0 is the refresh rate set value.", fileConfig);
			fputs("\n\n", fileConfig);

			fputs("# mouse_sens Options (Client):", fileConfig);
			fputs("\n# <uint 0 to inf> - 0 uses the default sensitivity.", fileConfig);
			fputs("\n\n", fileConfig);

			fputs("# hiresfix Options (Client):", fileConfig);
			fputs("\n# 0 - Disable hiresfix. User is not running the game at a resolution above 1920x1200", fileConfig);
			fputs("\n# 1 - Enable hiresfix. User is running the game at a resolution above 1920x1200", fileConfig);
			fputs("\n\n", fileConfig);

			fputs("# d3dex Options (Client):", fileConfig);
			fputs("\n# 0 - Disable d3dex.", fileConfig);
			fputs("\n# 1 - Enable d3dex.", fileConfig);
			fputs("\n\n", fileConfig);

			fputs("# controller_sens Options (Client):", fileConfig);
			fputs("\n# <uint 0 to inf> - 0 uses the default sensitivity.", fileConfig);
			fputs("\n\n", fileConfig);

			fputs("# crosshair_offset Options (Client):", fileConfig);
			fputs("\n# <0 to 0.53> - NaN disables the built in Crosshair adjustment.", fileConfig);
			fputs("\n\n", fileConfig);

			fputs("# disable_ingame_keyboard Options (Client):", fileConfig);
			fputs("\n# 0 - Normal Game Controls.", fileConfig);
			fputs("\n# 1 - Disables ONLY Keyboard when in-game & allows controllers when game is not in focus.", fileConfig);
			fputs("\n\n", fileConfig);

			fputs("# hide_ingame_chat Options (Client):", fileConfig);
			fputs("\n# 0 - In-game chat is displayed normally.", fileConfig);
			fputs("\n# 1 - In-game chat is hidden.", fileConfig);
			fputs("\n\n", fileConfig);
		}
		fputs("# enable_xdelay Options:", fileConfig);
		fputs("\n# 0 - Non-host players cannot delay the game start countdown timer.", fileConfig);
		fputs("\n# 1 - Non-host players can delay the game start countdown timer (native default).", fileConfig);
		fputs("\n\n", fileConfig);
		/*if (!H2IsDediServer) {
			fputs("# enable_hitmarker_sound Options (Client):", fileConfig);
			fputs("\n# 0 - Shooting players does not produce a hitmarker sound effect (default).", fileConfig);
			fputs("\n# 1 - Shooting players plays a hitmarker sound effect.", fileConfig);
			fputs("\n\n", fileConfig);
		}
		fputs("# voice_chat Options:", fileConfig);
		fputs("\n# 0 - Voice chat is not enabled, you cannot host voice servers or connect to them.", fileConfig);
		fputs("\n# 1 - Voice chat is enabled, you can host voice servers or connect to them (default).", fileConfig);
		fputs("\n\n", fileConfig);

		if (H2IsDediServer) {
			fputs("# mp_explosion_physics Options (Server):", fileConfig);
			fputs("\n# 0 - Explosions do not push players or vehicles they drive.", fileConfig);
			fputs("\n# 1 - Enables explosion physics for players in the game.", fileConfig);
			fputs("\n\n", fileConfig);

			fputs("# mp_sputnik Options (Server):", fileConfig);
			fputs("\n# 0 - Sputnik skull is off.", fileConfig);
			fputs("\n# 1 - Sputnik skull is on for all players.", fileConfig);
			fputs("\n\n", fileConfig);

			fputs("# mp_grunt_bday_party Options (Server):", fileConfig);
			fputs("\n# 0 - Grunt Birthday Party skull is off.", fileConfig);
			fputs("\n# 1 - Grunt Birthday Party skull is on for all players.", fileConfig);
			fputs("\n\n", fileConfig);

			fputs("# grenade_chain_react Options (Server):", fileConfig);
			fputs("\n# 0 - Grenades do not chain react in multiplayer.", fileConfig);
			fputs("\n# 1 - Grenades chain react in multiplayer.", fileConfig);
			fputs("\n\n", fileConfig);

			fputs("# banshee_bomb Options (Server):", fileConfig);
			fputs("\n# 0 - Players cannot use the Banshee Bomb in multiplayer.", fileConfig);
			fputs("\n# 1 - Players can use the Banshee Bomb in multiplayer.", fileConfig);
			fputs("\n\n", fileConfig);

			fputs("# mp_blind Options (Server):", fileConfig);
			fputs("\n# 0 - Players do not have missing HUD or First Person elements.", fileConfig);
			fputs("\n# 1 - Players cannot see their HUD.", fileConfig);
			fputs("\n# 2 - Players cannot see their First Person Model.", fileConfig);
			fputs("\n# 3 - Players cannot see their HUD or First Person Model.", fileConfig);
			fputs("\n\n", fileConfig);

			fputs("# banshee_bomb Options (Server):", fileConfig);
			fputs("\n# 0 - Players cannot use their Flashlight in multiplayer.", fileConfig);
			fputs("\n# 1 - Players can use their Flashlight in multiplayer.", fileConfig);
			fputs("\n\n", fileConfig);
		}*/

		fputs("# debug_log Options:", fileConfig);
		fputs("\n# 0 - Disables logging.", fileConfig);
		fputs("\n# 1 - Enables logging.", fileConfig);
		fputs("\n\n", fileConfig);

		fputs("# debug_log_level Options:", fileConfig);
		fputs("\n# 0 - Trace, tell me *everything*.", fileConfig);
		fputs("\n# 1 - Debug, give me the dirty details.", fileConfig);
		fputs("\n# 2 - Info, occasionally helpful information.", fileConfig);
		fputs("\n# 3 - Warning, what probably shouldn't be happening.", fileConfig);
		fputs("\n# 4 - Error, bad news only, please.", fileConfig);
		fputs("\n# 5 - Critical, I only want to see death and destruction.", fileConfig);
		fputs("\n\n", fileConfig);

		fputs("# debug_log_console Options:", fileConfig);
		fputs("\n# 0 - Disables console window logging.", fileConfig);
		fputs("\n# 1 - Enables console window logging, will display all output from all loggers.", fileConfig);
		fputs("\n\n", fileConfig);

		if (H2IsDediServer) {
			fputs("# server_name Options (Server):", fileConfig);
			fputs("\n# Sets the name of the server up to 31 characters long.", fileConfig);
			fputs("\n# Leave blank/empty for no effect.", fileConfig);
			fputs("\n\n", fileConfig);

			fputs("# server_playlist Options (Server):", fileConfig);
			fputs("\n# Sets the playlist of the server up to 255 characters long.", fileConfig);
			fputs("\n# Leave blank/empty for no effect.", fileConfig);
			fputs("\n\n", fileConfig);

			fputs("# login_identifier Options (Server):", fileConfig);
			fputs("\n# The email or username used to login to an account.", fileConfig);
			fputs("\n# Note: Server accounts *should not* be signed into multiple times concurrently *unless* it is all on the same computer (i.e. only exempt when running multiple server instances).", fileConfig);
			fputs("\n\n", fileConfig);

			fputs("# login_password Options (Server):", fileConfig);
			fputs("\n# The password used to login to the defined account.", fileConfig);
			fputs("\n\n", fileConfig);

			fputs("# additional_pcr_time Options (Server):", fileConfig);
			fputs("\n# By default, 25 seconds are added to post game carnage time from the playlist setting.", fileConfig);
			fputs("\n# Now you have the possibility to change it to your preference.", fileConfig);
			fputs("\n\n", fileConfig);
		}
		if (!H2IsDediServer) {
			fputs("# hotkey_... Options (Client):", fileConfig);
			fputs("\n# The number used is the keyboard Virtual-Key (VK) Code in base-10 integer form.", fileConfig);
			fputs("\n# The codes in hexadecimal (base-16) form can be found here:", fileConfig);
			fputs("\n# https://msdn.microsoft.com/en-us/library/windows/desktop/dd375731(v=vs.85).aspx", fileConfig);
			fputs("\n\n", fileConfig);

			fputs("# crosshair_... size Options (Client):", fileConfig);
			fputs("\n# 0 - Disables the crosshair width/height from being displayed.", fileConfig);
			fputs("\n# 1 - Default width/height size ", fileConfig);
			fputs("\n# Anything above 1 will change the width/height to the indicated size", fileConfig);
			fputs("\n# The size can range from a minimum of 0 to a maximum of 65535", fileConfig);
			fputs("\n\n", fileConfig);

		}
	
		fprintf_s(fileConfig, H2ConfigVersionStr, H2ConfigVersion);

		fprintf_s(fileConfig, "\nh2portable = %d", H2Portable);

		fprintf_s(fileConfig, "\nbase_port = %d", H2Config_base_port);

		fprintf_s(fileConfig, "\nwan_ip = %s", H2Config_str_wan);

		fprintf_s(fileConfig, "\nlan_ip = %s", H2Config_str_lan);

		if (H2IsDediServer) {
			//fputs("\nlanguage_code = -1x0", fileConfig);
		} else {
			fprintf_s(fileConfig, "\nlanguage_code = %dx%d", H2Config_language_code_main, H2Config_language_code_variant);
		}
		if (!H2IsDediServer) {
			fprintf_s(fileConfig, "\nlanguage_label_capture = %d", H2Config_custom_labels_capture_missing);

			fprintf_s(fileConfig, "\nskip_intro = %d", H2Config_skip_intro);

			fprintf_s(fileConfig, "\nraw_mouse_input = %d", H2Config_raw_input);

			fprintf_s(fileConfig, "\ndiscord_enable = %d", H2Config_discord_enable);

			//fprintf_s(fileConfig, "\ncontroller_aim_assist = %d", H2Config_controller_aim_assist);

			fprintf_s(fileConfig, "\nfps_limit = %d", H2Config_fps_limit);

			fprintf_s(fileConfig, "\nstatic_lod_state = %d", H2Config_static_lod_state);

			fprintf_s(fileConfig, "\nfield_of_view = %d", H2Config_field_of_view);

			fprintf_s(fileConfig, "\nvehicle_field_of_view = %d", H2Config_vehicle_field_of_view);

			fprintf_s(fileConfig, "\nrefresh_rate = %d", H2Config_refresh_rate);

			fprintf_s(fileConfig, "\nmouse_sens = %d", H2Config_mouse_sens);

			fprintf_s(fileConfig, "\nhiresfix = %d", H2Config_hiresfix);

			fprintf_s(fileConfig, "\nd3dex = %d", H2Config_d3dex);

			fprintf_s(fileConfig, "\ncontroller_sens = %d", H2Config_controller_sens);

			if (FloatIsNaN(H2Config_crosshair_offset)) {
				fputs("\ncrosshair_offset = NaN", fileConfig);
			}
			else {
				fprintf_s(fileConfig, "\ncrosshair_offset = %f", H2Config_crosshair_offset);
			}
			fprintf_s(fileConfig, "\ndisable_ingame_keyboard = %d", H2Config_disable_ingame_keyboard);

			fprintf_s(fileConfig, "\nhide_ingame_chat = %d", H2Config_hide_ingame_chat);

			//TODO
			//fputs("\ncustom_resolution = 0x0", fileConfig);
		}

		fprintf_s(fileConfig, "\nenable_xdelay = %d", H2Config_xDelay);

		/*if (!H2IsDediServer) {
			fputs("\nenable_hitmarker_sound = ", fileConfig); fputs(H2Config_hitmarker_sound ? "1" : "0", fileConfig);
		}
		fputs("\nvoice_chat = ", fileConfig); fputs(H2Config_voice_chat ? "1" : "0", fileConfig);

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

		fprintf_s(fileConfig, "\ndebug_log = %d", H2Config_debug_log);

		fprintf_s(fileConfig, "\ndebug_log_level = %d", H2Config_debug_log_level);

		fprintf_s(fileConfig, "\ndebug_log_console = %d", H2Config_debug_log_console);

		if (H2IsDediServer) {
			fprintf_s(fileConfig, "\nserver_name = %s", H2Config_dedi_server_name);

			fprintf_s(fileConfig, "\nserver_playlist = %s", H2Config_dedi_server_playlist);

			fprintf_s(fileConfig, "\nadditional_pcr_time = %d", H2Config_additional_pcr_time);
		}

		if (H2IsDediServer) {
			fprintf_s(fileConfig, "\nlogin_identifier = %s", H2Config_login_identifier);
			fprintf_s(fileConfig, "\nlogin_password = %s", H2Config_login_password);
		}

		if (!H2IsDediServer) {

			char hotkeyText[70];
			sprintf(hotkeyText, "\nhotkey_help = %d #", H2Config_hotkeyIdHelp);
			GetVKeyCodeString(H2Config_hotkeyIdHelp, hotkeyText + strlen(hotkeyText), 20);
			fputs(hotkeyText, fileConfig);

			sprintf(hotkeyText, "\nhotkey_toggle_debug = %d #", H2Config_hotkeyIdToggleDebug);
			GetVKeyCodeString(H2Config_hotkeyIdToggleDebug, hotkeyText + strlen(hotkeyText), 20);
			fputs(hotkeyText, fileConfig);

			sprintf(hotkeyText, "\nhotkey_align_window = %d #", H2Config_hotkeyIdAlignWindow);
			GetVKeyCodeString(H2Config_hotkeyIdAlignWindow, hotkeyText + strlen(hotkeyText), 20);
			fputs(hotkeyText, fileConfig);

			sprintf(hotkeyText, "\nhotkey_window_mode = %d #", H2Config_hotkeyIdWindowMode);
			GetVKeyCodeString(H2Config_hotkeyIdWindowMode, hotkeyText + strlen(hotkeyText), 20);
			fputs(hotkeyText, fileConfig);

			sprintf(hotkeyText, "\nhotkey_hide_ingame_chat = %d #", H2Config_hotkeyIdToggleHideIngameChat);
			GetVKeyCodeString(H2Config_hotkeyIdToggleHideIngameChat, hotkeyText + strlen(hotkeyText), 20);
			fputs(hotkeyText, fileConfig);

			sprintf(hotkeyText, "\nhotkey_guide = %d #", H2Config_hotkeyIdGuide);
			GetVKeyCodeString(H2Config_hotkeyIdGuide, hotkeyText + strlen(hotkeyText), 20);
			fputs(hotkeyText, fileConfig);

			sprintf(hotkeyText, "\nhotkey_console = %d #", H2Config_hotkeyIdConsole);
			GetVKeyCodeString(H2Config_hotkeyIdConsole, hotkeyText + strlen(hotkeyText), 20);
			fputs(hotkeyText, fileConfig);
 			fputs("\n\n", fileConfig);

			fprintf_s(fileConfig, "\ncrosshair_battle_rifle_width = %d", H2Config_BATRIF_WIDTH);
			fprintf_s(fileConfig, "\ncrosshair_battle_rifle_height = %d", H2Config_BATRIF_HEIGHT);
			fprintf_s(fileConfig, "\ncrosshair_smg_width = %d", H2Config_SMG_WIDTH);
			fprintf_s(fileConfig, "\ncrosshair_smg_height = %d", H2Config_SMG_HEIGHT);
			fprintf_s(fileConfig, "\ncrosshair_carbine_width = %d", H2Config_CRBN_WIDTH);
			fprintf_s(fileConfig, "\ncrosshair_carbine_height = %d", H2Config_CRBN_HEIGHT);
			fprintf_s(fileConfig, "\ncrosshair_beam_rifle_width = %d", H2Config_BEAMRIF_WIDTH);
			fprintf_s(fileConfig, "\ncrosshair_beam_rifle_height = %d", H2Config_BEAMRIF_HEIGHT);
			fprintf_s(fileConfig, "\ncrosshair_magnum_width = %d", H2Config_MAG_WIDTH);
			fprintf_s(fileConfig, "\ncrosshair_magnum_height = %d", H2Config_MAG_HEIGHT);
			fprintf_s(fileConfig, "\ncrosshair_plasma_rifle_width = %d", H2Config_PLASRIF_WIDTH);
			fprintf_s(fileConfig, "\ncrosshair_plasma_rifle_height = %d", H2Config_PLASRIF_HEIGHT);
			fprintf_s(fileConfig, "\ncrosshair_shotgun_width = %d", H2Config_SHTGN_WIDTH);
			fprintf_s(fileConfig, "\ncrosshair_shotgun_height = %d", H2Config_SHTGN_HEIGHT);
			fprintf_s(fileConfig, "\ncrosshair_sniper_width = %d", H2Config_SNIP_WIDTH);
			fprintf_s(fileConfig, "\ncrosshair_sniper_height = %d", H2Config_SNIP_HEIGHT);
			fprintf_s(fileConfig, "\ncrosshair_sword_width = %d", H2Config_SWRD_WIDTH);
			fprintf_s(fileConfig, "\ncrosshair_sword_height = %d", H2Config_SWRD_HEIGHT);
			fprintf_s(fileConfig, "\ncrosshair_rocket_launcher_width = %d", H2Config_ROCKLAUN_WIDTH);
			fprintf_s(fileConfig, "\ncrosshair_rocket_launcher_height = %d", H2Config_ROCKLAUN_HEIGHT);
			fprintf_s(fileConfig, "\ncrosshair_plasma_pistol_width = %d", H2Config_PLASPI_WIDTH);
			fprintf_s(fileConfig, "\ncrosshair_plasma_pistol_height = %d", H2Config_PLASPI_HEIGHT);
			fprintf_s(fileConfig, "\ncrosshair_bruteshot_width = %d", H2Config_BRUTESHOT_WIDTH);
			fprintf_s(fileConfig, "\ncrosshair_bruteshot_height = %d", H2Config_BRUTESHOT_HEIGHT);
			fprintf_s(fileConfig, "\ncrosshair_needler_width = %d", H2Config_NEED_WIDTH);
			fprintf_s(fileConfig, "\ncrosshair_needler_height = %d", H2Config_NEED_HEIGHT);
			fprintf_s(fileConfig, "\ncrosshair_sentinal_beam_width = %d", H2Config_SENTBEAM_WIDTH);
			fprintf_s(fileConfig, "\ncrosshair_sentinal_beam_height = %d", H2Config_SENTBEAM_HEIGHT);
		}

		fputs("\n", fileConfig);

		if (badConfigBuffer && badConfigBufferI > 0) {
			for (int i = 0; i < badConfigBufferI; i++) {
				fputs("\n", fileConfig);
				if (badConfigBuffer[i][0] != '#' && badConfigBuffer[i][0] != ';')
					fputs("#", fileConfig);
				fputs(badConfigBuffer[i], fileConfig);
			}
		}
		badConfigBufferFree();

		fputs("\n\n", fileConfig);

		if (oldConfigBuffer && oldConfigBufferI > 0) {
			for (int i = 0; i < oldConfigBufferI; i++) {
				fputs("\n", fileConfig);
				fputs(oldConfigBuffer[i], fileConfig);
			}
		}
		oldConfigBufferFree();

		fputs("\n", fileConfig);
#pragma endregion
		fclose(fileConfig);
	}

	addDebugText("End Saving H2Configuration File.");
}

#pragma region Established Config Variables
//if these variables read already
static bool est_h2portable = false;
static bool est_base_port = false;
static bool est_str_wan = false;
static bool est_str_lan = false;
static bool est_ip_master = false;
static bool est_language_code = false;
static bool est_language_label_capture = false;
static bool est_skip_intro = false;
static bool est_raw_input = false;
static bool est_discord_enable = false;
//static bool est_controller_aim_assist = false;
static bool est_fps_limit = false;
static bool est_static_lod_state = false;
static bool est_field_of_view = false;
static bool est_vehicle_field_of_view = false;
static bool est_refresh_rate = false;
static bool est_mouse_sens = false;
static bool est_controller_sens = false;
static bool est_hiresfix = false;
static bool est_d3dex = false;
static bool est_crosshair_offset = false;
static bool est_sens_controller = false;
static bool est_sens_mouse = false;
static bool est_disable_ingame_keyboard = false;
static bool est_hide_ingame_chat = false;
static bool est_xdelay = false;
/*static bool est_hitmarker_sound = false;
static bool est_voice_chat = false;
static bool est_als_mp_explosion_physics = false;
static bool est_als_mp_sputnik = false;
static bool est_als_mp_grunt_bday_party = false;
static bool est_als_grenade_chain_react = false;
static bool est_als_banshee_bomb = false;
static bool est_als_mp_blind = false;
static bool est_als_flashlight = false;*/
static bool est_debug_log = false;
static bool est_debug_log_level = false;
static bool est_debug_log_console = false;
static bool est_custom_resolution = false;
static bool est_server_name = false;
static bool est_additional_pcr_time = false;
static bool est_server_playlist = false;
static bool est_chatbox_commands = false;
static bool est_login_token = false;
static bool est_login_identifier = false;
static bool est_login_password = false;
static bool est_hotkey_help = false;
static bool est_hotkey_toggle_debug = false;
static bool est_hotkey_align_window = false;
static bool est_hotkey_window_mode = false;
static bool est_hotkey_hide_ingame_chat = false;
static bool est_hotkey_guide = false;
static bool est_hotkey_console = false;
static bool est_BATRIF_WIDTH = false;
static bool est_BATRIF_HEIGHT = false;
static bool est_SMG_WIDTH = false;
static bool est_SMG_HEIGHT = false;
static bool est_CRBN_WIDTH = false;
static bool est_CRBN_HEIGHT = false;
static bool est_BEAMRIF_WIDTH = false;
static bool est_BEAMRIF_HEIGHT = false;
static bool est_MAG_WIDTH = false;
static bool est_MAG_HEIGHT = false;
static bool est_PLASRIF_WIDTH = false;
static bool est_PLASRIF_HEIGHT = false;
static bool est_SHTGN_WIDTH = false;
static bool est_SHTGN_HEIGHT = false;
static bool est_SNIP_WIDTH = false;
static bool est_SNIP_HEIGHT = false;
static bool est_SWRD_WIDTH = false;
static bool est_SWRD_HEIGHT = false;
static bool est_ROCKLAUN_WIDTH = false;
static bool est_ROCKLAUN_HEIGHT = false;
static bool est_PLASPI_WIDTH = false;
static bool est_PLASPI_HEIGHT = false;
static bool est_BRUTESHOT_WIDTH = false;
static bool est_BRUTESHOT_HEIGHT = false;
static bool est_NEED_WIDTH = false;
static bool est_NEED_HEIGHT = false;
static bool est_SENTBEAM_WIDTH = false;
static bool est_SENTBEAM_HEIGHT = false;

static void est_reset_vars() {
	est_h2portable = false;
	est_base_port = false;
	est_str_wan = false;
	est_str_lan = false;
	est_ip_master = false;
	est_language_code = false;
	est_language_label_capture = false;
	est_skip_intro = false;
	est_raw_input = false;
	est_discord_enable = false;
	//est_controller_aim_assist = false;
	est_fps_limit = false;
	est_static_lod_state = false;
	est_field_of_view = false;
	est_vehicle_field_of_view = false;
	est_refresh_rate = false;
	est_mouse_sens = false;
	est_controller_sens = false;
	est_crosshair_offset = false;
	est_sens_controller = false;
	est_sens_mouse = false;
	est_hiresfix = false;
	est_d3dex = false;
	est_disable_ingame_keyboard = false;
	est_hide_ingame_chat = false;
	est_xdelay = false;
	/*est_hitmarker_sound = false;
	est_voice_chat = false;
	est_als_mp_explosion_physics = false;
	est_als_mp_sputnik = false;
	est_als_mp_grunt_bday_party = false;
	est_als_grenade_chain_react = false;
	est_als_banshee_bomb = false;
	est_als_mp_blind = false;
	est_als_flashlight = false;*/
	est_debug_log = false;
	est_debug_log_level = false;
	est_debug_log_console = false;
	est_custom_resolution = false;
	est_server_name = false;
	est_server_playlist = false;
	est_chatbox_commands = false;
	est_login_token = false;
	est_login_identifier = false;
	est_login_password = false;
	est_hotkey_help = false;
	est_hotkey_toggle_debug = false;
	est_hotkey_align_window = false;
	est_hotkey_window_mode = false;
	est_hotkey_hide_ingame_chat = false;
	est_hotkey_guide = false;
	est_hotkey_console = false;
	est_BATRIF_WIDTH = false;
	est_BATRIF_HEIGHT = false;
	est_SMG_WIDTH = false;
	est_SMG_HEIGHT = false;
	est_CRBN_WIDTH = false;
	est_CRBN_HEIGHT = false;
	est_BEAMRIF_WIDTH = false;
	est_BEAMRIF_HEIGHT = false;
	est_MAG_WIDTH = false;
	est_MAG_HEIGHT = false;
	est_PLASRIF_WIDTH = false;
	est_PLASRIF_HEIGHT = false;
	est_SHTGN_WIDTH = false;
	est_SHTGN_HEIGHT = false;
	est_SNIP_WIDTH = false;
	est_SNIP_HEIGHT = false;
	est_SWRD_WIDTH = false;
	est_SWRD_HEIGHT = false;
	est_ROCKLAUN_WIDTH = false;
	est_ROCKLAUN_HEIGHT = false;
	est_PLASPI_WIDTH = false;
	est_PLASPI_HEIGHT = false;
	est_BRUTESHOT_WIDTH = false;
	est_BRUTESHOT_HEIGHT = false;
	est_NEED_WIDTH = false;
	est_NEED_HEIGHT = false;
	est_SENTBEAM_WIDTH = false;
	est_SENTBEAM_HEIGHT = false;
}
#pragma endregion

static int interpretConfigSetting(char* fileLine, char* version, int lineNumber) {
	if (!version) {
		return 0;
	}
	else if (CmpVersions(H2ConfigVersion, version) != 0) {
		if (ownsConfigFile) {
			if (oldConfigBufferI >= oldConfigBufferLen) {
				if (!oldConfigBuffer) {
					oldConfigBuffer = (char**)malloc(sizeof(char*) * (oldConfigBufferLen += bufferIncSize));
				}
				else {
					oldConfigBuffer = (char**)realloc(oldConfigBuffer, sizeof(char*) * (oldConfigBufferLen += bufferIncSize));
				}
			}
			oldConfigBuffer[oldConfigBufferI] = fileLine;
			oldConfigBufferI++;
			return 0b10;
		}
	}
	else {
		bool unrecognised = false;
		bool duplicated = false;
		bool incorrect = false;
		bool dontSave = false;
		int fileLineLen = strlen(fileLine);
		int tempint1 = -1;
		unsigned short tempushort1 = -1;
		int tempint2 = -1;
		float tempfloat1 = NULL;
		char tempstr1[33] = { "" };
		if (fileLine[0] == '#' || fileLine[0] == ';' || fileLineLen <= 2) {
			unrecognised = true;
		}
		else if (sscanf(fileLine, "h2portable =%d", &tempint1) == 1) {
			if (est_h2portable) {
				duplicated = true;
			}
			else if (!(tempint1 == 0 || tempint1 == 1)) {
				incorrect = true;
			}
			else {
				H2Portable = (bool)tempint1;
				est_h2portable = true;
			}
		}
		else if (sscanf(fileLine, "base_port =%hu", &tempushort1) == 1) {
			if (est_base_port) {
				duplicated = true;
			}
			else if (!(tempushort1 > 0 && tempushort1 <= 0xFFFF - 9)) {
				incorrect = true;
			}
			else {
				H2Config_base_port = tempushort1;
				est_base_port = true;
			}
		}
		else if (strstr(fileLine, "wan_ip =")) {
			if (est_str_wan) {
				duplicated = true;
			}
			else {
				char* tempName = fileLine + strlen("wan_ip =");
				while (isspace(*tempName)) {
					tempName++;
				}
				snprintf(tempstr1, 16, tempName);
				for (int j = strlen(tempstr1) - 1; j > 0; j--) {
					if (isspace(tempstr1[j])) {
						tempstr1[j] = 0;
					}
					else {
						break;
					}
				}
				unsigned long resolvedAddr;
				if (strlen(tempstr1) > 0) {
					if ((resolvedAddr = inet_addr(tempstr1)) == INADDR_NONE) {
						incorrect = true;
					}
					else {
						strncpy(H2Config_str_wan, tempstr1, 16);
						H2Config_ip_wan = resolvedAddr;
						est_str_wan = true;
					}
				}
			}
		}
		else if (strstr(fileLine, "lan_ip =")) {
			if (est_str_lan) {
				duplicated = true;
			}
			else {
				char* tempName = fileLine + strlen("lan_ip =");
				while (isspace(*tempName)) {
					tempName++;
				}
				snprintf(tempstr1, 16, tempName);
				for (int j = strlen(tempstr1) - 1; j > 0; j--) {
					if (isspace(tempstr1[j])) {
						tempstr1[j] = 0;
					}
					else {
						break;
					}
				}
				unsigned long resolvedAddr;
				if (strlen(tempstr1) > 0) {
					if ((resolvedAddr = inet_addr(tempstr1)) == INADDR_NONE) {
						incorrect = true;
					}
					else {
						strncpy(H2Config_str_lan, tempstr1, 16);
						H2Config_ip_lan = resolvedAddr;
						est_str_lan = true;
					}
				}
			}
		}
		else if (sscanf(fileLine, "language_code =%dx%d", &tempint1, &tempint2) == 2) {
			if (est_language_code) {
				duplicated = true;
			}
			else if (!(tempint1 >= -1 && tempint1 <= 7 && tempint2 >= 0)) {
				incorrect = true;
			}
			else {
				H2Config_language_code_main = tempint1;
				H2Config_language_code_variant = tempint2;
				est_language_code = true;
			}
		}
		else if (!H2IsDediServer && sscanf(fileLine, "language_label_capture =%d", &tempint1) == 1) {
			if (est_language_label_capture) {
				duplicated = true;
			}
			else if (!(tempint1 == 0 || tempint1 == 1)) {
				incorrect = true;
			}
			else {
				H2Config_custom_labels_capture_missing = (bool)tempint1;
				est_language_label_capture = true;
			}
		}
		else if (!H2IsDediServer && sscanf(fileLine, "skip_intro =%d", &tempint1) == 1) {
			if (est_skip_intro) {
				duplicated = true;
			}
			else if (!(tempint1 == 0 || tempint1 == 1)) {
				incorrect = true;
			}
			else {
				H2Config_skip_intro = (bool)tempint1;
				est_skip_intro = true;
			}
		}
		else if (!H2IsDediServer && sscanf(fileLine, "raw_mouse_input =%d", &tempint1) == 1) {
			if (est_raw_input) {
				duplicated = true;
			}
			else if (!(tempint1 == 0 || tempint1 == 1)) {
				incorrect = true;
			}
			else {
				H2Config_raw_input = (bool)tempint1;
				est_raw_input = true;
			}
		}
		else if (!H2IsDediServer && sscanf(fileLine, "discord_enable =%d", &tempint1) == 1) {
			if (est_discord_enable) {
				duplicated = true;
			}
			else if (!(tempint1 == 0 || tempint1 == 1)) {
				incorrect = true;
			}
			else {
				H2Config_discord_enable = (bool)tempint1;
				est_discord_enable = true;
			}
		}
/*		else if (!H2IsDediServer && sscanf(fileLine, "controller_aim_assist =%d", &tempint1) == 1) {
			if (est_controller_aim_assist) {
				duplicated = true;
			}
			else if (!(tempint1 == 0 || tempint1 == 1)) {
				incorrect = true;
			}
			else {
				H2Config_controller_aim_assist = (bool)tempint1;
				est_controller_aim_assist = true;
			}
		}*/
		else if (!H2IsDediServer && sscanf(fileLine, "fps_limit =%d", &tempint1) == 1) {
			if (est_fps_limit) {
				duplicated = true;
			}
			else if (!(tempint1 >= 0)) {
				incorrect = true;
			}
			else {
				H2Config_fps_limit = tempint1;
				extern std::chrono::high_resolution_clock::duration desiredRenderTime;
				desiredRenderTime = std::chrono::duration_cast<std::chrono::high_resolution_clock::duration>(std::chrono::duration<double>(1.0 / (double)H2Config_fps_limit));
				est_fps_limit = true;
			}
		}
		else if (!H2IsDediServer && sscanf(fileLine, "static_lod_state =%d", &tempint1) == 1) {
			if (est_static_lod_state) {
				duplicated = true;
			}
			else if (!(tempint1 >= 0 && tempint1 <= 6)) {
				incorrect = true;
			}
			else {
				H2Config_static_lod_state = tempint1;
				est_static_lod_state = true;
			}
		}
		else if (!H2IsDediServer && sscanf(fileLine, "field_of_view =%d", &tempint1) == 1) {
			if (est_field_of_view) {
				duplicated = true;
			}
			else if (!(tempint1 >= 0)) {
				incorrect = true;
			}
			else {
				H2Config_field_of_view = tempint1;
				est_field_of_view = true;
			}
		}
		else if (!H2IsDediServer && sscanf(fileLine, "vehicle_field_of_view =%d", &tempint1) == 1) {
		if (est_vehicle_field_of_view) {
			duplicated = true;
		}
		else if (!(tempint1 >= 0)) {
			incorrect = true;
		}
		else {
			H2Config_vehicle_field_of_view = tempint1;
			est_vehicle_field_of_view = true;
		}

		}
		else if (!H2IsDediServer && sscanf(fileLine, "refresh_rate =%d", &tempint1) == 1) {
			if (est_refresh_rate) {
				duplicated = true;
			}
			else if (!(tempint1 >= 0)) {
				incorrect = true;
			}
			else {
				H2Config_refresh_rate = tempint1;
				est_refresh_rate = true;
			}
		}
		else if (!H2IsDediServer && sscanf(fileLine, "mouse_sens =%d", &tempint1) == 1) {
			if (est_mouse_sens) {
				duplicated = true;
			}
			else if (!(tempint1 >= 0)) {
				incorrect = true;
			}
			else {
				H2Config_mouse_sens = tempint1;
				est_mouse_sens = true;
			}
		}
		else if (!H2IsDediServer && sscanf(fileLine, "controller_sens =%d", &tempint1) == 1) {
			if (est_controller_sens) {
				duplicated = true;
			}
			else if (!(tempint1 >= 0)) {
				incorrect = true;
			}
			else {
				H2Config_controller_sens = tempint1;
				est_controller_sens = true;
			}
		}
		else if (!H2IsDediServer && sscanf(fileLine, "hiresfix =%d", &tempint1) == 1) {
			if (est_hiresfix) {
				duplicated = true;
			}
			else if (!(tempint1 == 0 || tempint1 == 1)) {
				incorrect = true;
			}
			else {
				H2Config_hiresfix = (bool)tempint1;
				est_hiresfix = true;
			}
		}
		else if (!H2IsDediServer && sscanf(fileLine, "d3dex =%d", &tempint1) == 1) {
			if (est_d3dex) {
				duplicated = true;
			}
			else if (!(tempint1 == 0 || tempint1 == 1)) {
				incorrect = true;
			}
			else {
				H2Config_d3dex = (bool)tempint1;
				est_d3dex = true;
			}
		}
		else if (!H2IsDediServer && sscanf(fileLine, "crosshair_offset =%f", &tempfloat1) == 1) {
			if (est_crosshair_offset) {
				duplicated = true;
			}
			else if (!(FloatIsNaN(tempfloat1) || tempfloat1 >= 0.0f || tempfloat1 <= 0.53f)) {
				incorrect = true;
			}
			else {
				H2Config_crosshair_offset = tempfloat1;
				est_crosshair_offset = true;
			}
		}
		else if (!H2IsDediServer && ownsConfigFile && sscanf(fileLine, "disable_ingame_keyboard =%d", &tempint1) == 1) {
			if (est_disable_ingame_keyboard) {
				duplicated = true;
			}
			else if (!(tempint1 == 0 || tempint1 == 1)) {
				incorrect = true;
			}
			else {
				H2Config_disable_ingame_keyboard = (bool)tempint1;
				est_disable_ingame_keyboard = true;
			}
		}
		else if (!H2IsDediServer && sscanf(fileLine, "hide_ingame_chat =%d", &tempint1) == 1) {
			if (est_hide_ingame_chat) {
				duplicated = true;
			}
			else if (!(tempint1 == 0 || tempint1 == 1)) {
				incorrect = true;
			}
			else {
				H2Config_hide_ingame_chat = (bool)tempint1;
				est_hide_ingame_chat = true;
			}
		}
		else if (!H2IsDediServer && sscanf(fileLine, "crosshair_battle_rifle_width =%d", &tempint1) == 1) {
			if (est_BATRIF_WIDTH) {
				duplicated = true;
			}
			else if (!(tempint1 >= 0 && tempint1 <= 65535)) {
				incorrect = true;
			}
			else {
				H2Config_BATRIF_WIDTH = tempint1;
				est_BATRIF_WIDTH = true;
			}
		}
		else if (!H2IsDediServer && sscanf(fileLine, "crosshair_battle_rifle_height =%d", &tempint1) == 1) {
			if (est_BATRIF_HEIGHT) {
				duplicated = true;
			}
			else if (!(tempint1 >= 0 && tempint1 <= 65535)) {
				incorrect = true;
			}
			else {
				H2Config_BATRIF_HEIGHT = tempint1;
				est_BATRIF_HEIGHT = true;
			}
		}
		else if (!H2IsDediServer && sscanf(fileLine, "crosshair_smg_width =%d", &tempint1) == 1) {
			if (est_SMG_WIDTH) {
				duplicated = true;
			}
			else if (!(tempint1 >= 0 && tempint1 <= 65535)) {
				incorrect = true;
			}
			else {
				H2Config_SMG_WIDTH = tempint1;
				est_SMG_WIDTH = true;
			}
		}
		else if (!H2IsDediServer && sscanf(fileLine, "crosshair_smg_height =%d", &tempint1) == 1) {
			if (est_SMG_HEIGHT) {
				duplicated = true;
			}
			else if (!(tempint1 >= 0 && tempint1 <= 65535)) {
				incorrect = true;
			}
			else {
				H2Config_SMG_HEIGHT = tempint1;
				est_SMG_HEIGHT = true;
			}
		}
		else if (!H2IsDediServer && sscanf(fileLine, "crosshair_carbine_width =%d", &tempint1) == 1) {
			if (est_CRBN_WIDTH) {
				duplicated = true;
			}
			else if (!(tempint1 >= 0 && tempint1 <= 65535)) {
				incorrect = true;
			}
			else {
				H2Config_CRBN_WIDTH = tempint1;
				est_CRBN_WIDTH = true;
			}
		}
		else if (!H2IsDediServer && sscanf(fileLine, "crosshair_carbine_height =%d", &tempint1) == 1) {
			if (est_CRBN_HEIGHT) {
				duplicated = true;
			}
			else if (!(tempint1 >= 0 && tempint1 <= 65535)) {
				incorrect = true;
			}
			else {
				H2Config_CRBN_HEIGHT = tempint1;
				est_CRBN_HEIGHT = true;
			}
		}
		else if (!H2IsDediServer && sscanf(fileLine, "crosshair_beam_rifle_width =%d", &tempint1) == 1) {
			if (est_BEAMRIF_WIDTH) {
				duplicated = true;
			}
			else if (!(tempint1 >= 0 && tempint1 <= 65535)) {
				incorrect = true;
			}
			else {
				H2Config_BEAMRIF_WIDTH = tempint1;
				est_BEAMRIF_WIDTH = true;
			}
		}
		else if (!H2IsDediServer && sscanf(fileLine, "crosshair_beam_rifle_height =%d", &tempint1) == 1) {
			if (est_BEAMRIF_HEIGHT) {
				duplicated = true;
			}
			else if (!(tempint1 >= 0 && tempint1 <= 65535)) {
				incorrect = true;
			}
			else {
				H2Config_BEAMRIF_HEIGHT = tempint1;
				est_BEAMRIF_HEIGHT = true;
			}
		}
		else if (!H2IsDediServer && sscanf(fileLine, "crosshair_magnum_width =%d", &tempint1) == 1) {
			if (est_MAG_WIDTH) {
				duplicated = true;
			}
			else if (!(tempint1 >= 0 && tempint1 <= 65535)) {
				incorrect = true;
			}
			else {
				H2Config_MAG_WIDTH = tempint1;
				est_MAG_WIDTH = true;
			}
		}
		else if (!H2IsDediServer && sscanf(fileLine, "crosshair_magnum_height =%d", &tempint1) == 1) {
			if (est_MAG_HEIGHT) {
				duplicated = true;
			}
			else if (!(tempint1 >= 0 && tempint1 <= 65535)) {
				incorrect = true;
			}
			else {
				H2Config_MAG_HEIGHT = tempint1;
				est_MAG_HEIGHT = true;
			}
		}
		else if (!H2IsDediServer && sscanf(fileLine, "crosshair_plasma_rifle_width =%d", &tempint1) == 1) {
			if (est_PLASRIF_WIDTH) {
				duplicated = true;
			}
			else if (!(tempint1 >= 0 && tempint1 <= 65535)) {
				incorrect = true;
			}
			else {
				H2Config_PLASRIF_WIDTH = tempint1;
				est_PLASRIF_WIDTH = true;
			}
		}
		else if (!H2IsDediServer && sscanf(fileLine, "crosshair_plasma_rifle_height =%d", &tempint1) == 1) {
			if (est_PLASRIF_HEIGHT) {
				duplicated = true;
			}
			else if (!(tempint1 >= 0 && tempint1 <= 65535)) {
				incorrect = true;
			}
			else {
				H2Config_PLASRIF_HEIGHT = tempint1;
				est_PLASRIF_HEIGHT = true;
			}
		}
		else if (!H2IsDediServer && sscanf(fileLine, "crosshair_shotgun_width =%d", &tempint1) == 1) {
			if (est_SHTGN_WIDTH) {
				duplicated = true;
			}
			else if (!(tempint1 >= 0 && tempint1 <= 65535)) {
				incorrect = true;
			}
			else {
				H2Config_SHTGN_WIDTH = tempint1;
				est_SHTGN_WIDTH = true;
			}
		}
		else if (!H2IsDediServer && sscanf(fileLine, "crosshair_shotgun_height =%d", &tempint1) == 1) {
			if (est_SHTGN_HEIGHT) {
				duplicated = true;
			}
			else if (!(tempint1 >= 0 && tempint1 <= 65535)) {
				incorrect = true;
			}
			else {
				H2Config_SHTGN_HEIGHT = tempint1;
				est_SHTGN_HEIGHT = true;
			}
		}
		else if (!H2IsDediServer && sscanf(fileLine, "crosshair_sniper_width =%d", &tempint1) == 1) {
			if (est_SNIP_WIDTH) {
				duplicated = true;
			}
			else if (!(tempint1 >= 0 && tempint1 <= 65535)) {
				incorrect = true;
			}
			else {
				H2Config_SNIP_WIDTH = tempint1;
				est_SNIP_WIDTH = true;
			}
		}
		else if (!H2IsDediServer && sscanf(fileLine, "crosshair_sniper_height =%d", &tempint1) == 1) {
			if (est_SNIP_HEIGHT) {
				duplicated = true;
			}
			else if (!(tempint1 >= 0 && tempint1 <= 65535)) {
				incorrect = true;
			}
			else {
				H2Config_SNIP_HEIGHT = tempint1;
				est_SNIP_HEIGHT = true;
			}
		}
		else if (!H2IsDediServer && sscanf(fileLine, "crosshair_sword_width =%d", &tempint1) == 1) {
			if (est_SWRD_WIDTH) {
				duplicated = true;
			}
			else if (!(tempint1 >= 0 && tempint1 <= 65535)) {
				incorrect = true;
			}
			else {
				H2Config_SWRD_WIDTH = tempint1;
				est_SWRD_WIDTH = true;
			}
		}
		else if (!H2IsDediServer && sscanf(fileLine, "crosshair_sword_height =%d", &tempint1) == 1) {
			if (est_SWRD_HEIGHT) {
				duplicated = true;
			}
			else if (!(tempint1 >= 0 && tempint1 <= 65535)) {
				incorrect = true;
			}
			else {
				H2Config_SWRD_HEIGHT = tempint1;
				est_SWRD_HEIGHT = true;
			}
		}
		else if (!H2IsDediServer && sscanf(fileLine, "crosshair_rocket_launcher_width =%d", &tempint1) == 1) {
			if (est_ROCKLAUN_WIDTH) {
				duplicated = true;
			}
			else if (!(tempint1 >= 0 && tempint1 <= 65535)) {
				incorrect = true;
			}
			else {
				H2Config_ROCKLAUN_WIDTH = tempint1;
				est_ROCKLAUN_WIDTH = true;
			}
		}
		else if (!H2IsDediServer && sscanf(fileLine, "crosshair_rocket_launcher_height =%d", &tempint1) == 1) {
			if (est_ROCKLAUN_HEIGHT) {
				duplicated = true;
			}
			else if (!(tempint1 >= 0 && tempint1 <= 65535)) {
				incorrect = true;
			}
			else {
				H2Config_ROCKLAUN_HEIGHT = tempint1;
				est_ROCKLAUN_HEIGHT = true;
			}
		}
		else if (!H2IsDediServer && sscanf(fileLine, "crosshair_plasma_pistol_width =%d", &tempint1) == 1) {
			if (est_PLASPI_WIDTH) {
				duplicated = true;
			}
			else if (!(tempint1 >= 0 && tempint1 <= 65535)) {
				incorrect = true;
			}
			else {
				H2Config_PLASPI_WIDTH = tempint1;
				est_PLASPI_WIDTH = true;
			}
		}
		else if (!H2IsDediServer && sscanf(fileLine, "crosshair_plasma_pistol_height =%d", &tempint1) == 1) {
			if (est_PLASPI_HEIGHT) {
				duplicated = true;
			}
			else if (!(tempint1 >= 0 && tempint1 <= 65535)) {
				incorrect = true;
			}
			else {
				H2Config_PLASPI_HEIGHT = tempint1;
				est_PLASPI_HEIGHT = true;
			}
		}
		else if (!H2IsDediServer && sscanf(fileLine, "crosshair_bruteshot_width =%d", &tempint1) == 1) {
			if (est_BRUTESHOT_WIDTH) {
				duplicated = true;
			}
			else if (!(tempint1 >= 0 && tempint1 <= 65535)) {
				incorrect = true;
			}
			else {
				H2Config_BRUTESHOT_WIDTH = tempint1;
				est_BRUTESHOT_WIDTH = true;
			}
		}
		else if (!H2IsDediServer && sscanf(fileLine, "crosshair_bruteshot_height =%d", &tempint1) == 1) {
			if (est_BRUTESHOT_HEIGHT) {
				duplicated = true;
			}
			else if (!(tempint1 >= 0 && tempint1 <= 65535)) {
				incorrect = true;
			}
			else {
				H2Config_BRUTESHOT_HEIGHT = tempint1;
				est_BRUTESHOT_HEIGHT = true;
			}
		}
		else if (!H2IsDediServer && sscanf(fileLine, "crosshair_needler_width =%d", &tempint1) == 1) {
			if (est_NEED_WIDTH) {
				duplicated = true;
			}
			else if (!(tempint1 >= 0 && tempint1 <= 65535)) {
				incorrect = true;
			}
			else {
				H2Config_NEED_WIDTH = tempint1;
				est_NEED_WIDTH = true;
			}
		}
		else if (!H2IsDediServer && sscanf(fileLine, "crosshair_needler_height =%d", &tempint1) == 1) {
			if (est_NEED_HEIGHT) {
				duplicated = true;
			}
			else if (!(tempint1 >= 0 && tempint1 <= 65535)) {
				incorrect = true;
			}
			else {
				H2Config_NEED_HEIGHT = tempint1;
				est_NEED_HEIGHT = true;
			}
		}
		else if (!H2IsDediServer && sscanf(fileLine, "crosshair_sentinal_beam_width =%d", &tempint1) == 1) {
			if (est_SENTBEAM_WIDTH) {
				duplicated = true;
			}
			else if (!(tempint1 >= 0 && tempint1 <= 65535)) {
				incorrect = true;
			}
			else {
				H2Config_SENTBEAM_WIDTH = tempint1;
				est_SENTBEAM_WIDTH = true;
			}
		}
		else if (!H2IsDediServer && sscanf(fileLine, "crosshair_sentinal_beam_height =%d", &tempint1) == 1) {
			if (est_SENTBEAM_HEIGHT) {
				duplicated = true;
			}
			else if (!(tempint1 >= 0 && tempint1 <= 65535)) {
				incorrect = true;
			}
			else {
				H2Config_SENTBEAM_HEIGHT = tempint1;
				est_SENTBEAM_HEIGHT = true;
			}
		}
		else if (sscanf(fileLine, "enable_xdelay =%d", &tempint1) == 1) {
			if (est_xdelay) {
				duplicated = true;
			}
			else if (!(tempint1 == 0 || tempint1 == 1)) {
				incorrect = true;
			}
			else {
				H2Config_xDelay = (bool)tempint1;
				est_xdelay = true;
			}
		}
/*		else if (sscanf(fileLine, "enable_hitmarker_sound =%d", &tempint1) == 1) {
			if (est_hitmarker_sound) {
				duplicated = true;
			}
			else if (!(tempint1 == 0 || tempint1 == 1)) {
				incorrect = true;
			}
			else {
				H2Config_hitmarker_sound = (bool)tempint1;
				est_hitmarker_sound = true;
			}
		}
		else if (sscanf(fileLine, "voice_chat =%d", &tempint1) == 1) {
			if (est_voice_chat) {
				duplicated = true;
			}
			else if (!(tempint1 == 0 || tempint1 == 1)) {
				incorrect = true;
			}
			else {
				H2Config_voice_chat = (bool)tempint1;
				est_voice_chat = true;
			}
		}
		else if (sscanf(fileLine, "mp_explosion_physics =%d", &tempint1) == 1) {
			if (est_als_mp_explosion_physics) {
				duplicated = true;
			}
			else if (!(tempint1 == 0 || tempint1 == 1)) {
				incorrect = true;
			}
			else {
				AdvLobbySettings_mp_explosion_physics = (bool)tempint1;
				est_als_mp_explosion_physics = true;
			}
		}
		else if (sscanf(fileLine, "mp_sputnik =%d", &tempint1) == 1) {
			if (est_als_mp_sputnik) {
				duplicated = true;
			}
			else if (!(tempint1 == 0 || tempint1 == 1)) {
				incorrect = true;
			}
			else {
				AdvLobbySettings_mp_sputnik = (bool)tempint1;
				est_als_mp_sputnik = true;
			}
		}
		else if (sscanf(fileLine, "mp_grunt_bday_party =%d", &tempint1) == 1) {
			if (est_als_mp_grunt_bday_party) {
				duplicated = true;
			}
			else if (!(tempint1 == 0 || tempint1 == 1)) {
				incorrect = true;
			}
			else {
				AdvLobbySettings_mp_grunt_bday_party = (bool)tempint1;
				est_als_mp_grunt_bday_party = true;
			}
		}
		else if (sscanf(fileLine, "grenade_chain_react =%d", &tempint1) == 1) {
			if (est_als_grenade_chain_react) {
				duplicated = true;
			}
			else if (!(tempint1 == 0 || tempint1 == 1)) {
				incorrect = true;
			}
			else {
				AdvLobbySettings_grenade_chain_react = (bool)tempint1;
				est_als_grenade_chain_react = true;
			}
		}
		else if (sscanf(fileLine, "banshee_bomb =%d", &tempint1) == 1) {
			if (est_als_banshee_bomb) {
				duplicated = true;
			}
			else if (!(tempint1 == 0 || tempint1 == 1)) {
				incorrect = true;
			}
			else {
				AdvLobbySettings_banshee_bomb = (bool)tempint1;
				est_als_banshee_bomb = true;
			}
		}
		else if (sscanf(fileLine, "mp_blind =%d", &tempint1) == 1) {
			if (est_als_mp_blind) {
				duplicated = true;
			}
			else if (!(tempint1 == 0 || tempint1 == 1 || tempint1 == 2 || tempint1 == 3)) {
				incorrect = true;
			}
			else {
				AdvLobbySettings_mp_blind = tempint1;
				est_als_mp_blind = true;
			}
		}
		else if (sscanf(fileLine, "flashlight =%d", &tempint1) == 1) {
			if (est_als_flashlight) {
				duplicated = true;
			}
			else if (!(tempint1 == 0 || tempint1 == 1)) {
				incorrect = true;
			}
			else {
				AdvLobbySettings_flashlight = (bool)tempint1;
				est_als_flashlight = true;
			}
		}*/
		else if (sscanf(fileLine, "debug_log =%d", &tempint1) == 1) {
			if (est_debug_log) {
				duplicated = true;
			}
			else if (!(tempint1 == 0 || tempint1 == 1)) {
				incorrect = true;
			}
			else {
				H2Config_debug_log = (bool)tempint1;
				est_debug_log = true;
			}
		}
		else if (sscanf(fileLine, "debug_log_level =%d", &tempint1) == 1) {
			if (est_debug_log_level) {
				duplicated = true;
			}
			else if (!(tempint1 >= 0 && tempint1 <= 5)) {
				incorrect = true;
			}
			else {
				H2Config_debug_log_level = tempint1;
				est_debug_log_level = true;
			}
		}
		else if (sscanf(fileLine, "debug_log_console =%d", &tempint1) == 1) {
			if (est_debug_log_console) {
				duplicated = true;
			}
			else if (!(tempint1 == 0 || tempint1 == 1)) {
				incorrect = true;
			}
			else {
				H2Config_debug_log_console = (bool)tempint1;
				est_debug_log_console = true;
			}
		}
		else if (H2IsDediServer && strstr(fileLine, "server_name =")) {
			if (est_server_name) {
				duplicated = true;
			}
			else {
				char* tempName = fileLine + strlen("server_name =");
				while (isspace(*tempName)) {
					tempName++;
				}
				snprintf(H2Config_dedi_server_name, 32, tempName);
				for (int j = strlen(H2Config_dedi_server_name) - 1; j > 0; j--) {
					if (isspace(H2Config_dedi_server_name[j])) {
						H2Config_dedi_server_name[j] = 0;
					}
					else {
						break;
					}
				}
				est_server_name = true;
			}
		}
		else if (H2IsDediServer && strstr(fileLine, "server_playlist =")) {
			if (est_server_playlist) {
				duplicated = true;
			}
			else {
				char* tempName = fileLine + strlen("server_playlist =");
				while (isspace(*tempName)) {
					tempName++;
				}
				snprintf(H2Config_dedi_server_playlist, 256, tempName);
				for (int j = strlen(H2Config_dedi_server_playlist) - 1; j > 0; j--) {
					if (isspace(H2Config_dedi_server_playlist[j])) {
						H2Config_dedi_server_playlist[j] = 0;
					}
					else {
						break;
					}
				}
				est_server_playlist = true;
			}
		}
		else if (H2IsDediServer && ownsConfigFile && strstr(fileLine, "login_identifier =")) {
			if (est_login_identifier) {
				duplicated = true;
			}
			else {
				char* tempName = fileLine + strlen("login_identifier =");
				while (isspace(*tempName)) {
					tempName++;
				}
				snprintf(H2Config_login_identifier, 255, tempName);
				for (int j = strlen(H2Config_login_identifier) - 1; j > 0; j--) {
					if (isspace(H2Config_login_identifier[j])) {
						H2Config_login_identifier[j] = 0;
					}
					else {
						break;
					}
				}
				est_login_identifier = true;
			}
		}
		else if (H2IsDediServer && ownsConfigFile && strstr(fileLine, "login_password =")) {
			if (est_login_password) {
				duplicated = true;
			}
			else {
				char* tempName = fileLine + strlen("login_password =");
				while (isspace(*tempName)) {
					tempName++;
				}
				snprintf(H2Config_login_password, 255, tempName);
				for (int j = strlen(H2Config_login_password) - 1; j > 0; j--) {
					if (isspace(H2Config_login_password[j])) {
						H2Config_login_password[j] = 0;
					}
					else {
						break;
					}
				}
				est_login_password = true;
			}
		}
		else if (!H2IsDediServer && sscanf(fileLine, "hotkey_help =%d", &tempint1) == 1) {
			if (est_hotkey_help) {
				duplicated = true;
			}
			else if (!(tempint1 >= 0)) {
				incorrect = true;
			}
			else {
				H2Config_hotkeyIdHelp = tempint1;
				est_hotkey_help = true;
			}
		}
		else if (!H2IsDediServer && sscanf(fileLine, "hotkey_toggle_debug =%d", &tempint1) == 1) {
			if (est_hotkey_toggle_debug) {
				duplicated = true;
			}
			else if (!(tempint1 >= 0)) {
				incorrect = true;
			}
			else {
				H2Config_hotkeyIdToggleDebug = tempint1;
				est_hotkey_toggle_debug = true;
			}
		}
		else if (!H2IsDediServer && sscanf(fileLine, "hotkey_align_window =%d", &tempint1) == 1) {
			if (est_hotkey_align_window) {
				duplicated = true;
			}
			else if (!(tempint1 >= 0)) {
				incorrect = true;
			}
			else {
				H2Config_hotkeyIdAlignWindow = tempint1;
				est_hotkey_align_window = true;
			}
		}
		else if (!H2IsDediServer && sscanf(fileLine, "hotkey_window_mode =%d", &tempint1) == 1) {
			if (est_hotkey_window_mode) {
				duplicated = true;
			}
			else if (!(tempint1 >= 0)) {
				incorrect = true;
			}
			else {
				H2Config_hotkeyIdWindowMode = tempint1;
				est_hotkey_window_mode = true;
			}
		}
		else if (!H2IsDediServer && sscanf(fileLine, "hotkey_hide_ingame_chat =%d", &tempint1) == 1) {
			if (est_hotkey_hide_ingame_chat) {
				duplicated = true;
			}
			else if (!(tempint1 >= 0)) {
				incorrect = true;
			}
			else {
				H2Config_hotkeyIdToggleHideIngameChat = tempint1;
				est_hotkey_hide_ingame_chat = true;
			}
		}
		else if (!H2IsDediServer && sscanf(fileLine, "hotkey_guide =%d", &tempint1) == 1) {
			if (est_hotkey_guide) {
				duplicated = true;
			}
			else if (!(tempint1 >= 0)) {
				incorrect = true;
			}
			else {
				H2Config_hotkeyIdGuide = tempint1;
				est_hotkey_guide = true;
			}
		}
		else if (!H2IsDediServer && sscanf(fileLine, "hotkey_console =%d", &tempint1) == 1) {
			if (est_hotkey_console) {
				duplicated = true;
			}
			else if (!(tempint1 >= 0)) {
				incorrect = true;
			}
			else {
				H2Config_hotkeyIdConsole = tempint1;
				est_hotkey_console = true;
			}
		}
		else if (!H2IsDediServer && sscanf(fileLine, "hotkey_console =%d", &tempint1) == 1) {
			if (est_hotkey_console) {
				duplicated = true;
			}
			else if (!(tempint1 >= 0)) {
				incorrect = true;
			}
			else {
				H2Config_hotkeyIdConsole = tempint1;
				est_hotkey_console = true;
			}
		}		
		else if (H2IsDediServer && sscanf(fileLine, "additional_pcr_time =%d", &tempint1) == 1) {
			if (est_additional_pcr_time) {
				duplicated = true;
			}
			else if (!(tempint1 >= 0)) {
				incorrect = true;
			}
			else {
				H2Config_additional_pcr_time = tempint1;
				est_additional_pcr_time = true;
			}
		}
		else {
			unrecognised = true;
		}

		if (unrecognised || duplicated || incorrect) {
			char textDebugBuffer[60];
			if (duplicated) {
				snprintf(textDebugBuffer, 60, "ERROR: Duplicated Config Setting on Line: %d", lineNumber);
				addDebugText(textDebugBuffer);
			}
			else if (incorrect) {
				snprintf(textDebugBuffer, 60, "ERROR: Incorrect Config Setting on Line: %d", lineNumber);
				addDebugText(textDebugBuffer);
			}
			if (ownsConfigFile && !dontSave) {
				if (badConfigBufferI >= badConfigBufferLen) {
					if (!badConfigBuffer) {
						badConfigBuffer = (char**)malloc(sizeof(char*) * (badConfigBufferLen += bufferIncSize));
					}
					else {
						badConfigBuffer = (char**)realloc(badConfigBuffer, sizeof(char*) * (badConfigBufferLen += bufferIncSize));
					}
				}
				badConfigBuffer[badConfigBufferI] = fileLine;
				badConfigBufferI++;
				return 0b10;
			}
		}
	}
	return 0;
}

void ReadH2Config() {
	addDebugText("Reading H2Configuration File...");

	int readInstanceIdFile = H2GetInstanceId();
	wchar_t local[1024];
	swprintf(local, 1024, L"%ws", H2AppDataLocal);
	H2Config_isConfigFileAppDataLocal = false;
	
	errno_t err = 0;
	FILE* fileConfig = nullptr;
	wchar_t fileConfigPath[1024];
	wchar_t fileConfigPathLog[1124];

	if (FlagFilePathConfig) {
		swprintf(fileConfigPath, 1024, FlagFilePathConfig);
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
			swprintf(fileConfigPath, 1024, H2ConfigFilenames[H2IsDediServer], checkFilePath, readInstanceIdFile);
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
			H2Config_language_code_main = current_language_main;
			H2Config_language_code_variant = current_language_sub;
		}

		est_reset_vars();
		oldConfigBufferFree();
		badConfigBufferFree();
		ReadIniFile(fileConfig, true, H2ConfigVersionReadStr, H2ConfigVersion, interpretConfigSetting);

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
	H2Config_disable_ingame_keyboard = (bool)(H2GetInstanceId() - 1);
	ReadH2Config();
}
void DeinitH2Config() {
	SaveH2Config();
}
#pragma endregion
