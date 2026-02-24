#include "stdafx.h"
#include "AccountLogin.h"

#include "cartographer/config/endpoints.h"
#include "shell/shell.h"

#include "H2MOD/Modules/Shell/Config.h"
#include "H2MOD/Modules/OnScreenDebug/OnscreenDebug.h"
#include "H2MOD/Utils/Utils.h"
#include "H2MOD/Modules/Accounts/Accounts.h"

#include "XLive/xnet/upnp.h"

enum
{
	MASTER_STATE_STR_SIZE = 256
};

static const char k_login_code_secondary_string[] = "login_code_secondary=";
static const char k_login_external_addr_string[] = "login_external_addr=";
static const char k_login_token_string[] = "login_token=";
static const char k_login_username_string[] = "login_username=";
static const char k_login_ab_online_string[] = "login_abOnline=";

static const char k_cartographer_login_url[] = k_cartographer_url_https"/login2";

static int masterState = -1;
static char masterStateStr[MASTER_STATE_STR_SIZE] = "Status: Offline";

bool AccountEdit_remember = true;

// TODO (Carefully) Cleanup and move

#pragma region Halo 2 Master Login

typedef NTSTATUS(WINAPI* RtlGetVersionPtr)(PRTL_OSVERSIONINFOW);

int GetMasterState()
{
	return masterState;
}

const char* GetMasterStateStr()
{
	return masterStateStr;
}

void UpdateMasterStatus(int new_state, const char* state_str, ...)
{
	va_list v;
	va_start(v, state_str);
	masterState = new_state;
	if (!shell_is_dedicated_server())
	{
		vsnprintf(masterStateStr, MASTER_STATE_STR_SIZE, state_str, v);
	}
	va_end(v);
}

void UpdateMasterLoginStatus(bool developer) {
	
	if (XUserSignedOnline(0))
	{
		const char* statusStr = "Status: Online";
		if (developer)
		{
			statusStr = "Status: Developer";
		}
		UpdateMasterStatus(10, statusStr);
	}
	else if (XUserSignedInLocally(0))
	{
		UpdateMasterStatus(2, "Status: Locally signed in");
	}
	else
	{
		UpdateMasterStatus(2, "Status: Offline");
	}
}

int ConfigureUserDetails(const char* username, const char* login_token, unsigned long long xuid, unsigned long xnaddr, unsigned long lanaddr, const char* machineUID, const char* abOnline, bool online_signin, bool developer) {

	bool xuidValid = xuid != 0;
	size_t usernameLen = strnlen(username, XUSER_NAME_SIZE);
	bool usernameValid = (usernameLen > 0 && usernameLen <= XUSER_MAX_NAME_LENGTH);
	bool machineUIdValid = strnlen(machineUID, 12 + 1) == 12;
	bool onlineIdValid = strnlen(abOnline, 40 + 1) == 40;

	// validation
	if (!xuidValid
		|| !usernameValid
		|| !machineUIdValid
		|| !onlineIdValid
		)
	{
		addDebugText("Login data invalid!");
		return 0;
	}

	int result = strlen(login_token) == 32 ? 1 : 2;

	XUserSetup(0, xuid, username, xnaddr, lanaddr, H2Config_base_port, machineUID, abOnline, online_signin);
	TEST_N_DEF(PC4);
	UpdateMasterLoginStatus(developer);

	if (online_signin)
	{
		if (!shell_is_dedicated_server())
		{
			xlive_upnp_forward_ports();
		}
	}

	if (H2CurrentAccountLoginToken) {
		free(H2CurrentAccountLoginToken);
	}
	if (result == 1) {
		H2CurrentAccountLoginToken = (char*)calloc(32 + 1, sizeof(char));
		snprintf(H2CurrentAccountLoginToken, 32 + 1, login_token);
	}
	else {
		H2CurrentAccountLoginToken = NULL;
	}

	//FIXME - remove the handle again after they sign out. display an error - yes no window when it's taken when you click on it.
	/*
	if (H2CurrentAccountLoginToken) {
		addDebugText("Post Config Port & Login Mutex.");
		wchar_t mutexName[255];
		swprintf(mutexName, L"Halo2Login#%s", H2CurrentAccountLoginToken);
		HANDLE mutex = CreateMutex(0, TRUE, mutexName);
		DWORD lastErr = GetLastError();
		char token_censored[33];
		strncpy(token_censored, H2CurrentAccountLoginToken, 32);
		token_censored[32] = 0;
		memset(token_censored + 4, '*', 24);
		if (lastErr == ERROR_ALREADY_EXISTS) {
			char NotificationPlayerText[120];
			sprintf(NotificationPlayerText, "Player Login Session %s already exists!\nOld session has been invalidated!", token_censored);
			addDebugText(NotificationPlayerText);
			MessageBoxA(NULL, NotificationPlayerText, "LOGIN OVERRIDDEN WARNING!", MB_OK);
		}
		char NotificationText4[120];
		sprintf(NotificationText4, "Login Token: %s.", token_censored);
		addDebugText(NotificationText4);
	}
	*/

	return result;
}

static int InterpretMasterLogin(char* response_content, char* prev_login_token) {
	int result = 0;

	char username[XUSER_NAME_SIZE] = {};
	char login_token[32 + 1] = {};
	unsigned long xnaddr = 0;
	unsigned long long xuid = 0;
	char machineUID[12 + 1] = {};
	char abOnline[40 + 1] = {};

	char tempstr1[128 + 1] = {};
	int tempint1 = -1;
	unsigned long long templlu1 = 0;

	char* index_current = response_content;
	char* index_next = NULL;
	while ((index_next = strstr(index_current, "<br>")))
	{
		char* fileLine = (char*)malloc((index_next - index_current + 1) * sizeof(char));
		csmemcpy(fileLine, index_current, index_next - index_current);
		fileLine[index_next - index_current] = 0;

		if (sscanf_s(fileLine, "return_code=%d", &tempint1) == 1) {
			addDebugText("Return code is: %d", tempint1);
		}
		else if (sscanf_s(fileLine, "login_code=%d", &tempint1) == 1) {
			addDebugText("Login code is: %d", tempint1);
			result = tempint1;
		}
		else if (strstr(fileLine, k_login_code_secondary_string)) {
			char* tempName = fileLine + NUMBEROF(k_login_code_secondary_string) - 1;
			while (isspace(*tempName)) {
				tempName++;
			}
			snprintf(tempstr1, 128 + 1, tempName);
			for (int j = strlen(tempstr1) - 1; j > 0; j--) {
				if (isspace(tempstr1[j])) {
					tempstr1[j] = 0;
				}
				else {
					break;
				}
			}
			if (strlen(tempstr1) > 0) {
				addDebugText("Login Code Secondary is: %s", tempstr1);
			}
		}
		else if (strstr(fileLine, k_login_external_addr_string)) {
			char* tempName = fileLine + NUMBEROF(k_login_external_addr_string) - 1;
			while (isspace(*tempName)) {
				tempName++;
			}
			snprintf(tempstr1, 128 + 1, tempName);
			for (int j = strlen(tempstr1) - 1; j > 0; j--) {
				if (isspace(tempstr1[j])) {
					tempstr1[j] = 0;
				}
				else {
					break;
				}
			}
			addDebugText("Client External IP Address is: %s", tempstr1);
			unsigned long resolvedAddr;
			if ((resolvedAddr = inet_addr(tempstr1)) != INADDR_NONE) {
				xnaddr = resolvedAddr;
			}
		}
		else if (strstr(fileLine, k_login_token_string)) {
			char* tempName = fileLine + NUMBEROF(k_login_token_string) - 1;
			while (isspace(*tempName)) {
				tempName++;
			}
			snprintf(tempstr1, 32 + 1, tempName);
			for (int j = strlen(tempstr1) - 1; j > 0; j--) {
				if (isspace(tempstr1[j])) {
					tempstr1[j] = 0;
				}
				else {
					break;
				}
			}
			if (strlen(tempstr1) == 32) {
				//addDebugText("Login token is: %s", tempstr1);
				strncpy_s(login_token, tempstr1, 32 + 1);
			}
		}
		else if (strstr(fileLine, k_login_username_string)) {
			char* tempName = fileLine + NUMBEROF(k_login_username_string) - 1;
			while (isspace(*tempName)) {
				tempName++;
			}
			strncpy_s(tempstr1, XUSER_NAME_SIZE, tempName, XUSER_MAX_NAME_LENGTH);
			for (int j = strlen(tempstr1) - 1; j > 0; j--) {
				if (isspace(tempstr1[j])) {
					tempstr1[j] = 0;
				}
				else {
					break;
				}
			}
			if (strlen(tempstr1) > 0) {
				addDebugText("Login username is: %s", tempstr1);
				strncpy_s(username, tempstr1, XUSER_MAX_NAME_LENGTH);
			}
		}
		else if (sscanf_s(fileLine, "login_xuid=%llu", &templlu1) == 1) {
			if (templlu1 > 0) {
				//addDebugText("User XUID is: %llu", templlu1);
				xuid = templlu1;
			}
		}
		else if (strstr(fileLine, "login_abEnet=")) {
			char* tempName = fileLine + strlen("login_abEnet=");
			while (isspace(*tempName)) {
				tempName++;
			}
			snprintf(tempstr1, 12 + 1, tempName);
			for (int j = strlen(tempstr1) - 1; j > 0; j--) {
				if (isspace(tempstr1[j])) {
					tempstr1[j] = 0;
				}
				else {
					break;
				}
			}
			if (strlen(tempstr1) == 12) {
				//addDebugText("Login machineUID is: %s", tempstr1);
				strncpy_s(machineUID, tempstr1, 12 + 1);
			}
		}
		else if (strstr(fileLine, k_login_ab_online_string)) {
			char* tempName = fileLine + NUMBEROF(k_login_ab_online_string) - 1;
			while (isspace(*tempName)) {
				tempName++;
			}
			snprintf(tempstr1, 40 + 1, tempName);
			for (int j = strlen(tempstr1) - 1; j > 0; j--) {
				if (isspace(tempstr1[j])) {
					tempstr1[j] = 0;
				}
				else {
					break;
				}
			}
			if (strlen(tempstr1) == 40) {
				//addDebugText("Login abOnline is: %s", tempstr1);
				strncpy_s(abOnline, tempstr1, 40 + 1);
			}
		}

		free(fileLine);
		index_current = index_next + 4;
	}

	if (result <= 0) {
		return result;
	}

	int user_configure_result = ConfigureUserDetails(username, login_token, xuid, xnaddr, H2Config_ip_lan, machineUID, abOnline, true, result == 4);
	if (user_configure_result != 0) {
		//allow no login_token from backend in DB emergencies / random logins.
		if (user_configure_result == 1) {
			if (prev_login_token) {
				if (H2AccountArrayLoginToken && H2AccountCount > 0) {
					for (int i = 0; i < H2AccountCount; i++) {
						if (H2AccountArrayLoginToken[i] && strncmp(H2AccountArrayLoginToken[i], prev_login_token, 32) == 0) {
							if (H2AccountArrayUsername[i]) {
								free(H2AccountArrayUsername[i]);
							}
							H2AccountArrayUsername[i] = (char*)calloc(XUSER_NAME_SIZE, sizeof(char));
							strncpy_s(H2AccountArrayUsername[i], XUSER_NAME_SIZE, username, XUSER_MAX_NAME_LENGTH);
							snprintf(H2AccountArrayLoginToken[i], 32 + 1, login_token);
							break;
						}
					}
				}
			}
			else {
				if (AccountEdit_remember) {
					H2AccountAccountAdd(username, login_token);
				}
			}
		}

	}
	else {
		result = ERROR_CODE_ACCOUNT_DATA;
	}
	
	return result;
}

bool HandleGuiLogin(char* ltoken, char* identifier, char* password, int* out_master_login_interpret_result) {

	bool result = false;
	char* rtn_result = 0;

	char* os_string = 0;
	int os_string_buflen = 0;
	static const char* (__cdecl* pwine_get_version)(void);
	HMODULE hntdll = GetModuleHandle(L"ntdll.dll");
	if (hntdll && (pwine_get_version = (const char* (__cdecl*)(void))GetProcAddress(hntdll, "wine_get_version"))) {
		os_string_buflen = strlen(pwine_get_version()) + 10;
		os_string = (char*)malloc(sizeof(char) * os_string_buflen);
		snprintf(os_string, os_string_buflen, "Wine: %s", pwine_get_version());
	}
	else {
		OSVERSIONINFO osvi;
		SecureZeroMemory(&osvi, sizeof(OSVERSIONINFO));
		osvi.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
		if (hntdll && (RtlGetVersionPtr)::GetProcAddress(hntdll, "RtlGetVersion") != nullptr) {
			//we don't have a manifest in place, so using GetVersionEx will be capped at 6.2
			//and versions will not be reflected properly in the backend, so we need to use the 
			//api method to get the real os version
			RtlGetVersionPtr rtlGetVersionPtr = (RtlGetVersionPtr)::GetProcAddress(hntdll, "RtlGetVersion");
			rtlGetVersionPtr(&osvi);
		} else {
#pragma warning( push )
#pragma warning( disable : 4996)
			GetVersionEx(&osvi);
#pragma warning( pop ) 
		}

		os_string_buflen = 512;
		os_string = (char*)calloc(os_string_buflen, sizeof(char));
		snprintf(os_string, os_string_buflen, "Windows: %u.%u", osvi.dwMajorVersion, osvi.dwMinorVersion);
	}
	addDebugText(os_string);


	char* escaped_user_login_token = encode_rfc3986(ltoken == 0 ? "" : ltoken);
	char* escaped_user_identifier = encode_rfc3986(identifier == 0 ? "" : identifier);
	char* escaped_user_password = encode_rfc3986(password == 0 ? "" : password);
	char http_request_body[] = "request_type=%08x&request_version=%s&user_identifier=%s&user_pass=%s&user_token=%s&op_sys=%s";

#if !defined(LC3)
	int http_request_body_build_len = (89 + 8 + strlen(DLL_VERSION_STR) + strlen(escaped_user_identifier) + strlen(escaped_user_password) + strlen(escaped_user_login_token) + strlen(os_string));
	char* http_request_body_build = (char*)malloc(sizeof(char) * http_request_body_build_len + 1);

	snprintf(http_request_body_build, http_request_body_build_len, http_request_body, ltoken == 0 ? 1 : 2, DLL_VERSION_STR, escaped_user_identifier, escaped_user_password, escaped_user_login_token, os_string);
#else
	TEST_N_DEF(LC3);
#endif

	free(os_string);
	free(escaped_user_login_token);
	free(escaped_user_identifier);
	free(escaped_user_password);

#if !defined(LC2)
	int error_code = MasterHttpResponse(k_cartographer_login_url, http_request_body_build, &rtn_result);
#else
	TEST_N_DEF(LC2);
#endif

	SecureZeroMemory(http_request_body_build, sizeof(char) * http_request_body_build_len + 1);
	free(http_request_body_build);

	if (error_code == 0) {
		error_code = InterpretMasterLogin(rtn_result, ltoken);
		if (error_code > 0) {
			result = true;
		}
		free(rtn_result);
	}

	if (error_code < 0) {
		addDebugText("ERROR Account Login: %d", error_code);

		if (error_code == ERROR_CODE_INVALID_LOGIN_TOKEN) {
			char* username = 0;
			for (int i = 0; i < H2AccountCount; i++) {
				if (H2AccountArrayLoginToken[i] && strcmp(H2AccountArrayLoginToken[i], ltoken) == 0) {
					username = H2AccountArrayUsername[i];
					break;
				}
			}
		}
	}
	else {
		if (error_code == SUCCESS_CODE_MACHINE_SERIAL_INSUFFICIENT) {
			result = false;
		}
	}

	if (out_master_login_interpret_result)
		*out_master_login_interpret_result = error_code;

	return result;
}


void AccountDediLogin(PXOVERLAPPED pOverlapped)
{
	addDebugText("Logging the Dedi Server in...");

	// clear LAN login info if we are logged in locally
	if (XUserSignedInLocally(0))
	{
		XUserSignOut(0);
	}

	// if we are not signed in online, sign us in
	if (!XUserSignedOnline(0))
	{
		//none of that stuff is setup for the dedi server yet since there are no gui commands for it.
		//currently credentials are taken from the config file.
		//also don't enable this since nothing's initialised for the server.
		addDebugText("Signing in dedicated server online.");
		if (HandleGuiLogin(0, H2Config_login_identifier, H2Config_login_password, nullptr))
		{
			XUserSignInSetStatusChanged(0);
		}
	}

	if (pOverlapped)
	{
		pOverlapped->InternalLow = ERROR_SUCCESS;
		pOverlapped->InternalHigh = 0;
		pOverlapped->dwExtendedError = S_OK;
	}
	return;
}

#pragma endregion Halo 2 Master Login

