#pragma once
#define WIN32_LEAN_AND_MEAN
#define JMP_OP_CODE 0xEB
#define JNZ_OP_CODE 0x75

void *DetourFunc(BYTE *src, const BYTE *dst, const unsigned int len);
void RetourFunc(BYTE *src, BYTE *restore, const unsigned int len);
void *DetourClassFunc(BYTE *src, const BYTE *dst, const unsigned int len);
void RetourClassFunc(BYTE *src, BYTE *restore, const unsigned int len);

void *VTableFunction(void *ClassPtr, DWORD index);
void Codecave(DWORD destAddress, VOID(*func)(VOID), BYTE nopCount);
void WriteBytes(DWORD destAddress, LPVOID bytesToWrite, const unsigned int numBytes);
void PatchCall(DWORD call_addr, DWORD new_function_ptr);
void WritePointer(DWORD offset, const void *ptr);
void PatchWinAPICall(DWORD call_addr, DWORD new_function_ptr);
void NopFill(DWORD address, const unsigned int length);
void ReadBytesProtected(DWORD address, BYTE* buf, BYTE count);

inline void PatchCall(DWORD call_addr, void *new_function_ptr)
{
	PatchCall(call_addr, reinterpret_cast<DWORD>(new_function_ptr));
}

inline void PatchCall(void *call_addr, void *new_function_ptr)
{
	PatchCall(reinterpret_cast<DWORD>(call_addr), reinterpret_cast<DWORD>(new_function_ptr));
}

inline void PatchWinAPICall(DWORD call_addr, void *new_function_ptr)
{
	PatchWinAPICall(call_addr, reinterpret_cast<DWORD>(new_function_ptr));
}

inline void PatchWinAPICall(void *call_addr, void *new_function_ptr)
{
	PatchWinAPICall(reinterpret_cast<DWORD>(call_addr), reinterpret_cast<DWORD>(new_function_ptr));
}

template <typename T>
inline void WriteValue(DWORD offset, T data)
{
	WriteBytes(offset, &data, sizeof(T));
}

inline void WriteJmpTo(DWORD call_addr, DWORD new_function_ptr)
{
	BYTE call_patch[1] = { 0xE9 };
	WriteBytes(call_addr, call_patch, 1);
	PatchCall(call_addr, new_function_ptr);
}

inline void WriteJmpTo(DWORD call_addr, void *new_function_ptr)
{
	WriteJmpTo(call_addr, reinterpret_cast<DWORD>(new_function_ptr));
}

inline void WriteJmpTo(void *call_addr, void *new_function_ptr)
{
	WriteJmpTo(reinterpret_cast<DWORD>(call_addr), reinterpret_cast<DWORD>(new_function_ptr));
}
