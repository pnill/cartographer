#include "GSRunLoop.h"
#include "H2Startup.h"
#include "H2OnscreenDebugLog.h"
#include "GSUtils.h"
#include "GSMenu.h"
#include "stdafx.h"
#include <d3d9.h>
#include <fstream>
#include "H2MOD.h"
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


int hotkeyIdToggleDebug = VK_F2;
void hotkeyFuncHideDebug() {
	setDebugTextDisplay(!getDebugTextDisplay());
}

int hotkeyIdAlignWindow = VK_F7;
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

int hotkeyIdWindowMode = VK_F8;
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

VOID CallWgit(int WgitScreenfunctionPtr) {

	//int __thiscall WgitScreenInitializer(int this)
	//signed int __thiscall Load_WgitScreen2(int this, __int16 a2, __int16 a3, int a4, int a5, int a6, signed int a7)
	//signed int __thiscall Load_WgitScreens(int tempmemaddrPtr, __int16 a2, int a3, int a4, int WgitScreenFuncPtr)
	//int __thiscall WgitScreenFinaliser(int tempmemaddrPtr)
	//---------------------------------------- -
	//	(Ida Function Prototypes)
	//-------------------------------------- -
	char* tmp = (char*)malloc(sizeof(char) * 0x20);

	int(__thiscall*WgitInitialize)(void*);
	WgitInitialize = (int(__thiscall*)(void*))((char*)H2BaseAddr + 0x20B0BC);
	signed int(__thiscall*WgitLoad)(void*, __int16, int, int, int);
	WgitLoad = (signed int(__thiscall*)(void*, __int16, int, int, int))((char*)H2BaseAddr + 0x20C226);
	void*(__thiscall*WgitFinalize)(void*);
	WgitFinalize = (void*(__thiscall*)(void*))((char*)H2BaseAddr + 0x20B11E);
	//Now Calling Menus.

	WgitInitialize(tmp);
	//int ui_type = 3;
	int ui_type = 1;
	WgitLoad(tmp, 1, 5, 4, WgitScreenfunctionPtr);
	void* rtnfnl = WgitFinalize(tmp);
	/*int a2 = 0x32;
	char* v9 = (char*)malloc(sizeof(char) * 100);
	int a5;
	int a6;
	int v10;
	if (rtnfnl)
	{
	//int __cdecl sub_132DFB3(signed int a1, int a2, int a3)
	typedef int(__stdcall *pFuncA)(int a1, int a2, char* a3);
	pFuncA sub_132DFB3 = (pFuncA)((char*)H2BaseAddr + 0x21DFB3);
	sub_132DFB3(a2, 0, v9);
	MessageBoxA(NULL, "fail", "Unknown Failure!", MB_OK);
	//char __thiscall sub_131D958(void *this, signed int a2, int a3)
	typedef char(__thiscall *pFuncB)(void* thisptr, int a2, int a3);
	pFuncB sub_131D958 = (pFuncB)((char*)H2BaseAddr + 0x20D958);
	sub_131D958(rtnfnl, a2, 0);

	//*(DWORD*)((char*)rtnfnl + 7040) = a5;
	//*(DWORD*)((char*)rtnfnl + 6756) = a6;
	//*(BYTE*)((char*)rtnfnl + 6752) = v10;
	}

	free(v9);*/

	free(tmp);
}
int ui_priority = 3;
//VK_ESCAPE
int hotkeyIdEsc = VK_F4;
void hotkeyFuncEsc() {
	if (H2IsDediServer) {
		return;
	}
	int GameGlobals = (int)*(int*)((char*)H2BaseAddr + 0x482D3C);
	DWORD* GameEngine = (DWORD*)(GameGlobals + 0x8);
	if (*GameEngine == 2) {
		int WgitScreenfunctionPtr = (int)((char*)H2BaseAddr + 0x20CDE7);

		char* tmp = (char*)malloc(sizeof(char) * 0x20);

		int(__thiscall*WgitInitialize)(void*);
		WgitInitialize = (int(__thiscall*)(void*))((char*)H2BaseAddr + 0x20B0BC);
		signed int(__thiscall*WgitLoad)(void*, __int16, int, int, int);
		WgitLoad = (signed int(__thiscall*)(void*, __int16, int, int, int))((char*)H2BaseAddr + 0x20C226);
		void*(__thiscall*WgitFinalize)(void*);
		WgitFinalize = (void*(__thiscall*)(void*))((char*)H2BaseAddr + 0x20B11E);
		//Now Calling Menus.

		WgitInitialize(tmp);
		//int ui_priority = 3;//quit halo 2 level
		//int ui_priority = 7;//6 >= broken
		//int ui_priority = 1;
		WgitLoad(tmp, 1, 1, 4, WgitScreenfunctionPtr);
		void* rtnfnl = WgitFinalize(tmp);

		free(tmp);
	}
}

bool IsCustomMenu = false;

int hotkeyIdTest = VK_F6;
void hotkeyFuncTest() {
	//int WgitScreenfunctionPtr = (int)((char*)H2BaseAddr + 0x20E032);//H2BetaErrorOk
	//int WgitScreenfunctionPtr = (int)((char*)H2BaseAddr + 0xE757);//PCR
	//int WgitScreenfunctionPtr = (int)((char*)H2BaseAddr + 0x20E0C0);//ok cancel?
	//int WgitScreenfunctionPtr = (int)((char*)H2BaseAddr + 0x24F819);

	//Works the same way.
	//int WgitScreenfunctionPtr = (int)((char*)H2BaseAddr + 0x2593CB);//Game Volume Ingame
	//int WgitScreenfunctionPtr = (int)((char*)H2BaseAddr + 0x2274EE);//Game Volume MM

	//Works the same way.
	//int WgitScreenfunctionPtr = (int)((char*)H2BaseAddr + 0x24925C);//Game Brightness MM
	//int WgitScreenfunctionPtr = (int)((char*)H2BaseAddr + 0x258C8C);//Game Brightness ingame

	int* MenuID = (int*)((char*)H2BaseAddr + 0x9758D8);

	if (*MenuID != 272) {
		int WgitScreenfunctionPtr = (int)(MenuGameBrightnessIngame);
		CallWgit(WgitScreenfunctionPtr);
	}

	//ui_priority += 1;

}

int hotkeyIdTest2 = VK_F9;
void hotkeyFuncTest2() {
	//int a3 = 0;
	//signed int(__cdecl* sub_603D0E)(signed int* a1);
	//sub_603D0E = (signed int(__cdecl*)(signed int*))((char*)H2BaseAddr + 0x263D0E);
	//sub_603D0E(&a3);

	//int WgitScreenfunctionPtr = (int)(MenuGameVolumeIngame);
	//int WgitScreenfunctionPtr = (int)(MenuGameResolutionMM);
	//CallWgit(WgitScreenfunctionPtr);

	/*BYTE assmSpeedPatch[8];
	memset(assmSpeedPatch, 0, 8);
	OverwriteAssembly((BYTE*)H2BaseAddr + 0x6AB7f, assmSpeedPatch, 8);
	*/

	/*
	//Enable Skulls in MP
	BYTE assmEnableMPSkulls[] = { 0x90, 0x90 };
	OverwriteAssembly((BYTE*)H2BaseAddr + 0xBD125, assmEnableMPSkulls, 2);

	int* SkullGruntBDay = (int*)((char*)H2BaseAddr + 0x4D8321);
	*SkullGruntBDay = 1;


	//Enable Grenade Chain Reactions in MP
	BYTE assmEnableGrenadeChainReact[] = { 0x90, 0x90 };
	OverwriteAssembly((BYTE*)H2BaseAddr + 0x182D74, assmEnableGrenadeChainReact, 2);

	BYTE assmEnableMPBansheeBomb[] = { 0x09 };
	OverwriteAssembly((BYTE*)H2BaseAddr + 0x92C06, assmEnableMPBansheeBomb, 1);
	*/

	//Change Language Without Restart (non-native characters will not display).
	BYTE assmLang[15];
	memset(assmLang, 1, 15);
	OverwriteAssembly((BYTE*)H2BaseAddr + 0x38300, assmLang, 15);
	BYTE* HasLoadedLanguage = (BYTE*)((char*)H2BaseAddr + 0x481908);
	*HasLoadedLanguage = 0;
	int GameGlobals = (int)*(int*)((char*)h2mod->GetBase() + 0x482D3C);
	BYTE* EngineMode = (BYTE*)(GameGlobals + 0x8);
	*EngineMode = 1;
	BYTE* QuitLevel = (BYTE*)((char*)H2BaseAddr + 0x482251);
	*QuitLevel = 1;


	//Zanzibar Wheel
	//int bbase = (int)*(int*)((char*)H2BaseAddr + 0x479E70);
	//*(float*)(bbase + 0x1479850) = 0.7;
	//*(float*)(bbase + 0x149B4C4) = 0.7;


	//Pimp Ma Hawg
	//int bbase = (int)*(int*)((char*)H2BaseAddr + 0xA3DA3C);
	/*(float*)(bbase + 0x75D5E8) = 8;
	*(float*)(bbase + 0x75D5EC) = 0.2;
	*(float*)(bbase + 0x75D5F0) = 0.05;
	*(float*)(bbase + 0x75D5F4) = 0.01;
	*(float*)(bbase + 0x75D59C) = 10;
	*(float*)(bbase + 0x75D52C) = 20;
	*(float*)(bbase + 0x75D534) = 10;*/

	/*(float*)(bbase + 0x75D5E8) = 20;
	*(float*)(bbase + 0x75D5EC) = 1;
	*(float*)(bbase + 0x75D5F0) = 0.05;
	*(float*)(bbase + 0x75D5F4) = 0.5;
	*(float*)(bbase + 0x75D59C) = 30;
	*(float*)(bbase + 0x75D52C) = 50;
	*(float*)(bbase + 0x75D534) = 50;*/

}

int hotkeyIdHelp = VK_F3;
void hotkeyFuncHelp() {
	addDebugText("------------------------------");
	addDebugText("Options:");
	char tempTextEntry[255];
	char hotkeyname[20];
	GetVKeyCodeString(hotkeyIdToggleDebug, hotkeyname, 20);
	PadCStringWithChar(hotkeyname, 20, ' ');
	snprintf(tempTextEntry, 255, "%s- Toggle hiding this text display.", hotkeyname);
	addDebugText(tempTextEntry);
	GetVKeyCodeString(hotkeyIdHelp, hotkeyname, 20);
	PadCStringWithChar(hotkeyname, 20, ' ');
	snprintf(tempTextEntry, 255, "%s- Print and show this help text.", hotkeyname);
	addDebugText(tempTextEntry);
	GetVKeyCodeString(hotkeyIdAlignWindow, hotkeyname, 20);
	PadCStringWithChar(hotkeyname, 20, ' ');
	snprintf(tempTextEntry, 255, "%s- Align/Correct window positioning (into Borderless).", hotkeyname);
	addDebugText(tempTextEntry);
	GetVKeyCodeString(hotkeyIdWindowMode, hotkeyname, 20);
	PadCStringWithChar(hotkeyname, 20, ' ');
	snprintf(tempTextEntry, 255, "%s- Toggle Windowed/Borderless mode.", hotkeyname);
	addDebugText(tempTextEntry);
	//addDebugText("F5      - Toggle online Coop mode.");
	//addDebugText("F10     - Fix in-game player camera from a white/black bad cutscene.");
	//addDebugText("Home    - Sight Possession Hack.");
	//addDebugText("Page Up - Set Lobby Privacy to OPEN.");
	//addDebugText("Page Dn - Set Lobby Privacy to INVITE ONLY.");
	addDebugText("------------------------------");
	setDebugTextDisplay(true);
}


const int hotkeyLen = 7;
int hotkeyListenLen = 6;//4
int* hotkeyId[hotkeyLen] = { &hotkeyIdHelp, &hotkeyIdToggleDebug, &hotkeyIdAlignWindow, &hotkeyIdWindowMode, &hotkeyIdTest, &hotkeyIdTest2, &hotkeyIdEsc };
bool hotkeyPressed[hotkeyLen] = { false, false, false, false, false, false, false };
void(*hotkeyFunc[hotkeyLen])(void) = { hotkeyFuncHelp, hotkeyFuncHideDebug, hotkeyFuncAlignWindow, hotkeyFuncWindowMode, hotkeyFuncTest, hotkeyFuncTest2, hotkeyFuncEsc };

bool halo2WindowExists = false;
bool halo2ServerFinishedLoading = false;
void GSMainLoop() {
	if (!H2IsDediServer && !halo2WindowExists && H2hWnd != NULL) {
		halo2WindowExists = true;
		SetWindowLong(H2hWnd, GWL_STYLE, GetWindowLong(H2hWnd, GWL_STYLE) | WS_SIZEBOX | WS_MAXIMIZEBOX | WS_SYSMENU);
		if (getPlayerNumber() > 1) {
			wchar_t titleOriginal[200];
			wchar_t titleMod[200];
			GetWindowText(H2hWnd, titleOriginal, 200);
			wsprintf(titleMod, L"%ls (P%d)", titleOriginal, getPlayerNumber());
			SetWindowText(H2hWnd, titleMod);
		}
	}
	if (H2IsDediServer && !halo2ServerFinishedLoading) {
		wchar_t* LanServerName = (wchar_t*)((BYTE*)H2BaseAddr + 0x52042A);
		if (wcslen(LanServerName) > 0 && wcslen(dedi_server_name) > 0) {
			halo2ServerFinishedLoading = true;
			swprintf(LanServerName, 32, dedi_server_name);
			QuitGSMainLoop = true;
		}
	}
	if (GetFocus() == H2hWnd || GetForegroundWindow() == H2hWnd) {

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

class later
{
public:
	template <class callable, class... arguments>
	later(int after, bool async, callable&& f, arguments&&... args)
	{
		std::function<typename std::result_of<callable(arguments...)>::type()> task(std::bind(std::forward<callable>(f), std::forward<arguments>(args)...));

		if (async)
		{
			std::thread([after, task]() {
				std::this_thread::sleep_for(std::chrono::milliseconds(after));
				task();
			}).detach();
		}
		else
		{
			std::this_thread::sleep_for(std::chrono::milliseconds(after));
			task();
		}
	}

};

DWORD WINAPI GSLoopMaker(LPVOID lpParam)
{
	while (!QuitGSMainLoop) {
		//loop every 4 milliseconds
		later later_test1(4, false, &GSMainLoop);
	}
	return 0;
}

void initGSRunLoop() {
	DWORD  dwThreadIdGSLooper;
	HANDLE hThread = CreateThread(NULL, 0, GSLoopMaker, NULL, 0, &dwThreadIdGSLooper);
}
