#include "H2Startup.h"
#include "H2OnscreenDebugLog.h"
#include "GSUtils.h"
#include "GSRunLoop.h"
#include "H2Config.h"
#include "H2Accounts.h"
#include <Shellapi.h>
#include "Hook.h"
#include "H2Tweaks.h"
#include <sys/stat.h>
#include "GSAccountLogin.h"
#include "Util\Debug.h"


bool H2IsDediServer;
DWORD H2BaseAddr;
wchar_t* H2ProcessFilePath = 0;
wchar_t* H2AppDataLocal = 0;
wchar_t* FlagFilePathConfig = 0;
HWND H2hWnd = NULL;

int instanceNumber = 0;
int H2GetInstanceId() {
	return instanceNumber;
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

wchar_t instanceMutexNames[2][14] = { L"Halo2Player%d", L"Halo2Server%d" };

void initInstanceNumber() {
	addDebugText("Determining Process Instance Number.");
	HANDLE mutex;
	DWORD lastErr;
	do {
		instanceNumber++;
		wchar_t mutexName[32];
		swprintf(mutexName, sizeof(mutexName), instanceMutexNames[H2IsDediServer], instanceNumber);
		mutex = CreateMutex(0, TRUE, mutexName);
		lastErr = GetLastError();
		if (lastErr == ERROR_ALREADY_EXISTS) {
			CloseHandle(mutex);
		}
	} while (lastErr == ERROR_ALREADY_EXISTS);

	char NotificationPlayerText[30];
	sprintf(NotificationPlayerText, "You are Instance #%d.", instanceNumber);
	addDebugText(NotificationPlayerText);
}

const wchar_t* xinputdllPath = L"xinput/p00/xinput9_1_0.dll";

void configureXinput() {
	if (!H2IsDediServer) {
		if (H2GetInstanceId() > 1) {
			BYTE xinputNumFix[] = { '0' + (H2GetInstanceId() / 10), 0, '0' + (H2GetInstanceId() % 10) };
			WriteBytes((DWORD)xinputdllPath + 16, xinputNumFix, 3);

			char pointerHex[20];
			sprintf(pointerHex, "%x", (DWORD)xinputdllPath);
			BYTE byteArray[4] = { 0,0,0,0 };
			HexToByteArray(byteArray, pointerHex);

			char totext[255];
			sprintf(totext, "injecting new xinputdll path instruction: %ls : %x : %x %x %x %x", (DWORD)xinputdllPath, (DWORD)xinputdllPath, byteArray[3], byteArray[2], byteArray[1], byteArray[0]);
			addDebugText(totext);

			BYTE assmXinputPushIntructionPart[] = { byteArray[3], byteArray[2], byteArray[1], byteArray[0] };
			WriteBytes(H2BaseAddr + 0x8AD28, assmXinputPushIntructionPart, 4);

			char xinputName[40];
			char xinputdir[12];
			sprintf(xinputdir, "xinput/p%d%d", H2GetInstanceId() / 10, H2GetInstanceId() % 10);
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
					BYTE assmXinputDuraznoNameEdit[] = { 0x30 + (H2GetInstanceId() / 10), 0x30 + (H2GetInstanceId() % 10), 0x30 + (H2GetInstanceId() % 10) };
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

void initLocalAppData() {
	addDebugText("Find AppData Local.");

	if (H2AppDataLocal != 0) {
		free(H2AppDataLocal);
		H2AppDataLocal = 0;
	}

	wchar_t* userprofile = _wgetenv(L"USERPROFILE");

	wchar_t local2[1024];

	swprintf(local2, 1024, L"%ws\\AppData\\Local\\", userprofile);
	struct _stat64i32 sb;
	if (_wstat(local2, &sb) == 0 && sb.st_mode & S_IFDIR) {
		swprintf(local2, 1024, L"%ws\\AppData\\Local\\Microsoft\\", userprofile);
		CreateDirectoryW(local2, NULL);
		int fperrno1 = GetLastError();
		if (fperrno1 == ERROR_ALREADY_EXISTS || fperrno1 == ERROR_SUCCESS) {
			swprintf(local2, 1024, L"%ws\\AppData\\Local\\Microsoft\\Halo 2\\", userprofile);
			CreateDirectoryW(local2, NULL);
			int fperrno1 = GetLastError();
			if (fperrno1 == ERROR_ALREADY_EXISTS || fperrno1 == ERROR_SUCCESS) {
				int appdatabuflen = wcslen(local2) + 1;
				H2AppDataLocal = (wchar_t*)malloc(sizeof(wchar_t) * appdatabuflen);
				wcscpy_s(H2AppDataLocal, appdatabuflen, local2);
			}
		}
	}
	else if (swprintf(local2, 1024, L"%ws\\Local Settings\\Application Data\\", userprofile), _wstat(local2, &sb) == 0 && sb.st_mode & S_IFDIR)
	{
		swprintf(local2, 1024, L"%ws\\Local Settings\\Application Data\\Microsoft\\", userprofile);
		CreateDirectoryW(local2, NULL);
		int fperrno1 = GetLastError();
		if (fperrno1 == ERROR_ALREADY_EXISTS || fperrno1 == ERROR_SUCCESS) {
			swprintf(local2, 1024, L"%ws\\Local Settings\\Application Data\\Microsoft\\Halo 2\\", userprofile);
			CreateDirectoryW(local2, NULL);
			int fperrno1 = GetLastError();
			if (fperrno1 == ERROR_ALREADY_EXISTS || fperrno1 == ERROR_SUCCESS) {
				int appdatabuflen = wcslen(local2) + 1;
				H2AppDataLocal = (wchar_t*)malloc(sizeof(wchar_t) * appdatabuflen);
				wcscpy_s(H2AppDataLocal, appdatabuflen, local2);
			}
		}
	}

	if (H2AppDataLocal == 0) {
		int appdatabuflen = wcslen(H2ProcessFilePath) + 1;
		H2AppDataLocal = (wchar_t*)malloc(sizeof(wchar_t) * appdatabuflen);
		swprintf(H2AppDataLocal, appdatabuflen, L"%ws", H2ProcessFilePath);
		addDebugText("ERROR: Could not find AppData Local. Using Process File Path:");
	}
	else {
		addDebugText("Found AppData Local:");
	}
	addDebugText(H2AppDataLocal);
}

typedef int(__cdecl *tsub_48BBF)();
tsub_48BBF psub_48BBF;
int __cdecl sub_48BBF() {
	int result = psub_48BBF();
	DeinitH2Startup();
	return result;
}

///Before the game window appears
void InitH2Startup() {
	Debug::init();

	int ArgCnt;
	LPWSTR* ArgList = CommandLineToArgvW(GetCommandLineW(), &ArgCnt);
	H2ProcessFilePath = (wchar_t*)malloc(wcslen(ArgList[0]) * sizeof(wchar_t));
	int rtncodepath = GetWidePathFromFullWideFilename(ArgList[0], H2ProcessFilePath);
	if (rtncodepath == -1) {
		swprintf(H2ProcessFilePath, 2, L"");
	}

	initLocalAppData();

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

		addDebugText("Hooking Shutdown Function");
		DWORD dwBack;
		psub_48BBF = (tsub_48BBF)DetourFunc((BYTE*)H2BaseAddr + 0x48BBF, (BYTE*)sub_48BBF, 11);
		VirtualProtect(psub_48BBF, 4, PAGE_EXECUTE_READWRITE, &dwBack);
	}
	
	initInstanceNumber();
	
	if (ArgList != NULL)
	{
		for (int i = 0; i < ArgCnt; i++)
		{
			if (wcsstr(ArgList[i], L"-h2config=") != NULL)
			{
				if (wcslen(ArgList[i]) < 255)
				{
					int pfcbuflen = wcslen(ArgList[i] + 10) + 1;
					FlagFilePathConfig = (wchar_t*)malloc(sizeof(wchar_t) * pfcbuflen);
					swprintf(FlagFilePathConfig, pfcbuflen, ArgList[i] + 10);
				}
			}
		}
	}

	InitH2Config();
	InitH2Accounts();

	configureXinput();

	InitH2Tweaks();
	extern void initGSCustomLanguage();
	initGSCustomLanguage();
	extern void initGSCustomMenu();
	initGSCustomMenu();
	extern void initGSRunLoop();
	initGSRunLoop();
	addDebugText("ProcessStartup finished.");
}

#include "GSDownload.h"

///After the game window appears
void InitH2Startup2() {
	InitGSDownload();

	if (H2IsDediServer) {
		addDebugText("Logging the Dedi Server in...");
		//none of that stuff is setup for the dedi server yet since there are no gui commands for it.
		//currently credentials are taken from the config file.
		//also don't enable this since nothing's initialised for the server.
		AccountEdit_remember = false;
		HandleGuiLogin(0, H2Config_login_identifier, H2Config_login_password);
	}
}

void DeinitH2Startup() {
	extern void deinitGSRunLoop();
	deinitGSRunLoop();
	extern void deinitGSCustomMenu();
	deinitGSCustomMenu();
	extern void deinitGSCustomLanguage();
	deinitGSCustomLanguage();
	DeinitH2Tweaks();
	DeinitH2Accounts();
	DeinitH2Config();
}
