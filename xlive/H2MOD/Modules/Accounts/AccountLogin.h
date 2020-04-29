#pragma once

void UpdateConnectionStatus();
bool HandleGuiLogin(char* token, char* identifier, char* password);
char ConfigureUserDetails(char* username, char* login_token, unsigned long long xuid, unsigned long saddr, unsigned long xnaddr, char* abEnet, char* abOnline, bool onlineSignIn);

extern bool AccountEdit_remember;
