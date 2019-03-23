
#include "H2MOD\Modules\Startup\Startup.h"
#include "H2MOD\Modules\Networking\Networking.h"
#include "H2MOD\Modules\OnScreenDebug\OnScreenDebug.h"
#include "H2MOD\Modules\Utils\Utils.h"
#include "H2MOD\Modules\RunLoop\RunLoop.h"
#include "..\H2MOD.h"

#include <Shellapi.h>
#include <sys/stat.h>
#include <string>
#include <sstream>
#include <io.h>
#include "Util\Hooks\Hook.h"
#include "Util\Debug\Debug.h"
#include "Util\hash.h"
#include "Util\filesys.h"

#include "H2MOD\Modules\Config\Config.h"
#include "H2MOD\Modules\Tweaks\Tweaks.h"
#include "H2MOD\Modules\Accounts\AccountLogin.h"
#include "H2MOD\Modules\Accounts\Accounts.h"


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
extern CustomNetwork *network;


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
		mutex = CreateMutexW(0, TRUE, mutexName);
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

bool configureXinput() {
	auto report_error = [](const std::string &message) {
		MessageBoxA(NULL, message.c_str(), "Xinput config error!", MB_OK);
	};

	if (!H2IsDediServer) {
		if (H2GetInstanceId() > 1) {
			swprintf(xinput_path, ARRAYSIZE(xinput_path), L"xinput/p%02d/xinput9_1_0.dll", H2GetInstanceId());
			TRACE_FUNC("Changing xinput path to '%s' : '%x'", xinput_path, xinput_path);

			WriteValue(H2BaseAddr + 0x8AD28, xinput_path);

			char xinputName[_MAX_PATH];
			char xinputdir[_MAX_PATH];
			sprintf(xinputdir, "xinput/p%02d", H2GetInstanceId());
			sprintf(xinputName, "%s/xinput9_1_0.dll", xinputdir);

			/* Creates a directory and displays error if it fails */
			auto create_dir = [=](const std::string &path) -> bool {
				if (LOG_CHECK(!CreateDirectoryA(path.c_str(), NULL) && GetLastError() != ERROR_ALREADY_EXISTS))
				{
					report_error("Failed to create '" + path + "' directory, check logs!");
					return false;
				}
				return true;
			};

			if (!create_dir("xinput") || !create_dir(xinputdir))
			{
				return false;
			}

			TRACE_FUNC_N("xinput path: %s", xinputName);
			if (_access_s(xinputName, 02))
			{
				if (errno == EACCES)
				{
					report_error("Can't access the xinput dll for writing");
					return false;
				}

				if (_access_s("xinput9_1_0.dll", 04) != 0) {
					char xinputError[] = "ERROR! An xinput9_1_0.dll does not exist in the local game directory or is inaccessible!\nFor \'Split-screen\' play, any supported .dll is required.";
					addDebugText(xinputError);
					MessageBoxA(NULL, xinputError, "DLL Missing Error", MB_OK);
					exit(EXIT_FAILURE);
				}
				int xinput_index = -1;
				constexpr char *xinput_md5_durazno_0_6_0_0 = "6140ae76b26a633ce2255f6fc88fbe34";
				constexpr long xinput_offset_durazno_0_6_0_0 = 0x196de;
				constexpr bool xinput_unicode_durazno_0_6_0_0 = true;
				constexpr char *xinput_md5_x360ce_3_3_1_444 = "8f24e36d5f0a71c8a412cec6323cd781";
				constexpr long xinput_offset_x360ce_3_3_1_444 = 0x1ea54;
				constexpr bool xinput_unicode_x360ce_3_3_1_444 = true;
				constexpr char *xinput_md5_x360ce_3_4_1_1357 = "5236623449893c0e1e98fc95f067fcff";
				constexpr long xinput_offset_x360ce_3_4_1_1357 = 0x16110;
				constexpr bool xinput_unicode_x360ce_3_4_1_1357 = false;
				constexpr int xinput_array_length = 3;
				static constexpr char* xinput_md5[xinput_array_length] = { xinput_md5_durazno_0_6_0_0, xinput_md5_x360ce_3_3_1_444, xinput_md5_x360ce_3_4_1_1357 };
				static constexpr long xinput_offset[xinput_array_length] = { xinput_offset_durazno_0_6_0_0, xinput_offset_x360ce_3_3_1_444, xinput_offset_x360ce_3_4_1_1357 };
				static constexpr bool xinput_unicode[xinput_array_length] = { xinput_unicode_durazno_0_6_0_0, xinput_unicode_x360ce_3_3_1_444, xinput_unicode_x360ce_3_4_1_1357 };
				std::string available_xinput_md5;
				if (!hashes::calc_file_md5("xinput9_1_0.dll", available_xinput_md5))
				{
					report_error("Failed to hash original xinput9_1_0.dll, file might be missing?");
					return false;
				}
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

				if (!LOG_CHECK(CopyFileA("xinput9_1_0.dll", xinputName, FALSE)))
				{
					report_error("Failed to copy Xinput DLL for patching!");
					return false;
				}

				FILE* xinput_patched = fopen(xinputName, "r+b");
				if (!xinput_patched) {
					fileFail(xinput_patched);
					report_error("Can't open xinput file for patching.");
					exit(EXIT_FAILURE);
				}

				int len_to_write = 2;
				BYTE assmXinputDuraznoNameEdit[] = { 0x30 + (H2GetInstanceId() / 10), 0x30 + (H2GetInstanceId() % 10), 0x30 + (H2GetInstanceId() % 10) };
				if (xinput_unicode[xinput_index]) {
					assmXinputDuraznoNameEdit[1] = 0x00;
					len_to_write = 3;
				}
				if (fseek(xinput_patched, xinput_offset[xinput_index], SEEK_SET) == 0 && fwrite(assmXinputDuraznoNameEdit, sizeof(BYTE), len_to_write, xinput_patched) == len_to_write) {
					addDebugText("Successfully copied and patched original xinput9_1_0.dll");
				}
				else {
					fclose(xinput_patched);
					remove(xinputName);
					char xinputError[255];
					sprintf(xinputError, "ERROR! Failed to write hex edit to file: %s", xinputName);
					addDebugText(xinputError);
					MessageBoxA(NULL, xinputError, "DLL Edit Error", MB_OK);
					exit(EXIT_FAILURE);
				}
				fclose(xinput_patched);
			}
		}
	}
	addDebugText("Finished Processing Instance Number.");
	return true;
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
	if (result <= _MAX_PATH && GetLastError() != ERROR_INSUFFICIENT_BUFFER) {
		DWORD version_info_size = GetFileVersionInfoSizeW(exe_file_path, NULL);
		if (version_info_size != 0) {

			BYTE *version_info = new BYTE[version_info_size];
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
	h2mod->SetBase(H2BaseAddr);
	if (game_info.process_type == H2Types::H2Server)
	{
		h2mod->Server = TRUE;
	}
	else if (game_info.process_type == H2Types::H2Game)
	{
		h2mod->Server = FALSE;
	}

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
	//checksum_log = logger::create(prepareLogFileName("checksum"), true);
	LeaveCriticalSection(&log_section);
#endif
	InitH2Accounts();

	if (!configureXinput())
		exit(EXIT_FAILURE);

	//apply any network hooks
	network->applyNetworkHooks();
	InitH2Tweaks();
	extern void initGSCustomLanguage();
	initGSCustomLanguage();
	extern void initGSCustomMenu();
	initGSCustomMenu();
	extern void initGSRunLoop();
	initGSRunLoop();
	addDebugText("ProcessStartup finished.");
}

#include "H2MOD\Modules\Updater\Updater.h"

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
