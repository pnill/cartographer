#include <windows.h>
#include <malloc.h>

#include "hook.h"
#pragma warning( disable :4996)

void *DetourFunc(BYTE *src, const BYTE *dst, const int len)
{
	BYTE *jmp = (BYTE*)malloc(len + 5);
	DWORD dwBack;

	VirtualProtect(src, len, PAGE_READWRITE, &dwBack);

	memcpy(jmp, src, len);
	jmp += len;

	jmp[0] = 0xE9;
	*(DWORD*)(jmp + 1) = (DWORD)(src + len - jmp) - 5;

	src[0] = 0xE9;
	*(DWORD*)(src + 1) = (DWORD)(dst - src) - 5;

	for (int i = 5; i < len; i++)
		src[i] = 0x90;

	VirtualProtect(src, len, dwBack, &dwBack);

	return (jmp - len);
}

void RetourFunc(BYTE *src, BYTE *restore, const int len)
{
	DWORD dwBack;

	VirtualProtect(src, len, PAGE_READWRITE, &dwBack);
	memcpy(src, restore, len);

	restore[0] = 0xE9;
	*(DWORD*)(restore + 1) = (DWORD)(src - restore) - 5;

	VirtualProtect(src, len, dwBack, &dwBack);
}


void *DetourClassFunc(BYTE *src, const BYTE *dst, const int len)
{
	BYTE *jmp = (BYTE*)malloc(len + 8);

	DWORD dwBack;
	VirtualProtect(src, len, PAGE_READWRITE, &dwBack);
	memcpy(jmp + 3, src, len);

	// calculate callback function call
	jmp[0] = 0x58;							// pop eax
	jmp[1] = 0x59;							// pop ecx
	jmp[2] = 0x50;							// push eax
	jmp[len + 3] = 0xE9;						// jmp
	*(DWORD*)(jmp + len + 4) = (DWORD)((src + len) - (jmp + len + 3)) - 5;

	// detour source function call
	src[0] = 0x58;							// pop eax;
	src[1] = 0x51;							// push ecx
	src[2] = 0x50;							// push eax
	src[3] = 0xE9;							// jmp
	*(DWORD*)(src + 4) = (DWORD)(dst - (src + 3)) - 5;

	for (int i = 8; i < len; i++)
		src[i] = 0x90;

	VirtualProtect(src, len, dwBack, &dwBack);

	return jmp;
}

void RetourClassFunc(BYTE *src, BYTE *restore, const int len)
{
	DWORD dwBack;

	VirtualProtect(src, len, PAGE_READWRITE, &dwBack);
	memcpy(src, restore + 3, len);

	restore[3] = 0xE9;
	*(DWORD*)(restore + 4) = (DWORD)(src - (restore + 3)) - 5;

	VirtualProtect(src, len, dwBack, &dwBack);
}

void *VTableFunction(void *ClassPtr, DWORD index)
{
	void **pVtable = *(void***)ClassPtr;
	return pVtable[index];
}

void WriteBytes(DWORD destAddress, LPVOID bytesToWrite, int numBytes)
{
	DWORD OldProtection;
	DWORD temp;

	VirtualProtect((LPVOID)destAddress, numBytes, PAGE_EXECUTE_READWRITE, &OldProtection);
	memcpy((LPVOID)destAddress, bytesToWrite, numBytes);
	VirtualProtect((LPVOID)destAddress, numBytes, OldProtection, &temp); //quick fix for exception that happens here
}

void PatchCall(DWORD call_addr, DWORD new_function_ptr) {
	DWORD callRelative = new_function_ptr - (call_addr + 5);
	WritePointer(call_addr + 1, reinterpret_cast<void*>(callRelative));
}

void WritePointer(DWORD offset, void *ptr) {
	BYTE* pbyte = (BYTE*)&ptr;
	BYTE assmNewFuncRel[0x4] = { pbyte[0], pbyte[1], pbyte[2], pbyte[3] };
	WriteBytes(offset, assmNewFuncRel, 0x4);
}

void PatchWinAPICall(DWORD call_addr, DWORD new_function_ptr)
{
	BYTE call = 0xE8;
	WriteValue(call_addr, call);

	PatchCall(call_addr, new_function_ptr);

	// pad the extra unused byte
	BYTE padding = 0x90;
	WriteValue(call_addr + 5, padding);
}

VOID Codecave(DWORD destAddress, VOID(*func)(VOID), BYTE nopCount)
{

	DWORD offset = (PtrToUlong(func) - destAddress) - 5;


	BYTE nopPatch[0xFF] = { 0 };

	BYTE patch[5] = { 0xE8, 0x00, 0x00, 0x00, 0x00 };
	memcpy(patch + 1, &offset, sizeof(DWORD));
	WriteBytes(destAddress, patch, 5);

	if (nopCount == 0)
		return;


	memset(nopPatch, 0x90, nopCount);


	WriteBytes(destAddress + 5, nopPatch, nopCount);
}
