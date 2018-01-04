#pragma once

void *DetourFunc(BYTE *src, const BYTE *dst, const int len);
void RetourFunc(BYTE *src, BYTE *restore, const int len);
void *DetourClassFunc(BYTE *src, const BYTE *dst, const int len);
void RetourClassFunc(BYTE *src, BYTE *restore, const int len);

void *VTableFunction(void *ClassPtr, DWORD index);
void Codecave(DWORD destAddress, VOID(*func)(VOID), BYTE nopCount);
void WriteBytes(DWORD destAddress, LPVOID bytesToWrite, int numBytes);

