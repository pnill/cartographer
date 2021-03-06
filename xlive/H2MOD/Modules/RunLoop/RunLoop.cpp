#include "stdafx.h"
#include "RunLoop.h"


#include "H2MOD\Modules\Utils\Utils.h"
#include "H2MOD\Modules\Startup\Startup.h"
#include "H2MOD/Modules/MapManager/MapManager.h"
#include "H2MOD\Modules\OnScreenDebug\OnscreenDebug.h"

#include "H2MOD\Modules\CustomMenu\CustomMenu.h"
#include "H2MOD\Modules\Networking\NetworkSession\NetworkSession.h"
#include "H2MOD\Modules\Config\Config.h"
#include "XLive\xnet\IpManagement\XnIp.h"
#include "H2MOD\Modules\Networking\NetworkStats\NetworkStats.h"
#include "H2MOD/Modules/Stats/StatsHandler.h"
#include "H2MOD/Modules/EventHandler/EventHandler.h"
#include "H2MOD/GUI/GUI.h"
#include "H2MOD/Modules/MainLoopPatches/UncappedFPS/UncappedFPS.h"
#include "H2MOD/Modules/Input/ControllerInput.h"
#include "Util/Memory.h"
#include "Blam/Engine/Game/GameTimeGlobals.h"

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

		if (H2GetInstanceId() > 1) {
			wchar_t titleMod[256];
			wchar_t titleOriginal[256];
			GetWindowText(H2hWnd, titleOriginal, 256);
			wsprintf(titleMod, L"%ls (P%d)", titleOriginal, H2GetInstanceId());
			SetWindowText(H2hWnd, titleMod);
		}
	}
	if (H2IsDediServer)
	{
		StatsHandler::verifyPlayerRanks();

	}
	EventHandler::executeGameLoopCallbacks();
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

	timeBeginPeriod(1);

	auto p_main_game_time_initialize_defaults = Memory::GetAddressRelative<void(__cdecl*)()>(0x42869F, 0x424841);
	return p_main_game_time_initialize_defaults();
}

void __cdecl game_modules_dispose() {
	auto p_game_modules_dispose = Memory::GetAddress<void(__cdecl*)()>(0x48BBF, 0x41E60);
	p_game_modules_dispose();

	DeinitH2Startup();

	// reset time resolution to system default on game exit (initialization happens in main_game_time_initialize_defaults_hook())
	timeEndPeriod(1);
}

void(*main_game_loop)();

void main_game_loop_hook() {
	if (!QuitGSMainLoop)
		GSMainLoop();

	main_game_loop();


	mapManager->leaveSessionIfAFK();

	extern void frameTimeManagement();
	frameTimeManagement();
}

static char HookedServerShutdownCheck() {
	if (!QuitGSMainLoop)
		GSMainLoop();

	bool& Quit_Exit_Game = *(bool*)((char*)H2BaseAddr + 0x4a7083);

	//original test - if game should shutdown
	return Quit_Exit_Game;
}
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

typedef float(__cdecl p_main_game_time_system_update)(char a1, float a2);
p_main_game_time_system_update* main_game_time_system_update;

typedef void(__cdecl p_render_audio)();
p_render_audio* render_audio;

typedef void(__stdcall p_sub_B1D31F)(int a1, int a2);
p_sub_B1D31F* sub_B1D31F;

typedef void(_cdecl p_sub_AF8716)(int a1);
p_sub_AF8716* sub_AF8716;



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
	if (get_game_life_cycle() != life_cycle_in_game) {
		float _a2;
		int _a3;
		game_time_globals_prep(a1, &_a2, &_a3);
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
	if (!b_sys_init)
	{
		startTime = timeGetTime();
		b_sys_init = true;
		return 1;
	}
	int currentTime = timeGetTime();
	float result = (float)(currentTime - startTime) * 0.001;
	startTime = currentTime;
	return result;
}

int* dword_F52268;
int* max_tick_count;
byte* sound_impulse_unk;
byte* sound_impulse_called;
int* dword_F52260;
byte* b_restart_game_loop;
const int input_devices_update = 0x2F9AC;
const int vibrations_update = 0x90438;
const int loaded_custom_maps_data = 0x482D70;
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
	int v9; // [esp+Ch] [ebp-38h]
	char v10; // [esp+21h] [ebp-23h]
	char Interpolate; // [esp+22h] [ebp-22h]
	bool v12; // [esp+23h] [ebp-21h]
	static float v13;// [esp+24h] [ebp-20h]
	signed int a3; // [esp+28h] [ebp-1Ch]
	float v15; // [esp+2Ch] [ebp-18h]
	static float out_dt; // [esp+30h] [ebp-14h]
	static int out_target_ticks; // [esp+34h] [ebp-10h]
	int v18; // [esp+40h] [ebp-4h]
	v1 = sub_AF87A1(); //Some sort of initializer for timing.
	a3 = v1;
	Interpolate = 1;
	if (!(*dword_F52268 & 1)) //Game loop init
	{
		*dword_F52268 |= 1u;
		*max_tick_count = 8;
		v18 = -1; //Never used
	}
	if (cinematic_is_running())
	{
		a3 = 1;
		v1 = 1;
		Interpolate = 1;
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
		Interpolate = 1;
	}
	v2 = 0;
	while (1)
	{
		v12 = v2 == 0;
		if (v2 == v1 - 1 || (v10 = 0, !v1))
			v10 = 1;
		if (v2 == 0)
		{
			sub_AD96EB();

			if (!sub_AD985E())
				return;
			sub_B09783();
		}
		if (restart_game_loop())
			break;
		__asm {
			mov eax, [input_devices_update]
			add eax, [H2BaseAddr]
			call eax
		}
		if (!game_minimized())
		{
			if (H2Config_controller_modern)
			{
				ControllerInput::procces_input();
			}
			else
			{
				sub_B328A8();
			}
		}
		if (v12)
		{
			sub_B16834();
			sub_B02590();
			sub_B5DD5C();
			sub_AD7902();
			sub_B361EC();
			sub_B1BA65();
			__asm {
				push 1
				mov ecx, [loaded_custom_maps_data]
				add ecx, [H2BaseAddr]
				mov eax, [0x4D31F]
				add eax, [H2BaseAddr]
				call eax //Call sub_B1D31F
			}
		}
		if (!game_freeze())
		{
			out_dt = 0.0;
			out_target_ticks = 0;
			if (Interpolate)
			{
				//v15 = main_game_time_system_update(0, 0.0);
				v15 = alt_system_time_update();
			}
			else
			{
				if (v1 > 0 && sub_B4BFD1())
					v3 = time_globals::get()->seconds_per_tick;
				else
					v3 = 0.0;
				a2 = v3;
				v15 = main_game_time_system_update(1, a2);
				if (*dword_F52260 < 2)
				{
					++*dword_F52260;
					sub_AF8716(1);
				}
				else
				{
					*dword_F52260 = 0;
				}
			}
			if (game_freeze())
				v5 = 0.0;
			else
				v5 = v15;
			v13 = v5;
			if (!game_minimized())
			{
				sub_CDCA7D(v15, v9);
				sub_B0A221();
			}
			if (v12)
			{
				sub_C8542F();
				sub_C853C7();
				sub_C7E7C5();
				sub_C7D83F();
			}
			if (game_in_simulation())
			{
				game_time_globals_prep(v13, &out_dt, &out_target_ticks);
				local_players_update_and_send_synchronous_actions(v13, out_dt);
				simulation_update(out_target_ticks, &out_dt);
				if (v10)
					game_network_dispatcher();
				//game_effects_update(out_dt);
				director_update(v13);
				observer_update(v13);
				//TODO: Fix Controller Vibrations
				//__asm{
				//	fstp dword ptr [v13]
				//	mov eax, [vibrations_update]
				//	add eax, [H2BaseAddr]
				//	call eax
				//}
			}
			else
			{
				sub_C7E9D3();
				game_network_dispatcher();
				vibrations_clear();
			}
			if (v10)
			{
				/*v7 = get_xbox_achievements_data();
				XAchievements(v7);*/
			}
			if (game_minimized())
			{
				vibrations_clear();
			}
			else if (v10)
			{
				v0 = system_milliseconds();
				present_rendered_screen();
				v8 = system_milliseconds() - v0;
				//DWORD* init_flags_array = h2mod->GetAddress<DWORD*>(0x46d820);
				//if (init_flags_array[2] == 0)
				//	render_audio();
				if (*sound_impulse_called)
				{
					*sound_impulse_called = 0;
					*sound_impulse_unk = 1;
					sub_AF8716(v8);
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
	double tick_time = (static_cast<double>(end_tick.QuadPart - start_tick.QuadPart) / freq.QuadPart);// -render_time;
	if (tick_time >= time_globals::get()->seconds_per_tick || !init)
	{
		QueryPerformanceCounter(&start_tick);
		if (!QuitGSMainLoop)
			GSMainLoop();
		init = true;
		DWORD* init_flags_array = h2mod->GetAddress<DWORD*>(0x46d820);
		if (init_flags_array[2] == 0)
			render_audio();
		if (game_in_simulation())
		{
			game_effects_update(time_globals::get()->seconds_per_tick);
		}
		//game_main_loop();
		//main_game_loop();

		EventHandler::executeGameLoopCallbacks();

		mapManager->leaveSessionIfAFK();
	}
	if (H2Config_fps_limit != 0) {
		QueryPerformanceCounter(&end_render);
		render_time = static_cast<double>(end_render.QuadPart - start_render.QuadPart) / freq.QuadPart;
		if (render_time >= (1.0f / H2Config_fps_limit)) {
			QueryPerformanceCounter(&start_render);
			game_main_loop();
			//present_rendered_screen();
		}
	}
	else
	{
		game_main_loop();
	}

}
void initGSRunLoop() {
	addDebugText("Pre GSRunLoop Hooking.");
	if (H2IsDediServer) {
		addDebugText("Hooking Loop & Shutdown Function");
		PatchCall(H2BaseAddr + 0xc6cb, HookedServerShutdownCheck);
	}
	else {
		addDebugText("Hooking Loop Function");
		main_game_loop = (void(*)())((char*)H2BaseAddr + 0x399CC);

		if (!H2Config_experimental_fps) {
			PatchCall(H2BaseAddr + 0x39E64, main_game_loop_hook);
		}
		else {


			sub_9AA221 = (void(*)())((char*)H2BaseAddr + 0x3A221);
			sub_C8542F = (void(*)())((char*)H2BaseAddr + 0x1B542F);
			sub_C853C7 = (void(*)())((char*)H2BaseAddr + 0x1B53C7);
			sub_C7D83F = (void(*)())((char*)H2BaseAddr + 0x1AD83F);
			sub_B02590 = (void(*)())((char*)H2BaseAddr + 0x325A2);
			sub_B0A221 = (void(*)())((char*)H2BaseAddr + 0x3A221);
			sub_C7E9D3 = (void(*)())((char*)H2BaseAddr + 0x1AE9D3);
			sub_AD7902 = (void(*)())((char*)H2BaseAddr + 0x7902);
			sub_AD96EB = (void(*)())((char*)H2BaseAddr + 0x96EB);
			sub_B09783 = (void(*)())((char*)H2BaseAddr + 0x39783);
			sub_B727EB = (void(*)())((char*)H2BaseAddr + 0xA27EB);

			sub_C7E7C5 = h2mod->GetAddress<p_sub_C7E7C5*>(0x1AE7C5);
			sub_B328A8 = h2mod->GetAddress<p_sub_B328A8*>(0x628A8);
			sub_B5DD5C = h2mod->GetAddress<p_sub_B5DD5C*>(0x8DD5C);
			sub_B16834 = h2mod->GetAddress<p_sub_B16834*>(0x46834);
			sub_B1BA65 = h2mod->GetAddress<p_sub_B1BA65*>(0x4BA65);
			sub_B361EC = h2mod->GetAddress<p_sub_B361EC*>(0x661EC);
			sub_AF87A1 = h2mod->GetAddress<p_sub_AF87A1*>(0x287A1);
			sub_AD985E = h2mod->GetAddress<p_sub_AD985E*>(0x985E);
			sub_B4BFD1 = h2mod->GetAddress<p_sub_B4BFD1*>(0x7BFD1);
			sub_9A96B1 = h2mod->GetAddress<p_sub_9A96B1*>(0x396B1);
			sub_CDCA7D = h2mod->GetAddress<p_sub_B7CA7D*>(0x20CA7D);
			sub_AF8716 = h2mod->GetAddress<p_sub_AF8716*>(0x28716);
			sub_B1D31F = h2mod->GetAddress<p_sub_B1D31F*>(0x4D31F);



			vibrations_clear = (void(*)())((char*)H2BaseAddr + 0x901B8);
			game_network_dispatcher = (void(*)())((char*)H2BaseAddr + 0x1B5456);
			restart_game_loop = h2mod->GetAddress<p_restart_game_loop*>(0x286E1);
			game_time_globals_prep = h2mod->GetAddress<p_game_time_globals_prep*>(0x7C1BF);
			present_rendered_screen = h2mod->GetAddress<p_present_rendered_screen*>(0x27002A);
			game_in_simulation = h2mod->GetAddress<p_game_in_simulation*>(0x1ADD30);
			game_freeze = h2mod->GetAddress<p_game_freeze*>(0x145B);
			game_minimized = h2mod->GetAddress<p_game_minimized*>(0x28729);
			render_audio = h2mod->GetAddress<p_render_audio*>(0x2DF87);
			system_milliseconds = h2mod->GetAddress<p_system_milliseconds*>(0x37E51);
			cinematic_in_progress = h2mod->GetAddress<c_cinematic_in_progress*>(0x3a928);
			cinematic_is_running = h2mod->GetAddress<c_cinematic_is_running*>(0x3a938);
			observer_update = h2mod->GetAddress<p_observer_update*>(0x83E6A);
			local_players_update_and_send_synchronous_actions = h2mod->GetAddress<p_local_players_update_and_send_synchronous_actions*>(0x93857);
			simulation_update = h2mod->GetAddress<p_simulation_update*>(0x4A5D0);
			game_effects_update = h2mod->GetAddress<p_game_effects_update*>(0x48CDC);
			director_update = h2mod->GetAddress<p_director_update*>(0x5A658);
			main_game_time_system_update = h2mod->GetAddress<p_main_game_time_system_update*>(0x28814);



			dword_F52268 = h2mod->GetAddress<int*>(0x482268);
			max_tick_count = h2mod->GetAddress<int*>(0x482264);
			sound_impulse_unk = h2mod->GetAddress<byte*>(0x48225B);
			sound_impulse_called = h2mod->GetAddress<byte*>(0x48225A);
			dword_F52260 = h2mod->GetAddress<int*>(0x482260);
			b_restart_game_loop = h2mod->GetAddress<byte*>(0x479EA0);

			//PatchCall(h2mod->GetAddress(0x39D04), alt_prep_time);
			PatchCall(H2BaseAddr + 0x39E64, alt_main_game_loop_hook);
			//PatchCall(H2BaseAddr + 0x39e64, game_main_loop);
			QueryPerformanceFrequency(&freq);
			//Remove original render call
			NopFill(h2mod->GetAddress(0x39DAA), 5);
		}
	}

	PatchCall(Memory::GetAddressRelative(0x439E3D, 0x40BA40), main_game_time_initialize_defaults_hook);
	PatchCall(Memory::GetAddress(0x39E7C, 0xC6F7), game_modules_dispose);

	addDebugText("Post GSRunLoop Hooking.");
}

void deinitGSRunLoop() {

}