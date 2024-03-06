#include "stdafx.h"
#include "cseries.h"

bool g_catch_exceptions = true;

void display_assert(char const* condition, char const* file, int32 line, bool assertion_failed)
{
	return;
}

void* csmemset(void* dst, int32 val, size_t size)
{
	return memset(dst, val, size);
}

void* csmemcpy(void* dst, const void* src, size_t size)
{
	return memcpy(dst, src, size);
}