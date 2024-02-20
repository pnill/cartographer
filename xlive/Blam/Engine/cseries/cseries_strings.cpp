#include "stdafx.h"
#include "cseries_strings.h"

size_t csstrnlen(const char* string, size_t size)
{
	return strnlen(string, size);
}

char* csstrnzcpy(char* dst, const char* src, size_t size)
{
	return strncpy(dst, src, size);
}

char* csstrnzcat(char* s1, char const* s2, size_t size)
{
	return strncat(s1, s2, size);
}

int csstricmp(const char* s1, const char* s2)
{
	return strcmp(s1, s2);
}