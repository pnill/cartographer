#include "stdafx.h"
#include "cache_file_verification.h"

HANDLE __cdecl cache_file_verification_initialize(void)
{
	return INVOKE(0x000388d3, 0x0, cache_file_verification_initialize);
}
