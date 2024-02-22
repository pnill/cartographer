#include "stdafx.h"
#include "AccountCreate.h"

#include "H2MOD/Modules/Shell/Config.h"
#include "H2MOD/Modules/OnScreenDebug/OnscreenDebug.h"
#include "H2MOD/Modules/CustomMenu/CustomLanguage.h"
#include "H2MOD/Utils/Utils.h"

#include "H2MOD/Modules/CustomMenu/c_error_menu.h"

#define ERROR_CODE_INVALID_PARAM -1
#define ERROR_CODE_INVALID_EMAIL -2
#define ERROR_CODE_INVALID_USERNAME -3
#define ERROR_CODE_INVALID_PASSWORD -4
#define ERROR_CODE_TAKEN_EMAIL -5
#define ERROR_CODE_TAKEN_USERNAME -6
#define ERROR_CODE_BANNED_EMAIL_DOMAIN -7

// TODO (Carefully) Cleanup and move

static int InterpretMasterCreate(char* response_content) {
	int result = 0;//will stay as 0 when master only returns "return_code=xxx<br>"

	int tempint1 = -1;
	char tempstr1[512] = { "" };
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
			result = tempint1;
		}
		else if (strstr(fileLine, "username=")) {
			char* tempName = fileLine + strlen("username=");
			while (isspace(*tempName)) {
				tempName++;
			}
			snprintf(tempstr1, 32, tempName);
			for (int j = strlen(tempstr1) - 1; j > 0; j--) {
				if (isspace(tempstr1[j])) {
					tempstr1[j] = 0;
				}
				else {
					break;
				}
			}
			if (strlen(tempstr1) > 0) {
				addDebugText("Username Should be: %s", tempstr1);
				char* username = H2CustomLanguageGetLabel(CMLabelMenuId_AccountCreate, 1);
				strncpy_s(username, XUSER_NAME_SIZE, tempstr1, XUSER_MAX_NAME_LENGTH);
			}
		}

		free(fileLine);
		index_current = index_next + 4;
	}


	if (result > 0) {
		
	}

	return result;
}


bool HandleGuiAccountCreate(char* username, char* email, char* password) {
	bool result = false;
	char* rtn_result = 0;

	char http_request_body[] = "username=%s&email=%s&password=%s&request_version=%s";
	char http_request_body_build[1024] = { "" };

	char* escaped_user_username = encode_rfc3986(username);
	char* escaped_user_email = encode_rfc3986(email);
	char* escaped_user_password = encode_rfc3986(password);
	sprintf_s(http_request_body_build, http_request_body, escaped_user_username, escaped_user_email, escaped_user_password, DLL_VERSION_STR);
	free(escaped_user_username);
	free(escaped_user_email);
	free(escaped_user_password);

#ifndef LC4
	int rtn_code = MasterHttpResponse(std::string(cartographerURL + "/create1").c_str(), http_request_body_build, &rtn_result);
#else
	TEST_N_DEF(LC4);
#endif
	

	if (rtn_code == 0) {
		rtn_code = InterpretMasterCreate(rtn_result);
		if (rtn_code > 0) {
			result = true;
		}
		free(rtn_result);
	}
	if (rtn_code <= 0) {
		addDebugText("ERROR Account Create: %d", rtn_code);
		if (rtn_code == 0 || rtn_code == ERROR_CODE_CURL_SOCKET_FAILED || rtn_code == ERROR_CODE_CURL_HANDLE || rtn_code == ERROR_CODE_CURL_EASY_PERF
			|| rtn_code == ERROR_CODE_INVALID_PARAM) {
			//internal error
			CustomMenuCall_Error_Inner(CMLabelMenuId_Error, 0xFFFFF014, 0xFFFFF015);
		}
		else if (rtn_code == ERROR_CODE_INVALID_EMAIL) {
			CustomMenuCall_Error_Inner(CMLabelMenuId_Error, 0xFFFFF01A, 0xFFFFF01B);
		}
		else if (rtn_code == ERROR_CODE_INVALID_USERNAME) {
			CustomMenuCall_Error_Inner(CMLabelMenuId_Error, 0xFFFFF01C, 0xFFFFF01D);
		}
		else if (rtn_code == ERROR_CODE_INVALID_PASSWORD) {
			CustomMenuCall_Error_Inner(CMLabelMenuId_Error, 0xFFFFF01E, 0xFFFFF01F);
		}
		else if (rtn_code == ERROR_CODE_TAKEN_EMAIL) {
			CustomMenuCall_Error_Inner(CMLabelMenuId_Error, 0xFFFFF020, 0xFFFFF021);
		}
		else if (rtn_code == ERROR_CODE_TAKEN_USERNAME) {
			CustomMenuCall_Error_Inner(CMLabelMenuId_Error, 0xFFFFF022, 0xFFFFF023);
		}
		else if (rtn_code == ERROR_CODE_BANNED_EMAIL_DOMAIN) {
			CustomMenuCall_Error_Inner(CMLabelMenuId_Error, 0xFFFFF024, 0xFFFFF025);
		}
		else {
			//unknown error!
			CustomMenuCall_Error_Inner(CMLabelMenuId_Error, 0xFFFFF012, 0xFFFFF013);
		}
	}
	else {

	}
	return result;
}
