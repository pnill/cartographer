#include "H2Startup.h"
#include "H2OnscreenDebugLog.h"
#include "GSUtils.h"
#include "GSRunLoop.h"
#include <Shellapi.h>

int language_code = -1;
bool skip_intro = false;
bool disable_ingame_keyboard = false;
bool H2IsDediServer;
DWORD H2BaseAddr;
HWND H2hWnd = NULL;

int playerNumber = 0;
int getPlayerNumber() {
	return playerNumber;
}

const wchar_t* xinputdllPath = { L"xinput/p00/xinput9_1_0.dll" };

void initPlayerNumber() {

	HANDLE mutex;
	DWORD lastErr;
	do {
		playerNumber++;
		wchar_t mutexName[32];
		swprintf(mutexName, L"Halo2Player%d", playerNumber);
		mutex = CreateMutex(0, TRUE, mutexName);
		lastErr = GetLastError();
		if (lastErr == ERROR_ALREADY_EXISTS) {
			CloseHandle(mutex);
		}
	} while (lastErr == ERROR_ALREADY_EXISTS);

	char NotificationPlayerText[20];
	sprintf(NotificationPlayerText, "You are Player #%d", playerNumber);
	addDebugText(NotificationPlayerText);
	//MessageBoxA(NULL, NotificationPlayerText, "Mutex Created!", MB_OK);

	if (!H2IsDediServer) {
		if (getPlayerNumber() > 1) {
			BYTE xinputNumFix[] = { '0' + (getPlayerNumber() / 10), 0, '0' + (getPlayerNumber() % 10) };
			OverwriteAssembly((BYTE*)xinputdllPath + 16, xinputNumFix, 3);

			char pointerHex[20];
			sprintf(pointerHex, "%x", (DWORD)xinputdllPath);
			BYTE byteArray[4] = { 0,0,0,0 };
			HexToByteArray(byteArray, pointerHex);

			char totext[255];
			sprintf(totext, "injecting new xinputdll push instruction: %ls : %x : %x %x %x %x", (DWORD)xinputdllPath, (DWORD)xinputdllPath, byteArray[3], byteArray[2], byteArray[1], byteArray[0]);
			addDebugText(totext);

			BYTE assmXinputPushIntructionPart[] = { byteArray[3], byteArray[2], byteArray[1], byteArray[0] };
			OverwriteAssembly((BYTE*)H2BaseAddr + 0x8AD28, assmXinputPushIntructionPart, 4);

			char xinputName[40];
			char xinputdir[12];
			sprintf(xinputdir, "xinput/p%d%d", getPlayerNumber() / 10, getPlayerNumber() % 10);
			sprintf(xinputName, "%s/xinput9_1_0.dll", xinputdir);
			wchar_t xinputdir2[30];
			MultiByteToWideChar(CP_ACP, 0, xinputdir, -1, xinputdir2, strlen(xinputdir) + 1);
			CreateDirectory(L"xinput", NULL);
			CreateDirectory(xinputdir2, NULL);
			if (FILE *file = fopen(xinputName, "r")) {
				fclose(file);
			}
			else {
				char durazno_xinput_md5[] = "6140ae76b26a633ce2255f6fc88fbe34";
				char available_xinput_md5[33];
				int hasherr = ComputeFileMd5Hash(L"xinput9_1_0.dll", available_xinput_md5);
				FILE* file1 = NULL;
				if (hasherr == 0 && (file1 = fopen("xinput9_1_0.dll", "rb"))) {
					if (strcmp(durazno_xinput_md5, available_xinput_md5)) {
						char xinputError[] = "ERROR! xinput9_1_0.dll does not match the supported version required in the local game directory! For \'Split-screen\' play, the Durazno Xinput v0.6 DLL is required.";
						addDebugText(xinputError);
						MessageBoxA(NULL, xinputError, "Incorrect DLL Error", MB_OK);
						exit(EXIT_FAILURE);
					}
					FILE* file2 = fopen(xinputName, "wb");
					char buffer[BUFSIZ];
					size_t n;
					while ((n = fread(buffer, sizeof(char), sizeof(buffer), file1)) > 0) {
						if (fwrite(buffer, sizeof(char), n, file2) != n) {
							char xinputError[255];
							sprintf(xinputError, "ERROR! Failed to write copied file: %s", xinputName);
							addDebugText(xinputError);
							MessageBoxA(NULL, xinputError, "DLL Copy Error", MB_OK);
							exit(EXIT_FAILURE);
						}
					}
					fclose(file1);
					fclose(file2);

					FILE* file3 = fopen(xinputName, "r+b");
					BYTE assmXinputDuraznoNameEdit[] = { 0x30 + (getPlayerNumber() / 10), 0x00, 0x30 + (getPlayerNumber() % 10) };
					if (fseek(file3, 0x196DE, SEEK_SET) == 0 && fwrite(assmXinputDuraznoNameEdit, sizeof(BYTE), 3, file3) == 3) {
						addDebugText("Successfully copied and patched original xinput9_1_0.dll");
					}
					else {
						fclose(file3);
						remove(xinputName);
						char xinputError[255];
						sprintf(xinputError, "ERROR! Failed to write hex edit to file: %s", xinputName);
						addDebugText(xinputError);
						MessageBoxA(NULL, xinputError, "DLL Edit Error", MB_OK);
						exit(EXIT_FAILURE);
					}
					fclose(file3);
				}
				else if (hasherr == -1 || !file1) {
					char xinputError[] = "ERROR! xinput9_1_0.dll does not exist in the local game directory! For \'Split-screen\' play, the Durazno Xinput v0.6 DLL is required.";
					addDebugText(xinputError);
					MessageBoxA(NULL, xinputError, "DLL Missing Error", MB_OK);
					exit(EXIT_FAILURE);
				}
				else {
					char xinputError[200];
					snprintf(xinputError, 200, "Hash Error Num: %x - msg: %s", hasherr, available_xinput_md5);
					addDebugText(xinputError);
					MessageBoxA(NULL, xinputError, "MD5 Hash Error", MB_OK);
					exit(EXIT_FAILURE);
				}
			}
		}
	}
}

void fileFail(FILE* fp) {
	int fperrno = GetLastError();
	if (fperrno == EACCES || fperrno == EIO || fperrno == EPERM) {
		MessageBoxA(NULL, "Cannot write a file. Please restart Halo 2 in Administrator mode!", "Permission Error!", MB_OK);
	}
	else if (!fp) {
		char NotificationPlayerText[20];
		sprintf(NotificationPlayerText, "Error #%x", fperrno);
		addDebugText(NotificationPlayerText);
		MessageBoxA(NULL, NotificationPlayerText, "Unknown File Failure!", MB_OK);
	}
	exit(1);
}

void ReadStartupOptions() {
	LPWSTR fileStartupini = new WCHAR[256];
	swprintf(fileStartupini, L"h2startup%d.ini", getPlayerNumber());

	int ArgCnt;
	LPWSTR* ArgList = CommandLineToArgvW(GetCommandLineW(), &ArgCnt);
	if (ArgList != NULL)
	{
		for (int i = 0; i < ArgCnt; i++)
		{
			if (wcsstr(ArgList[i], L"-h2startup=") != NULL)
			{
				if (wcslen(ArgList[i]) < 255)
					lstrcpyW(fileStartupini, ArgList[i] + 10);
			}
		}
	}
	//Variables read check
	bool est_language_code = false;
	bool est_skip_intro = false;
	bool est_disable_ingame_keyboard = false;
	//Hotkeys
	bool est_hotkey_help = false;
	bool est_hotkey_toggle_debug = false;
	bool est_hotkey_align_window = false;
	bool est_hotkey_window_mode = false;

	int flagged[256];
	int flagged_pos = -1;
	FILE *fp;
	if (fp = _wfopen(fileStartupini, L"r")) {
		flagged_pos = 0;
		char string[256];
		while (fgets(string, 255, fp)) {
			if (flagged_pos > 256) {
				MessageBoxA(NULL, "There are too many bad lines in SETUP config!", "File config overflow!", MB_OK);
				exit(1);
			}
			if (strlen(string) < 4 || string[0] == '#' || (string[0] == '/' && string[1] == '/')) {
				continue;
			}
			if (strstr(string, "language_code =")) {
				int temp;
				sscanf(string + strlen("language_code ="), "%d", &temp);
				if (est_language_code || temp < -1 || temp > 7) {
					flagged[flagged_pos++] = FindStartOfLine(fp, strlen(string));
				}
				else {
					language_code = temp;
					est_language_code = true;
				}
			}
			else if (strstr(string, "skip_intro =")) {
				int temp;
				sscanf(string + strlen("skip_intro ="), "%d", &temp);
				if (est_skip_intro || !(temp == 0 || temp == 1)) {
					flagged[flagged_pos++] = FindStartOfLine(fp, strlen(string));
				}
				else {
					skip_intro = (bool)temp;
					est_skip_intro = true;
				}
			}
			else if (strstr(string, "disable_ingame_keyboard =")) {
				int temp;
				sscanf(string + strlen("disable_ingame_keyboard ="), "%d", &temp);
				if (est_disable_ingame_keyboard || !(temp == 0 || temp == 1)) {
					flagged[flagged_pos++] = FindStartOfLine(fp, strlen(string));
				}
				else {
					disable_ingame_keyboard = (bool)temp;
					est_disable_ingame_keyboard = true;
				}
			}
			else if (strstr(string, "hotkey_help =")) {
				int temp;
				sscanf(string + strlen("hotkey_help ="), "%d", &temp);
				if (est_hotkey_help || !(temp >= 0)) {
					flagged[flagged_pos++] = FindStartOfLine(fp, strlen(string));
				}
				else {
					hotkeyIdHelp = temp;
					est_hotkey_help = true;
				}
			}
			else if (strstr(string, "hotkey_toggle_debug =")) {
				int temp;
				sscanf(string + strlen("hotkey_toggle_debug ="), "%d", &temp);
				if (est_hotkey_toggle_debug || !(temp >= 0)) {
					flagged[flagged_pos++] = FindStartOfLine(fp, strlen(string));
				}
				else {
					hotkeyIdToggleDebug = temp;
					est_hotkey_toggle_debug = true;
				}
			}
			else if (strstr(string, "hotkey_align_window =")) {
				int temp;
				sscanf(string + strlen("hotkey_align_window ="), "%d", &temp);
				if (est_hotkey_align_window || !(temp >= 0)) {
					flagged[flagged_pos++] = FindStartOfLine(fp, strlen(string));
				}
				else {
					hotkeyIdAlignWindow = temp;
					est_hotkey_align_window = true;
				}
			}
			else if (strstr(string, "hotkey_window_mode =")) {
				int temp;
				sscanf(string + strlen("hotkey_window_mode ="), "%d", &temp);
				if (est_hotkey_window_mode || !(temp >= 0)) {
					flagged[flagged_pos++] = FindStartOfLine(fp, strlen(string));
				}
				else {
					hotkeyIdWindowMode = temp;
					est_hotkey_window_mode = true;
				}
			}
		}
		fclose(fp);
		fp = NULL;
		if (!flagged_pos && !(est_language_code && est_skip_intro && est_disable_ingame_keyboard && est_hotkey_help && est_hotkey_toggle_debug && est_hotkey_align_window && est_hotkey_window_mode)) {
			flagged_pos = -2;
		}
	}
	if (flagged_pos) {
		if (flagged_pos == -1) {
			if (fp = _wfopen(fileStartupini, L"w")) {
				fclose(fp);
			}
			else {
				fileFail(fp);
			}
		}
		if (fp = _wfopen(fileStartupini, L"r+")) {
			if (flagged_pos == -1) {
				fputs("# language_code Options (Client):", fp);
				fputs("\n# -1 - System Default", fp);
				fputs("\n# 0  - Chinese", fp);
				fputs("\n# 1  - German", fp);
				fputs("\n# 2  - Spanish", fp);
				fputs("\n# 3  - French", fp);
				fputs("\n# 4  - Italian", fp);
				fputs("\n# 5  - Japanese", fp);
				fputs("\n# 6  - Korean", fp);
				fputs("\n# 7  - English", fp);
				fputs("\n\n", fp);
				fputs("# skip_intro Options (Client):", fp);
				fputs("\n# 0 - Normal Intro", fp);
				fputs("\n# 1 - No Intro", fp);
				fputs("\n\n", fp);
				fputs("# disable_ingame_keyboard Options (Client):", fp);
				fputs("\n# 0 - Normal Game Controls", fp);
				fputs("\n# 1 - Disables ONLY Keyboard when in-game & allows controllers when game is not in focus", fp);
				fputs("\n\n", fp);
				fputs("# hotkey_... Options (Client):", fp);
				fputs("\n# The number used is the keyboard Virtual-Key (VK) Code in base-10 integer form.", fp);
				fputs("\n# The codes in hexadecimal (base-16) form can be found here:", fp);
				fputs("\n# https://msdn.microsoft.com/en-us/library/windows/desktop/dd375731(v=vs.85).aspx", fp);
				fputs("\n\n", fp);
			}
			else if (flagged_pos > 0) {
				for (int i = flagged_pos - 1; i >= 0; i--) {
					fseek(fp, flagged[i], SEEK_SET);
					fputc('#', fp);
				}
			}
			fseek(fp, 0, SEEK_END);
			if (!est_language_code) {
				fputs("\nlanguage_code = -1", fp);
			}
			if (!est_skip_intro) {
				fputs("\nskip_intro = 0", fp);
			}
			if (!est_disable_ingame_keyboard) {
				char disable_ingame_keyboard_entry[40];
				sprintf(disable_ingame_keyboard_entry, "\ndisable_ingame_keyboard = %d", (bool)(getPlayerNumber() - 1));
				fputs(disable_ingame_keyboard_entry, fp);
			}
			if (!est_hotkey_help) {
				char hotkeyText[60];
				sprintf(hotkeyText, "\nhotkey_help = %d #", hotkeyIdHelp);
				GetVKeyCodeString(hotkeyIdHelp, hotkeyText + strlen(hotkeyText), 20);
				fputs(hotkeyText, fp);
			}
			if (!est_hotkey_toggle_debug) {
				char hotkeyText[60];
				sprintf(hotkeyText, "\nhotkey_toggle_debug = %d #", hotkeyIdToggleDebug);
				GetVKeyCodeString(hotkeyIdToggleDebug, hotkeyText + strlen(hotkeyText), 20);
				fputs(hotkeyText, fp);
			}
			if (!est_hotkey_align_window) {
				char hotkeyText[60];
				sprintf(hotkeyText, "\nhotkey_align_window = %d #", hotkeyIdAlignWindow);
				GetVKeyCodeString(hotkeyIdAlignWindow, hotkeyText + strlen(hotkeyText), 20);
				fputs(hotkeyText, fp);
			}
			if (!est_hotkey_window_mode) {
				char hotkeyText[60];
				sprintf(hotkeyText, "\nhotkey_window_mode = %d #", hotkeyIdWindowMode);
				GetVKeyCodeString(hotkeyIdWindowMode, hotkeyText + strlen(hotkeyText), 20);
				fputs(hotkeyText, fp);
			}
			fclose(fp);
		}
		else {
			fileFail(fp);
		}
	}
	char NotificationPlayerText[255];
	if (flagged_pos == -1) {
		sprintf(NotificationPlayerText, "Successfully created new config file: %ls", fileStartupini);
		addDebugText(NotificationPlayerText);
	}
	else if (flagged_pos == -2) {
		sprintf(NotificationPlayerText, "Successfully updated & loaded config file: %ls", fileStartupini);
		addDebugText(NotificationPlayerText);
	}
	else {
		sprintf(NotificationPlayerText, "Successfully loaded config file: %ls", fileStartupini);
		addDebugText(NotificationPlayerText);
	}
}

void ProcessH2Startup() {
	initDebugText();
	//halo2ThreadID = GetCurrentThreadId();
	if (GetModuleHandle(L"H2Server.exe"))
	{
		H2BaseAddr = (DWORD)GetModuleHandle(L"H2Server.exe");
		H2IsDediServer = true;
		addDebugText("Process is Dedi-Server");
	}
	else
	{
		H2BaseAddr = (DWORD)GetModuleHandle(L"halo2.exe");
		H2IsDediServer = false;
		addDebugText("Process is Client");
	}
	initPlayerNumber();
	ReadStartupOptions();

	if (!H2IsDediServer) {
		bool IntroHQ = true;//clients should set on halo2.exe -highquality

		if (language_code >= 0 && language_code <= 7) {
			BYTE assmLang[15];
			memset(assmLang, language_code, 15);
			OverwriteAssembly((BYTE*)H2BaseAddr + 0x38300, assmLang, 15);
		}

		if (skip_intro) {
			BYTE assmIntroSkip[] = { 0x3F };
			OverwriteAssembly((BYTE*)H2BaseAddr + 0x221C0E, assmIntroSkip, 1);
		}

		if (!skip_intro && IntroHQ) {
			BYTE assmIntroHQ[] = { 0xEB };
			OverwriteAssembly((BYTE*)H2BaseAddr + 0x221C29, assmIntroHQ, 1);
		}

		//Allows unlimited clients
		BYTE assmUnlimitedClients[41];
		memset(assmUnlimitedClients, 0x00, 41);
		OverwriteAssembly((BYTE*)H2BaseAddr + 0x39BCF0, assmUnlimitedClients, 41);

		//Allows on a remote desktop connection
		BYTE assmRemoteDesktop[] = { 0xEB };
		OverwriteAssembly((BYTE*)H2BaseAddr + 0x7E54, assmRemoteDesktop, 1);

		//multi-process splitscreen input hacks
		if (disable_ingame_keyboard) {
			//Allows to repeat last movement when lose focus in mp, unlocks METHOD E from point after intro vid
			BYTE getFocusB[] = { 0x00 };
			OverwriteAssembly((BYTE*)H2BaseAddr + 0x2E3C5, getFocusB, 1);
			//Allows input when not in focus.
			BYTE getFocusE[] = { 0x90, 0x90, 0x90, 0x90, 0x90, 0x90 };
			OverwriteAssembly((BYTE*)H2BaseAddr + 0x2F9EA, getFocusE, 6);
			OverwriteAssembly((BYTE*)H2BaseAddr + 0x2F9FC, getFocusE, 6);
			OverwriteAssembly((BYTE*)H2BaseAddr + 0x2FA09, getFocusE, 6);
			//Disables the keyboard only when in-game and not in a menu.
			BYTE disableKeyboard1[] = { 0x90, 0x90, 0x90 };
			OverwriteAssembly((BYTE*)H2BaseAddr + 0x2FA8A, disableKeyboard1, 3);
			BYTE disableKeyboard2[] = { 0x00 };
			OverwriteAssembly((BYTE*)H2BaseAddr + 0x2FA92, disableKeyboard2, 1);
			BYTE disableKeyboard3[] = { 0x90, 0x90, 0x90, 0x90, 0x90, 0x90 };
			OverwriteAssembly((BYTE*)H2BaseAddr + 0x2FA67, disableKeyboard3, 6);
		}

	}

	addDebugText("ProcessStartup finished.");
}
