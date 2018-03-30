#include "GSRunLoop.h"
#include "H2Startup.h"
#include "H2Config.h"
#include "H2OnscreenDebugLog.h"
#include "GSUtils.h"
#include "stdafx.h"
#include <d3d9.h>
#include <fstream>
#include "H2MOD.h"
#include <winsock.h>
#include "GSCustomMenu.h"
extern LPDIRECT3DDEVICE9 pDevice;

bool QuitGSMainLoop = false;
RECT rectScreenOriginal;

void setBorderless(int originX, int originY, int width, int height) {
	SetWindowLong(H2hWnd, GWL_STYLE, GetWindowLong(H2hWnd, GWL_STYLE) & ~(WS_THICKFRAME | WS_BORDER | WS_DLGFRAME));// | WS_SIZEBOX
	//SetWindowLong(halo2hWnd, GWL_STYLE, GetWindowLong(halo2hWnd, GWL_EXSTYLE) & ~(WS_EX_DLGMODALFRAME | WS_EX_CLIENTEDGE | WS_EX_STATICEDGE));

	SetWindowPos(H2hWnd, NULL, originX, originY, width, height, 0);// SWP_FRAMECHANGED | SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER | SWP_NOOWNERZORDER);

}

void setWindowed(int originX, int originY, int width, int height) {
	SetWindowLong(H2hWnd, GWL_STYLE, GetWindowLong(H2hWnd, GWL_STYLE) | WS_THICKFRAME | WS_BORDER | WS_DLGFRAME);
	SetWindowPos(H2hWnd, NULL, originX, originY, width, height, SWP_FRAMECHANGED);
}


void hotkeyFuncHideDebug() {
	setDebugTextDisplay(!getDebugTextDisplay());
}

void hotkeyFuncAlignWindow() {
	if (H2IsDediServer) {
		return;
	}
	if (!pDevice || !H2hWnd) {
		return;
	}
	HMONITOR monitor = MonitorFromWindow(H2hWnd, MONITOR_DEFAULTTONEAREST);
	MONITORINFO info;
	info.cbSize = sizeof(MONITORINFO);
	GetMonitorInfo(monitor, &info);
	int monitor_width = info.rcMonitor.right - info.rcMonitor.left;
	int monitor_height = info.rcMonitor.bottom - info.rcMonitor.top;
	int interval_width = monitor_width / 2;
	int interval_height = monitor_height / 2;
	D3DVIEWPORT9 pViewport;
	pDevice->GetViewport(&pViewport);
	int width = interval_width * round(pViewport.Width / (double)interval_width);
	int height = interval_height * round(pViewport.Height / (double)interval_height);
	RECT gameWindowRect;
	GetWindowRect(H2hWnd, &gameWindowRect);
	int monitorXOffset = gameWindowRect.left - info.rcMonitor.left;
	int monitorYOffset = gameWindowRect.top - info.rcMonitor.top;
	int padX = interval_width * round(monitorXOffset / (double)interval_width);
	int padY = interval_height * round(monitorYOffset / (double)interval_height);
	int posX = info.rcMonitor.left + padX;
	int posY = info.rcMonitor.top + padY;

	setBorderless(posX, posY, width, height);
}

void hotkeyFuncWindowMode() {
	if (H2IsDediServer) {
		return;
	}
	if (!pDevice || !H2hWnd) {
		return;
	}
	/*wchar_t title[255];
	wsprintf(title, L"Confirm Window Mode for Player %d", getPlayerNumber());
	int msgboxID = MessageBox(halo2hWnd,
	L"Go to Borderless Mode?\nNo = Windowed mode.\nWarning: Clicking the same option that is currently active can have weird side effects.",
	title,
	MB_ICONEXCLAMATION | MB_YESNOCANCEL
	);*/
	//if (msgboxID == IDYES) {}
	if (GetWindowLong(H2hWnd, GWL_STYLE) & (WS_THICKFRAME | WS_BORDER | WS_DLGFRAME)) {
		RECT rectScreen;
		GetWindowRect(H2hWnd, &rectScreen);
		D3DVIEWPORT9 pViewport;
		pDevice->GetViewport(&pViewport);
		int width = pViewport.Width;
		int height = pViewport.Height;
		long borderPadX = 0;
		long borderPadY = 0;
		int excessY = GetSystemMetrics(SM_CYCAPTION);

		WINDOWPLACEMENT place3;
		GetWindowPlacement(H2hWnd, &place3);
		if ((place3.flags & WPF_RESTORETOMAXIMIZED) == WPF_RESTORETOMAXIMIZED) {
			WINDOWPLACEMENT place2;
			GetWindowPlacement(H2hWnd, &place2);
			place2.showCmd = (place2.showCmd | SW_SHOWNOACTIVATE) & ~SW_MAXIMIZE;
			SetWindowPlacement(H2hWnd, &place2);
			borderPadX = GetSystemMetrics(SM_CXSIZEFRAME);
			borderPadY = GetSystemMetrics(SM_CYSIZEFRAME);
		}
		GetWindowRect(H2hWnd, &rectScreenOriginal);

		setBorderless(rectScreen.left + borderPadX, rectScreen.top + borderPadY, width, height + excessY);

	}
	else {
		//else if (msgboxID == IDNO) {
		long width = rectScreenOriginal.right - rectScreenOriginal.left;// -GetSystemMetrics(SM_CXSIZEFRAME) - GetSystemMetrics(SM_CXSIZEFRAME);
		long height = rectScreenOriginal.bottom - rectScreenOriginal.top;// -GetSystemMetrics(SM_CYSIZEFRAME) - GetSystemMetrics(SM_CYSIZEFRAME);
		setWindowed(rectScreenOriginal.left, rectScreenOriginal.top, width, height);
	}
}

void hotkeyFuncToggleHideIngameChat() {
	if (H2IsDediServer) {
		return;
	}
	H2Config_hide_ingame_chat = !H2Config_hide_ingame_chat;
	if (H2Config_hide_ingame_chat) {
		addDebugText("Hiding In-game Chat Menu.");
	}
	else {
		addDebugText("Showing In-game Chat Menu.");
	}
}

void hotkeyFuncGuide() {
	if (H2IsDediServer) {
		return;
	}
	GSCustomMenuCall_Guide();
}

//VK_ESCAPE
int hotkeyIdEsc = VK_F4;
void hotkeyFuncEsc() {
	if (H2IsDediServer) {
		return;
	}
	int GameGlobals = (int)*(int*)((char*)H2BaseAddr + 0x482D3C);
	DWORD* GameEngine = (DWORD*)(GameGlobals + 0x8);
	if (*GameEngine == 2) {
		//int WgitScreenfunctionPtr = (int)((char*)H2BaseAddr + 0x20e0c0);//Alt+F4 ingame
	}
}

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


#include "CUser.h"
int hotkeyIdTest = VK_F5;
void hotkeyFuncTest() {

	for (int i = 0; i < get_scenario_volume_count(); i++) {
		kill_volume_disable(i);
	}
	char moaartexxt[300];
	sprintf(moaartexxt, "Number of Volumes Disabled: %d", get_scenario_volume_count());
	addDebugText(moaartexxt);
	
	return;

	extern CUserManagement User;
	if (User.LocalUserLoggedIn()) {
		User.UnregisterLocal();
	}
	else {
		char H2Config_login_token[33] = { "" };
		addDebugText("keypress send");
		User.RegisterLocalRequest(H2Config_login_token, 0);
		Sleep(2000);
		addDebugText("get");
		User.RegisterLocalRequest(H2Config_login_token, 1);
	}
}


int hotkeyIdTest2 = VK_F6;
void hotkeyFuncTest2() {
	addDebugText("unconfused");

	GSCustomMenuCall_Obscure();

	return;
	//0 - Go to main menu root.
	//1 - Activate Product to continue playing diag.
	//2 - opens main menu wgit (6) - (doesn't render when ingame).
	//3 - Post Game Carnage Report (crashes when ingame)
	//5 - Post Game Carnage Report (crashes when ingame)
	//6 - Post Game Carnage Report (crashes when ingame)
	//7 - Post Game Carnage Report (crashes when ingame)
	//8 - Log User Out (crashes when ingame)
	//9 - ESRB Warning
	//  - 0x9100
	void(__cdecl* sub_20CE70)(signed int) = (void(__cdecl*)(int))((char*)H2BaseAddr + 0x20CE70);
	sub_20CE70(1);
	return;

	extern int keyHandler_itr;
	keyHandler_itr++;
	char NotificationPlayerText2[30];
	snprintf(NotificationPlayerText2, 30, "keyHandler_itr: %d", keyHandler_itr);
	addDebugText(NotificationPlayerText2);
}

void hotkeyFuncHelp() {
	addDebugText("------------------------------");
	addDebugText("Options:");
	char tempTextEntry[255];
	char hotkeyname[20];
	GetVKeyCodeString(H2Config_hotkeyIdToggleDebug, hotkeyname, 20);
	PadCStringWithChar(hotkeyname, 20, ' ');
	snprintf(tempTextEntry, 255, "%s- Toggle hiding this text display.", hotkeyname);
	addDebugText(tempTextEntry);
	GetVKeyCodeString(H2Config_hotkeyIdHelp, hotkeyname, 20);
	PadCStringWithChar(hotkeyname, 20, ' ');
	snprintf(tempTextEntry, 255, "%s- Print and show this help text.", hotkeyname);
	addDebugText(tempTextEntry);
	GetVKeyCodeString(H2Config_hotkeyIdAlignWindow, hotkeyname, 20);
	PadCStringWithChar(hotkeyname, 20, ' ');
	snprintf(tempTextEntry, 255, "%s- Align/Correct window positioning (into Borderless).", hotkeyname);
	addDebugText(tempTextEntry);
	GetVKeyCodeString(H2Config_hotkeyIdWindowMode, hotkeyname, 20);
	PadCStringWithChar(hotkeyname, 20, ' ');
	snprintf(tempTextEntry, 255, "%s- Toggle Windowed/Borderless mode.", hotkeyname);
	addDebugText(tempTextEntry);
	GetVKeyCodeString(H2Config_hotkeyIdToggleHideIngameChat, hotkeyname, 20);
	PadCStringWithChar(hotkeyname, 20, ' ');
	snprintf(tempTextEntry, 255, "%s- Toggles hiding the in-game chat menu.", hotkeyname);
	addDebugText(tempTextEntry);
	GetVKeyCodeString(H2Config_hotkeyIdConsole, hotkeyname, 20);
	PadCStringWithChar(hotkeyname, 20, ' ');
	snprintf(tempTextEntry, 255, "%s- Toggles hiding the Console Menu.", hotkeyname);
	addDebugText(tempTextEntry);
	//addDebugText("F5      - Toggle online Coop mode.");
	//addDebugText("F10     - Fix in-game player camera from a white/black bad cutscene.");
	//addDebugText("Home    - Sight Possession Hack.");
	//addDebugText("Page Up - Set Lobby Privacy to OPEN.");
	//addDebugText("Page Dn - Set Lobby Privacy to INVITE ONLY.");
	addDebugText("------------------------------");
	setDebugTextDisplay(true);
}

const int hotkeyLen = 9;
//GSFIXME: Set only completed 6
int hotkeyListenLen = 6;
int* hotkeyId[hotkeyLen] = { &H2Config_hotkeyIdHelp, &H2Config_hotkeyIdToggleDebug, &H2Config_hotkeyIdAlignWindow, &H2Config_hotkeyIdWindowMode, &H2Config_hotkeyIdToggleHideIngameChat, &H2Config_hotkeyIdGuide, &hotkeyIdTest, &hotkeyIdTest2, &hotkeyIdEsc };
bool hotkeyPressed[hotkeyLen] = { false, false, false, false, false, false, false, false, false };
void(*hotkeyFunc[hotkeyLen])(void) = { hotkeyFuncHelp, hotkeyFuncHideDebug, hotkeyFuncAlignWindow, hotkeyFuncWindowMode, hotkeyFuncToggleHideIngameChat, hotkeyFuncGuide, hotkeyFuncTest, hotkeyFuncTest2, hotkeyFuncEsc };

int prevPartyPrivacy = 0;

bool halo2WindowExists = false;
bool halo2ServerOnce1 = false;
void GSMainLoop() {
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

		//if (custom_resolution_x > 0 && custom_resolution_y > 0) {
		//	SetWindowPos(H2hWnd, NULL, 0, 0, 500, 500, SWP_NOMOVE | SWP_FRAMECHANGED);
		//	SetWindowPos(H2hWnd, NULL, 0, 0, custom_resolution_x, custom_resolution_y, SWP_NOMOVE | SWP_FRAMECHANGED);// SWP_FRAMECHANGED |  | SWP_NOSIZE | SWP_NOZORDER | SWP_NOOWNERZORDER);
		//}
		if (H2GetInstanceId() > 1) {
			wchar_t titleOriginal[200];
			wchar_t titleMod[200];
			GetWindowText(H2hWnd, titleOriginal, 200);
			wsprintf(titleMod, L"%ls (P%d)", titleOriginal, H2GetInstanceId());
			SetWindowText(H2hWnd, titleMod);
		}
	}
	if (H2IsDediServer && !halo2ServerOnce1) {
		halo2ServerOnce1 = true;
		pushHostLobby();
		wchar_t* LanServerName = (wchar_t*)((BYTE*)H2BaseAddr + 0x52042A);
		if (strlen(H2Config_dedi_server_name) > 0) {
			swprintf(LanServerName, 32, L"%hs", H2Config_dedi_server_name);
		}
	}

	int partyPrivacy;
	if (H2IsDediServer) {
		partyPrivacy = *(int*)((BYTE*)H2BaseAddr + 0x534850);
	}
	else {
		partyPrivacy = *(int*)((BYTE*)H2BaseAddr + 0x50A398);
	}
	extern bool isHost;
	if (prevPartyPrivacy > 0 && partyPrivacy == 0 && isHost) {
		pushHostLobby();
	}
	prevPartyPrivacy = partyPrivacy;

	if (!H2IsDediServer && (GetFocus() == H2hWnd || GetForegroundWindow() == H2hWnd)) {

		for (int i = 0; i < hotkeyListenLen; i++) {
			//& 0x8000 is pressed
			//& 0x1 Key just transitioned from released to pressed.
			if (GetAsyncKeyState(*hotkeyId[i]) & 0x8000) {
				hotkeyPressed[i] = true;
			}
			else if (!(GetAsyncKeyState(*hotkeyId[i]) & 0x8000) && hotkeyPressed[i]) {
				hotkeyPressed[i] = false;
				hotkeyFunc[i]();
			}
		}
	}
}

signed int(*sub_287a1)();

static signed int HookedClientRandFunc() {
	if (!QuitGSMainLoop)
		GSMainLoop();
	
	signed int result = sub_287a1();
	return result;
}

static char HookedServerShutdownCheck() {
	if (!QuitGSMainLoop)
		GSMainLoop();
	
	BYTE& Quit_Exit_Game = *(BYTE*)((char*)H2BaseAddr + 0x4a7083);

	if (Quit_Exit_Game) {
		DeinitH2Startup();
	}

	//original test - if game should shutdown
	return Quit_Exit_Game;
}

void initGSRunLoop() {
	addDebugText("Pre GSRunLoop Hooking.");
	if (H2IsDediServer) {
		addDebugText("Hooking Loop & Shutdown Function");
		PatchCall(H2BaseAddr + 0xc6cb, HookedServerShutdownCheck);
	}
	else {
		addDebugText("Hooking Loop Function");
		sub_287a1 = (signed int(*)())((char*)H2BaseAddr + 0x287a1);
		PatchCall(H2BaseAddr + 0x399f3, HookedClientRandFunc);
	}
	addDebugText("Post GSRunLoop Hooking.");
}

void deinitGSRunLoop() {

}
