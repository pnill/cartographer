#pragma once
#include "3rdparty/detours/include/detours.h"

/* macros */

#define JMP_OP_CODE 0xEB
#define JNZ_OP_CODE 0x75

#define DETOUR_BEGIN() \
do \
{ \
	DetourTransactionBegin(); \
	DetourUpdateThread(GetCurrentThread()); \
} while (0)

#define DETOUR_ATTACH(_ptr_func, _address, _target_ptr) \
do \
{ \
	(_ptr_func) = (_address); \
	DetourAttach(&(PVOID&)(_ptr_func), _target_ptr); \
} while (0)

#define DETOUR_COMMIT() \
	DetourTransactionCommit();

/* prototypes */

void *DetourFunc(BYTE *src, const BYTE *dst, const unsigned int len);
void RetourFunc(BYTE *src, BYTE *restore, const unsigned int len);
void *DetourClassFunc(BYTE *src, const BYTE *dst, const unsigned int len);
void RetourClassFunc(BYTE *src, BYTE *restore, const unsigned int len);

void Codecave(uintptr_t destAddress, VOID(*func)(VOID), BYTE nopCount);
void WriteBytes(uintptr_t destAddress, LPVOID bytesToWrite, const unsigned int numBytes);
void PatchCall(uintptr_t call_addr, uintptr_t new_function_ptr);
void WritePointer(uintptr_t offset, const void *ptr);
void PatchWinAPICall(uintptr_t call_addr, uintptr_t new_function_ptr);
void NopFill(uintptr_t address, const unsigned int length);
void ReadBytesProtected(uintptr_t address, BYTE* buf, BYTE count);

inline void PatchCall(uintptr_t call_addr, void *new_function_ptr)
{
	PatchCall(call_addr, (uintptr_t)(new_function_ptr));
}

inline void PatchCall(void *call_addr, void *new_function_ptr)
{
	PatchCall((uintptr_t)call_addr, (uintptr_t)new_function_ptr);
}

inline void PatchWinAPICall(uintptr_t call_addr, void *new_function_ptr)
{
	PatchWinAPICall(call_addr, (uintptr_t)(new_function_ptr));
}

inline void PatchWinAPICall(void *call_addr, void *new_function_ptr)
{
	PatchWinAPICall((uintptr_t)(call_addr), (uintptr_t)(new_function_ptr));
}

template <typename T>
inline void WriteValue(uintptr_t offset, T data)
{
	WriteBytes(offset, &data, sizeof(T));
}

inline void WriteJmpTo(uintptr_t call_addr, uintptr_t new_function_ptr)
{
	BYTE call_patch[1] = { 0xE9 };
	WriteBytes(call_addr, call_patch, 1);
	PatchCall(call_addr, new_function_ptr);
}

inline void WriteJmpTo(uintptr_t call_addr, void *new_function_ptr)
{
	WriteJmpTo(call_addr, (uintptr_t)(new_function_ptr));
}

inline void WriteJmpTo(void *call_addr, void *new_function_ptr)
{
	WriteJmpTo((uintptr_t)(call_addr), (uintptr_t)(new_function_ptr));
}
