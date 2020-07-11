#pragma once

void InitH2Accounts();
void DeinitH2Accounts();
void SaveH2Accounts();
bool ReadH2Accounts();

void H2AccountAccountAdd(const char* username, const char* token);
void H2AccountAccountRemove(int accountArrayIndex);

extern int H2AccountCount;
extern int H2AccountLastUsed;
extern char** H2AccountArrayUsername;
extern char** H2AccountArrayLoginToken;

extern char* H2CurrentAccountLoginToken;
