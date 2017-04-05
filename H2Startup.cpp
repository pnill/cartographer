#include "H2Startup.h"
#include "H2OnscreenDebugLog.h"

int playerNumber = 0;
int getPlayerNumber() {
	return playerNumber;
}


void ProcessH2Startup() {
	initDebugText();
	/*halo2ThreadID = GetCurrentThreadId();
	if (GetModuleHandle(L"H2Server.exe"))
	{
		MemAddrBase = (DWORD)GetModuleHandle(L"H2Server.exe");
		IsDediServer = TRUE;
		addDebugText("Process is Dedi-Server");
	}
	else
	{
		MemAddrBase = (DWORD)GetModuleHandle(L"halo2.exe");
		IsDediServer = FALSE;
		addDebugText("Process is Client");
	}
	initPlayerNumber();
	ReadStartupOptions();

	if (!IsDediServer) {
		bool IntroHQ = TRUE;//clients should set on halo2.exe -highquality

		if (language_code >= 0 && language_code <= 7) {
			BYTE assmLang[15];
			memset(assmLang, language_code, 15);
			OverwriteAssembly((BYTE*)MemAddrBase + 0x38300, assmLang, 15);
		}

		if (skip_intro) {
			BYTE assmIntroSkip[] = { 0x3F };
			OverwriteAssembly((BYTE*)MemAddrBase + 0x221C0E, assmIntroSkip, 1);
		}

		if (!skip_intro && IntroHQ) {
			BYTE assmIntroHQ[] = { 0xEB };
			OverwriteAssembly((BYTE*)MemAddrBase + 0x221C29, assmIntroHQ, 1);
		}

		//Allows unlimited clients
		BYTE assmUnlimitedClients[41];
		memset(assmUnlimitedClients, 0x00, 41);
		OverwriteAssembly((BYTE*)MemAddrBase + 0x39BCF0, assmUnlimitedClients, 41);

		//Allows on a remote desktop connection
		BYTE assmRemoteDesktop[] = { 0xEB };
		OverwriteAssembly((BYTE*)MemAddrBase + 0x7E54, assmRemoteDesktop, 1);

		//multi-process splitscreen input hacks
		if (disable_ingame_keyboard) {
			//Allows to repeat last movement when lose focus in mp, unlocks METHOD E from point after intro vid
			BYTE getFocusB[] = { 0x00 };
			OverwriteAssembly((BYTE*)MemAddrBase + 0x2E3C5, getFocusB, 1);
			//Allows input when not in focus.
			BYTE getFocusE[] = { 0x90, 0x90, 0x90, 0x90, 0x90, 0x90 };
			OverwriteAssembly((BYTE*)MemAddrBase + 0x2F9EA, getFocusE, 6);
			OverwriteAssembly((BYTE*)MemAddrBase + 0x2F9FC, getFocusE, 6);
			OverwriteAssembly((BYTE*)MemAddrBase + 0x2FA09, getFocusE, 6);
			//Disables the keyboard only when in-game and not in a menu.
			BYTE disableKeyboard1[] = { 0x90, 0x90, 0x90 };
			OverwriteAssembly((BYTE*)MemAddrBase + 0x2FA8A, disableKeyboard1, 3);
			BYTE disableKeyboard2[] = { 0x00 };
			OverwriteAssembly((BYTE*)MemAddrBase + 0x2FA92, disableKeyboard2, 1);
			BYTE disableKeyboard3[] = { 0x90, 0x90, 0x90, 0x90, 0x90, 0x90 };
			OverwriteAssembly((BYTE*)MemAddrBase + 0x2FA67, disableKeyboard3, 6);
		}

	}

	addDebugText("ProcessStartup finished.");*/
}
