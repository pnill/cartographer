#pragma once
#define ALT_MAIN_LOOP_HOOK 1

void initGSRunLoop();
void deinitGSRunLoop();
typedef void(_cdecl p_present_rendered_screen)();
p_present_rendered_screen* present_rendered_screen;

typedef char(p_sub_9A96B1)();
p_sub_9A96B1* sub_9A96B1;

typedef bool(p_game_freeze)();
p_game_freeze* game_freeze;

typedef char(p_game_minimized)();
p_game_minimized* game_minimized;

typedef bool(p_game_in_simulation)();
p_game_in_simulation* game_in_simulation;

typedef int(__cdecl p_sub_B7CA7D)(float a1, float a2);
p_sub_B7CA7D* sub_CDCA7D;

typedef int(__cdecl p_sub_AF87A1)();
p_sub_AF87A1* sub_AF87A1;

void(*sub_9AA221)();
void(*sub_C8542F)();
void(*sub_C853C7)();
void(*sub_C7D83F)();
void(*sub_B02590)();
void(*sub_B0A221)();
void(*sub_C7E9D3)();
void(*sub_AD7902)();
void(*sub_AD96EB)();
void(*sub_B09783)();
void(*sub_B727EB)();
void(*vibrations_clear)();
void(*game_network_dispatcher)();

typedef void(__cdecl p_sub_C7E7C5)();
p_sub_C7E7C5* sub_C7E7C5;

typedef int(p_sub_B16834)();
p_sub_B16834* sub_B16834;

typedef char(p_sub_B361EC)();
p_sub_B361EC* sub_B361EC;

typedef BYTE*(p_sub_B5DD5C)();
p_sub_B5DD5C* sub_B5DD5C;

typedef bool(p_sub_B4BFD1)();
p_sub_B4BFD1* sub_B4BFD1;

typedef char(p_sub_AD985E)();
p_sub_AD985E* sub_AD985E;

typedef bool __cdecl c_cinematic_in_progress();
c_cinematic_in_progress* cinematic_in_progress;

typedef char c_cinematic_is_running();
c_cinematic_is_running* cinematic_is_running;

typedef void(__cdecl p_sub_B328A8)();
p_sub_B328A8* sub_B328A8;

typedef char(p_sub_B1BA65)();
p_sub_B1BA65* sub_B1BA65;

typedef char(p_restart_game_loop)();
p_restart_game_loop* restart_game_loop;

typedef void(__cdecl p_local_players_update_and_send_synchronous_actions)(float a1, float a2);
p_local_players_update_and_send_synchronous_actions* local_players_update_and_send_synchronous_actions;

typedef void(__cdecl p_simulation_update)(int target_ticks, float *a3);
p_simulation_update* simulation_update;

typedef void(__cdecl p_game_effects_update)(float a1);
p_game_effects_update* game_effects_update;

typedef char(__cdecl p_director_update)(float a1);
p_director_update* director_update;

typedef void(__cdecl p_observer_update)(float a1);
p_observer_update* observer_update;

typedef void(__cdecl p_game_time_globals_prep)(float a1, float* a2, int* a3);
p_game_time_globals_prep* game_time_globals_prep;

typedef int(__cdecl p_system_milliseconds)();
p_system_milliseconds* system_milliseconds;

typedef float(__cdecl main_game_time_system_update)(bool a1, float a2);
main_game_time_system_update* p_main_time_update;

typedef void(__cdecl p_render_audio)();
p_render_audio* render_audio;

typedef void(__stdcall p_sub_B1D31F)(int a1, int a2);
p_sub_B1D31F* sub_B1D31F;

typedef void(_cdecl p_sub_AF8716)(int a1);
p_sub_AF8716* sub_AF8716;
