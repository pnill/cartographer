#pragma once

void InitH2Config();
void DeinitH2Config();
void SaveH2Config();
void ReadH2Config();

#define DLL_VERSION_MAJOR               0
#define DLL_VERSION_MINOR               5
#define DLL_VERSION_REVISION            6
#define DLL_VERSION_BUILD				0

#define DLL_VERSION            DLL_VERSION_MAJOR, DLL_VERSION_MINOR, DLL_VERSION_REVISION, DLL_VERSION_BUILD
#define STRINGIZE2(s) #s
#define STRINGIZE(s) STRINGIZE2(s)
#define DLL_VERSION_STR        STRINGIZE(DLL_VERSION_MAJOR) "." STRINGIZE(DLL_VERSION_MINOR) "." STRINGIZE(DLL_VERSION_REVISION) "." STRINGIZE(DLL_VERSION_BUILD)

extern unsigned long H2Config_master_ip;
extern unsigned short H2Config_master_port_login;
extern unsigned short H2Config_master_port_relay;

extern bool H2Portable;
extern bool H2Config_isConfigFileAppDataLocal;
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
//extern bool H2Config_controller_aim_assist;
extern int H2Config_fps_limit;
extern int H2Config_static_lod_state;
extern int H2Config_field_of_view;
extern int H2Config_vehicle_field_of_view;
extern int H2Config_mouse_sens;
extern int H2Config_controller_sens;
extern int H2Config_refresh_rate;
extern bool H2Config_hiresfix;
extern bool H2Config_d3dex;
extern float H2Config_crosshair_offset;
extern bool H2Config_disable_ingame_keyboard;
extern bool H2Config_hide_ingame_chat;
extern bool H2Config_xDelay;
extern bool H2Config_voice_chat;
extern char H2Config_dedi_server_name[32];
extern char H2Config_dedi_server_playlist[256];
extern int H2Config_additional_pcr_time;
extern bool H2Config_debug_log;
extern int H2Config_debug_log_level;
extern bool H2Config_debug_log_console;
extern char H2Config_login_identifier[255];
extern char H2Config_login_password[255];
extern int H2Config_hotkeyIdHelp;
extern int H2Config_hotkeyIdToggleDebug;
extern int H2Config_hotkeyIdAlignWindow;
extern int H2Config_hotkeyIdWindowMode;
extern int H2Config_hotkeyIdToggleHideIngameChat;
extern int H2Config_hotkeyIdGuide;
extern int H2Config_hotkeyIdConsole;

extern int H2Config_BATRIF_WIDTH;
extern int H2Config_BATRIF_HEIGHT;
extern int H2Config_SMG_WIDTH;
extern int H2Config_SMG_HEIGHT;
extern int H2Config_CRBN_WIDTH;
extern int H2Config_CRBN_HEIGHT;
extern int H2Config_BEAMRIF_WIDTH;
extern int H2Config_BEAMRIF_HEIGHT;
extern int H2Config_MAG_WIDTH;
extern int H2Config_MAG_HEIGHT;
extern int H2Config_PLASRIF_WIDTH;
extern int H2Config_PLASRIF_HEIGHT;
extern int H2Config_SHTGN_WIDTH;
extern int H2Config_SHTGN_HEIGHT;
extern int H2Config_SNIP_WIDTH;
extern int H2Config_SNIP_HEIGHT;
extern int H2Config_SWRD_WIDTH;
extern int H2Config_SWRD_HEIGHT;
extern int H2Config_ROCKLAUN_WIDTH;
extern int H2Config_ROCKLAUN_HEIGHT;
extern int H2Config_PLASPI_WIDTH;
extern int H2Config_PLASPI_HEIGHT;
extern int H2Config_BRUTESHOT_WIDTH;
extern int H2Config_BRUTESHOT_HEIGHT;
extern int H2Config_NEED_WIDTH;
extern int H2Config_NEED_HEIGHT;
extern int H2Config_SENTBEAM_WIDTH;
extern int H2Config_SENTBEAM_HEIGHT;

