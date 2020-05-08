#include "stdafx.h"
#include "H2MOD\Modules\OnScreenDebug\OnscreenDebug.h"
#include "H2MOD\Modules\CustomMenu\CustomLanguage.h"
#include "H2MOD\Modules\Startup\Startup.h"
#include "H2MOD\Modules\CustomMenu\CustomMenu.h"
#include "H2MOD\Modules\Utils\Utils.h"
#include "H2MOD\Modules\Accounts\Accounts.h"
#include "H2MOD\Modules\Accounts\AccountLogin.h"
#include "H2MOD\Modules\Config\Config.h"
#include "XLive\xnet\IpManagement\XnIp.h"
#include "XLive\xnet\Sockets\XSocket.h"

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
		if (!h2mod->Server)
			snprintf(ServerStatus, 250, "Status: Online");
	}
	else if (userSignedInLocally(0))
	{
		MasterState = 2;
		if (!h2mod->Server)
			snprintf(ServerStatus, 250, "Status: Locally signed in");
	}
	else
	{
		MasterState = 2;
		if (!h2mod->Server)
			snprintf(ServerStatus, 250, "Status: Offline");
	}
}

char ConfigureUserDetails(char* username, char* login_token, unsigned long long xuid, unsigned long saddr, unsigned long xnaddr, char* abEnet, char* abOnline, bool onlineSignIn) {

	if (strlen(username) <= 0 || xuid == 0 || saddr == 0 || strlen(abEnet) != 12 || strlen(abOnline) != 40) {
		return 0;
	}

	char result = strlen(login_token) == 32 ? 1 : 2;

	XNADDR pxna;
	SecureZeroMemory(&pxna, sizeof(XNADDR));

	pxna.ina.s_addr = xnaddr;
	pxna.inaOnline.s_addr = saddr;
	pxna.wPortOnline = htons(H2Config_base_port);

	BYTE abEnet2[6];
	SecureZeroMemory(abEnet2, sizeof(abEnet2));
	for (int i = 0; i < 6; i++) {
		sscanf(&abEnet[i * 2], "%2hhx", &abEnet2[i]);
	}

	char abOnline2[20];
	SecureZeroMemory(abOnline2, sizeof(abOnline2));
	for (int i = 0; i < 20; i++) {
		sscanf(&abOnline[i * 2], "%2hhx", &abOnline2[i]);
	}
	memcpy(&pxna.abEnet, abEnet2, 6);
	memcpy(&pxna.abOnline, abOnline2, 20);

	XUserSetup(0, xuid, username, onlineSignIn);
	ipManager.SetupLocalConnectionInfo(&pxna);

	UpdateConnectionStatus();

	if (onlineSignIn) {
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
			char NotificationPlayerText[60];
			snprintf(NotificationPlayerText, 60, "Return code is: %d", tempint1);
			addDebugText(NotificationPlayerText);
		}
		else if (sscanf(fileLine, "login_code=%d", &tempint1) == 1) {
			char NotificationPlayerText[60];
			snprintf(NotificationPlayerText, 60, "Login code is: %d", tempint1);
			addDebugText(NotificationPlayerText);
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
				char NotificationPlayerText[150];
				snprintf(NotificationPlayerText, 150, "Login Code Secondary is: %s", tempstr1);
				addDebugText(NotificationPlayerText);
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
			char NotificationPlayerText[100];
			snprintf(NotificationPlayerText, 100, "Client External IP Address is: %s", tempstr1);
			addDebugText(NotificationPlayerText);
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
				//char NotificationPlayerText[60];
				//snprintf(NotificationPlayerText, 60, "H2Master Relay IP is: %s", tempstr1);
				//addDebugText(NotificationPlayerText);
				H2Config_master_ip = resolvedAddr;
			}
		}
		else if (sscanf(fileLine, "login_master_relay_port=%d", &tempint1) == 1) {
			if (tempint1 >= 0) {
				//char NotificationPlayerText[60];
				//snprintf(NotificationPlayerText, 60, "H2Master Relay Port is: %d", tempint1);
				//addDebugText(NotificationPlayerText);
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
				//char NotificationPlayerText[60];
				//snprintf(NotificationPlayerText, 60, "Login Token is: %s", tempstr1);
				//addDebugText(NotificationPlayerText);
				strncpy(login_token, tempstr1, 33);
			}
		}
		else if (strstr(fileLine, "login_username=")) {
			char* tempName = fileLine + strlen("login_username=");
			while (isspace(*tempName)) {
				tempName++;
			}
			strncpy_s(tempstr1, sizeof(tempstr1), tempName, strnlen_s(tempName, XUSER_MAX_NAME_LENGTH));
			for (int j = strlen(tempstr1) - 1; j > 0; j--) {
				if (isspace(tempstr1[j])) {
					tempstr1[j] = 0;
				}
				else {
					break;
				}
			}
			if (strlen(tempstr1) > 0) {
				char NotificationPlayerText[60];
				snprintf(NotificationPlayerText, 60, "Login Username is: %s", tempstr1);
				addDebugText(NotificationPlayerText);
				strncpy_s(username, tempstr1, strnlen_s(tempstr1, XUSER_MAX_NAME_LENGTH));
			}
		}
		else if (sscanf(fileLine, "login_xuid=%llu", &templlu1) == 1) {
			if (templlu1 > 0) {
				//char NotificationPlayerText[60];
				//snprintf(NotificationPlayerText, 60, "User XUID is: %llu", templlu1);
				//addDebugText(NotificationPlayerText);
				xuid = templlu1;
			}
		}
		else if (sscanf(fileLine, "login_secure_addr=%x", &tempulong) == 1) {
			if (tempulong > 0) {
				//char NotificationPlayerText[60];
				//snprintf(NotificationPlayerText, 60, "User saddr is: %08zx", tempulong);
				//addDebugText(NotificationPlayerText);
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
				//char NotificationPlayerText[60];
				//snprintf(NotificationPlayerText, 60, "Login abEnet is: %s", tempstr1);
				//addDebugText(NotificationPlayerText);
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
				//char NotificationPlayerText[80];
				//snprintf(NotificationPlayerText, 80, "Login abOnline is: %s", tempstr1);
				//addDebugText(NotificationPlayerText);
				strncpy(abOnline, tempstr1, 41);
			}
		}

		free(fileLine);
		index_current = index_next + 4;
	}

	if (result > 0) {
		int result_details;
		if (result_details = ConfigureUserDetails(username, login_token, xuid, saddr, xnaddr, abEnet, abOnline, true)) {
			//allow no login_token from backend in DB emergencies / random logins.
			if (result_details == 1) {
				if (prev_login_token) {
					if (H2AccountBufferLoginToken && H2AccountCount > 0) {
						for (int i = 0; i < H2AccountCount; i++) {
							if (H2AccountBufferLoginToken[i] && strcmp(H2AccountBufferLoginToken[i], prev_login_token) == 0) {
								if (H2AccountBufferUsername[i]) {
									free(H2AccountBufferUsername[i]);
								}
								H2AccountBufferUsername[i] = (char*)calloc(XUSER_NAME_SIZE, sizeof(char));
								strncpy_s(H2AccountBufferUsername[i], XUSER_NAME_SIZE, username, strnlen_s(username, XUSER_MAX_NAME_LENGTH));
								snprintf(H2AccountBufferLoginToken[i], 33, login_token);
								break;
							}
						}
					}
				}
				else {
					if (AccountEdit_remember) {
						H2AccountBufferAdd(login_token, username);
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
		os_string = (char*)malloc(sizeof(char) * os_string_buflen);
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

	int master_login_interpret_result = -1;
	int master_http_response_result = MasterHttpResponse("https://cartographer.online/login2", http_request_body_build, rtn_result);

	for (int i = strlen(http_request_body_build) - 1; i >= 0; i--) {
		http_request_body_build[i] = 0;
	}
	free(http_request_body_build);

	if (master_http_response_result == 0) {
		master_login_interpret_result = InterpretMasterLogin(rtn_result, ltoken);
		if (master_login_interpret_result > 0) {
			result = true;
		}
		free(rtn_result);
	}
	if (master_login_interpret_result < 0) {
		char NotificationPlayerText[40];
		sprintf(NotificationPlayerText, "ERROR Account Login: %d", master_login_interpret_result);
		addDebugText(NotificationPlayerText);

		if (master_login_interpret_result == ERROR_CODE_INVALID_LOGIN_TOKEN) {
			char* username = 0;
			for (int i = 0; i < H2AccountCount; i++) {
				if (H2AccountBufferLoginToken[i] && strcmp(H2AccountBufferLoginToken[i], ltoken) == 0) {
					username = H2AccountBufferUsername[i];
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
		if (master_login_interpret_result == SUCCESS_CODE_MACHINE_SERIAL_INSUFFICIENT) {
			result = false;
		}
	}

	if (out_master_login_interpret_result)
		*out_master_login_interpret_result = master_login_interpret_result;

	return result;
}

#pragma endregion

