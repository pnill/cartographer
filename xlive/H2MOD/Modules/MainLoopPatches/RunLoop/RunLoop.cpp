#include "stdafx.h"

#include "RunLoop.h"
#include "Blam\Engine\Game\GameTimeGlobals.h"
#include "H2MOD\Modules\Shell\Shell.h"
#include "H2MOD\Modules\Shell\Config.h"
#include "H2MOD\GUI\GUI.h"
#include "H2MOD\Engine\Engine.h"
#include "H2MOD\Modules\Shell\Startup\Startup.h"
#include "H2MOD\Modules\Stats\StatsHandler.h"
#include "H2MOD\Modules\CustomMenu\CustomMenu.h"
#include "H2MOD\Modules\EventHandler\EventHandler.hpp"
#include "H2MOD\Modules\Input\ControllerInput.h"
#include "H2MOD\Modules\MainLoopPatches\MainGameTime\MainGameTime.h"
#include "H2MOD\Modules\MainLoopPatches\UncappedFPS2\UncappedFPS2.h"
#include "H2MOD\Modules\MapManager\MapManager.h"
#include "H2MOD\Modules\OnScreenDebug\OnscreenDebug.h"

#include "H2MOD\Utils\Utils.h"
#include "XLive\xnet\IpManagement\XnIp.h"

#include "Util\Hooks\Hook.h"

#define TIMER_RESOLUTION 1

typedef void(_cdecl* present_rendered_screen_t)();
present_rendered_screen_t p_present_rendered_screen;

typedef char(__cdecl* sub_9A96B1_t)();
sub_9A96B1_t p_sub_9A96B1;

typedef bool(__cdecl* game_freeze_t)();
game_freeze_t p_game_freeze;

typedef bool(__cdecl* game_in_simulation_t)();
game_in_simulation_t p_game_in_simulation;

typedef int(__cdecl* sub_B7CA7D_t)(float a1);
sub_B7CA7D_t p_sub_CDCA7D;

typedef int(__cdecl* sub_AF87A1_t)();
sub_AF87A1_t p_sub_AF87A1;

void(__cdecl* p_sub_9AA221)();
void(__cdecl* p_sub_C8542F)();
void(__cdecl* p_sub_C853C7)();
void(__cdecl* p_sub_C7D83F)();
void(__cdecl* p_sub_B02590)();
void(__cdecl* p_sub_B0A221)();
void(__cdecl* p_sub_C7E9D3)();
void(__cdecl* p_sub_AD7902)();
void(__cdecl* p_sub_AD96EB)();
void(__cdecl* p_sub_B09783)();
void(__cdecl* p_sub_B727EB)();
void(__cdecl* p_vibrations_clear)();
void(__cdecl* p_game_network_dispatcher)();

typedef void(__cdecl* sub_C7E7C5_t)();
sub_C7E7C5_t p_sub_C7E7C5;

typedef int(__cdecl* sub_B16834_t)();
sub_B16834_t p_sub_B16834;

typedef char(__cdecl* sub_B361EC_t)();
sub_B361EC_t p_sub_B361EC;

typedef BYTE*(__cdecl* sub_B5DD5C_t)();
sub_B5DD5C_t p_sub_B5DD5C;

typedef bool(__cdecl* sub_B4BFD1_t)();
sub_B4BFD1_t p_sub_B4BFD1;

typedef char(__cdecl* sub_AD985E_t)();
sub_AD985E_t p_sub_AD985E;

typedef bool (__cdecl* cinematic_in_progress_t)();
cinematic_in_progress_t p_cinematic_in_progress;

typedef char (__cdecl* cinematic_is_running_t)();
cinematic_is_running_t p_cinematic_is_running;

typedef void(__cdecl* input_update_t)();
input_update_t p_input_update;

typedef void(__cdecl* input_abstraction_update_t)();
input_abstraction_update_t p_input_abstraction_update;

typedef char(__cdecl* sub_B1BA65_t)();
sub_B1BA65_t p_sub_B1BA65;

typedef bool(__cdecl* restart_game_loop_t)();
restart_game_loop_t p_restart_game_loop;

typedef void(__cdecl* local_players_update_and_send_synchronous_actions_t)(float a1, float a2);
local_players_update_and_send_synchronous_actions_t p_local_players_update_and_send_synchronous_actions;

typedef void(__cdecl* simulation_update_t)(int target_ticks, float* a3);
simulation_update_t p_simulation_update;

typedef void(__cdecl* game_frame_t)(float a1);
game_frame_t p_game_frame;

typedef char(__cdecl* director_update_t)(float a1);
director_update_t p_director_update;

typedef void(__cdecl* observer_update_t)(float a1);
observer_update_t p_observer_update;

typedef void(__cdecl* game_time_globals_prep_t)(float a1, float* a2, int* a3);
game_time_globals_prep_t p_game_time_globals_prep;

typedef int(__cdecl* system_milliseconds_t)();
system_milliseconds_t p_system_milliseconds;

typedef float(__cdecl* main_game_time_system_update_t)(bool a1, float a2);
main_game_time_system_update_t p_main_time_update;

typedef void(__cdecl* render_audio_t)();
render_audio_t p_render_audio;

typedef void(__thiscall* sub_B1D31F_t)(void* thisx, bool a2);
sub_B1D31F_t p_sub_B1D31F;

typedef void(_cdecl* sub_AF8716_t)(int a1);
sub_AF8716_t p_sub_AF8716;

// rumble_update aka vibrations update
typedef void(__cdecl* rumble_update_t)(float dt);
rumble_update_t p_rumble_update;

extern LPDIRECT3DDEVICE9 pDevice;

bool QuitGSMainLoop = false;

DWORD* get_scenario_global_address() {
	return (DWORD*)(H2BaseAddr + 0x479e74);
}

int get_scenario_volume_count() {
	int volume_count = *(int*)(*get_scenario_global_address() + 0x108);
	return volume_count;
}

void kill_volume_disable(int volume_id) {
	void(__cdecl* kill_volume_disable)(int volume_id);
	kill_volume_disable = (void(__cdecl*)(int))((char*)H2BaseAddr + 0xb3ab8);
	kill_volume_disable(volume_id);
}

void kill_volume_enable(int volume_id) {
	void(__cdecl* kill_volume_enable)(int volume_id);
	kill_volume_enable = (void(__cdecl*)(int))((char*)H2BaseAddr + 0xb3a64);
	kill_volume_enable(volume_id);
}

void GSMainLoop() {
	static bool halo2WindowExists = false;
	if (!H2IsDediServer && !halo2WindowExists && H2hWnd != NULL) {
		halo2WindowExists = true;
		DWORD Display_Mode = 1;
		HKEY hKeyVideoSettings = NULL;
		if (RegOpenKeyExW(HKEY_CURRENT_USER, L"Software\\Microsoft\\Halo 2\\Video Settings", 0, KEY_READ, &hKeyVideoSettings) == ERROR_SUCCESS) {
			GetDWORDRegKey(hKeyVideoSettings, L"DisplayMode", &Display_Mode);
			RegCloseKey(hKeyVideoSettings);
		}
		if (Display_Mode) {
			SetWindowLong(H2hWnd, GWL_STYLE, GetWindowLong(H2hWnd, GWL_STYLE) | WS_SIZEBOX | WS_MAXIMIZEBOX | WS_SYSMENU);
		}

		if (_Shell::GetInstanceId() > 1) {
			wchar_t titleMod[256];
			wchar_t titleOriginal[256];
			GetWindowText(H2hWnd, titleOriginal, 256);
			wsprintf(titleMod, L"%ls (P%d)", titleOriginal, _Shell::GetInstanceId());
			SetWindowText(H2hWnd, titleMod);
		}
	}
	if(H2IsDediServer)
	{
		StatsHandler::playerRanksUpdateTick();
	}
	//EventHandler::executeGameLoopCallbacks();
	/*
	static bool halo2ServerOnce1 = false;
	if (H2IsDediServer && !halo2ServerOnce1) {
		halo2ServerOnce1 = true;
		pushHostLobby();
		wchar_t* LanServerName = (wchar_t*)((BYTE*)H2BaseAddr + 0x52042A);
		if (strlen(H2Config_dedi_server_name) > 0) {
			swprintf(LanServerName, 32, L"%hs", H2Config_dedi_server_name);
		}
	}

	static int prevPartyPrivacy = 0;
	int partyPrivacy;
	if (H2IsDediServer) {
		partyPrivacy = *(int*)(H2BaseAddr + 0x534850);
	}
	else {
		partyPrivacy = *(int*)(H2BaseAddr + 0x50A398);
	}
	if (prevPartyPrivacy > 0 && partyPrivacy == 0) {
		pushHostLobby();
	}
	prevPartyPrivacy = partyPrivacy;
	*/
}

void __cdecl main_game_time_initialize_defaults_hook()
{
	// windows 10 version 2004 and above added behaviour changes to how windows timer resolution works, and we have to explicitly set the time resolution
	// and since they were added, when playing on a laptop on battery it migth add heavy stuttering when using a frame limiter based on Sleep function (or std::this_thread::sleep_for) implementation
	// the game sets them already but only during the loading screen period, then it resets to system default when the loading screen ends 
	// (tho i think in the new implementation is working on a per thread basis now instead of global frequency, since it still works even when the game resets after loading screen ends and loading screen runs in another thread)

	// More code in Tweaks.cpp in InitH2Tweaks

	// More details @ https://randomascii.wordpress.com/2020/10/04/windows-timer-resolution-the-great-rule-change/

	timeBeginPeriod(TIMER_RESOLUTION);

	auto p_main_game_time_initialize_defaults = Memory::GetAddressRelative<void(__cdecl*)()>(0x42869F, 0x424841);
	return p_main_game_time_initialize_defaults();
}

void __cdecl game_modules_dispose_hook() {
	auto p_game_modules_dispose = Memory::GetAddress<void(__cdecl*)()>(0x48BBF, 0x41E60);
	p_game_modules_dispose();

	DeinitH2Startup();

	// reset time resolution to system default on game exit (initialization happens in main_game_time_initialize_defaults_hook())
	timeEndPeriod(TIMER_RESOLUTION);
}

// rasterizer_present hook
// used to limit framerate using our implementation
void rasterizer_present_hook(int a1) {
	typedef void(__cdecl* rasterizer_present_t)(int);
	auto p_rasterizer_present = Memory::GetAddress<rasterizer_present_t>(0x26271A);

	p_rasterizer_present(a1);
	XLiveThrottleFramerate(H2Config_fps_limit);
}

void (__cdecl* p_main_game_loop)();
void main_game_loop_hook() {
	if (!QuitGSMainLoop)
		GSMainLoop();

	EventHandler::GameLoopEventExecute(EventExecutionType::execute_before);
	mapManager->MapDownloadUpdateTick();
	// update local user network stats
	gXnIp.GetLocalUserXn()->pckStats.PckDataSampleUpdate();
	p_main_game_loop();
	EventHandler::GameLoopEventExecute(EventExecutionType::execute_after);
}

static char HookedServerShutdownCheck() {
	if (!QuitGSMainLoop)
		GSMainLoop();
	
	bool& Quit_Exit_Game = *(bool*)((char*)H2BaseAddr + 0x4a7083);

	//original test - if game should shutdown
	return Quit_Exit_Game;
}

extern bool b_XboxTick;

// we disable some broken code added by hired gun, that is also disabled while running a cinematic 
// this should fix the built in frame limiter (while minimized)
// as well as the game speeding up while minimized
bool __cdecl cinematic_in_progress_hook()
{
	H2Config_Experimental_Rendering_Mode experimental_rendering_mode = H2Config_experimental_fps;

	switch (experimental_rendering_mode)
	{
	case _rendering_mode_old:
		// TODO: get_game_life_cycle is only used with networked sessions, meaning this will not work in single player
		// and i keep it this way because the EventHandler in UncappedFPS2.cpp uses the game's life cycle as well
		return p_cinematic_is_running() || Engine::get_game_life_cycle() == _life_cycle_in_game || _Shell::IsGameMinimized();

	// these two options disable the hacks that hired gun added to the main loop
	case _rendering_mode_new:
	case _rendering_mode_original_game_frame_limit:
		return true;

	case _rendering_mode_none:
	default:
		return p_cinematic_is_running() || b_XboxTick || _Shell::IsGameMinimized();
	}

	return false;
}

bool __cdecl should_limit_framerate_hook()
{
	H2Config_Experimental_Rendering_Mode experimental_rendering_mode = H2Config_experimental_fps;

	switch (experimental_rendering_mode)
	{
	case _rendering_mode_original_game_frame_limit:
		return false; // e_render_original_game_frame_limit handles frame limit in MainGameTime.cpp
	case _rendering_mode_none:
	case _rendering_mode_new:
	case _rendering_mode_old:
	default:
		return (_Shell::IsGameMinimized() || b_XboxTick);
	}

	return false;
}

LARGE_INTEGER freq;
LARGE_INTEGER start_tick;
LARGE_INTEGER end_tick;
LARGE_INTEGER start_render;
LARGE_INTEGER end_render;
double render_time;
bool init = false;
float tps = (1.0f / 60);


void alt_prep_time(float a1, float *a2, int *a3)
{
	if (Engine::get_game_life_cycle() != _life_cycle_in_game) {
		float _a2;
		int _a3;
		p_game_time_globals_prep(a1, &_a2, &_a3);
		*a2 = _a2;
		*a3 = _a3;
	}
	else {
		*a2 = a1;
		*a3 = 1;
	}
}
bool b_sys_init = false;
int startTime;

float alt_system_time_update()
{
	// always update the main time, because it might be used by other code
	// shouldn't interfere with anything else
	float original_time_update = p_main_time_update(false, 0.0f);
	
	if(!b_sys_init)
	{
		startTime = timeGetTime();
		b_sys_init = true;
		return 1;
	}

	int currentTime = timeGetTime();
	float result = (float)(currentTime - startTime) * 0.001;
	startTime = currentTime;

	if (p_cinematic_is_running() || _Shell::IsGameMinimized())
		result = original_time_update;

	return result;
}

int* dword_F52268;
int* max_tick_count;
bool* sound_impulse_unk;
bool* sound_impulse_called;
int* dword_F52260;
bool* b_restart_game_loop;
void __cdecl game_main_loop()
{
	int v0; // edi
	int v1; // esi
	LONG v2; // ebp
	double v3; // st7
	float a2; // ST0C_4
	float v5; // xmm0_4
	float v6; // xmm0_4
	DWORD *v7; // eax
	int v8; // esi
	char v10; // [esp+21h] [ebp-23h]
	bool Interpolate; // [esp+22h] [ebp-22h]
	bool v12; // [esp+23h] [ebp-21h]
	static float v13;// [esp+24h] [ebp-20h]
	signed int a3; // [esp+28h] [ebp-1Ch]
	float v15; // [esp+2Ch] [ebp-18h]
	static float out_dt; // [esp+30h] [ebp-14h]
	static int out_target_ticks; // [esp+34h] [ebp-10h]
	int v18; // [esp+40h] [ebp-4h]
	v1 = p_sub_AF87A1(); //Some sort of initializer for timing.
	a3 = v1;
	Interpolate = true; // by default this is false, and needs to be set to true if a cinematic is running, to run the main loop how it originally worked in H2X
						// (Nuke: also theres no interpolation in the game, it has to be added, mo idea why this is called like this because it's quite misleading)
	if (!(*dword_F52268 & 1)) //Game loop init
	{
		*dword_F52268 |= 1u;
		*max_tick_count = 8;
		v18 = -1; //Never used
	}
	if (cinematic_in_progress_hook())
	{
		// disable the shitty hack that hired gun added to force a game tick to execute everytime a frame is rendered
		a3 = 1;
		v1 = 1;
		Interpolate = true;
	}
	else
	{
		*sound_impulse_unk = 0;
	}
	*sound_impulse_called = 0;
	if (v1 > *max_tick_count)
	{
		a3 = 1;
		v1 = 1;
		Interpolate = true;
	}
	v2 = 0;
	while (1)
	{
		v12 = v2 == 0;
		if (v2 == v1 - 1 || (v10 = 0, !v1))
			v10 = 1;
		if (v2 == 0)
		{
			p_sub_AD96EB();

			if (!p_sub_AD985E())
				return;
			p_sub_B09783();
		}
		if (p_restart_game_loop())
			break;
		
		p_input_update();
		if (!_Shell::IsGameMinimized()) 
		{
			if (H2Config_controller_modern)
			{
				ControllerInput::procces_input();
			}
			else
			{
				p_input_abstraction_update();
			}
		}
		if (v12)
		{
			p_sub_B16834();
			p_sub_B02590();
			p_sub_B5DD5C();
			p_sub_AD7902();
			p_sub_B361EC();
			p_sub_B1BA65();
			void* loaded_custom_maps_data = Memory::GetAddress<void*>(0x482D70);
			p_sub_B1D31F(loaded_custom_maps_data, true);
		}
		if (!p_game_freeze())
		{
			out_dt = 0.0;
			out_target_ticks = 0;
			if (Interpolate)
			{
				v15 = alt_system_time_update(); // (Nuke: no idea why main_time_update is replaced by this but p_main_time_update does the same thing, just gets a time delta)
			}
			else
			{
				if (v1 > 0 && p_sub_B4BFD1())
					v3 = time_globals::get()->seconds_per_tick;
				else
					v3 = 0.0f;
				a2 = v3;
				v15 = p_main_time_update(true, a2);
				if (*dword_F52260 < 2)
				{
					++*dword_F52260;
					p_sub_AF8716(1);
				}
				else
				{
					*dword_F52260 = 0;
				}
			}
			if (p_game_freeze())
				v5 = 0.0;
			else
				v5 = v15;
			v13 = v5;
			if (!_Shell::IsGameMinimized())
			{
				p_sub_CDCA7D(v15);
				p_sub_B0A221();
			}
			if (v12)
			{
				p_sub_C8542F();
				p_sub_C853C7();
				p_sub_C7E7C5();
				p_sub_C7D83F();
			}
			if (p_game_in_simulation())
			{
				p_game_time_globals_prep(v13, &out_dt, &out_target_ticks);
				p_local_players_update_and_send_synchronous_actions(v13, out_dt);
				p_simulation_update(out_target_ticks, &out_dt);
				if (v10)
					p_game_network_dispatcher();
				//game_frame(out_dt);
				p_director_update(v13);
				p_observer_update(v13);
				p_rumble_update(v13);
			}
			else
			{
				p_sub_C7E9D3();
				p_game_network_dispatcher();
				p_vibrations_clear();
			}
			if (v10)
			{
				/*v7 = get_xbox_achievements_data();
				XAchievements(v7);*/
			}
			if (_Shell::IsGameMinimized())
			{
				p_vibrations_clear();
			}
			else if (v10)
			{
				v0 = p_system_milliseconds();
				p_present_rendered_screen();
				v8 = p_system_milliseconds() - v0;
				//DWORD* init_flags_array = Memory::GetAddress<DWORD*>(0x46d820);
				//if (init_flags_array[2] == 0)
				//	render_audio();
				if (*sound_impulse_called)
				{
					*sound_impulse_called = 0;
					*sound_impulse_unk = 1;
					p_sub_AF8716(v8);
				}
				v1 = a3;
			}
		}
		if (++v2 >= v1)
			return;
	}
	//sub_AF86E7 replacement, as it is a call that is only made here.
	*b_restart_game_loop = 0;
}
int update_bsp = 0x4A16D;

float __cdecl fps_get_seconds_per_frame()
{
	return (1.0f / H2Config_fps_limit);
}
void alt_main_game_loop_hook()
{
	QueryPerformanceCounter(&end_tick);
	double tick_time = static_cast<double>(end_tick.QuadPart - start_tick.QuadPart) / freq.QuadPart;// -render_time;
	if (tick_time >= time_globals::get()->seconds_per_tick || !init)
	{
		QueryPerformanceCounter(&start_tick);
		if (!QuitGSMainLoop)
			GSMainLoop();
		init = true;

		DWORD* init_flags_array = Memory::GetAddress<DWORD*>(0x46d820);
		if (init_flags_array[2] == 0 && !_Shell::IsGameMinimized())
			p_render_audio();

		if (p_game_in_simulation())
		{
			p_game_frame(time_globals::get()->seconds_per_tick);
		}
		//game_main_loop();
		//main_game_loop();
	}
	if (H2Config_fps_limit != 0) {
		QueryPerformanceCounter(&end_render);
		render_time = static_cast<double>(end_render.QuadPart - start_render.QuadPart) / freq.QuadPart;
		if (render_time >= (1.0f / H2Config_fps_limit)) {
			QueryPerformanceCounter(&start_render);
			EventHandler::GameLoopEventExecute(EventExecutionType::execute_before);
			game_main_loop();
			EventHandler::GameLoopEventExecute(EventExecutionType::execute_after);
			//present_rendered_screen();
		}
	}
	else
	{
		EventHandler::GameLoopEventExecute(EventExecutionType::execute_before);
		game_main_loop();
		EventHandler::GameLoopEventExecute(EventExecutionType::execute_after);
	}
	
}

void initialize_main_loop_function_pointers()
{
	p_sub_9AA221 = (void(*)())((char*)H2BaseAddr + 0x3A221);
	p_sub_C8542F = (void(*)())((char*)H2BaseAddr + 0x1B542F);
	p_sub_C853C7 = (void(*)())((char*)H2BaseAddr + 0x1B53C7);
	p_sub_C7D83F = (void(*)())((char*)H2BaseAddr + 0x1AD83F);
	p_sub_B02590 = (void(*)())((char*)H2BaseAddr + 0x325A2);
	p_sub_B0A221 = (void(*)())((char*)H2BaseAddr + 0x3A221);
	p_sub_C7E9D3 = (void(*)())((char*)H2BaseAddr + 0x1AE9D3);
	p_sub_AD7902 = (void(*)())((char*)H2BaseAddr + 0x7902);
	p_sub_AD96EB = (void(*)())((char*)H2BaseAddr + 0x96EB);
	p_sub_B09783 = (void(*)())((char*)H2BaseAddr + 0x39783);
	p_sub_B727EB = (void(*)())((char*)H2BaseAddr + 0xA27EB);

	p_sub_C7E7C5 = Memory::GetAddress<sub_C7E7C5_t>(0x1AE7C5);
	p_sub_B5DD5C = Memory::GetAddress<sub_B5DD5C_t>(0x8DD5C);
	p_sub_B16834 = Memory::GetAddress<sub_B16834_t>(0x46834);
	p_sub_B1BA65 = Memory::GetAddress<sub_B1BA65_t>(0x4BA65);
	p_sub_B361EC = Memory::GetAddress<sub_B361EC_t>(0x661EC);
	p_sub_AF87A1 = Memory::GetAddress<sub_AF87A1_t>(0x287A1);
	p_sub_AD985E = Memory::GetAddress<sub_AD985E_t>(0x985E);
	p_sub_B4BFD1 = Memory::GetAddress<sub_B4BFD1_t>(0x7BFD1);
	p_sub_9A96B1 = Memory::GetAddress<sub_9A96B1_t>(0x396B1);
	p_sub_CDCA7D = Memory::GetAddress<sub_B7CA7D_t>(0x20CA7D);
	p_sub_AF8716 = Memory::GetAddress<sub_AF8716_t>(0x28716);
	p_sub_B1D31F = Memory::GetAddress<sub_B1D31F_t>(0x4D31F);

	p_vibrations_clear = (void(*)())((char*)H2BaseAddr + 0x901B8);
	p_game_network_dispatcher = (void(*)())((char*)H2BaseAddr + 0x1B5456);
	p_restart_game_loop = Memory::GetAddress<restart_game_loop_t>(0x286E1);
	p_game_time_globals_prep = Memory::GetAddress<game_time_globals_prep_t>(0x7C1BF);
	p_present_rendered_screen = Memory::GetAddress<present_rendered_screen_t>(0x27002A);
	p_game_in_simulation = Memory::GetAddress<game_in_simulation_t>(0x1ADD30);
	p_game_freeze = Memory::GetAddress<game_freeze_t>(0x145B);
	p_render_audio = Memory::GetAddress<render_audio_t>(0x2DF87);
	p_system_milliseconds = Memory::GetAddress<system_milliseconds_t>(0x37E51);
	p_observer_update = Memory::GetAddress<observer_update_t>(0x83E6A);
	p_local_players_update_and_send_synchronous_actions = Memory::GetAddress<local_players_update_and_send_synchronous_actions_t>(0x93857);
	p_simulation_update = Memory::GetAddress<simulation_update_t>(0x4A5D0);
	p_game_frame = Memory::GetAddress<game_frame_t>(0x48CDC);
	p_director_update = Memory::GetAddress<director_update_t>(0x5A658);
	p_main_time_update = Memory::GetAddress<main_game_time_system_update_t>(0x28814);
	p_cinematic_in_progress = Memory::GetAddress<cinematic_in_progress_t>(0x3a928);
	p_cinematic_is_running = Memory::GetAddress<cinematic_is_running_t>(0x3a938);
	p_rumble_update = Memory::GetAddress<rumble_update_t>(0x90438);
	p_input_abstraction_update = Memory::GetAddress<input_abstraction_update_t>(0x628A8);
	p_input_update = Memory::GetAddress<input_update_t>(0x2F9AC);
		
	dword_F52268 = Memory::GetAddress<int*>(0x482268);
	max_tick_count = Memory::GetAddress<int*>(0x482264);
	sound_impulse_unk = Memory::GetAddress<bool*>(0x48225B);
	sound_impulse_called = Memory::GetAddress<bool*>(0x48225A);
	dword_F52260 = Memory::GetAddress<int*>(0x482260);
	b_restart_game_loop = Memory::GetAddress<bool*>(0x479EA0);
}

void InitRunLoop() {
	LOG_INFO_GAME("{} - initializing", __FUNCTION__);

	addDebugText("Pre RunLoop hooking.");
	if (H2IsDediServer) {
		addDebugText("Hooking loop & shutdown Function");
		PatchCall(H2BaseAddr + 0xc6cb, HookedServerShutdownCheck);
	}
	else {
		addDebugText("Hooking loop Function");
		p_main_game_loop = (void(*)())((char*)H2BaseAddr + 0x399CC);

		H2Config_Experimental_Rendering_Mode experimental_rendering_mode = H2Config_experimental_fps;

		// always init these pointers
		initialize_main_loop_function_pointers();

		// frame limiter hooks
		PatchCall(Memory::GetAddress(0x19073C), rasterizer_present_hook);
		PatchCall(Memory::GetAddress(0x19074C), rasterizer_present_hook);

		switch (experimental_rendering_mode)
		{
		case _rendering_mode_old:
			PatchCall(H2BaseAddr + 0x39E64, main_game_loop_hook);
			UncappedFPS2::Init();
			break;
		case _rendering_mode_new:
			//PatchCall(Memory::GetAddress(0x39D04), alt_prep_time);
			PatchCall(H2BaseAddr + 0x39E64, alt_main_game_loop_hook);
			//PatchCall(H2BaseAddr + 0x39e64, game_main_loop);
			QueryPerformanceFrequency(&freq);
			//Remove original render call
			NopFill(Memory::GetAddress(0x39DAA), 5);
			break;

		case _rendering_mode_original_game_frame_limit:
			PatchCall(H2BaseAddr + 0x39E64, main_game_loop_hook);
			MainGameTime::ApplyPatches();
			MainGameTime::fps_limiter_enabled = true;
			break;

		case _rendering_mode_none:
		default:
			PatchCall(H2BaseAddr + 0x39E64, main_game_loop_hook);
			break;
		} // switch (experimental_rendering_mode)

		// apply the code that fixes and determines if the amin loop should be throttled
		PatchCall(Memory::GetAddress(0x288B5), should_limit_framerate_hook);
		PatchCall(Memory::GetAddress(0x39A2A), cinematic_in_progress_hook);

		// stop Hold to Zoom.
		NopFill(Memory::GetAddress(0x9355C), 4);
	}

	PatchCall(Memory::GetAddressRelative(0x439E3D, 0x40BA40), main_game_time_initialize_defaults_hook);
	PatchCall(Memory::GetAddress(0x39E7C, 0xC6F7), game_modules_dispose_hook);

	addDebugText("Post RunLoop Hooking.");
}

void DeinitRunLoop() {

}
