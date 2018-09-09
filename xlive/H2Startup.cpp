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
#include <string>
#include <sstream>
#include "Util/hash.h"
#include "util/filesys.h"

#ifndef NO_TRACE
logger *xlive_trace_log = nullptr;
logger *h2mod_log = nullptr;
logger *network_log = nullptr;
logger *checksum_log = nullptr;
#endif


ProcessInfo game_info;

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

void initInstanceNumber() {
	addDebugText("Determining Process Instance Number.");
	HANDLE mutex;
	DWORD lastErr;
	do {
		instanceNumber++;
		wchar_t mutexName[32];
		swprintf(mutexName, sizeof(mutexName), (H2IsDediServer ? L"Halo2Server%d" : L"Halo2Player%d"), instanceNumber);
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

wchar_t xinput_path[_MAX_PATH];

void configureXinput() {
	if (!H2IsDediServer) {
		if (H2GetInstanceId() > 1) {
			swprintf(xinput_path, ARRAYSIZE(xinput_path), L"xinput/p%02d/xinput9_1_0.dll", H2GetInstanceId());
			TRACE_FUNC("Changing xinput path to '%s' : '%x'", xinput_path, xinput_path);

			WriteValue(H2BaseAddr + 0x8AD28, xinput_path);

			char xinputName[_MAX_PATH];
			char xinputdir[_MAX_PATH];
			sprintf(xinputdir, "xinput/p%02d", H2GetInstanceId());
			sprintf(xinputName, "%s/xinput9_1_0.dll", xinputdir);
			CreateDirectoryA("xinput", NULL);
			int fperrno1 = GetLastError();
			if (!(fperrno1 == ERROR_ALREADY_EXISTS || fperrno1 == ERROR_SUCCESS)) {
				fileFail(NULL);
				MessageBoxA(NULL, "Error 7g546.", "Unknown Error", MB_OK);
				exit(EXIT_FAILURE);
			}
			CreateDirectoryA(xinputdir, NULL);
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
				std::string available_xinput_md5;
				int hasherr = hashes::calc_file_md5("xinput9_1_0.dll", available_xinput_md5);
				FILE* file1 = NULL;
				if (hasherr == 0 && (file1 = fopen("xinput9_1_0.dll", "rb"))) {
					for (int i = 0; i < xinput_array_length; i++) {
						if (strcmp(xinput_md5[i], available_xinput_md5.c_str()) == 0) {
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
					snprintf(xinputError, 200, "Hash Error Num: %x - msg: %s", hasherr, available_xinput_md5.c_str());
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

CRITICAL_SECTION log_section;

H2Types detect_process_type()
{
	// try and detect type based on module name.
	if (GetModuleHandleW(L"h2server.exe"))
		return H2Types::H2Server;
	else if (GetModuleHandleW(L"Halo2.exe"))
		return H2Types::H2Game;

	// fallback to checking file information in case the file was renamed.
	wchar_t exe_file_path[_MAX_PATH + 1];
	int result = GetModuleFileNameW(NULL, exe_file_path, ARRAYSIZE(exe_file_path));
	if (0 < result <= _MAX_PATH) {
		DWORD version_info_size = GetFileVersionInfoSizeW(exe_file_path, NULL);
		if (version_info_size != 0) {

			void *version_info = new BYTE[version_info_size];
			 if (GetFileVersionInfoW(exe_file_path, NULL, version_info_size, version_info)) {
				wchar_t *orginal_filename;
				size_t filename_len;
				// shouldn't be hardcoded but who cares
				VerQueryValueW(version_info, L"\\StringFileInfo\\040904b0\\OriginalFilename", (LPVOID*)&orginal_filename, &filename_len);

				std::wstring exe_orginal_filename = orginal_filename;
				delete[] version_info;

				if (exe_orginal_filename == L"h2server.exe")
					return H2Types::H2Server;
				else if (exe_orginal_filename == L"Halo2.exe")
					return H2Types::H2Game;
			}
		}
	}
	return H2Types::Invalid;
}

inline std::string prepareLogFileName(std::string logFileName) {
	std::string instanceNumber("");
	if (H2GetInstanceId() > 1) {
		std::stringstream stream;
		stream << H2GetInstanceId();
		instanceNumber = ".";
		instanceNumber += stream.str();
	}
	std::string filename = logFileName;
	filename += instanceNumber;
	filename += ".log";
	return filename;
}

///Before the game window appears
void InitH2Startup() {
	InitializeCriticalSection(&log_section);
	Debug::init();

	game_info.base = GetModuleHandle(NULL);
	game_info.process_type = detect_process_type();

	if (game_info.process_type == H2Types::Invalid)
	{
		MessageBoxA(NULL, "Project Cartographer loaded into unsupported process, will now exit!", "ERROR!", MB_OK);
		TerminateProcess(GetCurrentProcess(), 1);
	}

	H2BaseAddr = (DWORD)game_info.base;

	int ArgCnt;
	LPWSTR* ArgList = CommandLineToArgvW(GetCommandLineW(), &ArgCnt);
	H2ProcessFilePath = (wchar_t*)malloc(wcslen(ArgList[0]) * sizeof(wchar_t));
	int rtncodepath = GetWidePathFromFullWideFilename(ArgList[0], H2ProcessFilePath);
	if (rtncodepath == -1) {
		swprintf(H2ProcessFilePath, 2, L"");
	}

	// fix the game not finding the files it needs if the current directory is not the install directory
	SetCurrentDirectoryW(GetExeDirectoryWide().c_str());

	initLocalAppData();

	initDebugText();
	//halo2ThreadID = GetCurrentThreadId();
	if (game_info.process_type == H2Types::H2Server) {
		H2IsDediServer = true;
		addDebugText("Process is Dedi-Server");
	}
	else {
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
#ifndef NO_TRACE
	EnterCriticalSection(&log_section);
	if (H2Config_debug_log) {
		xlive_trace_log = logger::create(prepareLogFileName("xlive_trace"));
		TRACE("Log started (xLiveLess " DLL_VERSION_STR ")\n");
		h2mod_log = logger::create(prepareLogFileName("h2mod"));
		TRACE_GAME("Log started (H2MOD " DLL_VERSION_STR ")\n");
		network_log = logger::create(prepareLogFileName("h2network"));
		TRACE_GAME_NETWORK("Log started (H2MOD - Network " DLL_VERSION_STR ")\n");
	}
	checksum_log = logger::create(prepareLogFileName("checksum"), true);
	LeaveCriticalSection(&log_section);
#endif
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
