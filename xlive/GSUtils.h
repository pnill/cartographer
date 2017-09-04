#pragma once
#include "stdafx.h"

///Use instead: WriteBytesASM(DWORD destAddress, LPVOID patch, DWORD numBytes);
void OverwriteAssembly(BYTE* srcAddr, BYTE* writeAssm, int lenAssm);
void PatchCall(DWORD call_addr, DWORD new_function_ptr);
void HexToByteArray(BYTE* byteArray, char* pointerHex);
int FindLineStart(FILE* fp, int lineStrLen);
void GetVKeyCodeString(int vkey, char* rtnString, int strLen);
void PadCStringWithChar(char* strToPad, int toFullLength, char c);
int ComputeFileMd5Hash(wchar_t* filepath, char* rtnMd5);
int GetWidePathFromFullWideFilename(wchar_t* filepath, wchar_t* rtnpath);
LONG GetDWORDRegKey(HKEY hKey, wchar_t* strValueName, DWORD* nValue);
void pushHostLobby();

#define J( symbol1, symbol2 ) _DO_JOIN( symbol1, symbol2 )
#define _DO_JOIN( symbol1, symbol2 ) symbol1##symbol2
#define NopFill(Address, len)                       \
	BYTE J(NopFIll_, __LINE__ )[len];               \
	std::fill_n(J(NopFIll_, __LINE__ ), len, 0x90); \
	WriteBytesASM(Address, J(NopFIll_, __LINE__ ), len)
