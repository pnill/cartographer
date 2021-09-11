#include "stdafx.h"
#include "H2MOD\Modules\OnScreenDebug\OnscreenDebug.h"
#include "H2MOD\Modules\CustomMenu\CustomLanguage.h"
#include "H2MOD\Modules\Startup\Startup.h"
#include "H2MOD\Modules\CustomMenu\CustomMenu.h"
#include "H2MOD\Modules\Utils\Utils.h"
#include "H2MOD\Modules\Accounts\Accounts.h"
#include "H2MOD\Modules\Accounts\AccountLogin.h"
#include "H2MOD\Modules\Config\Config.h"


#include "XLive\xnet\upnp.h"

bool AccountEdit_remember = true;

#pragma region h2master_login

typedef LONG NTSTATUS, *PNTSTATUS;
#define STATUS_SUCCESS (0x00000000)

typedef NTSTATUS(WINAPI* RtlGetVersionPtr)(PRTL_OSVERSIONINFOW);

void UpdateConnectionStatus() {
	extern int MasterState;
	extern char* ServerStatus;
	if (userSignedOnline(0)) {
		MasterState = 10;
		if (!Memory::isDedicatedServer())
			snprintf(ServerStatus, 250, "Status: Online");
	}
	else if (userSignedInLocally(0))
	{
		MasterState = 2;
		if (!Memory::isDedicatedServer())
			snprintf(ServerStatus, 250, "Status: Locally signed in");
	}
	else
	{
		MasterState = 2;
		if (!Memory::isDedicatedServer())
			snprintf(ServerStatus, 250, "Status: Offline");
	}
}

char ConfigureUserDetails(char* username, char* login_token, unsigned long long xuid, unsigned long xnaddr, unsigned long lanaddr, const char* abEnet, const char* abOnline, bool onlineSignIn) {

	if (strlen(username) <= 0 || xuid == 0 || strnlen(abEnet, 12) != 12 || strnlen(abOnline, 40) != 40) {
		return 0;
	}

	char result = strlen(login_token) == 32 ? 1 : 2;

	XUserSetup(0, xuid, username, xnaddr, lanaddr, abEnet, abOnline, onlineSignIn);

	UpdateConnectionStatus();

	if (onlineSignIn) {
		if (!Memory::isDedicatedServer())
			ForwardPorts();
	}

	if (H2CurrentAccountLoginToken) {
		free(H2CurrentAccountLoginToken);
	}
	if (result == 1) {
		H2CurrentAccountLoginToken = (char*)calloc(33, sizeof(char));
		snprintf(H2CurrentAccountLoginToken, 33, login_token);
	}
	else {
		H2CurrentAccountLoginToken = 0;
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
	int result = 0;//will stay as 0 when master only returns "return_code=xxx<br>"

	char username[XUSER_NAME_SIZE] = { "" };
	char login_token[33] = { "" };
	unsigned long long xuid = 0;
	unsigned long saddr = 0;
	unsigned long xnaddr = 0;
	char abEnet[13] = { "" };
	char abOnline[41] = { "" };


	char tempstr1[129] = { "" };
	int tempint1 = -1;
	unsigned long long templlu1 = 0;
	unsigned int tempuint1 = 0;
	unsigned long tempulong = 0;

	char* index_current = response_content;
	char* index_next = 0;
	while (index_next = strstr(index_current, "<br>")) {
		char* fileLine = (char*)malloc((index_next - index_current + 1) * sizeof(char));
		memcpy(fileLine, index_current, index_next - index_current);
		fileLine[index_next - index_current] = 0;


		if (sscanf(fileLine, "return_code=%d", &tempint1) == 1) {
			addDebugText("Return code is: %d", tempint1);
		}
		else if (sscanf(fileLine, "login_code=%d", &tempint1) == 1) {
			addDebugText("Login code is: %d", tempint1);
			result = tempint1;
		}
		else if (strstr(fileLine, "login_code_secondary=")) {
			char* tempName = fileLine + strlen("login_code_secondary=");
			while (isspace(*tempName)) {
				tempName++;
			}
			snprintf(tempstr1, 128, tempName);
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
		else if (strstr(fileLine, "login_external_addr=")) {
			char* tempName = fileLine + strlen("login_external_addr=");
			while (isspace(*tempName)) {
				tempName++;
			}
			snprintf(tempstr1, 45, tempName);
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
				if (strlen(H2Config_str_wan) <= 0 && strlen(H2Config_str_lan) > 0) {
					H2Config_ip_wan = resolvedAddr;
				}
				xnaddr = resolvedAddr;
			}
		}
		else if (strstr(fileLine, "login_master_relay_ip=")) {
			char* tempName = fileLine + strlen("login_master_relay_ip=");
			while (isspace(*tempName)) {
				tempName++;
			}
			snprintf(tempstr1, 45, tempName);
			for (int j = strlen(tempstr1) - 1; j > 0; j--) {
				if (isspace(tempstr1[j])) {
					tempstr1[j] = 0;
				}
				else {
					break;
				}
			}
			unsigned long resolvedAddr;
			if ((resolvedAddr = inet_addr(tempstr1)) != INADDR_NONE || strcmp(tempstr1, "255.255.255.255") == 0) {
				//addDebugText("H2Master Relay IP is: %s", tempstr1);
				H2Config_master_ip = resolvedAddr;
			}
		}
		else if (sscanf(fileLine, "login_master_relay_port=%d", &tempint1) == 1) {
			if (tempint1 >= 0) {
				//addDebugText("H2Master Relay Port is: %d", tempint1);
				H2Config_master_port_relay = tempint1;
			}
		}
		else if (strstr(fileLine, "login_token=")) {
			char* tempName = fileLine + strlen("login_token=");
			while (isspace(*tempName)) {
				tempName++;
			}
			snprintf(tempstr1, 33, tempName);
			for (int j = strlen(tempstr1) - 1; j > 0; j--) {
				if (isspace(tempstr1[j])) {
					tempstr1[j] = 0;
				}
				else {
					break;
				}
			}
			if (strlen(tempstr1) == 32) {
				//addDebugText("Login Token is: %s", tempstr1);
				strncpy(login_token, tempstr1, 33);
			}
		}
		else if (strstr(fileLine, "login_username=")) {
			char* tempName = fileLine + strlen("login_username=");
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
				addDebugText("Login Username is: %s", tempstr1);
				strncpy_s(username, tempstr1, XUSER_MAX_NAME_LENGTH);
			}
		}
		else if (sscanf(fileLine, "login_xuid=%llu", &templlu1) == 1) {
			if (templlu1 > 0) {
				//addDebugText("User XUID is: %llu", templlu1);
				xuid = templlu1;
			}
		}
		else if (sscanf(fileLine, "login_secure_addr=%x", &tempulong) == 1) {
			if (tempulong > 0) {
				//addDebugText("User saddr is: %08zx", tempulong);
				saddr = tempulong;
			}
		}
		else if (strstr(fileLine, "login_abEnet=")) {
			char* tempName = fileLine + strlen("login_abEnet=");
			while (isspace(*tempName)) {
				tempName++;
			}
			snprintf(tempstr1, 13, tempName);
			for (int j = strlen(tempstr1) - 1; j > 0; j--) {
				if (isspace(tempstr1[j])) {
					tempstr1[j] = 0;
				}
				else {
					break;
				}
			}
			if (strlen(tempstr1) == 12) {
				//addDebugText("Login abEnet is: %s", tempstr1);
				strncpy(abEnet, tempstr1, 13);
			}
		}
		else if (strstr(fileLine, "login_abOnline=")) {
			char* tempName = fileLine + strlen("login_abOnline=");
			while (isspace(*tempName)) {
				tempName++;
			}
			snprintf(tempstr1, 41, tempName);
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
				strncpy(abOnline, tempstr1, 41);
			}
		}

		free(fileLine);
		index_current = index_next + 4;
	}

	if (result > 0) {
		int result_details;
		if (result_details = ConfigureUserDetails(username, login_token, xuid, xnaddr, H2Config_ip_lan, abEnet, abOnline, true)) {
			//allow no login_token from backend in DB emergencies / random logins.
			if (result_details == 1) {
				if (prev_login_token) {
					if (H2AccountArrayLoginToken && H2AccountCount > 0) {
						for (int i = 0; i < H2AccountCount; i++) {
							if (H2AccountArrayLoginToken[i] && strcmp(H2AccountArrayLoginToken[i], prev_login_token) == 0) {
								if (H2AccountArrayUsername[i]) {
									free(H2AccountArrayUsername[i]);
								}
								H2AccountArrayUsername[i] = (char*)calloc(XUSER_NAME_SIZE, sizeof(char));
								strncpy_s(H2AccountArrayUsername[i], XUSER_NAME_SIZE, username, XUSER_MAX_NAME_LENGTH);
								snprintf(H2AccountArrayLoginToken[i], 33, login_token);
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
			if (!H2IsDediServer) {
				extern char* ServerStatus;
				if (result == 4) {
					snprintf(ServerStatus, 250, "Status: Developer");
				}
			}
		}
		else {
			result = ERROR_CODE_ACCOUNT_DATA;
		}
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
			GetVersionEx(&osvi);
		}

		os_string_buflen = 30;
		os_string = (char*)calloc(os_string_buflen, sizeof(char));
		snprintf(os_string, os_string_buflen, "Windows: %u.%u", osvi.dwMajorVersion, osvi.dwMinorVersion);
	}
	addDebugText(os_string);

	char http_request_body[] = "request_type=%08x&request_version=%s&user_identifier=%s&user_pass=%s&user_token=%s&op_sys=%s";

	char* escaped_user_login_token = encode_rfc3986(ltoken == 0 ? "" : ltoken);
	char* escaped_user_identifier = encode_rfc3986(identifier == 0 ? "" : identifier);
	char* escaped_user_password = encode_rfc3986(password == 0 ? "" : password);
	
	int http_request_body_build_len = (89 + 8 + strlen(DLL_VERSION_STR) + strlen(escaped_user_identifier) + strlen(escaped_user_password) + strlen(escaped_user_login_token) + strlen(os_string));
	char* http_request_body_build = (char*)malloc(sizeof(char) * http_request_body_build_len + 1);

	char* http_request_body_build2 = http_request_body_build + snprintf(http_request_body_build, http_request_body_build_len, http_request_body, ltoken == 0 ? 1 : 2, DLL_VERSION_STR, escaped_user_identifier, escaped_user_password, escaped_user_login_token, os_string);

	free(os_string);

	free(escaped_user_login_token);
	free(escaped_user_identifier);
	free(escaped_user_password);

	int error_code = MasterHttpResponse(std::string(cartographerURL + "/login2"), http_request_body_build, rtn_result);

	for (int i = strlen(http_request_body_build) - 1; i >= 0; i--) {
		http_request_body_build[i] = 0;
	}
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

			if (username) {
				char* login_identifier = H2CustomLanguageGetLabel(CMLabelMenuId_AccountEdit, 1);
				snprintf(login_identifier, strlen(username) + 1, username);
			}
		}
	}
	else {
		if (!H2IsDediServer) {
			char* login_identifier = H2CustomLanguageGetLabel(CMLabelMenuId_AccountEdit, 1);
			SecureZeroMemory(login_identifier, strlen(login_identifier));
		}
		if (error_code == SUCCESS_CODE_MACHINE_SERIAL_INSUFFICIENT) {
			result = false;
		}
	}

	if (out_master_login_interpret_result)
		*out_master_login_interpret_result = error_code;

	return result;
}

#pragma region Online Server Sign-in

// 5257: ??
HRESULT WINAPI XLiveManageCredentials(LPCWSTR lpszLiveIdName, LPCWSTR lpszLiveIdPassword, DWORD dwCredFlags, PXOVERLAPPED pXOverlapped)
{
	LOG_TRACE_XLIVE(L"XLiveManageCredentials (lpszLiveIdName = {0}, lpszLiveIdPassword = {1}, dwCredFlags = {2:#x}, pXOverlapped = {3:p})",
		lpszLiveIdName, lpszLiveIdPassword, dwCredFlags, (void*)pXOverlapped);

	if (pXOverlapped)
	{
		pXOverlapped->InternalLow = ERROR_SUCCESS;
		pXOverlapped->InternalHigh = 0;
		pXOverlapped->dwExtendedError = 0;
	}

	// not done - error now
	return S_OK;
}

// #5259: XLiveSignin
HRESULT WINAPI XLiveSignin(PWSTR pszLiveIdName, PWSTR pszLiveIdPassword, DWORD dwFlags, PXOVERLAPPED pOverlapped)
{
	LOG_TRACE_XLIVE("XLiveSignin() - signin in");

	addDebugText("Logging the Dedi Server in...");

	// clear LAN login info if we are logged in locally
	if (userSignedInLocally(0))
	{
		XUserSignOut(0);
	}

	// if we are not signed in online, sign us in
	if (!userSignedOnline(0))
	{
		//none of that stuff is setup for the dedi server yet since there are no gui commands for it.
		//currently credentials are taken from the config file.
		//also don't enable this since nothing's initialised for the server.
		addDebugText("Signing in dedicated server online.");
		HandleGuiLogin(0, H2Config_login_identifier, H2Config_login_password, nullptr);
	}
	
	if (pOverlapped)
	{
		pOverlapped->InternalLow = ERROR_SUCCESS;
		pOverlapped->InternalHigh = 0;
		pOverlapped->dwExtendedError = S_OK;
	}

	return S_OK;
}

// #5258: XLiveSignout
HRESULT WINAPI XLiveSignout(PXOVERLAPPED pXOverlapped)
{
	LOG_TRACE_XLIVE("XLiveSignout");

	XUserSignOut(0);

	if (pXOverlapped)
	{
		pXOverlapped->InternalLow = ERROR_SUCCESS;
		pXOverlapped->InternalHigh = 0;
		pXOverlapped->dwExtendedError = S_OK;
	}

	return S_OK;
}
#pragma endregion

#pragma endregion

