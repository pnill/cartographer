#pragma once

const int ERROR_CODE_INVALID_PARAM = -1;
const int ERROR_CODE_INVALID_VERSION = -2;
const int ERROR_CODE_INVALID_LOGIN_TOKEN = -4;
const int ERROR_CODE_INVALID_LOGIN_ID = -5;
const int ERROR_CODE_INVALID_PASSWORD = -6;

const int ERROR_CODE_MACHINE_BANNED = -10;
const int ERROR_CODE_ACCOUNT_DATA = -44;
const int ERROR_CODE_ACCOUNT_BANNED = -11;
const int ERROR_CODE_ACCOUNT_DISABLED = -12;
const int ERROR_CODE_MACHINE_SERIAL_INSUFFICIENT = -13;
const int SUCCESS_CODE_MACHINE_SERIAL_INSUFFICIENT = 7;

void UpdateConnectionStatus();
bool HandleGuiLogin(char* token, char* identifier, char* password, int* master_login_interpret_result);
char ConfigureUserDetails(char* username, char* login_token, unsigned long long xuid, unsigned long lanaddr, unsigned long xnaddr, const char* abEnet, const char* abOnline, bool onlineSignIn);

extern bool AccountEdit_remember;
