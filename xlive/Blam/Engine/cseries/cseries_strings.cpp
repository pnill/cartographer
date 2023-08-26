#include "stdafx.h"
#include "cseries_strings.h"

// TODO reimplement this properly
size_t csstrnlen(char* string, size_t size)
{
	return strnlen(string, size);
}

int csstricmp(const char* s1, const char* s2)
{
	return strcmp(s1, s2);
}