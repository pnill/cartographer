#include "stdafx.h"
#include "Startup.h"

#include "../Config.h"
#include "../H2MODShell.h"

#include "cseries/cseries_windows_debug_pc.h"
#include "sapien/patches_initialize.h"
#include "shell/shell.h"
#include "shell/shell_windows.h"
#include "tool/tool_patches_initialize.h"

#include "H2MOD.h"
#include "H2MOD/Modules/Accounts/AccountLogin.h"
#include "H2MOD/Modules/OnScreenDebug/OnscreenDebug.h"
#include "H2MOD/Utils/Utils.h"

/* constants */

static const wchar_t k_client_process_name[] = L"Halo2Client";
static const wchar_t k_server_process_name[] = L"H2Server";
static const wchar_t k_microsoft_folder[] = L"\\Microsoft";
static const wchar_t k_appdata_default_path[] = L"\\Halo 2\\";
static const wchar_t k_appdata_dev_preview_path[] = L"DevPreview\\";

wchar_t g_h2_process_file_path[MAX_PATH];
wchar_t g_h2_appdata_local_path[MAX_PATH];

static void startup_force_working_directory_to_process_directory(void);

static void startup_init_h2_game(void);

static void startup_init_h2_tools(e_h2_type type);

void PostH2Config() {

	wchar_t mutexName2[256];
	swprintf(mutexName2, ARRAYSIZE(mutexName2), L"Halo2BasePort#%d", H2Config_base_port);
	/*HANDLE mutex2 =*/ CreateMutex(0, TRUE, mutexName2);
	DWORD lastErr2 = GetLastError();
	if (lastErr2 == ERROR_ALREADY_EXISTS)
	{
		addDebugText("Base port %d is already bound to!\nExpect MP to not work!", H2Config_base_port);
		_Shell::OpenMessageBox(NULL, MB_ICONWARNING, "BASE PORT BIND WARNING!", "Base port %d is already bound to!\nExpect MP to not work!", H2Config_base_port);
	}
	addDebugText("Base port: %d.", H2Config_base_port);
}

void InitLocalAppData()
{
	addDebugText("Find AppData Local.");

	wchar_t appdata_path[MAX_PATH];
	ugetenv(appdata_path, NUMBEROF(appdata_path), L"localappdata");

	ustrncat(appdata_path, k_microsoft_folder, NUMBEROF(appdata_path));
	ustrncat(appdata_path, k_appdata_default_path, NUMBEROF(appdata_path));

	// Make sure main folder exists and create if not
	const errno_t error = _waccess_s(appdata_path, 6);
	if (error != ERROR_SUCCESS)
	{
		CreateDirectoryW(appdata_path, NULL);
	}
	
	// Run folder checks if we're using dev preview paths 
	if (USE_DEV_PREVIEW_CONFIG_FILE_PATHS)
	{
		ustrncat(appdata_path, k_appdata_dev_preview_path, NUMBEROF(appdata_path));
		
		// Make sure dev preview folder exists and create if not
		const errno_t dev_path_error = _waccess_s(appdata_path, 6);
		if (dev_path_error != ERROR_SUCCESS)
		{
			CreateDirectoryW(appdata_path, NULL);
		}
	}

	ustrncpy(g_h2_appdata_local_path, appdata_path, MAX_PATH);
	return;
}

///Before the game window appears
void InitH2Startup()
{
	DETOUR_BEGIN();
	cseries_windows_debug_initialize();
	
	const e_h2_type type = Memory::Initialize();

	if (type == _h2_type_game ||
		type == _h2_type_server)
	{
		startup_init_h2_game();
	}
	else if (
		type == _h2_type_ek_sapien ||
		type == _h2_type_ek_tool ||
		type == _h2_type_ek_guerilla)
	{
		DETOUR_COMMIT();
		startup_init_h2_tools(type);
	}

	return;
}

///After the game window appears
void H2DedicatedServerStartup() {
	// initialize default data to run under LAN
	// if the server runs in LIVE mode, check XLiveSignIn/XLiveSignOut in AccountLogin.cpp
	if (shell_is_dedicated_server())
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

// use only after initLocalAppData has been called
// by default useAppDataLocalPath is set to true, if not specified
void log_file_name_prepare(const wchar_t* logFileName, c_static_wchar_string<MAX_PATH>* path)
{
	const bool is_dedi = shell_is_dedicated_server();
	const wchar_t* process_name = is_dedi ? k_server_process_name : k_client_process_name;

	// We set the instance string based on whether or not 
	const wchar_t* instance_string = NULL;
	wchar_t instance_number_string[4];

	if (!config_use_instance_name(&instance_string))
	{
		usnprintf(instance_number_string, NUMBEROF(instance_number_string), L"%d", shell_get_instance_num());
		instance_string = instance_number_string;
	}

	c_static_wchar_string<MAX_PATH> folders;
	folders.append(L"logs\\");
	folders.append(process_name);
	folders.append(L"\\instance");
	folders.append(instance_string);

	// Place logs in server folder when portable or dedi
	if (g_h2_portable || is_dedi)
	{
		path->set(g_h2_process_file_path);
		path->append(L"\\");
	}
	else
	{
		path->set(g_h2_appdata_local_path);
	}
	path->append(folders.get_string());

	// try making logs directory
	SHCreateDirectoryEx(NULL, path->get_string(), NULL);

	path->append(L"\\");
	path->append(logFileName);
	path->append(L".log");
	return;
}

static void startup_force_working_directory_to_process_directory(void)
{
	int ArgCnt;
	LPWSTR* ArgList = CommandLineToArgvW(GetCommandLineW(), &ArgCnt);

	c_static_wchar_string<MAX_PATH> path(ArgList[0]);
	
	// Remove the exe name by terminating the string at the last backslash character
	const int32 backslash_index = path.last_index_of(L"\\");
	path.get_buffer()[backslash_index + 1] = L'\0';

	ustrncpy(g_h2_process_file_path, path.get_string(), MAX_PATH);

	// Force the current working directory to the one where halo2.exe is located
	SetCurrentDirectoryW(g_h2_process_file_path);
	return;
}

static void startup_init_h2_game(void)
{
	shell_windows_initialize();
	shell_apply_patches();
	shell_windows_apply_patches();

	startup_force_working_directory_to_process_directory();

	DETOUR_COMMIT();

	InitLocalAppData();

	// initialize curl
	curl_global_init(CURL_GLOBAL_ALL);
	atexit([] { curl_global_cleanup(); });

	addDebugText(shell_is_dedicated_server() ? "Process is Dedi-Server" : "Process is Client");

	H2MOD::Initialize();

	addDebugText("ProcessStartup finished.");
	return;
}

static void startup_init_h2_tools(e_h2_type type)
{
	switch (type)
	{
	case _h2_type_ek_sapien:
		sapien_apply_patches();
		break;
	case _h2_type_ek_tool:
		tool_apply_patches();
		break;
	case _h2_type_ek_guerilla:
		break;
	default:
		unreachable();
	}
	return;
}
