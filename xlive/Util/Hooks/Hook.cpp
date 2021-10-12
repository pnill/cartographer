
#include <malloc.h>


#include "hook.h"
#pragma warning( disable :4996)

#define VirtualProtectAndExecutePatch(_address, _length, _protection, _code) \
	DWORD dwBack[2]; \
	VirtualProtect((_address), (_length), (_protection), &dwBack[0]); \
	_code \
	VirtualProtect((_address), (_length), dwBack[0], &dwBack[1]); \

void *DetourFunc(BYTE *src, const BYTE *dst, const unsigned int len)
{
	BYTE *jmp = (BYTE*)VirtualAlloc(nullptr, len + 5, MEM_COMMIT, PAGE_EXECUTE_READWRITE);

	VirtualProtectAndExecutePatch(src, len, PAGE_READWRITE, // parameters

	// code
	memcpy(jmp, src, len);
	jmp += len;

	jmp[0] = 0xE9;
	*(DWORD*)(jmp + 1) = (DWORD)(src + len - jmp) - 5;

	src[0] = 0xE9;
	*(DWORD*)(src + 1) = (DWORD)(dst - src) - 5;

	for (int i = 5; i < len; i++)
		src[i] = 0x90;
	);

	return (jmp - len);
}

void RetourFunc(BYTE *src, BYTE *restore, const unsigned int len)
{
	VirtualProtectAndExecutePatch(src, len, PAGE_READWRITE,

	memcpy(src, restore, len);
	restore[0] = 0xE9;
	*(DWORD*)(restore + 1) = (DWORD)(src - restore) - 5;
	);
}


void *DetourClassFunc(BYTE *src, const BYTE *dst, const unsigned int len)
{
	BYTE *jmp = (BYTE*)VirtualAlloc(nullptr, len + 8, MEM_COMMIT, PAGE_EXECUTE_READWRITE);

	VirtualProtectAndExecutePatch(src, len, PAGE_READWRITE,

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
	);

	return jmp;
}

void RetourClassFunc(BYTE *src, BYTE *restore, const unsigned int len)
{
	VirtualProtectAndExecutePatch(src, len, PAGE_READWRITE,

	memcpy(src, restore + 3, len);
	restore[3] = 0xE9;
	*(DWORD*)(restore + 4) = (DWORD)(src - (restore + 3)) - 5;
	);
}

void *VTableFunction(void *ClassPtr, DWORD index)
{
	void **pVtable = *(void***)ClassPtr;
	return pVtable[index];
}

void WriteBytes(DWORD destAddress, LPVOID bytesToWrite, const unsigned int numBytes)
{
	VirtualProtectAndExecutePatch((LPVOID)destAddress, numBytes, PAGE_EXECUTE_READWRITE,

	memcpy((LPVOID)destAddress, bytesToWrite, numBytes);
	);
}

void PatchCall(DWORD call_addr, DWORD new_function_ptr) {
	DWORD callRelative = new_function_ptr - (call_addr + 5);
	WritePointer(call_addr + 1, reinterpret_cast<void*>(callRelative));
}

void NopFill(DWORD address, const unsigned int length)
{
	VirtualProtectAndExecutePatch((LPVOID)address, length, PAGE_EXECUTE_READWRITE,

	memset((LPVOID)address, 0x90, length);
	);
}

void ReadBytesProtected(DWORD address, BYTE* buf, BYTE count)
{
	VirtualProtectAndExecutePatch((LPVOID)address, count, PAGE_EXECUTE_READWRITE,

	memcpy(buf, (LPVOID)address, count);
	);
}

void WritePointer(DWORD offset, void *ptr) {
	WriteValue<DWORD>(offset, *(DWORD*)&ptr);
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

	BYTE patch[5] = { 0xE8, 0x00, 0x00, 0x00, 0x00 };
	memcpy(patch + 1, &offset, sizeof(DWORD));
	WriteBytes(destAddress, patch, 5);

	if (nopCount == 0)
		return;

	NopFill(destAddress + 5, nopCount);
}
