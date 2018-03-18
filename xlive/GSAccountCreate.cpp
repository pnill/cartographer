#include "GSAccountCreate.h"
#include "H2OnscreenDebugLog.h"
#include "H2Config.h"
#include "GSCustomLanguage.h"
#include "H2Startup.h"
#include "GSCustomMenu.h"
#include "GSUtils.h"

const int ERROR_CODE_INVALID_PARAM = -1;
const int ERROR_CODE_INVALID_EMAIL = -2;
const int ERROR_CODE_INVALID_USERNAME = -3;
const int ERROR_CODE_INVALID_PASSWORD = -4;
const int ERROR_CODE_TAKEN_EMAIL = -5;
const int ERROR_CODE_TAKEN_USERNAME = -6;
const int ERROR_CODE_BANNED_EMAIL_DOMAIN = -7;

static int InterpretMasterCreate(char* response_content) {
	int result = 0;//will stay as 0 when master only returns "return_code=xxx<br>"

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
				char NotificationPlayerText[60];
				snprintf(NotificationPlayerText, 60, "Username Should be: %s", tempstr1);
				addDebugText(NotificationPlayerText);
				char* username = H2CustomLanguageGetLabel(CMLabelMenuId_AccountCreate, 1);
				strncpy(username, tempstr1, 32);
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

	char http_request_body[] = "username=%s&email=%s&password=%s";
	char http_request_body_build[400];

	char* escaped_user_username = encode_rfc3986(username);
	char* escaped_user_email = encode_rfc3986(email);
	char* escaped_user_password = encode_rfc3986(password);
	sprintf_s(http_request_body_build, http_request_body, escaped_user_username, escaped_user_email, escaped_user_password);
	free(escaped_user_username);
	free(escaped_user_email);
	free(escaped_user_password);

	int rtn_code = MasterHttpResponse("https://cartographer.online/create1", http_request_body_build, rtn_result);
	if (rtn_code == 0) {
		rtn_code = InterpretMasterCreate(rtn_result);
		if (rtn_code > 0) {
			result = true;
		}
		free(rtn_result);
	}
	if (rtn_code <= 0) {
		char NotificationPlayerText[40];
		sprintf(NotificationPlayerText, "ERROR Account Create: %d", rtn_code);
		addDebugText(NotificationPlayerText);
		if (rtn_code == 0 || rtn_code == ERROR_CODE_CURL_SOCKET_FAILED || rtn_code == ERROR_CODE_CURL_HANDLE || rtn_code == ERROR_CODE_CURL_EASY_PERF
			|| rtn_code == ERROR_CODE_INVALID_PARAM) {
			//internal error
			GSCustomMenuCall_Error_Inner(CMLabelMenuId_Error, 0xFFFFF014, 0xFFFFF015);
		}
		else if (rtn_code == ERROR_CODE_INVALID_EMAIL) {
			GSCustomMenuCall_Error_Inner(CMLabelMenuId_Error, 0xFFFFF01A, 0xFFFFF01B);
		}
		else if (rtn_code == ERROR_CODE_INVALID_USERNAME) {
			GSCustomMenuCall_Error_Inner(CMLabelMenuId_Error, 0xFFFFF01C, 0xFFFFF01D);
		}
		else if (rtn_code == ERROR_CODE_INVALID_PASSWORD) {
			GSCustomMenuCall_Error_Inner(CMLabelMenuId_Error, 0xFFFFF01E, 0xFFFFF01F);
		}
		else if (rtn_code == ERROR_CODE_TAKEN_EMAIL) {
			GSCustomMenuCall_Error_Inner(CMLabelMenuId_Error, 0xFFFFF020, 0xFFFFF021);
		}
		else if (rtn_code == ERROR_CODE_TAKEN_USERNAME) {
			GSCustomMenuCall_Error_Inner(CMLabelMenuId_Error, 0xFFFFF022, 0xFFFFF023);
		}
		else if (rtn_code == ERROR_CODE_BANNED_EMAIL_DOMAIN) {
			GSCustomMenuCall_Error_Inner(CMLabelMenuId_Error, 0xFFFFF024, 0xFFFFF025);
		}
		else {
			//unknown error!
			GSCustomMenuCall_Error_Inner(CMLabelMenuId_Error, 0xFFFFF012, 0xFFFFF013);
		}
	}
	else {

	}
	return result;
}
