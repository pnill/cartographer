#pragma once

void *DetourFunc(BYTE *src, const BYTE *dst, const int len);
void RetourFunc(BYTE *src, BYTE *restore, const int len);
void *DetourClassFunc(BYTE *src, const BYTE *dst, const int len);
void RetourClassFunc(BYTE *src, BYTE *restore, const int len);

void *VTableFunction(void *ClassPtr, DWORD index);
void Codecave(DWORD destAddress, VOID(*func)(VOID), BYTE nopCount);
void WriteBytes(DWORD destAddress, LPVOID bytesToWrite, int numBytes);
void PatchCall(DWORD call_addr, DWORD new_function_ptr);
void WritePointer(DWORD offset, void *ptr);
void PatchWinAPICall(DWORD call_addr, DWORD new_function_ptr);

inline void PatchCall(DWORD call_addr, void *new_function_ptr)
{
	PatchCall(call_addr, reinterpret_cast<DWORD>(new_function_ptr));
}

inline void PatchWinAPICall(DWORD call_addr, void * new_function_ptr)
{
	PatchWinAPICall(call_addr, reinterpret_cast<DWORD>(new_function_ptr));
}

inline void WritePointer(DWORD offset, const void *ptr) {
	WritePointer(offset, const_cast<void*>(ptr));
}

template <typename value_type>
inline void WriteValue(DWORD offset, value_type data)
{
	WriteBytes(offset, &data, sizeof(data));
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
