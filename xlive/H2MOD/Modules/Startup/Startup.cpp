#include "stdafx.h"

#include "H2MOD\Modules\Startup\Startup.h"
#include "H2MOD\Modules\Networking\Networking.h"
#include "H2MOD\Modules\OnScreenDebug\OnScreenDebug.h"
#include "H2MOD\Modules\Utils\Utils.h"

#include "Util\Hooks\Hook.h"
#include "Util\Debug\Debug.h"
#include "Util\hash.h"
#include "Util\filesys.h"

#include "H2MOD\Modules\Config\Config.h"
#include "H2MOD\Modules\Tweaks\Tweaks.h"
#include "H2MOD\Modules\Accounts\AccountLogin.h"
#include "H2MOD\Modules\Accounts\Accounts.h"

namespace filesystem = std::filesystem;

// xLiveLess specific logger
h2log *xlive_log = nullptr;

// Mod specific logger
h2log *h2mod_log = nullptr;

// General network logger
h2log *network_log = nullptr;

// Map loading logger
h2log *checksum_log = nullptr;

// OnScreenDebug logger
h2log *onscreendebug_log = nullptr;

// Console logger, receives output from all loggers
h2log *console_log = nullptr;

// Voice chat logger
h2log* voice_log = nullptr;

std::random_device rd;

ProcessInfo game_info;

bool H2IsDediServer = false;
DWORD H2BaseAddr = NULL;
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
		swprintf(mutexName, ARRAYSIZE(mutexName), (H2IsDediServer ? L"Halo2Server%d" : L"Halo2Player%d"), instanceNumber);
		mutex = CreateMutexW(0, TRUE, mutexName);
		lastErr = GetLastError();
		if (lastErr == ERROR_ALREADY_EXISTS) {
			CloseHandle(mutex);
		}
	} while (lastErr == ERROR_ALREADY_EXISTS);
	addDebugText("You are Instance #%d.", instanceNumber);
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
	addDebugText("Base port: %d.", H2Config_base_port);
}

wchar_t xinput_path[_MAX_PATH];

bool configureXinput() {
	auto report_error = [](const std::string &message) {
		MessageBoxA(NULL, message.c_str(), "Xinput config error!", MB_OK);
	};

	if (!H2IsDediServer) {
		if (H2GetInstanceId() > 1) {
			swprintf(xinput_path, ARRAYSIZE(xinput_path), L"xinput/p%02d/xinput9_1_0.dll", H2GetInstanceId());
			LOG_TRACE_FUNCW(L"Changing xinput path to '{0}' : '{1}'", xinput_path, xinput_path);

			WritePointer(H2BaseAddr + 0x8AD28, xinput_path);

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

			LOG_TRACE_FUNC("xinput path: {}", xinputName);
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

	swprintf(local2, ARRAYSIZE(local2), L"%ws\\AppData\\Local\\", userprofile);
	struct _stat64i32 sb;
	if (_wstat(local2, &sb) == 0 && sb.st_mode & S_IFDIR) {
		swprintf(local2, ARRAYSIZE(local2), L"%ws\\AppData\\Local\\Microsoft\\", userprofile);
		CreateDirectoryW(local2, NULL);
		int fperrno1 = GetLastError();
		if (fperrno1 == ERROR_ALREADY_EXISTS || fperrno1 == ERROR_SUCCESS) {
#if USE_DEV_PREVIEW_CONFIG_FILE_PATHS
			swprintf(local2, ARRAYSIZE(local2), L"%ws\\AppData\\Local\\Microsoft\\Halo 2\\DevPreview\\", userprofile);
#else
			swprintf(local2, ARRAYSIZE(local2), L"%ws\\AppData\\Local\\Microsoft\\Halo 2\\", userprofile);
#endif
			CreateDirectoryW(local2, NULL);
			int fperrno1 = GetLastError();
			if (fperrno1 == ERROR_ALREADY_EXISTS || fperrno1 == ERROR_SUCCESS) {
				int appdatabuflen = wcslen(local2) + 1;
				H2AppDataLocal = (wchar_t*)calloc(1, sizeof(wchar_t) * appdatabuflen);
				wcscpy_s(H2AppDataLocal, appdatabuflen, local2);
			}
		}
	}
	else if (swprintf(local2, ARRAYSIZE(local2), L"%ws\\Local Settings\\Application Data\\", userprofile), _wstat(local2, &sb) == 0 && sb.st_mode & S_IFDIR)
	{
		swprintf(local2, ARRAYSIZE(local2), L"%ws\\Local Settings\\Application Data\\Microsoft\\", userprofile);
		CreateDirectoryW(local2, NULL);
		int fperrno1 = GetLastError();
		if (fperrno1 == ERROR_ALREADY_EXISTS || fperrno1 == ERROR_SUCCESS) {
#if USE_DEV_PREVIEW_CONFIG_FILE_PATHS
			swprintf(local2, ARRAYSIZE(local2), L"%ws\\Local Settings\\Application Data\\Microsoft\\Halo 2\\DevPreview\\", userprofile);
#else
			swprintf(local2, ARRAYSIZE(local2), L"%ws\\Local Settings\\Application Data\\Microsoft\\Halo 2\\", userprofile);
#endif
			CreateDirectoryW(local2, NULL);
			int fperrno1 = GetLastError();
			if (fperrno1 == ERROR_ALREADY_EXISTS || fperrno1 == ERROR_SUCCESS) {
				int appdatabuflen = wcslen(local2) + 1;
				H2AppDataLocal = (wchar_t*)calloc(1, sizeof(wchar_t) * appdatabuflen);
				wcscpy_s(H2AppDataLocal, appdatabuflen, local2);
			}
		}
	}

	if (H2AppDataLocal == nullptr) {
		int appdatabuflen = wcslen(H2ProcessFilePath) + 1;
		H2AppDataLocal = (wchar_t*)calloc(1, sizeof(wchar_t) * appdatabuflen);
		swprintf(H2AppDataLocal, appdatabuflen, L"%ws", H2ProcessFilePath);
		addDebugText("ERROR: Could not find AppData Local. Using Process File Path:");
	}
	else {
		addDebugText("Found AppData Local: %s", H2AppDataLocal);
	}
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

// use only after initLocalAppData has been called
// by default useAppDataLocalPath is set to true, if not specified
std::wstring prepareLogFileName(std::wstring logFileName, bool useAppDataLocalPath) {
	std::wstring filename = (useAppDataLocalPath ? H2AppDataLocal : L"");
	std::wstring processName(H2IsDediServer ? L"H2Server" : L"Halo2Client");
	std::wstring folders(L"logs\\" + processName + L"\\instance" + std::to_wstring(H2GetInstanceId()));
	filename += folders;
	// try making logs directory
	if (!filesystem::create_directories(filename) && !filesystem::is_directory(filesystem::status(filename)))
	{
		// try locally if we didn't already
		if (useAppDataLocalPath
			&& filesystem::create_directories(folders) || filesystem::is_directory(filesystem::status(folders)))
			filename = folders;
		else
			filename = L""; // fine then
	}
	filename += L"\\" + logFileName + L".log";
	return filename;
}

void ServerStartupFixes()
{
	
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

	if (game_info.process_type == H2Types::H2Server)
	{
		H2IsDediServer = true;
	}
	else if (game_info.process_type == H2Types::H2Game)
	{
		H2IsDediServer = false;
	}
	Memory::setBaseAddress(H2BaseAddr, H2IsDediServer);

	initInstanceNumber();

	int ArgCnt;
	LPWSTR* ArgList = CommandLineToArgvW(GetCommandLineW(), &ArgCnt);
	H2ProcessFilePath = (wchar_t*)calloc(1, wcslen(ArgList[0]) * sizeof(wchar_t));
	int rtncodepath = GetWidePathFromFullWideFilename(ArgList[0], H2ProcessFilePath);
	if (rtncodepath == -1) {
		std::wstring path = GetExeDirectoryWide();
		path.append(L"\\");
		H2ProcessFilePath = (wchar_t*)calloc(1, path.length() * sizeof(wchar_t));
		swprintf(H2ProcessFilePath, path.c_str());
	}
	// fix the game not finding the files it needs if the current directory is not the install directory
	SetCurrentDirectoryW(GetExeDirectoryWide().c_str());
	//If H2ProcessFilePath is empty (Server Console Mode?) set to working directory
	
	initLocalAppData();

	// initialize curl
	curl_global_init(CURL_GLOBAL_ALL);

	// after localAppData filepath initialized, we can initialize OnScreenDebugLog
	initOnScreenDebugText();

	if (H2IsDediServer) {
		addDebugText("Process is Dedi-Server");
	}
	else {
		addDebugText("Process is Client");
	}

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
	postConfig();
	EnterCriticalSection(&log_section);

	// prepare default log files if enabled, after we read the H2Config
	bool should_enable_console_log = H2Config_debug_log && H2Config_debug_log_console;
	console_log = h2log::create_console("CONSOLE MAIN", should_enable_console_log, H2Config_debug_log_level);

	xlive_log = h2log::create("XLive", prepareLogFileName(L"h2xlive"), H2Config_debug_log, H2Config_debug_log_level);
	LOG_DEBUG_XLIVE(DLL_VERSION_STR "\n");
	h2mod_log = h2log::create("H2MOD", prepareLogFileName(L"h2mod"), H2Config_debug_log, H2Config_debug_log_level);
	LOG_DEBUG_GAME(DLL_VERSION_STR "\n");
	network_log = h2log::create("Network", prepareLogFileName(L"h2network"), H2Config_debug_log, H2Config_debug_log_level);
	LOG_DEBUG_NETWORK(DLL_VERSION_STR "\n");
#if COMPILE_WITH_VOICE
	voice_log = h2log::create("Voice", prepareLogFileName(L"voicechat"), H2Config_debug_log, H2Config_debug_log_level);
	LOG_DEBUG(voice_log, DLL_VERSION_STR "\n");
#endif

	//checksum_log = h2log::create("Checksum", prepareLogFileName(L"checksum"), true, 0);
	LeaveCriticalSection(&log_section);
	InitH2Accounts();

	if (!configureXinput())
		exit(EXIT_FAILURE);

	//apply any network hooks
	CustomNetwork::applyNetworkHooks();
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

	// initialize default data to run under LAN
	// if the server runs in LIVE mode, check XLiveSignIn/XLiveSignOut in AccountLogin.cpp
	if (H2IsDediServer)
	{
		addDebugText("Signing in dedicated server locally.");

		AccountEdit_remember = false;

		BYTE abEnet[6];
		BYTE abOnline[20];
		XNetRandom(abEnet, sizeof(abEnet));
		XNetRandom(abOnline, sizeof(abOnline));
		ConfigureUserDetails("[Username]", "12345678901234567890123456789012", rand(), 0, H2Config_ip_lan, ByteToHexStr(abEnet, sizeof(abEnet)).c_str(), ByteToHexStr(abOnline, sizeof(abOnline)).c_str(), false);
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
	curl_global_cleanup();
}
