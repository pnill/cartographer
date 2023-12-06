#include "stdafx.h"
#include "cseries.h"

void* csmemset(void* dst, int32 val, size_t size)
{
	return memset(dst, val, size);
}

void* csmemcpy(void* dst, const void* src, size_t size)
{
	return memcpy(dst, src, size);
}