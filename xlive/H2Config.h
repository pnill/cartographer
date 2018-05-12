#pragma once

void InitH2Config();
void DeinitH2Config();
void SaveH2Config();
void ReadH2Config();

#define DLL_VERSION_MAJOR               0
#define DLL_VERSION_MINOR               4
#define DLL_VERSION_REVISION            0
#define DLL_VERSION_BUILD				4

#define DLL_VERSION            DLL_VERSION_MAJOR, DLL_VERSION_MINOR, DLL_VERSION_REVISION, DLL_VERSION_BUILD
#define STRINGIZE2(s) #s
#define STRINGIZE(s) STRINGIZE2(s)
#define DLL_VERSION_STR        STRINGIZE(DLL_VERSION_MAJOR) "." STRINGIZE(DLL_VERSION_MINOR) "." STRINGIZE(DLL_VERSION_REVISION) "." STRINGIZE(DLL_VERSION_BUILD)

extern unsigned long H2Config_master_ip;
extern unsigned short H2Config_master_port_login;
extern unsigned short H2Config_master_port_relay;

extern bool H2Portable;
extern unsigned short H2Config_base_port;
extern char H2Config_str_wan[16];
extern char H2Config_str_lan[16];
extern unsigned long H2Config_ip_wan;
extern unsigned long H2Config_ip_lan;
extern int H2Config_language_code_main;
extern int H2Config_language_code_variant;
extern bool H2Config_custom_labels_capture_missing;
extern bool H2Config_skip_intro;
extern bool H2Config_raw_input;
extern bool H2Config_discord_enable;
extern bool H2Config_controller_aim_assist;
extern int H2Config_fps_limit;
extern int H2Config_static_lod_state;
extern int H2Config_field_of_view;
extern float H2Config_crosshair_offset;
extern int H2Config_sens_controller;
extern int H2Config_sens_mouse;
extern bool H2Config_disable_ingame_keyboard;
extern bool H2Config_hide_ingame_chat;
extern bool H2Config_xDelay;
extern int H2Config_custom_resolution_x;
extern int H2Config_custom_resolution_y;
extern char H2Config_dedi_server_name[32];
extern char H2Config_dedi_server_playlist[256];
extern bool H2Config_map_downloading_enable;
extern bool H2Config_chatbox_commands;
extern bool H2Config_debug_log;
extern char H2Config_login_identifier[255];
extern char H2Config_login_password[255];
extern int H2Config_hotkeyIdHelp;
extern int H2Config_hotkeyIdToggleDebug;
extern int H2Config_hotkeyIdAlignWindow;
extern int H2Config_hotkeyIdWindowMode;
extern int H2Config_hotkeyIdToggleHideIngameChat;
extern int H2Config_hotkeyIdGuide;
extern int H2Config_hotkeyIdConsole;

