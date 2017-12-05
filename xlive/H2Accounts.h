#pragma once

void InitH2Accounts();
void DeinitH2Accounts();
void SaveH2Accounts();
bool ReadH2Accounts();

void H2AccountBufferAdd(char* token, char* username);

extern int H2AccountCount;
extern int H2AccountBufferI;
extern int H2AccountLastUsed;
extern char** H2AccountBufferUsername;
extern char** H2AccountBufferLoginToken;

extern char* H2CurrentAccountLoginToken;
