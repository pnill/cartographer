#pragma once
#include "stdafx.h"

void OverwriteAssembly(BYTE* srcAddr, BYTE* writeAssm, int lenAssm);
void HexToByteArray(BYTE* byteArray, char* pointerHex);
int FindStartOfLine(FILE* fp, int lineStrLen);
void GetVKeyCodeString(int vkey, char* rtnString, int strLen);
void PadCStringWithChar(char* strToPad, int toFullLength, char c);
