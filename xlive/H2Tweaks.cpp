#include "H2Tweaks.h"
#include "H2Startup.h"
#include "H2Config.h"
#include "GSUtils.h"
#include "Hook.h"
#include "H2OnscreenDebugLog.h"
#include "GSCustomMenu.h"

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
	swprintf(mutexName2, L"Halo2BasePort#%d", H2Config_base_port);
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

	BYTE jmp[1] = { 0xEB };
	WriteBytes(H2BaseAddr + 0x30E684, jmp, 0x1); //AI_MP enable patch
}

void H2Tweaks::disableAI_MP() {

	if (H2IsDediServer) 
		return;

	BYTE jnz[1] = { 0x75 };
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

