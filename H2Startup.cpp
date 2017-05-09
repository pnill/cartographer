#include "H2Startup.h"
#include "H2OnscreenDebugLog.h"
#include "GSUtils.h"
#include "GSRunLoop.h"
#include <Shellapi.h>
#include "Hook.h"
#include "GSSecurity.h"

int language_code = -1;
bool skip_intro = false;
bool disable_ingame_keyboard = false;
int custom_resolution_x = 0;
int custom_resolution_y = 0;
bool hide_ingame_chat = false;
wchar_t dedi_server_name[32];
wchar_t dedi_server_playlist[256];
bool H2IsDediServer;
DWORD H2BaseAddr;
wchar_t* processFilePath;
HWND H2hWnd = NULL;

int playerNumber = 0;
int getPlayerNumber() {
	return playerNumber;
}

void fileFail(FILE* fp) {
	int fperrno = GetLastError();
	if (fperrno == EACCES || fperrno == EIO || fperrno == EPERM) {
		MessageBoxA(NULL, "Cannot write a file. Please restart Halo 2 in Administrator mode!", "Permission Error!", MB_OK);
	}
	else if (fperrno == ESRCH) {
		MessageBoxA(NULL, "Probably a missing folder issue if file writing related. Please restart Halo 2 in Administrator mode!", "Permission Error!", MB_OK);
	}
	else if (!fp) {
		char NotificationPlayerText[20];
		sprintf(NotificationPlayerText, "Error 0x%x", fperrno);
		addDebugText(NotificationPlayerText);
		MessageBoxA(NULL, NotificationPlayerText, "Unknown File Failure!", MB_OK);
	}
}

const wchar_t* xinputdllPath = L"xinput/p00/xinput9_1_0.dll";
wchar_t* instanceMutexClient = L"Halo2Player%d";
wchar_t* instanceMutexServer = L"Halo2Server%d";

void initPlayerNumber() {
	addDebugText("Determining Process Instance Number.");
	HANDLE mutex;
	DWORD lastErr;
	do {
		playerNumber++;
		wchar_t mutexName[32];
		wchar_t* instanceName = instanceMutexClient;
		if (H2IsDediServer) {
			instanceName = instanceMutexServer;
		}
		swprintf(mutexName, instanceName, playerNumber);
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
			sprintf(totext, "injecting new xinputdll path instruction: %ls : %x : %x %x %x %x", (DWORD)xinputdllPath, (DWORD)xinputdllPath, byteArray[3], byteArray[2], byteArray[1], byteArray[0]);
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
			int fperrno1 = GetLastError();
			if (!(fperrno1 == ERROR_ALREADY_EXISTS || fperrno1 == ERROR_SUCCESS)) {
				fileFail(NULL);
				MessageBoxA(NULL, "Error 7g546.", "Unknown Error", MB_OK);
				exit(EXIT_FAILURE);
			}
			CreateDirectory(xinputdir2, NULL);
			int fperrno2 = GetLastError();
			if (!(fperrno2 == ERROR_ALREADY_EXISTS || fperrno2 == ERROR_SUCCESS)) {
				fileFail(NULL);
				MessageBoxA(NULL, "Error 7g547.", "Unknown Error", MB_OK);
				exit(EXIT_FAILURE);
			}
			if (FILE *file = fopen(xinputName, "r")) {
				fclose(file);
			}
			else {
				int xinput_index = -1;
				char xinput_md5_durazno_0_6_0_0[] = "6140ae76b26a633ce2255f6fc88fbe34";
				long xinput_offset_durazno_0_6_0_0 = 0x196de;
				bool xinput_unicode_durazno_0_6_0_0 = true;
				char xinput_md5_x360ce_3_3_1_444[] = "8f24e36d5f0a71c8a412cec6323cd781";
				long xinput_offset_x360ce_3_3_1_444 = 0x1ea54;
				bool xinput_unicode_x360ce_3_3_1_444 = true;
				char xinput_md5_x360ce_3_4_1_1357[] = "5236623449893c0e1e98fc95f067fcff";
				long xinput_offset_x360ce_3_4_1_1357 = 0x16110;
				bool xinput_unicode_x360ce_3_4_1_1357 = false;
				const int xinput_array_length = 3;
				char* xinput_md5[xinput_array_length] = { xinput_md5_durazno_0_6_0_0, xinput_md5_x360ce_3_3_1_444, xinput_md5_x360ce_3_4_1_1357 };
				long xinput_offset[xinput_array_length] = { xinput_offset_durazno_0_6_0_0, xinput_offset_x360ce_3_3_1_444, xinput_offset_x360ce_3_4_1_1357 };
				bool xinput_unicode[xinput_array_length] = { xinput_unicode_durazno_0_6_0_0, xinput_unicode_x360ce_3_3_1_444, xinput_unicode_x360ce_3_4_1_1357 };
				char available_xinput_md5[33];
				int hasherr = ComputeFileMd5Hash(L"xinput9_1_0.dll", available_xinput_md5);
				FILE* file1 = NULL;
				if (hasherr == 0 && (file1 = fopen("xinput9_1_0.dll", "rb"))) {
					for (int i = 0; i < xinput_array_length; i++) {
						if (strcmp(xinput_md5[i], available_xinput_md5) == 0) {
							xinput_index = i;
							break;
						}
					}
					if (xinput_index < 0) {
						char xinputError[] = "ERROR! For \'Split-screen\' play, a supported xinput9_1_0.dll is required to be installed in the local game directory!\nOr you may install a custom one manually in the xinput/p??/ folders.";
						addDebugText(xinputError);
						MessageBoxA(NULL, xinputError, "Incorrect DLL Error", MB_OK);
						exit(EXIT_FAILURE);
					}
					FILE* file2 = fopen(xinputName, "wb");
					if (!file2) {
						fileFail(file2);
						MessageBoxA(NULL, "Error bf58i.", "Unknown Error", MB_OK);
						exit(EXIT_FAILURE);
					}
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
					int len_to_write = 2;
					BYTE assmXinputDuraznoNameEdit[] = { 0x30 + (getPlayerNumber() / 10), 0x30 + (getPlayerNumber() % 10), 0x30 + (getPlayerNumber() % 10) };
					if (xinput_unicode[xinput_index]) {
						assmXinputDuraznoNameEdit[1] = 0x00;
						len_to_write = 3;
					}
					if (fseek(file3, xinput_offset[xinput_index], SEEK_SET) == 0 && fwrite(assmXinputDuraznoNameEdit, sizeof(BYTE), len_to_write, file3) == len_to_write) {
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
					char xinputError[] = "ERROR! An xinput9_1_0.dll does not exist in the local game directory!\nFor \'Split-screen\' play, any supported .dll is required.";
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
	addDebugText("Finished Processing Instance Number.");
}

void ReadStartupOptions() {
	addDebugText("Begin Read Startup Options.");

	wchar_t fileStartupini[1024];
	swprintf(fileStartupini, 1024, L"%wsh2startup%d.ini", processFilePath, getPlayerNumber());

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

	char awerg[1034];
	sprintf(awerg, "PATH: %ws", fileStartupini);
	addDebugText(awerg);

	//Variables read check
	bool est_language_code = false;
	bool est_skip_intro = false;
	bool est_disable_ingame_keyboard = false;
	bool est_custom_resolution = false;
	bool est_server_name = false;
	dedi_server_name[31] = dedi_server_name[0] = 0;
	bool est_server_playlist = false;
	dedi_server_playlist[255] = dedi_server_playlist[0] = 0;
	//Hotkeys
	bool est_hotkey_help = false;
	bool est_hotkey_toggle_debug = false;
	bool est_hotkey_align_window = false;
	bool est_hotkey_window_mode = false;
	bool est_hotkey_hide_ingame_chat = false;

	int flagged[256];
	int flagged_pos = -1;
	FILE *fp;
	if (fp = _wfopen(fileStartupini, L"r")) {
		addDebugText("File found.");
		flagged_pos = 0;
		char string[512];
		while (fgets(string, 511, fp)) {
			if (flagged_pos > 512) {
				addDebugText("File config overflow! There are too many bad lines in SETUP config!");
				MessageBoxA(NULL, "There are too many bad lines in SETUP config!", "File config overflow!", MB_OK);
				exit(EXIT_FAILURE);
			}
			if (strlen(string) < 4 || string[0] == '#' || (string[0] == '/' && string[1] == '/')) {
				continue;
			}
			if (strstr(string, "language_code =")) {
				int temp;
				sscanf(string + strlen("language_code ="), "%d", &temp);
				if (est_language_code || temp < -1 || temp > 7) {
					flagged[flagged_pos++] = FindLineStart(fp, strlen(string));
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
					flagged[flagged_pos++] = FindLineStart(fp, strlen(string));
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
					flagged[flagged_pos++] = FindLineStart(fp, strlen(string));
				}
				else {
					disable_ingame_keyboard = (bool)temp;
					est_disable_ingame_keyboard = true;
				}
			}
			else if (strstr(string, "custom_resolution =")) {
				int tempX;
				int tempY;
				sscanf(string + strlen("custom_resolution ="), "%dx%d", &tempX, &tempY);
				if (est_custom_resolution || !(tempX >= 0 && tempY >= 0)) {
					flagged[flagged_pos++] = FindLineStart(fp, strlen(string));
				}
				else {
					custom_resolution_x = tempX;
					custom_resolution_y = tempY;
					est_custom_resolution = true;
				}
			}
			else if (strstr(string, "server_name =")) {
				if (est_server_name) {
					flagged[flagged_pos++] = FindLineStart(fp, strlen(string));
				}
				else {
					char* tempName = string + strlen("server_name =");
					while (isspace(*tempName)) {
						tempName++;
					}
					swprintf(dedi_server_name, 32, L"%hs", tempName);
					for (int j = wcslen(dedi_server_name) - 1; j > 0; j--) {
						if (isspace(dedi_server_name[j])) {
							dedi_server_name[j] = 0;
						}
						else {
							break;
						}
					}
					est_server_name = true;
				}
			}
			else if (strstr(string, "server_playlist =")) {
				if (est_server_playlist) {
					flagged[flagged_pos++] = FindLineStart(fp, strlen(string));
				}
				else {
					char* tempName = string + strlen("server_playlist =");
					while (isspace(*tempName)) {
						tempName++;
					}
					swprintf(dedi_server_playlist, 256, L"%hs", tempName);
					for (int j = wcslen(dedi_server_playlist)-1; j > 0; j--) {
						if (isspace(dedi_server_playlist[j])) {
							dedi_server_playlist[j] = 0;
						}
						else {
							break;
						}
					}
					est_server_playlist = true;
				}
			}
			else if (strstr(string, "hotkey_help =")) {
				int temp;
				sscanf(string + strlen("hotkey_help ="), "%d", &temp);
				if (est_hotkey_help || !(temp >= 0)) {
					flagged[flagged_pos++] = FindLineStart(fp, strlen(string));
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
					flagged[flagged_pos++] = FindLineStart(fp, strlen(string));
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
					flagged[flagged_pos++] = FindLineStart(fp, strlen(string));
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
					flagged[flagged_pos++] = FindLineStart(fp, strlen(string));
				}
				else {
					hotkeyIdWindowMode = temp;
					est_hotkey_window_mode = true;
				}
			}
			else if (strstr(string, "hotkey_hide_ingame_chat =")) {
				int temp;
				sscanf(string + strlen("hotkey_hide_ingame_chat ="), "%d", &temp);
				if (est_hotkey_hide_ingame_chat || !(temp >= 0)) {
					flagged[flagged_pos++] = FindLineStart(fp, strlen(string));
				}
				else {
					hotkeyIdToggleHideIngameChat = temp;
					est_hotkey_hide_ingame_chat = true;
				}
			}
		}
		fclose(fp);
		fp = NULL;
		if (!flagged_pos && !(est_language_code && est_skip_intro && est_disable_ingame_keyboard /*&& est_custom_resolution*/ && est_server_name && est_server_playlist && est_hotkey_help && est_hotkey_toggle_debug && est_hotkey_align_window && est_hotkey_window_mode && est_hotkey_hide_ingame_chat)) {
			flagged_pos = -2;
		}
	}
	if (flagged_pos) {
		addDebugText("Edits to be made.");
		if (flagged_pos == -1) {
			if (fp = _wfopen(fileStartupini, L"w")) {
				fclose(fp);
			}
			else {
				addDebugText("Unknown Error: Error 78t6.");
				fileFail(fp);
				MessageBoxA(NULL, "Error 78t6.", "Unknown Error", MB_OK);
				exit(EXIT_FAILURE);
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
				//fputs("# custom_resolution Options (Client):", fp);
				//fputs("\n# <width>x<height> - Sets the resolution of the game via the Windows Registry.", fp);
				//fputs("\n# 0x0, 0x?, ?x0 - these do not do modify anything where ? is >= 0.", fp);
				//fputs("\n\n", fp);
				fputs("# server_name Options (Server):", fp);
				fputs("\n# Sets the name of the server up to 31 characters long.", fp);
				fputs("\n# Leave blank/empty for no effect.", fp);
				fputs("\n\n", fp);
				fputs("# server_playlist Options (Server):", fp);
				fputs("\n# Sets the playlist of the server up to 255 characters long.", fp);
				fputs("\n# Leave blank/empty for no effect.", fp);
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
			//if (!est_custom_resolution) {
			//	fputs("\ncustom_resolution = 0x0", fp);
			//}
			if (!est_server_name) {
				fputs("\nserver_name = ", fp);
			}
			if (!est_server_playlist) {
				fputs("\nserver_playlist = ", fp);
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
			if (!est_hotkey_hide_ingame_chat) {
				char hotkeyText[60];
				sprintf(hotkeyText, "\nhotkey_hide_ingame_chat = %d #", hotkeyIdToggleHideIngameChat);
				GetVKeyCodeString(hotkeyIdToggleHideIngameChat, hotkeyText + strlen(hotkeyText), 20);
				fputs(hotkeyText, fp);
			}
			fclose(fp);
		}
		else {
			addDebugText("Unknown Error: Error bn689.");
			fileFail(fp);
			MessageBoxA(NULL, "Error bn689.", "Unknown Error", MB_OK);
			exit(EXIT_FAILURE);
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
	addDebugText("End Read Startup Options.");
}

typedef int(__cdecl *thookServ1)(HKEY, LPCWSTR);
thookServ1 phookServ1;
int __cdecl LoadRegistrySettings(HKEY hKey, LPCWSTR lpSubKey) {
	char result =
		phookServ1(hKey, lpSubKey);
	addDebugText("Post Server Registry Read.");
	if (wcslen(dedi_server_name) > 0) {
		wchar_t* PreLoadServerName = (wchar_t*)((BYTE*)H2BaseAddr + 0x3B49B4);
		swprintf(PreLoadServerName, 15, dedi_server_name);
		//char temp[27];
		//snprintf(temp, 27, "%ws", dedi_server_name);
		//MessageBoxA(NULL, temp, "Server Pre name thingy", MB_OK);
		wchar_t* LanServerName = (wchar_t*)((BYTE*)H2BaseAddr + 0x52042A);
		swprintf(LanServerName, 2, L"");
	}
	if (wcslen(dedi_server_playlist) > 0) {
		wchar_t* ServerPlaylist = (wchar_t*)((BYTE*)H2BaseAddr + 0x3B3704);
		swprintf(ServerPlaylist, 256, dedi_server_playlist);
	}
	addDebugText("Initialising GSRunLoop.");
	extern void initGSRunLoop();
	initGSRunLoop();
	return result;
}

typedef int(__stdcall *thookServ2)();
thookServ2 phookServ2;
int __stdcall PreReadyLoad() {
	MessageBoxA(NULL, "aaaaaaas", "PreCrash", MB_OK);
	int result =
		phookServ2();
	MessageBoxA(NULL, "aaaaaaasaaaaaa", "PostCrash", MB_OK);
	if (wcslen(dedi_server_name) > 0) {
		wchar_t* PreLoadServerName = (wchar_t*)((BYTE*)H2BaseAddr + 0x52042A);
		swprintf(PreLoadServerName, 32, dedi_server_name);
		char temp[27];
		snprintf(temp, 32, "%ws", dedi_server_name);
		MessageBoxA(NULL, temp, "AAA Server Pre name thingy", MB_OK);
	}
	return result;
}

static bool NotDisplayIngameChat() {
	int GameGlobals = (int)*(int*)((DWORD)H2BaseAddr + 0x482D3C);
	DWORD* GameEngine = (DWORD*)(GameGlobals + 0x8);
	BYTE* GameState = (BYTE*)((DWORD)H2BaseAddr + 0x420FC4);

	if (hide_ingame_chat) {
		int GameTimeGlobals = (int)*(int*)((DWORD)H2BaseAddr + 0x4C06E4);
		DWORD* ChatOpened = (DWORD*)(GameTimeGlobals + 0x354);//MP Only?
		if (*ChatOpened == 2) {
			extern void hotkeyFuncToggleHideIngameChat();
			hotkeyFuncToggleHideIngameChat();
		}
	}

	if (hide_ingame_chat) {
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

void ProcessH2Startup() {
	int ArgCnt;
	LPWSTR* ArgList = CommandLineToArgvW(GetCommandLineW(), &ArgCnt);
	processFilePath = (wchar_t*)malloc(wcslen(ArgList[0]) * sizeof(wchar_t));
	int rtncodepath = GetWidePathFromFullWideFilename(ArgList[0], processFilePath);
	if (rtncodepath == -1) {
		swprintf(processFilePath, L"");
	}

	initDebugText();
	//halo2ThreadID = GetCurrentThreadId();
	if (GetModuleHandle(L"H2Server.exe")) {
		H2BaseAddr = (DWORD)GetModuleHandle(L"H2Server.exe");
		H2IsDediServer = true;
		addDebugText("Process is Dedi-Server");
	}
	else {
		H2BaseAddr = (DWORD)GetModuleHandle(L"halo2.exe");
		H2IsDediServer = false;
		addDebugText("Process is Client");
	}
	
	initPlayerNumber();
	ReadStartupOptions();

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

		if (custom_resolution_x > 0 && custom_resolution_y > 0) {
			if (custom_resolution_x != (int)tempResX || custom_resolution_y != (int)tempResY) {
				LSTATUS err;
				if (err = RegOpenKeyExW(HKEY_CURRENT_USER, L"Software\\Microsoft\\Halo 2\\Video Settings", 0, KEY_ALL_ACCESS, &hKeyResolution) == ERROR_SUCCESS) {
					RegSetValueEx(hKeyResolution, L"ScreenResX", NULL, REG_DWORD, (const BYTE*)&custom_resolution_x, sizeof(custom_resolution_x));
					RegSetValueEx(hKeyResolution, L"ScreenResY", NULL, REG_DWORD, (const BYTE*)&custom_resolution_y, sizeof(custom_resolution_y));
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

		if (language_code >= 0 && language_code <= 7) {
			BYTE assmLang[15];
			memset(assmLang, language_code, 15);
			OverwriteAssembly((BYTE*)H2BaseAddr + 0x38300, assmLang, 15);
			BYTE* HasLoadedLanguage = (BYTE*)((char*)H2BaseAddr + 0x481908);
			*HasLoadedLanguage = 0;
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

		//Disables the ESRB warning after the intro video (only occurs for English Language).
		BYTE disableEsrbWarning[] = { 0xFF, 0xFF, 0xFF, 0xFF };
		OverwriteAssembly((BYTE*)H2BaseAddr + 0x23EE8B, disableEsrbWarning, 4);

		//Redirects the is_campaign call that the in-game chat renderer makes so we can show/hide it as we like.
		DWORD chatFunc = (DWORD)NotDisplayIngameChat;

		DWORD instCallAddr2 = H2BaseAddr + 0x22667B;
		DWORD callRelative2 = chatFunc - (instCallAddr2 + 5);
		BYTE* pbyte2 = (BYTE*)&callRelative2;
		BYTE assmFuncChatRel2[4] = { pbyte2[0], pbyte2[1], pbyte2[2], pbyte2[3] };
		OverwriteAssembly((BYTE*)instCallAddr2 + 1, assmFuncChatRel2, 4);

		DWORD instCallAddr1 = H2BaseAddr + 0x226628;
		DWORD callRelative1 = chatFunc - (instCallAddr1 + 5);
		BYTE* pbyte1 = (BYTE*)&callRelative1;
		BYTE assmFuncChatRel1[4] = { pbyte1[0], pbyte1[1], pbyte1[2], pbyte1[3] };
		OverwriteAssembly((BYTE*)instCallAddr1 + 1, assmFuncChatRel1, 4);
	}
	addDebugText("End Startup Tweaks.");
	extern void GSSecStartLoop();
	GSSecStartLoop();
	extern void GSSecSweetLeetHaxA(int);
	GSSecSweetLeetHaxA(0);
	addDebugText("ProcessStartup finished.");
}
