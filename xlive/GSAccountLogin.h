#pragma once

bool HandleGuiLogin(char* token, char* identifier, char* password);
bool ConfigureUserDetails(char* username, char* login_token, unsigned long long xuid, unsigned long saddr, unsigned long xnaddr, char* abEnet, char* abOnline);

extern bool AccountEdit_remember;
