
#include "H2MOD\Modules\Startup\Startup.h"
#include "H2MOD\Modules\OnScreenDebug\OnScreenDebug.h"
#include "H2MOD\Modules\Utils\Utils.h"
#include "H2MOD\Modules\RunLoop\RunLoop.h"

#include <Shellapi.h>
#include <sys/stat.h>
#include <string>
#include <sstream>
#include "Util\Hooks\Hook.h"
#include "Util\Debug\Debug.h"
#include "Util\hash.h"
#include "Util\filesys.h"
#include "Util\Hooks\Hook.h"

#include "H2MOD\Modules\Updater\Updater.h"
#include "H2MOD\Modules\Config\Config.h"
#include "H2MOD\Modules\Tweaks\Tweaks.h"
#include "H2MOD\Modules\Accounts\AccountLogin.h"
#include "H2MOD\Modules\Accounts\Accounts.h"
#include "H2MOD\Modules\Networking\Networking.h"


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
	if (0 < (result <= _MAX_PATH)) {
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

#pragma region initialization hook
template <typename T = void>
static inline T *GetAddress(DWORD client, DWORD server = 0)
{
	return reinterpret_cast<T*>(H2BaseAddr + (H2IsDediServer ? server : client));
}

#pragma region func_wrappers
void real_math_initialize()
{
	typedef int (real_math_initialize)();
	auto real_math_initialize_impl = GetAddress<real_math_initialize>(0x000340d7);
	real_math_initialize_impl();
}

void async_initialize()
{
	typedef int (async_initialize)();
	auto async_initialize_impl = GetAddress<async_initialize>(0x00032ce5);
	async_initialize_impl();
}

bool init_gfwl_gamestore()
{
	typedef char (init_gfwl_gamestore)();
	auto init_gfwl_gamestore_impl = GetAddress<init_gfwl_gamestore>(0x00202f3e);
	return init_gfwl_gamestore_impl();
}
// not sure if this is all it does
HANDLE init_data_checksum_info()
{
	typedef HANDLE init_data_checksum_info();
	auto init_data_checksum_info_impl = GetAddress<init_data_checksum_info>(0x000388d3);
	return init_data_checksum_info_impl();
}

// returns memory
void *runtime_state_init()
{
	typedef void *runtime_state_init();
	auto runtime_state_init_impl = GetAddress<runtime_state_init>(0x00037ed5);
	return runtime_state_init_impl();
}

void global_preferences_initialize()
{
	typedef void global_preferences_initialize();
	auto global_preferences_initialize_impl = GetAddress<global_preferences_initialize>(0x325FD);
	global_preferences_initialize_impl();
}

void font_initialize()
{
	typedef void __cdecl font_initialize();
	auto font_initialize_impl = GetAddress<font_initialize>(0x00031dff);
	font_initialize_impl();
}

bool tag_files_open()
{
	typedef bool tag_files_open();
	auto tag_files_open_impl = GetAddress<tag_files_open>(0x30D58);
	return tag_files_open_impl();
}

void init_timing(int a1)
{
	typedef DWORD(__cdecl init_timing)(int a1);
	auto init_timing_impl = GetAddress<init_timing>(0x37E39);
	init_timing_impl(a1);
}

void game_state_initialize(void *data)
{
	typedef void __fastcall game_state_initialize(void *data);
	auto game_state_initialize_impl = GetAddress<game_state_initialize>(0x00030aa6);
	game_state_initialize_impl(data);
}

bool rasterizer_initialize()
{
	typedef char rasterizer_initialize();
	auto rasterizer_initialize_impl = GetAddress<rasterizer_initialize>(0x00263359);
	return rasterizer_initialize_impl();
}

bool input_initialize()
{
	typedef char input_initialize();
	auto input_initialize_impl = GetAddress<input_initialize>(0x2FD23);
	return input_initialize_impl();
}

void sound_initialize()
{
	typedef void sound_initialize();
	auto sound_initialize_impl = GetAddress<sound_initialize>(0x2979E);
	return sound_initialize_impl();
}

#pragma endregion

int flag_log_count[flags::count];
BOOL __cdecl is_init_flag_set(flags id)
{
	if (flag_log_count[id] < 10)
	{
		TRACE_GAME("is_init_flag_set() : flag %i", id);
		flag_log_count[id]++;
		if (flag_log_count[id] == 10)
			TRACE_GAME("is_init_flag_set() : flag %i logged to many times ignoring", id);
	}
	DWORD* init_flags_array = reinterpret_cast<DWORD*>(H2BaseAddr + 0x0046d820);
	return init_flags_array[id] != 0;
}

const static int max_mointor_count = 9;
bool engine_basic_init()
{
	DWORD* flags_array = reinterpret_cast<DWORD*>(H2BaseAddr + 0x0046d820);
	memset(flags_array, 0x00, flags::count); // should be zero initalized anyways but the game does it

	flags_array[flags::nointro] = H2Config_skip_intro;

	HANDLE(*fn_c000285fd)() = (HANDLE(*)())(GetAddress(0x000285fd));

	init_gfwl_gamestore();
	init_data_checksum_info();
	runtime_state_init();

	int arg_count;
	wchar_t **cmd_line_args = LOG_CHECK(CommandLineToArgvW(GetCommandLineW(), &arg_count));
	if (cmd_line_args && arg_count > 1) {
		for (int i = 1; i < arg_count; i++) {
			wchar_t* cmd_line_arg = cmd_line_args[i];

			if (_wcsicmp(cmd_line_arg, L"-windowed") == 0) {
				flags_array[flags::windowed] = 1;
			}
			else if (_wcsicmp(cmd_line_arg, L"-nosound") == 0) {
				flags_array[flags::nosound] = 1;
				WriteValue(H2BaseAddr + 0x479EDC, 1);
			}
			else if (_wcsicmp(cmd_line_arg, L"-novsync") == 0) {
				flags_array[flags::novsync] = 1;
			}
			else if (_wcsicmp(cmd_line_arg, L"-nointro") == 0) {
				flags_array[flags::nointro] = 1;
			}
			else if (_wcsnicmp(cmd_line_arg, L"-monitor:", 9) == 0) {
				int monitor_id = _wtol(&cmd_line_arg[9]);
				flags_array[flags::monitor_count] = min(max(0, monitor_id), max_mointor_count);
			}
			else if (_wcsicmp(cmd_line_arg, L"-highquality") == 0) {
				flags_array[flags::high_quality] = 1;
			}
			else if (_wcsicmp(cmd_line_arg, L"-depthbiasfix") == 0)
			{
				// Fixes issue #118
				/* g_depth_bias always NULL rather than taking any value from
				shader tag before calling g_D3DDevice->SetRenderStatus(D3DRS_DEPTHBIAS, g_depth_bias); */
				NopFill<8>(reinterpret_cast<DWORD>(GetAddress(0x269FD5)));
			}
#ifdef _DEBUG
			else if (_wcsnicmp(cmd_line_arg, L"-dev_flag:", 10) == 0) {
				int flag_id = _wtol(&cmd_line_arg[10]);
				flags_array[min(max(0, flag_id), flags::count - 1)] = 1;
			}
#endif
		}
	}
	LocalFree(cmd_line_args);

	if (flags_array[flags::unk22])
		init_timing(1000 * flags_array[flags::unk22]);
	real_math_initialize();
	async_initialize();
	global_preferences_initialize();
	font_initialize();

	if (!LOG_CHECK(tag_files_open()))
		return false;
	void *var_c004ae8e0 = GetAddress(0x004ae8e0);
	game_state_initialize(var_c004ae8e0);

	// modifies esi need to check what the caller sets that too
	//char(*fn_c001a9de6)() = (char(*)())(GetAddress(0x001a9de6));
	//char result_c001a9de6 = fn_c001a9de6();
	if (!LOG_CHECK(rasterizer_initialize()))
		return false;

	input_initialize();
	// not needed but bungie did it so there might be some reason
	sound_initialize();

	struct FakePBuffer {
		HANDLE id;
		DWORD dwSize;
		DWORD magic;
		LPBYTE pbData;
	};
	//extern LONG WINAPI XLivePBufferAllocate(DWORD size, FakePBuffer **pBuffer);
	//extern DWORD WINAPI XLivePBufferSetByte(FakePBuffer * pBuffer, DWORD offset, BYTE value);
	LONG(__stdcall* XLivePBufferAllocate)(DWORD size, FakePBuffer **pBuffer) = (LONG(__stdcall*)(DWORD, FakePBuffer**))(GetAddress(0x0000e886));
	DWORD(__stdcall* XLivePBufferSetByte)(FakePBuffer * pBuffer, DWORD offset, BYTE value) = (DWORD(__stdcall*)(FakePBuffer*, DWORD, BYTE))(GetAddress(0x0000e880));
	DWORD* var_c00479e78 = GetAddress<DWORD>(0x00479e78);
	XLivePBufferAllocate(2, (FakePBuffer**)&var_c00479e78);
	XLivePBufferSetByte((FakePBuffer*)var_c00479e78, 0, 0);
	XLivePBufferSetByte((FakePBuffer*)var_c00479e78, 1, 0);

	//SLDLInitialize

	// does some weird stuff with sound, might setup volume
	HANDLE result_c000285fd = fn_c000285fd();

	//SLDLOpen
	//SLDLConsumeRight
	//SLDLClose
	// This call would disable the multiplayer buttons in the mainmenu. Likely setup this way from SLDL.
	//XLivePBufferSetByte((FakePBuffer*)var_c00479e78, 0, 1);

	return true;
}

#pragma region func_wrappers
bool InitPCCInfo()
{
	typedef bool __cdecl InitPCCInfo();
	auto InitPCCInfoImpl = GetAddress<InitPCCInfo>(0x260DDD);
	return InitPCCInfoImpl();
}

void run_main_loop()
{
	typedef int __cdecl run_main_loop();
	auto run_main_loop_impl = GetAddress<run_main_loop>(0x39E2C);
	run_main_loop_impl();
}

void main_engine_dispose()
{
	typedef int main_engine_dispose();
	auto main_engine_dispose_impl = GetAddress<main_engine_dispose>(0x48A9);
	main_engine_dispose_impl();
}

void show_error_message_by_id(int id)
{
	typedef void __cdecl show_error_message_by_id(int id);
	auto show_error_message_by_id_impl = GetAddress<show_error_message_by_id>(0x4A2E);
	show_error_message_by_id_impl(id);
}
#pragma endregion

void show_fatal_error(int error_id)
{
	TRACE_FUNC("error_id : %d", error_id);

	auto destory_window = [](HWND handle) {
		if (handle)
			DestroyWindow(handle);
	};

	HWND hWnd = *GetAddress<HWND>(0x46D9C4);
	HWND d3d_window = *GetAddress<HWND>(0x46D9C8); // not sure what this window is actual for, used in IDirect3DDevice9::Present
	destory_window(hWnd);
	destory_window(d3d_window);
	show_error_message_by_id(error_id);
}

char is_remote_desktop()
{
	TRACE_FUNC("check disabled");
	return 0;
}


int __stdcall WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd)
{
	// set args
	WriteValue(H2BaseAddr + 0x46D9BC, lpCmdLine); // command_line_args
	WriteValue(H2BaseAddr + 0x46D9C0, hInstance); // g_instance
	WriteValue(H2BaseAddr + 0x46D9CC, nShowCmd); // g_CmdShow

	// window setup
	wcscpy_s(reinterpret_cast<wchar_t*>(H2BaseAddr + 0x46D9D4), 0x40, L"halo"); // ClassName
	wcscpy_s(reinterpret_cast<wchar_t*>(H2BaseAddr + 0x46DA54), 0x40, L"Halo 2 - Project Cartographer"); // WindowName
	WNDPROC g_WndProc = reinterpret_cast<WNDPROC>(H2BaseAddr + 0x790E);
	WriteValue(H2BaseAddr + 0x46D9D0, g_WndProc); // g_WndProc_ptr
	if (!LOG_CHECK(InitPCCInfo()))
	{
		TRACE_FUNC("Failed to get PCC info / insufficient system resources");
		run_async(
			MessageBoxA(NULL, "Failed to get compatibility info.", "PCC Error", S_OK);
		)
			show_error_message_by_id(108);
		// todo: load some default values here?
	}
	// mouse cursor setup
	HCURSOR cursor = LOG_CHECK(LoadCursor(NULL, MAKEINTRESOURCE(0x7F00)));
	WriteValue(H2BaseAddr + 0x46D9B8, cursor); // g_hCursor

	// mess around with xlive (not calling XLiveInitialize etc)
	WriteValue<BYTE>(H2BaseAddr + 0x4FAD98, 1);

	// intialize some basic game subsystems
	if (LOG_CHECK(engine_basic_init()))
	{
		run_main_loop(); // actually run game
		main_engine_dispose(); // cleanup
	}
	else
	{
		TRACE_FUNC("Engine startup failed!");
		show_fatal_error(108);
		return 1;
	}

	int g_fatal_error_id = *reinterpret_cast<int*>(H2BaseAddr + 0x46DAD4);
	if (g_fatal_error_id) // check if the game exited cleanly
	{
		show_fatal_error(g_fatal_error_id);
		return 1;
	}
	else
	{
		return 0;
	}
}

void applyGameInitializationHooks()
{
	if (H2IsDediServer == false) 
	{
		WriteJmpTo(GetAddress(0x7E43), WinMain);
		WriteJmpTo(GetAddress(0x39EA2), is_remote_desktop);
		WriteJmpTo(GetAddress(0x4544), is_init_flag_set);
	}
}
#pragma endregion

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

	applyGameInitializationHooks();
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
