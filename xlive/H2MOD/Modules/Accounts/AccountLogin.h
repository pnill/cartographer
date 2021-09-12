#pragma once

#define ERROR_CODE_INVALID_PARAM -1
#define ERROR_CODE_INVALID_VERSION -2
#define ERROR_CODE_INVALID_LOGIN_TOKEN -4
#define ERROR_CODE_INVALID_LOGIN_ID -5
#define ERROR_CODE_INVALID_PASSWORD -6

#define ERROR_CODE_MACHINE_BANNED -10
#define ERROR_CODE_ACCOUNT_DATA -44
#define ERROR_CODE_ACCOUNT_BANNED -11
#define ERROR_CODE_ACCOUNT_DISABLED -12
#define ERROR_CODE_MACHINE_SERIAL_INSUFFICIENT -13
#define SUCCESS_CODE_MACHINE_SERIAL_INSUFFICIENT 7

void UpdateConnectionStatus();
bool HandleGuiLogin(char* token, char* identifier, char* password, int* master_login_interpret_result);
char ConfigureUserDetails(char* username, char* login_token, unsigned long long xuid, unsigned long xnaddr, unsigned long lanaddr, const char* abEnet, const char* abOnline, bool onlineSignIn);

extern bool AccountEdit_remember;
