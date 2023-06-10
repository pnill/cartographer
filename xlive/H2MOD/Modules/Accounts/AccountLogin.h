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

int GetMasterState();
const char* GetMasterStateStr();
void UpdateMasterLoginStatus(bool developer = false);
void UpdateMasterStatus(int state, const char* state_str, ...);
bool HandleGuiLogin(char* token, char* identifier, char* password, int* master_login_interpret_result);
int ConfigureUserDetails(const char* username, const char* login_token, unsigned long long xuid, unsigned long xnaddr, unsigned long lanaddr, const char* machineUID, const char* abOnline, bool onlineSignIn, bool developer = false);

extern bool AccountEdit_remember;
