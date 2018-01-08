#include "H2Config.h"
#include "GSUtils.h"
#include <Shellapi.h>
#include "H2OnscreenDebugLog.h"
#include <math.h>
#include "H2Startup.h"

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
//int H2Config_ip_master;
int H2Config_language_code_main = -1;
int H2Config_language_code_variant = 0;
bool H2Config_custom_labels_capture_missing = false;
bool H2Config_skip_intro = false;
bool H2Config_raw_input = false;
bool H2Config_discord_enable = true;
bool H2Config_controller_aim_assist = true;
int H2Config_fps_limit = 60;
int H2Config_field_of_view = 0;
float H2Config_crosshair_offset = NAN;
int H2Config_sens_controller = 0;
int H2Config_sens_mouse = 0;
bool H2Config_disable_ingame_keyboard = false;
bool H2Config_hide_ingame_chat = false;
bool H2Config_xDelay = true;
int H2Config_custom_resolution_x = 0;
int H2Config_custom_resolution_y = 0;
char H2Config_dedi_server_name[32] = { "" };
char H2Config_dedi_server_playlist[256] = { "" };
bool H2Config_map_downloading_enable = false;
bool H2Config_chatbox_commands = false;
bool H2Config_debug_log = false;
char H2Config_login_identifier[255] = { "" };
char H2Config_login_password[255] = { "" };

int H2Config_hotkeyIdHelp = VK_F3;
int H2Config_hotkeyIdToggleDebug = VK_F2;
int H2Config_hotkeyIdAlignWindow = VK_F7;
int H2Config_hotkeyIdWindowMode = VK_F8;
int H2Config_hotkeyIdToggleHideIngameChat = VK_F9;
int H2Config_hotkeyIdGuide = VK_HOME;
int H2Config_hotkeyIdConsole = VK_F10;


bool ownsConfigFile = false;
bool isConfigFileAppDataLocal = false;

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
	else if (H2Portable || !isConfigFileAppDataLocal) {
		swprintf(fileConfigPath, 1024, H2ConfigFilenames[H2IsDediServer], H2ProcessFilePath, H2GetInstanceId());
	}
	else {
		swprintf(fileConfigPath, 1024, H2ConfigFilenames[H2IsDediServer], H2AppDataLocal, H2GetInstanceId());
	}
	
	wchar_t fileConfigPathLog[1124];
	swprintf(fileConfigPathLog, 1024, L"Saving Config: \"%ws\"", fileConfigPath);
	addDebugText(fileConfigPathLog);
	FILE* fileConfig = _wfopen(fileConfigPath, L"wb");

	if (fileConfig == NULL) {
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

			fputs("# controller_aim_assist Options (Client):", fileConfig);
			fputs("\n# 0 - Disables aim assist for controllers.", fileConfig);
			fputs("\n# 1 - Enables aim assist for controllers.", fileConfig);
			fputs("\n\n", fileConfig);

			fputs("# fps_limit Options (Client):", fileConfig);
			fputs("\n# <uint> - 0 disables the built in frame limiter. >0 is the fps limit of the game.", fileConfig);
			fputs("\n\n", fileConfig);

			fputs("# field_of_view Options (Client):", fileConfig);
			fputs("\n# <uint 0 to 110> - 0 disables the built in FoV adjustment. >0 is the FoV set value.", fileConfig);
			fputs("\n\n", fileConfig);

			fputs("# crosshair_offset Options (Client):", fileConfig);
			fputs("\n# <0 to 0.53> - NaN disables the built in Crosshair adjustment.", fileConfig);
			fputs("\n\n", fileConfig);

			fputs("# controller_sensitivity Option (Client):", fileConfig);
			fputs("\n# <value> Change controller sensitivity to your preference.", fileConfig);
			fputs("\n\n", fileConfig);

			fputs("# mouse_sensitivity Option (Client):", fileConfig);
			fputs("\n# <value> Change mouse sensitivity to your preference.", fileConfig);
			fputs("\n\n", fileConfig);

			fputs("# disable_ingame_keyboard Options (Client):", fileConfig);
			fputs("\n# 0 - Normal Game Controls.", fileConfig);
			fputs("\n# 1 - Disables ONLY Keyboard when in-game & allows controllers when game is not in focus.", fileConfig);
			fputs("\n\n", fileConfig);

			fputs("# hide_ingame_chat Options (Client):", fileConfig);
			fputs("\n# 0 - In-game chat is displayed normally.", fileConfig);
			fputs("\n# 1 - In-game chat is hidden.", fileConfig);
			fputs("\n\n", fileConfig);

			//fputs("# custom_resolution Options (Client):", fileConfig);
			//fputs("\n# <width>x<height> - Sets the resolution of the game via the Windows Registry.", fileConfig);
			//fputs("\n# 0x0, 0x?, ?x0 - these do not do modify anything where ? is >= 0.", fileConfig);
			//fputs("\n\n", fileConfig);
		}
		fputs("# enable_xdelay Options:", fileConfig);
		fputs("\n# 0 - Non-host players cannot delay the game start countdown timer.", fileConfig);
		fputs("\n# 1 - Non-host players can delay the game start countdown timer (native default).", fileConfig);
		fputs("\n\n", fileConfig);

		fputs("# debug_log Options:", fileConfig);
		fputs("\n# 0 - Disables excess logging.", fileConfig);
		fputs("\n# 1 - Enables excess logging.", fileConfig);
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
		}
		if (!H2IsDediServer) {
			fputs("# hotkey_... Options (Client):", fileConfig);
			fputs("\n# The number used is the keyboard Virtual-Key (VK) Code in base-10 integer form.", fileConfig);
			fputs("\n# The codes in hexadecimal (base-16) form can be found here:", fileConfig);
			fputs("\n# https://msdn.microsoft.com/en-us/library/windows/desktop/dd375731(v=vs.85).aspx", fileConfig);
			fputs("\n\n", fileConfig);
		}

		int fputbufferlen = strlen(H2ConfigVersionStr) + 1;
		char* fputbuffer = (char*)malloc(sizeof(char) * fputbufferlen);
		snprintf(fputbuffer, fputbufferlen, H2ConfigVersionStr, H2ConfigVersion);
		fputs(fputbuffer, fileConfig);
		free(fputbuffer);

		char settingOutBuffer[60];

		fputs("\nh2portable = ", fileConfig); fputs(H2Portable ? "1" : "0", fileConfig);

		sprintf(settingOutBuffer, "\nbase_port = %d", H2Config_base_port);
		fputs(settingOutBuffer, fileConfig);

		fputs("\nwan_ip = ", fileConfig); fputs(H2Config_str_wan, fileConfig);

		fputs("\nlan_ip = ", fileConfig); fputs(H2Config_str_lan, fileConfig);

		if (H2IsDediServer) {
			//fputs("\nlanguage_code = -1x0", fileConfig);
		} else {
			sprintf(settingOutBuffer, "\nlanguage_code = %dx%d", H2Config_language_code_main, H2Config_language_code_variant);
			fputs(settingOutBuffer, fileConfig);
		}
		if (!H2IsDediServer) {
			fputs("\nlanguage_label_capture = ", fileConfig); fputs(H2Config_custom_labels_capture_missing ? "1" : "0", fileConfig);

			fputs("\nskip_intro = ", fileConfig); fputs(H2Config_skip_intro ? "1" : "0", fileConfig);

			fputs("\nraw_mouse_input = ", fileConfig); fputs(H2Config_raw_input ? "1" : "0", fileConfig);

			fputs("\ndiscord_enable = ", fileConfig); fputs(H2Config_discord_enable ? "1" : "0", fileConfig);

			fputs("\ncontroller_aim_assist = ", fileConfig); fputs(H2Config_controller_aim_assist ? "1" : "0", fileConfig);

			sprintf(settingOutBuffer, "\nfps_limit = %d", H2Config_fps_limit);
			fputs(settingOutBuffer, fileConfig);

			sprintf(settingOutBuffer, "\nfield_of_view = %d", H2Config_field_of_view);
			fputs(settingOutBuffer, fileConfig);

			if (FloatIsNaN(H2Config_crosshair_offset)) {
				fputs("\ncrosshair_offset = NaN", fileConfig);
			}
			else {
				sprintf(settingOutBuffer, "\ncrosshair_offset = %f", H2Config_crosshair_offset);
				fputs(settingOutBuffer, fileConfig);
			}

			sprintf(settingOutBuffer, "\ncontroller_sensitivity = %d", H2Config_sens_controller);
			fputs(settingOutBuffer, fileConfig);

			sprintf(settingOutBuffer, "\nmouse_sensitivity = %d", H2Config_sens_mouse);
			fputs(settingOutBuffer, fileConfig);

			fputs("\ndisable_ingame_keyboard = ", fileConfig); fputs(H2Config_disable_ingame_keyboard ? "1" : "0", fileConfig);

			fputs("\nhide_ingame_chat = ", fileConfig); fputs(H2Config_hide_ingame_chat ? "1" : "0", fileConfig);

			//TODO
			//fputs("\ncustom_resolution = 0x0", fileConfig);
		}
		fputs("\nenable_xdelay = ", fileConfig); fputs(H2Config_xDelay ? "1" : "0", fileConfig);

		fputs("\ndebug_log = ", fileConfig); fputs(H2Config_debug_log ? "1" : "0", fileConfig);

		if (H2IsDediServer) {
			fputs("\nserver_name = ", fileConfig); fputs(H2Config_dedi_server_name, fileConfig);

			fputs("\nserver_playlist = ", fileConfig); fputs(H2Config_dedi_server_playlist, fileConfig);
		}

		//fputs("\nmap_downloading_enable = ", fileConfig); fputs(H2Config_map_downloading_enable ? "1" : "0", fileConfig);

		if (!H2IsDediServer) {
			//fputs("\nchatbox_commands = ", fileConfig); fputs(H2Config_chatbox_commands ? "1" : "0", fileConfig);
		}
		if (H2IsDediServer) {
			fputs("\nlogin_identifier = ", fileConfig); fputs(H2Config_login_identifier, fileConfig);
			fputs("\nlogin_password = ", fileConfig); fputs(H2Config_login_password, fileConfig);
		}

		char hotkeyText[70];

		if (!H2IsDediServer) {

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
static bool est_controller_aim_assist = false;
static bool est_fps_limit = false;
static bool est_field_of_view = false;
static bool est_crosshair_offset = false;
static bool est_sens_controller = false;
static bool est_sens_mouse = false;
static bool est_disable_ingame_keyboard = false;
static bool est_hide_ingame_chat = false;
static bool est_xdelay = false;
static bool est_debug_log = false;
static bool est_custom_resolution = false;
static bool est_server_name = false;
static bool est_server_playlist = false;
static bool est_map_downloading_enable = false;
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
	est_controller_aim_assist = false;
	est_fps_limit = false;
	est_field_of_view = false;
	est_crosshair_offset = false;
	est_sens_controller = false;
	est_sens_mouse = false;
	est_disable_ingame_keyboard = false;
	est_hide_ingame_chat = false;
	est_xdelay = false;
	est_debug_log = false;
	est_custom_resolution = false;
	est_server_name = false;
	est_server_playlist = false;
	est_map_downloading_enable = false;
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
		else if (!H2IsDediServer && sscanf(fileLine, "controller_aim_assist =%d", &tempint1) == 1) {
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
		}
		else if (!H2IsDediServer && sscanf(fileLine, "fps_limit =%d", &tempint1) == 1) {
			if (est_fps_limit) {
				duplicated = true;
			}
			else if (!(tempint1 >= 0)) {
				incorrect = true;
			}
			else {
				H2Config_fps_limit = tempint1;
				extern float desiredRenderTime;
				desiredRenderTime = (1000.f / H2Config_fps_limit);
				est_fps_limit = true;
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
		else if (!H2IsDediServer && sscanf(fileLine, "controller_sensitivity =%d", &tempint1) == 1) {
			if (est_sens_controller) {
				duplicated = true;
			}
			else if (!(tempint1 >= 0)) {
				incorrect = true;
			}
			else {
				H2Config_sens_controller = tempint1;
				est_sens_controller = true;
			}
		}
		else if (!H2IsDediServer && sscanf(fileLine, "mouse_sensitivity =%d", &tempint1) == 1) {
			if (est_sens_mouse) {
				duplicated = true;
			}
			else if (!(tempint1 >= 0)) {
				incorrect = true;
			}
			else {
				H2Config_sens_mouse = tempint1;
				est_sens_mouse = true;
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
		else if (!H2IsDediServer && sscanf(fileLine, "custom_resolution =%dx%d", &tempint1, &tempint2) == 2) {
			if (est_custom_resolution) {
				duplicated = true;
			}
			else if (!(tempint1 >= 0 && tempint2 >= 0)) {
				incorrect = true;
			}
			else {
				H2Config_custom_resolution_x = tempint1;
				H2Config_custom_resolution_y = tempint2;
				est_custom_resolution = true;
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
		else if (sscanf(fileLine, "map_downloading_enable =%d", &tempint1) == 1) {
			if (est_map_downloading_enable) {
				duplicated = true;
			}
			else if (!(tempint1 == 0 || tempint1 == 1)) {
				incorrect = true;
			}
			else {
				H2Config_map_downloading_enable = (bool)tempint1;
				est_map_downloading_enable = true;
			}
		}
		else if (!H2IsDediServer && sscanf(fileLine, "chatbox_commands =%d", &tempint1) == 1) {
			if (est_chatbox_commands) {
				duplicated = true;
			}
			else if (!(tempint1 == 0 || tempint1 == 1)) {
				incorrect = true;
			}
			else {
				H2Config_chatbox_commands = (bool)tempint1;
				est_chatbox_commands = true;
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
	isConfigFileAppDataLocal = false;
	
	FILE* fileConfig;
	wchar_t fileConfigPath[1024];
	wchar_t fileConfigPathLog[1124];

	if (FlagFilePathConfig) {
		swprintf(fileConfigPath, 1024, FlagFilePathConfig);
		swprintf(fileConfigPathLog, 1124, L"Reading Flag Config: \"%ws\"", fileConfigPath);
		addDebugText(fileConfigPathLog);
		fileConfig = _wfopen(fileConfigPath, L"rb");
	}
	else {
		do {
			wchar_t* checkFilePath = H2ProcessFilePath;
			if (isConfigFileAppDataLocal) {
				checkFilePath = local;
			}
			swprintf(fileConfigPath, 1024, H2ConfigFilenames[H2IsDediServer], checkFilePath, readInstanceIdFile);
			swprintf(fileConfigPathLog, 1124, L"Reading Config: \"%ws\"", fileConfigPath);
			addDebugText(fileConfigPathLog);
			fileConfig = _wfopen(fileConfigPath, L"rb");

			if (!fileConfig) {
				addDebugText("H2Configuration File does not exist.");
			}
			isConfigFileAppDataLocal = !isConfigFileAppDataLocal;
			if (!fileConfig && !isConfigFileAppDataLocal) {
				--readInstanceIdFile;
			}
		} while (!fileConfig && readInstanceIdFile > 0);
		isConfigFileAppDataLocal = !isConfigFileAppDataLocal;
	}

	if (!fileConfig) {
		addDebugText("ERROR: No H2Configuration Files Could Be Found!");
		isConfigFileAppDataLocal = true;
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
