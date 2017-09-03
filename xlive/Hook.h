#ifndef _DETOURS_H
#define _DETOURS_H

void *DetourFunc(BYTE *src, const BYTE *dst, const int len);
void RetourFunc(BYTE *src, BYTE *restore, const int len);
void *DetourClassFunc(BYTE *src, const BYTE *dst, const int len);
void RetourClassFunc(BYTE *src, BYTE *restore, const int len);

void *VTableFunction(void *ClassPtr, DWORD index);
void Codecave(DWORD destAddress, VOID(*func)(VOID), BYTE nopCount);
void WriteBytesASM(DWORD destAddress, LPVOID patch, DWORD numBytes);

#endif