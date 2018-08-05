#include "H2Tweaks.h"
#include "H2Startup.h"
#include "H2Config.h"
#include "GSUtils.h"
#include "Hook.h"
#include "H2OnscreenDebugLog.h"
#include "GSCustomMenu.h"
#include "H2MOD.h";

#define _USE_MATH_DEFINES
#include "math.h"

#pragma region Done_Tweaks

typedef int(__cdecl *thookServ1)(HKEY, LPCWSTR);
thookServ1 phookServ1;
int __cdecl LoadRegistrySettings(HKEY hKey, LPCWSTR lpSubKey) {
	char result =
		phookServ1(hKey, lpSubKey);
	addDebugText("Post Server Registry Read.");
	if (strlen(H2Config_dedi_server_name) > 0) {
		wchar_t* PreLoadServerName = (wchar_t*)((BYTE*)H2BaseAddr + 0x3B49B4);
		swprintf(PreLoadServerName, 15, L"%hs", H2Config_dedi_server_name);
		//char temp[27];
		//snprintf(temp, 27, "%ws", dedi_server_name);
		//MessageBoxA(NULL, temp, "Server Pre name thingy", MB_OK);
		wchar_t* LanServerName = (wchar_t*)((BYTE*)H2BaseAddr + 0x52042A);
		swprintf(LanServerName, 2, L"");
	}
	if (strlen(H2Config_dedi_server_playlist) > 0) {
		wchar_t* ServerPlaylist = (wchar_t*)((BYTE*)H2BaseAddr + 0x3B3704);
		swprintf(ServerPlaylist, 256, L"%hs", H2Config_dedi_server_playlist);
	}
	return result;
}

typedef int(__stdcall *thookServ2)();
thookServ2 phookServ2;
int __stdcall PreReadyLoad() {
	MessageBoxA(NULL, "aaaaaaas", "PreCrash", MB_OK);
	int result =
		phookServ2();
	MessageBoxA(NULL, "aaaaaaasaaaaaa", "PostCrash", MB_OK);
	if (strlen(H2Config_dedi_server_name) > 0) {
		wchar_t* PreLoadServerName = (wchar_t*)((BYTE*)H2BaseAddr + 0x52042A);
		swprintf(PreLoadServerName, 32, L"%hs", H2Config_dedi_server_name);
		char temp[27];
		snprintf(temp, 32, H2Config_dedi_server_name);
		MessageBoxA(NULL, temp, "AAA Server Pre name thingy", MB_OK);
	}
	return result;
}

static bool NotDisplayIngameChat() {
	int GameGlobals = (int)*(int*)((DWORD)H2BaseAddr + 0x482D3C);
	DWORD* GameEngine = (DWORD*)(GameGlobals + 0x8);
	BYTE* GameState = (BYTE*)((DWORD)H2BaseAddr + 0x420FC4);

	if (H2Config_hide_ingame_chat) {
		int GameTimeGlobals = (int)*(int*)((DWORD)H2BaseAddr + 0x4C06E4);
		DWORD* ChatOpened = (DWORD*)(GameTimeGlobals + 0x354);//MP Only?
		if (*ChatOpened == 2) {
			extern void hotkeyFuncToggleHideIngameChat();
			hotkeyFuncToggleHideIngameChat();
		}
	}

	if (H2Config_hide_ingame_chat) {
		return true;
	}
	else if (*GameEngine != 3 && *GameState == 3) {
		//Enable chat in engine mode and game state mp.
		return false;
	}
	else {
		//original test - if is campaign
		return *GameEngine == 1;
	}
}

typedef char(__cdecl *thookChangePrivacy)(int);
thookChangePrivacy phookChangePrivacy;
char __cdecl HookChangePrivacy(int privacy) {
	char result =
		phookChangePrivacy(privacy);
	if (result == 1 && privacy == 0) {
		pushHostLobby();
	}
	return result;
}


void postConfig() {

	wchar_t mutexName2[255];
	swprintf(mutexName2, ARRAYSIZE(mutexName2), L"Halo2BasePort#%d", H2Config_base_port);
	HANDLE mutex2 = CreateMutex(0, TRUE, mutexName2);
	DWORD lastErr2 = GetLastError();
	if (lastErr2 == ERROR_ALREADY_EXISTS) {
		char NotificationPlayerText[120];
		sprintf(NotificationPlayerText, "Base port %d is already bound to!\nExpect MP to not work!", H2Config_base_port);
		addDebugText(NotificationPlayerText);
		MessageBoxA(NULL, NotificationPlayerText, "BASE PORT BIND WARNING!", MB_OK);
	}
	char NotificationText5[120];
	sprintf(NotificationText5, "Base port: %d.", H2Config_base_port);
	addDebugText(NotificationText5);

	RefreshTogglexDelay();
}

#pragma endregion

int(__cdecl* sub_20E1D8)(int, int, int, int, int, int);

int __cdecl sub_20E1D8_boot(int a1, int a2, int a3, int a4, int a5, int a6) {
	//a2 == 0x5 - system link lost connection
	if (a2 == 0xb9) {
		//boot them offline.
		H2Config_master_ip = inet_addr("127.0.0.1");
		H2Config_master_port_relay = 2001;
		extern int MasterState;
		MasterState = 2;
		extern char* ServerStatus;
		snprintf(ServerStatus, 250, "Status: Offline");
	}
	int result = sub_20E1D8(a1, a2, a3, a4, a5, a6);
	return result;
}


#pragma region SLDL_Hack
//Taken from another project by Glitchy Scripts.
#define GA_H2C 0x10
#define GA_H2D 0x20
#define GA_FN 0x2
#define GA_VAR 0x4
BYTE* GetAddress(DWORD type, DWORD offset) {
	if (!(type & (GA_FN | GA_VAR) && type & GA_H2C)) {
		//Oops, bad offset.
		__debugbreak();
	}
	offset += (DWORD)H2BaseAddr;
	return (BYTE*)offset;
}

typedef bool(*tfn_c00004a6b)();
tfn_c00004a6b pfn_c00004a6b;
bool fn_c00004a6b()
{
	// Since fn_c00004567 has been reversed and the anti-hack voided, this function (which appears to be the game's weird anti-hack initializer) is no longer required.
	bool result = true;//pfn_c00004a6b();
	return result;
}

typedef void(__stdcall *tfn_c00030aa6)(void*);
tfn_c00030aa6 pfn_c00030aa6;
void __stdcall fn_c00030aa6_game_state_initialize(void* thisptr) //__thiscall
{
	//pfn_c00030aa6(thisptr);
	//return;

	BYTE& var_c0047a728 = *(BYTE*)(GetAddress(GA_VAR | GA_H2C, 0x0047a728));
	DWORD& var_c0047cd34 = *(DWORD*)(GetAddress(GA_VAR | GA_H2C, 0x0047cd34));
	DWORD& var_c0047cd28 = *(DWORD*)(GetAddress(GA_VAR | GA_H2C, 0x0047cd28));
	DWORD& var_c0047cd2c = *(DWORD*)(GetAddress(GA_VAR | GA_H2C, 0x0047cd2c));
	DWORD& var_c0047cd3c = *(DWORD*)(GetAddress(GA_VAR | GA_H2C, 0x0047cd3c));
	DWORD& var_c0047cd48 = *(DWORD*)(GetAddress(GA_VAR | GA_H2C, 0x0047cd48));
	DWORD& var_c0039dfd8 = *(DWORD*)(GetAddress(GA_VAR | GA_H2C, 0x0039dfd8));//FN ptr

	DWORD*(__cdecl* fn_c0008bc27)(DWORD*) = (DWORD*(__cdecl*)(DWORD*))(GetAddress(GA_FN | GA_H2C, 0x0008bc27));
	DWORD*(__cdecl* fn_c0008b703)(int, int) = (DWORD*(__cdecl*)(int, int))(GetAddress(GA_FN | GA_H2C, 0x0008b703));
	int(__cdecl* fn_c00287ba9)(signed int, int, unsigned int) = (int(__cdecl*)(signed int, int, unsigned int))(GetAddress(GA_FN | GA_H2C, 0x00287ba9));
	//0x0008b897 is gonna be a challenge to reverse. The rest look simple.
	char(*fn_c0008b897)() = (char(*)())(GetAddress(GA_FN | GA_H2C, 0x0008b897));
	unsigned int(__cdecl* fn_c0008bc69)(unsigned int*, BYTE*, int) = (unsigned int(__cdecl*)(unsigned int*, BYTE*, int))(GetAddress(GA_FN | GA_H2C, 0x0008bc69));

	if (!var_c0047a728)
	{
		fn_c0008bc27(&var_c0047cd34);
		var_c0047cd28 = (DWORD)fn_c0008b703(0x3BE000, 0x40000);
		fn_c00287ba9((signed int)var_c0047cd28, 0, 0x3FE000u);
		fn_c0008b897();
		char* v1 = (char*)var_c0047cd28 + var_c0047cd2c;
		thisptr = (void*)0x12F8;
		var_c0047cd2c += 0x12F8;
		fn_c0008bc69((unsigned int*)&var_c0047cd34, (BYTE*)&thisptr, 4);
		var_c0047cd3c = (int)v1;
		DWORD* v2 = (DWORD*)(var_c0047cd28 + var_c0047cd2c);
		var_c0047a728 = 1;
		thisptr = (void*)4;
		var_c0047cd2c += 4;
		fn_c0008bc69((unsigned int*)&var_c0047cd34, (BYTE*)&thisptr, 4);
		var_c0047cd48 = (int)v2;
		if (v2)
			*v2 = (DWORD)&var_c0039dfd8;
	}
}

typedef bool(*tfn_c00004567)();
tfn_c00004567 pfn_c00004567;

enum flags : int
{
	windowed,
	unk,
	nosound,
	unk1,
	unk2,
	novsync,
	unk3,
	unk4,
	unk5,
	unk6,
	unk7,
	unk8,
	unk9,
	unk10,
	unk11,
	unk12,
	unk13,
	unk14,
	unk15,
	unk16,
	unk17,
	unk18,
	monitor,
	unk19,
	unk20,
	unk21,
	unk22,
	unk23,
	high_quality,
	unk24,

	count
};
static_assert(flags::count == 30, "Bad flags count");

bool fn_c00004567()
{
	typedef DWORD(*t_flags_array)[flags::count];
	auto flags_array = reinterpret_cast<t_flags_array>(H2BaseAddr + 0x0046d820);
	memset(flags_array, 0x00, sizeof(flags::count));

	DWORD& var_c004ae8e0 = *(DWORD*)(GetAddress(GA_VAR | GA_H2C, 0x004ae8e0));
	bool(*fn_c00202f3e)() = (bool(*)())(GetAddress(GA_FN | GA_H2C, 0x00202f3e));
	HANDLE(*fn_c000388d3)() = (HANDLE(*)())(GetAddress(GA_FN | GA_H2C, 0x000388d3));
	int(*fn_c0003844e)() = (int(*)())(GetAddress(GA_FN | GA_H2C, 0x0003844e));
	void*(*fn_c00037ed5_runtime_state_initialize_cseries_initialize)() = (void*(*)())(GetAddress(GA_FN | GA_H2C, 0x00037ed5));
	wchar_t*(__cdecl* fn_c00001014_CommandLineToArgvW)(wchar_t**, int, int*) = (wchar_t*(__cdecl*)(wchar_t**, int, int*))(GetAddress(GA_FN | GA_H2C, 0x00001014));
	DWORD(__cdecl* fn_c00037e39_init_timing)(int) = (DWORD(__cdecl*)(int))(GetAddress(GA_FN | GA_H2C, 0x00037e39));
	bool(*fn_c00004994_shell_platform_initialize)() = (bool(*)())(GetAddress(GA_FN | GA_H2C, 0x00004994));
	int(*fn_c000340d7_real_math_initialize)() = (int(*)())(GetAddress(GA_FN | GA_H2C, 0x000340d7));
	bool(*fn_c00032ce5_async_initialize)() = (bool(*)())(GetAddress(GA_FN | GA_H2C, 0x00032ce5));
	void(*fn_c0003285c_global_preferences_initialize)() = (void(*)())(GetAddress(GA_FN | GA_H2C, 0x0003285c));
	int(*fn_c00031dff_font_initialize)() = (int(*)())(GetAddress(GA_FN | GA_H2C, 0x00031dff));
	bool(*fn_c00030d58_tag_files_open)() = (bool(*)())(GetAddress(GA_FN | GA_H2C, 0x00030d58));
	//void(__stdcall* fn_c00030aa6_game_state_initialize)(void*) = (void(__stdcall*)(void*))(GetAddress(GA_FN | GA_H2C, 0x00030aa6));
	char(*fn_c001a9de6)() = (char(*)())(GetAddress(GA_FN | GA_H2C, 0x001a9de6));
	char(*fn_c00263359_rasterizer_initialize)() = (char(*)())(GetAddress(GA_FN | GA_H2C, 0x00263359));
	HANDLE(*fn_c000285fd)() = (HANDLE(*)())(GetAddress(GA_FN | GA_H2C, 0x000285fd));
	char(*fn_c0002fd23_input_initialize)() = (char(*)())(GetAddress(GA_FN | GA_H2C, 0x0002fd23));
	HANDLE(*fn_c0002979e_sound_initialize)() = (HANDLE(*)())(GetAddress(GA_FN | GA_H2C, 0x0002979e));

	bool result_c00202f3e = fn_c00202f3e();
	HANDLE result_c000388d3 = fn_c000388d3();
	int result_c0003844e = fn_c0003844e();
	void* result_c00037ed5 = fn_c00037ed5_runtime_state_initialize_cseries_initialize();

	wchar_t* cmd_line_args = GetCommandLineW();
	if (cmd_line_args) {
		wchar_t cmd_line_args_split[0x8000] = L"";
		wcsncpy_s(cmd_line_args_split, 0x8000, cmd_line_args, 0xFFFFFFFF);

		wchar_t* cmd_line_args_ptr[1024] = { 0 };
		memset(cmd_line_args_ptr, 0, 1024 * sizeof(wchar_t*));

		int args_str_length = 0;
		DWORD arg_c00001014_eax = (DWORD)cmd_line_args_split;
		DWORD arg_c00001014_1 = (DWORD)cmd_line_args_ptr;
		DWORD arg_c00001014_3 = (DWORD)&args_str_length;
		__asm {
			push arg_c00001014_3
			push 1024
			push arg_c00001014_1
			mov eax, arg_c00001014_eax
			call fn_c00001014_CommandLineToArgvW
			add esp, 0xC
		}

		for (int i = 0; i < args_str_length; i++) {
			wchar_t* cmd_line_arg = cmd_line_args_ptr[i];

			if (wcsicmp(cmd_line_arg, L"-windowed") == 0) {
				*flags_array[flags::windowed] = 1;
			}
			else if (wcsicmp(cmd_line_arg, L"-nosound") == 0) {
				*flags_array[flags::nosound] = 1;
			}
			else if (wcsicmp(cmd_line_arg, L"-novsync") == 0) {
				*flags_array[flags::novsync] = 1;
			}
			else if (wcsnicmp(cmd_line_arg, L"-monitor:", 9) == 0) {
				long(__cdecl* fn_c002876f4_wtol)(wchar_t const*) = (long(__cdecl*)(wchar_t const*))(GetAddress(GA_FN | GA_H2C, 0x002876f4));
				int monitor_num_char = fn_c002876f4_wtol(&cmd_line_arg[9]);
				if (monitor_num_char < 0)
					monitor_num_char = 0;
				if (monitor_num_char > 9)
					monitor_num_char = 9;
				*flags_array[flags::monitor] = monitor_num_char;
			}
			else if (wcsicmp(cmd_line_arg, L"-highquality") == 0) {
				*flags_array[flags::high_quality] = 1;
			}
		}
	}

	if (*flags_array[flags::nosound]) {
		void(*fn_c00028b83)() = (void(*)())(GetAddress(GA_FN | GA_H2C, 0x00028b83));
		fn_c00028b83();
	}

	bool result_c00004994 = fn_c00004994_shell_platform_initialize();
	if (!result_c00004994)
		return false;
	if (*flags_array[flags::unk22])
		fn_c00037e39_init_timing(*flags_array[flags::unk22]);
	int result_c000340d7 = fn_c000340d7_real_math_initialize();
	bool result_c00032ce5 = fn_c00032ce5_async_initialize();
	fn_c0003285c_global_preferences_initialize();
	int result_c00031dff = fn_c00031dff_font_initialize();
	bool result_c00030d58 = fn_c00030d58_tag_files_open();
	if (!result_c00030d58)
		return result_c00030d58;
	fn_c00030aa6_game_state_initialize((void*)var_c004ae8e0);//A Windows Media Center exit corruption is due to this somehow. If using my reversed version the problem goes away.
	char result_c001a9de6 = fn_c001a9de6();
	char result_c00263359 = fn_c00263359_rasterizer_initialize();
	if (!result_c00263359)
		return result_c00263359;

	struct FakePBuffer {
		HANDLE id;
		DWORD dwSize;
		DWORD magic;
		LPBYTE pbData;
	};
	//extern LONG WINAPI XLivePBufferAllocate(DWORD size, FakePBuffer **pBuffer);
	//extern DWORD WINAPI XLivePBufferSetByte(FakePBuffer * pBuffer, DWORD offset, BYTE value);
	LONG(__stdcall* XLivePBufferAllocate)(DWORD size, FakePBuffer **pBuffer) = (LONG(__stdcall*)(DWORD, FakePBuffer**))(GetAddress(GA_FN | GA_H2C, 0x0000e886));
	DWORD(__stdcall* XLivePBufferSetByte)(FakePBuffer * pBuffer, DWORD offset, BYTE value) = (DWORD(__stdcall*)(FakePBuffer*, DWORD, BYTE))(GetAddress(GA_FN | GA_H2C, 0x0000e880));
	DWORD& var_c00479e78 = *(DWORD*)(GetAddress(GA_VAR | GA_H2C, 0x00479e78));
	XLivePBufferAllocate(2, (FakePBuffer**)&var_c00479e78);
	XLivePBufferSetByte((FakePBuffer*)var_c00479e78, 0, 0);
	XLivePBufferSetByte((FakePBuffer*)var_c00479e78, 1, 0);

	//SLDLInitialize

	HANDLE result_c000285fd = fn_c000285fd();

	//SLDLOpen
	//SLDLConsumeRight
	//SLDLClose
	// This call would disable the multiplayer buttons in the mainmenu. Likely setup this way from SLDL.
	//XLivePBufferSetByte((FakePBuffer*)var_c00479e78, 0, 1);

	char result_c0002fd23 = fn_c0002fd23_input_initialize();
	HANDLE result_c0002979e = fn_c0002979e_sound_initialize();

	return true;
}
#pragma endregion


void InitH2Tweaks() {
	postConfig();

	addDebugText("Begin Startup Tweaks.");

	if (H2IsDediServer) {
		DWORD dwBack;

		phookServ1 = (thookServ1)DetourFunc((BYTE*)H2BaseAddr + 0x8EFA, (BYTE*)LoadRegistrySettings, 11);
		VirtualProtect(phookServ1, 4, PAGE_EXECUTE_READWRITE, &dwBack);

		//phookServ2 = (thookServ2)DetourFunc((BYTE*)H2BaseAddr + 0xBA3C, (BYTE*)PreReadyLoad, 11);
		//VirtualProtect(phookServ2, 4, PAGE_EXECUTE_READWRITE, &dwBack);
	}
	else {//is client

		DWORD dwBack;
		//Hook a function which changes the party privacy to detect if the lobby becomes open.
		//Problem is if you want to set it via mem poking, it won't push the lobby to the master automatically.
		//phookChangePrivacy = (thookChangePrivacy)DetourFunc((BYTE*)H2BaseAddr + 0x2153ce, (BYTE*)HookChangePrivacy, 11);
		//VirtualProtect(phookChangePrivacy, 4, PAGE_EXECUTE_READWRITE, &dwBack);

		//Scrapped for now, maybe.
		DWORD tempResX = 0;
		DWORD tempResY = 0;

		HKEY hKeyResolution = NULL;
		if (RegOpenKeyExW(HKEY_CURRENT_USER, L"Software\\Microsoft\\Halo 2\\Video Settings", 0, KEY_READ, &hKeyResolution) == ERROR_SUCCESS) {
			GetDWORDRegKey(hKeyResolution, L"ScreenResX", &tempResX);
			GetDWORDRegKey(hKeyResolution, L"ScreenResY", &tempResY);
			RegCloseKey(hKeyResolution);
		}

		if (H2Config_custom_resolution_x > 0 && H2Config_custom_resolution_y > 0) {
			if (H2Config_custom_resolution_x != (int)tempResX || H2Config_custom_resolution_y != (int)tempResY) {
				LSTATUS err;
				if (err = RegOpenKeyExW(HKEY_CURRENT_USER, L"Software\\Microsoft\\Halo 2\\Video Settings", 0, KEY_ALL_ACCESS, &hKeyResolution) == ERROR_SUCCESS) {
					RegSetValueEx(hKeyResolution, L"ScreenResX", NULL, REG_DWORD, (const BYTE*)&H2Config_custom_resolution_x, sizeof(H2Config_custom_resolution_x));
					RegSetValueEx(hKeyResolution, L"ScreenResY", NULL, REG_DWORD, (const BYTE*)&H2Config_custom_resolution_y, sizeof(H2Config_custom_resolution_y));
					RegCloseKey(hKeyResolution);
				}
				else {
					char errorMsg[200];
					sprintf(errorMsg, "Error: 0x%x. Unable to make Screen Resolution changes.\nPlease try restarting Halo 2 with Administrator Privileges.", err);
					addDebugText(errorMsg);
					MessageBoxA(NULL, errorMsg, "Registry Write Error", MB_OK);
					exit(EXIT_FAILURE);
				}
			}
		}

		bool IntroHQ = true;//clients should set on halo2.exe -highquality

		if (H2Config_skip_intro) {
			BYTE assmIntroSkip[] = { 0x3F };
			WriteBytes(H2BaseAddr + 0x221C0E, assmIntroSkip, 1);
		}

		if (!H2Config_skip_intro && IntroHQ) {
			BYTE assmIntroHQ[] = { 0xEB };
			WriteBytes(H2BaseAddr + 0x221C29, assmIntroHQ, 1);
		}

		//Allows unlimited clients
		BYTE assmUnlimitedClients[41];
		memset(assmUnlimitedClients, 0x00, 41);
		WriteBytes(H2BaseAddr + 0x39BCF0, assmUnlimitedClients, 41);

		//Allows on a remote desktop connection
		BYTE assmRemoteDesktop[] = { 0xEB };
		WriteBytes(H2BaseAddr + 0x7E54, assmRemoteDesktop, 1);

		//Disables the ESRB warning (only occurs for English Language).
		//disables the one if no intro vid occurs.
		BYTE& ESRB = *((BYTE*)H2BaseAddr + 0x411030);
		ESRB = 0;
		//disables the one after the intro video.
		BYTE assmIntroESRBSkip[] = { 0x00 };
		WriteBytes(H2BaseAddr + 0x3a0fa, assmIntroESRBSkip, 1);
		WriteBytes(H2BaseAddr + 0x3a1ce, assmIntroESRBSkip, 1);

		//Redirects the is_campaign call that the in-game chat renderer makes so we can show/hide it as we like.
		PatchCall(H2BaseAddr + 0x22667B, NotDisplayIngameChat);
		PatchCall(H2BaseAddr + 0x226628, NotDisplayIngameChat);

		//hook the gui popup for when the player is booted.
		sub_20E1D8 = (int(__cdecl*)(int, int, int, int, int, int))((char*)H2BaseAddr + 0x20E1D8);
		PatchCall(H2BaseAddr + 0x21754C, &sub_20E1D8_boot);

		//SLDL anti-hack removed and code is now accessible.
		//Side effect is that the multiplayer buttons at the mainmenu will never be greyed out due to a bad 'key'.
		pfn_c00004a6b = (tfn_c00004a6b)DetourFunc((BYTE*)H2BaseAddr + 0x00004a6b, (BYTE*)fn_c00004a6b, 5);
		VirtualProtect(pfn_c00004a6b, 4, PAGE_EXECUTE_READWRITE, &dwBack);
		pfn_c00004567 = (tfn_c00004567)DetourFunc((BYTE*)H2BaseAddr + 0x00004567, (BYTE*)fn_c00004567, 7);
		VirtualProtect(pfn_c00004567, 4, PAGE_EXECUTE_READWRITE, &dwBack);
	}
	addDebugText("End Startup Tweaks.");
}

void DeinitH2Tweaks() {

}

void setSens(short input_type, int sens) {

	if (H2IsDediServer)
		return;

	if (sens == 0)
		return; //return if sensitivity is 0

	if (input_type == 1) { //controller
		*(float*)(H2BaseAddr + 0x4A89BC) = (float)(40.0 + 10.0 * (double)sens); //y-axis
		*(float*)(H2BaseAddr + 0x4A89B8) = (float)(80.0 + 20.0 * (double)sens); //x-axis
	}
	else if (input_type == 0) { //mouse 
		*(float*)(H2BaseAddr + 0x4A89B4) = (float)(25.0 + 10.0 * (double)sens); //y-axis
		*(float*)(H2BaseAddr + 0x4A89B0) = (float)(50.0 + 20.0 * (double)sens); //x-axis
	}
}

void H2Tweaks::setFOV(int field_of_view_degrees) {

	if (H2IsDediServer)
		return;

	if (field_of_view_degrees > 0 && field_of_view_degrees <= 110)
	{
		float current_FOV = *(float*)(H2BaseAddr + 0x41D984);

		//int res_width = *(int*)(H2BaseAddr + 0xA3DA00); //wip
		//int res_height = *(int*)(H2BaseAddr + 0xA3DA04);

		const float default_radians_FOV = 70.0f * M_PI / 180.0;

		float calculated_radians_FOV = ((float)field_of_view_degrees * M_PI / 180.0) / default_radians_FOV;
		*(float*)(H2BaseAddr + 0x41D984) = calculated_radians_FOV; //First Person
		*(float*)(H2BaseAddr + 0x413780) = calculated_radians_FOV * 1.24f; //Vehicle
	}
}

void H2Tweaks::setCrosshairPos(float crosshair_offset) {

	if (H2IsDediServer)
		return;

	if (!FloatIsNaN(crosshair_offset)) {
		DWORD CrosshairY = *(DWORD*)(H2BaseAddr + 0x479E70) + 0x1AF4 + 0xF0 + 0x1C;
		*(float*)CrosshairY = crosshair_offset;
	}
}


void H2Tweaks::setCrosshairSize(int size) {

	
	if (H2IsDediServer)
		return;
	DWORD BATRIF1 = *(DWORD*)(H2BaseAddr + 0x479E70) + 0x7aa750;
	DWORD BATRIF2 = *(DWORD*)(H2BaseAddr + 0x479E70) + 0x7aa752;
	DWORD SMG1 = *(DWORD*)(H2BaseAddr + 0x479E70) + 0x7A9F9C;
	DWORD SMG2 = *(DWORD*)(H2BaseAddr + 0x479E70) + 0x7A9F9E;
	DWORD CRBN1 = *(DWORD*)(H2BaseAddr + 0x479E70) + 0x7ab970;
	DWORD CRBN2 = *(DWORD*)(H2BaseAddr + 0x479E70) + 0x7ab972;
	DWORD BEAMRIF1 = *(DWORD*)(H2BaseAddr + 0x479E70) + 0x7AA838;
	DWORD BEAMRIF2 = *(DWORD*)(H2BaseAddr + 0x479E70) + 0x7AA83A;
	DWORD MAG1 = *(DWORD*)(H2BaseAddr + 0x479E70) + 0x7AA33C;
	DWORD MAG2 = *(DWORD*)(H2BaseAddr + 0x479E70) + 0x7AA33E;
	DWORD PLASRIF1 = *(DWORD*)(H2BaseAddr + 0x479E70) + 0x7AA16C;
	DWORD PLASRIF2 = *(DWORD*)(H2BaseAddr + 0x479E70) + 0x7AA16E;
	DWORD SHTGN1 = *(DWORD*)(H2BaseAddr + 0x479E70) + 0x7AA424;
	DWORD SHTGN2 = *(DWORD*)(H2BaseAddr + 0x479E70) + 0x7AA426;
	DWORD SNIP1 = *(DWORD*)(H2BaseAddr + 0x479E70) + 0x7AA994;
	DWORD SNIP2 = *(DWORD*)(H2BaseAddr + 0x479E70) + 0x7AA996;
	DWORD SWRD1 = *(DWORD*)(H2BaseAddr + 0x479E70) + 0x7AA8AC;
	DWORD SWRD2 = *(DWORD*)(H2BaseAddr + 0x479E70) + 0x7AA8AE;
	DWORD ROCKLAUN1 = *(DWORD*)(H2BaseAddr + 0x479E70) + 0x7AA3B0;
	DWORD ROCKLAUN2 = *(DWORD*)(H2BaseAddr + 0x479E70) + 0x7AA3B2;
	DWORD PLASPI1 = *(DWORD*)(H2BaseAddr + 0x479E70) + 0x7AA0F8;
	DWORD PLASPI2 = *(DWORD*)(H2BaseAddr + 0x479E70) + 0x7AA0FA;
	DWORD BRUTESHOT1 = *(DWORD*)(H2BaseAddr + 0x479E70) + 0x7AA7C4;
	DWORD BRUTESHOT2 = *(DWORD*)(H2BaseAddr + 0x479E70) + 0x7AA7C6;
	DWORD NEED1 = *(DWORD*)(H2BaseAddr + 0x479E70) + 0x7AA254;
	DWORD NEED2 = *(DWORD*)(H2BaseAddr + 0x479E70) + 0x7AA256;
	DWORD SENTBEAM1 = *(DWORD*)(H2BaseAddr + 0x479E70) + 0x7AB5D0;
	DWORD SENTBEAM2 = *(DWORD*)(H2BaseAddr + 0x479E70) + 0x7AB5D2;

	DWORD WEAPONS[] = {BATRIF1, BATRIF2, SMG1, SMG2, CRBN1, CRBN2, BEAMRIF1, BEAMRIF2, MAG1, MAG2, PLASRIF1, PLASRIF2, SHTGN1, SHTGN2, SNIP1, SNIP2, SWRD1, SWRD2, ROCKLAUN1, ROCKLAUN2, PLASPI1, PLASPI2, BRUTESHOT1, BRUTESHOT2, NEED1, NEED2, SENTBEAM1, SENTBEAM2};
	
	int disabled[] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
	int verySmall[] = { 30, 30, 40, 40, 39, 26, 26, 10, 25, 25, 45, 45, 65, 65, 12, 12, 55, 53, 63, 63, 53, 45, 51, 62, 56, 17, 35, 19};
	int defaultSize[] = { 70, 70, 110, 110, 78, 52, 26, 10, 50, 50, 90, 90,110, 110, 20, 20, 110, 106, 126, 126, 106, 91, 102, 124, 112, 34, 70, 38 };
	int small[] = { 40, 40, 65, 65, 57, 38, 26, 10, 35, 35, 55, 55, 80, 80, 15, 15, 82, 79, 90, 90, 79, 68, 76, 93, 84, 25, 52, 27};
	int large[] = { 80, 80, 130, 130, 114, 76, 52, 20, 70, 70, 110, 110, 160, 160, 30, 30, 164, 158, 180, 180, 158, 136, 152, 186, 168, 50, 104, 57 };
	int* tempArray;
	if (h2mod->get_engine_type() == EngineType::MULTIPLAYER_ENGINE) {
		switch (size) {
		case 1:
			tempArray = disabled;
			break;
		case 2:
			tempArray = verySmall;
			break;
		case 3:
			tempArray = small;
			break;
		case 4:
			tempArray = large;
			break;
		default:
			tempArray = defaultSize;
			break;
		}
		for (int i = 0; i < 28; i++) {
			*(int*)WEAPONS[i] = tempArray[i];
		}
	}

}

void H2Tweaks::applyHitfix() {
	//at some point we need to find a better way to fix this crap

	int offset = 0x47CD54;
	//TRACE_GAME("[h2mod] Hitfix is being run on Client!");
	if (H2IsDediServer) {
		offset = 0x4A29BC;
		//TRACE_GAME("[h2mod] Hitfix is actually being run on the Dedicated Server!");
	}

	DWORD AddressOffset = *(DWORD*)(H2BaseAddr + offset);

	*(float*)(AddressOffset + 0xA4EC88) = 1200.0f; // battle_rifle_bullet.proj Initial Velocity 
	*(float*)(AddressOffset + 0xA4EC8C) = 1200.0f; //battle_rifle_bullet.proj Final Velocity
	*(float*)(AddressOffset + 0xB7F914) = 4000.0f; //sniper_bullet.proj Initial Velocity
	*(float*)(AddressOffset + 0xB7F918) = 4000.0f; //sniper_bullet.proj Final Velocity
	//FIXME COOP will break because of one of these tags not existing.
	*(float*)(AddressOffset + 0xCE4598) = 4000.0f; //beam_rifle_beam.proj Initial Velocity
	*(float*)(AddressOffset + 0xCE459C) = 4000.0f; //beam_rifle_beam.proj Final Velocity
	*(float*)(AddressOffset + 0x81113C) = 200.0f; //gauss_turret.proj Initial Velocity def 90
	*(float*)(AddressOffset + 0x811140) = 200.0f; //gauss_turret.proj Final Velocity def 90
	*(float*)(AddressOffset + 0x97A194) = 800.0f; //magnum_bullet.proj initial def 400
	*(float*)(AddressOffset + 0x97A198) = 800.0f; //magnum_bullet.proj final def 400
	*(float*)(AddressOffset + 0x7E7E20) = 2000.0f; //bullet.proj (chaingun) initial def 800
	*(float*)(AddressOffset + 0x7E7E24) = 2000.0f; //bullet.proj (chaingun) final def 800

}

void H2Tweaks::applyShaderTweaks() {
	//patches in order to get elite glowing lights back on
	//thanks Himanshu for help

	if (H2IsDediServer)
		return;

	int materialIndex = 0;
	DWORD currentMaterial;
	DWORD shader;
	DWORD sharedMapAddr = *(DWORD*)(H2BaseAddr + 0x482290);
	DWORD tagMemOffset = 0xE67D7C;

	DWORD tagMem = sharedMapAddr + tagMemOffset;
	DWORD MaterialsMem = *(DWORD*)(tagMem + 0x60 + 4) + sharedMapAddr; // reflexive materials block
	//int count = *(int*)(tagMem + 0x60);

	for (materialIndex = 0; materialIndex < 19; materialIndex++) { // loop through materials
		currentMaterial = MaterialsMem + (0x20 * materialIndex);
		shader = currentMaterial + 0x8;

		if (materialIndex == 3 || materialIndex == 7) { // arms/hands shaders 
			*(DWORD*)(shader + 4) = 0xE3652900;
		}

		else if (materialIndex == 5 || materialIndex == 8) { //legs shaders
			*(DWORD*)(shader + 4) = 0xE371290C;
		}

		else if (materialIndex == 15) { // inset_lights_mp shader
			*(DWORD*)(shader + 4) = 0xE38E2929;
		}
	 }
}

char ret_0() {
	return 0; //for 60 fps cinematics
}

void H2Tweaks::enable60FPSCutscenes() {

	if (H2IsDediServer)
		return;

	//60 fps cinematics enable
	PatchCall(H2BaseAddr + 0x97774, ret_0);
	PatchCall(H2BaseAddr + 0x7C378, ret_0);

}

void H2Tweaks::disable60FPSCutscenes() {

	if (H2IsDediServer)
		return;

	typedef char(__cdecl *is_cutscene_fps_cap)(); //restore orig func
	is_cutscene_fps_cap pIs_cutscene_fps_cap = (is_cutscene_fps_cap)(H2BaseAddr + 0x3A938);

	//60 fps cinematics disable
	PatchCall(H2BaseAddr + 0x97774, pIs_cutscene_fps_cap);
	PatchCall(H2BaseAddr + 0x7C378, pIs_cutscene_fps_cap);
}

void H2Tweaks::enableAI_MP() {

	if (H2IsDediServer) //TODO: get server offset
		return;

	BYTE jmp[1] = { JMP_RAW_BYTE };
	WriteBytes(H2BaseAddr + 0x30E684, jmp, 0x1); //AI_MP enable patch
}

void H2Tweaks::disableAI_MP() {

	if (H2IsDediServer) 
		return;

	BYTE jnz[1] = { JNZ_RAW_BYTE };
	WriteBytes(H2BaseAddr + 0x30E684, jnz, 0x1); //AI_MP disable patch
}

void H2Tweaks::PatchPingMeterCheck() {
	//halo2.exe+1D4E35 

	if (H2IsDediServer)
		return;

	BYTE assmPatchPingCheck[2] = { 0x75, 0x18 };
	WriteBytes(H2BaseAddr + 0x1D4E35, assmPatchPingCheck, 2);
}

void H2Tweaks::FixRanksIcons() {

	if (H2IsDediServer)
		return;

	int THINGY = *(int*)(H2BaseAddr + 0xA40564);
	BYTE* assmOffset = (BYTE*)(THINGY + 0x800);
	const int assmlen = 20;
	BYTE assmOrigRankIcon[assmlen] = { 0x92,0x00,0x14,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x6D,0x74,0x69,0x62,0xCA,0x02,0xEC,0xE4 };
	BYTE assmPatchFixRankIcon[assmlen] = { 0xCC,0x01,0x1C,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x6D,0x74,0x69,0x62,0xE6,0x02,0x08,0xE5 };
	bool shouldPatch = true;
	for (int i = 0; i < assmlen; i++) {
		if (*(assmOffset + i) != assmOrigRankIcon[i]) {
			shouldPatch = false;
			break;
		}
	}
	if (shouldPatch) {
		WriteBytes((DWORD)assmOffset, assmPatchFixRankIcon, assmlen);
		addDebugText("Patching Rank Icon Fix.");
	}
}
