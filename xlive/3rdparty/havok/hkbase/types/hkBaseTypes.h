#pragma once

#define HK_NULL 0

#define HK_CALL __cdecl
#define HK_FORCE_INLINE __forceinline
#define HK_ALIGNED_VARIABLE(NAME, ALIGNMENT) __declspec(align(ALIGNMENT)) NAME
