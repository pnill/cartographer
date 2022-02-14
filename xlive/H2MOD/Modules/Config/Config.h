
#pragma once

#include "H2MOD.h"
#include "H2MOD/Modules/Input/ControllerInput.h"

void InitH2Config();
void DeinitH2Config();
void SaveH2Config();
void ReadH2Config();

// added this until the code gets released
#define USE_DEV_PREVIEW_CONFIG_FILE_PATHS 1

#define DLL_VERSION_MAJOR               0
#define DLL_VERSION_MINOR               6
#define DLL_VERSION_REVISION            5
#define DLL_VERSION_BUILD				1

#define DLL_VERSION            DLL_VERSION_MAJOR, DLL_VERSION_MINOR, DLL_VERSION_REVISION, DLL_VERSION_BUILD
#define STRINGIZE2(s) #s
#define STRINGIZE(s) STRINGIZE2(s)
#define DLL_VERSION_STR        STRINGIZE(DLL_VERSION_MAJOR) "." STRINGIZE(DLL_VERSION_MINOR) "." STRINGIZE(DLL_VERSION_REVISION) "." STRINGIZE(DLL_VERSION_BUILD)

struct _H2Config_language {
	int code_main;
	int code_variant;
};

enum H2Config_Deadzone_Type : byte {
	Axial,
	Radial,
	Both
};

enum e_override_texture_resolution : int
{
	tex_low,
	tex_default,
	tex_high,
	tex_ultra
};

enum H2Config_Experimental_Rendering_Mode : byte
{
	e_render_none,
	e_render_old,
	e_render_new,
	e_render_original_game_frame_limit
};

extern std::string cartographerURL;
extern std::string cartographerMapRepoURL;

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

extern _H2Config_language H2Config_language;
extern bool H2Config_custom_labels_capture_missing;
extern bool H2Config_skip_intro;
extern bool H2Config_raw_input;
extern bool H2Config_discord_enable;
extern bool H2Config_controller_aim_assist;
extern int H2Config_fps_limit;
extern int H2Config_static_lod_state;
extern int H2Config_field_of_view;
extern int H2Config_vehicle_field_of_view;
extern bool H2Config_static_first_person;
extern float H2Config_mouse_sens;
extern bool H2Config_mouse_uniform;
extern float H2Config_controller_sens;
extern bool H2Config_controller_modern;
extern H2Config_Deadzone_Type H2Config_Controller_Deadzone;
extern float H2Config_Deadzone_A_X;
extern float H2Config_Deadzone_A_Y;
extern float H2Config_Deadzone_Radial;
extern __int16 H2Config_refresh_rate;
extern bool H2Config_hiresfix;
extern bool H2Config_shader_lod_max;
extern bool H2Config_light_suppressor;
extern bool H2Config_d3dex;
extern float H2Config_crosshair_offset;
extern bool H2Config_disable_ingame_keyboard;
extern bool H2Config_hide_ingame_chat;
extern bool H2Config_xDelay;
extern bool H2Config_voice_chat;
extern char H2Config_dedi_server_name[XUSER_NAME_SIZE];
extern char H2Config_dedi_server_playlist[256];
extern int H2Config_additional_pcr_time;
extern bool H2Config_debug_log;
extern int H2Config_debug_log_level;
extern bool H2Config_debug_log_console;
extern char H2Config_login_identifier[255];
extern char H2Config_login_password[255];
extern short H2Config_team_bit_flags;
extern bool H2Config_team_flag_array[8];
extern byte H2Config_team_enabled_count;
extern char H2Config_stats_authkey[32 + 1];
extern bool H2Config_vip_lock;
extern bool H2Config_force_even;
extern bool H2Config_koth_random;
extern H2Config_Experimental_Rendering_Mode H2Config_experimental_fps;
extern bool H2Config_anti_cheat_enabled;

extern int H2Config_hotkeyIdHelp;
extern int H2Config_hotkeyIdToggleDebug;
extern int H2Config_hotkeyIdAlignWindow;
extern int H2Config_hotkeyIdWindowMode;
extern int H2Config_hotkeyIdToggleHideIngameChat;
extern int H2Config_hotkeyIdGuide;
extern int H2Config_hotkeyIdConsole;
extern int H2Config_minimum_player_start;

extern float H2Config_raw_mouse_scale;
extern float H2Config_crosshair_scale;
extern point2d H2Config_BATRIF;
extern point2d H2Config_SMG;
extern point2d H2Config_CRBN;
extern point2d H2Config_BEAMRIF;
extern point2d H2Config_MAG;
extern point2d H2Config_PLASRIF;
extern point2d H2Config_SHTGN;
extern point2d H2Config_SNIP;
extern point2d H2Config_SWRD;
extern point2d H2Config_ROCKLAUN;
extern point2d H2Config_PLASPI;
extern point2d H2Config_BRUTESHOT;
extern point2d H2Config_NEED;
extern point2d H2Config_SENTBEAM;
extern ControllerInput::CustomControllerLayout H2Config_CustomLayout;

extern e_override_texture_resolution H2Config_Override_Shadows;
extern e_override_texture_resolution H2Config_Override_Water;

extern bool H2Config_upnp_enable;
extern bool H2Config_melee_fix;
extern bool H2Config_no_events;
extern bool H2Config_spooky_boy;
extern bool H2Config_event_music;