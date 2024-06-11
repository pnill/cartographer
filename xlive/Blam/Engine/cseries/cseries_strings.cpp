#include "stdafx.h"
#include "cseries_strings.h"

size_t csstrnlen(const char* s, size_t size)
{
	ASSERT(s);
	ASSERT(size > 0 && size <= MAXIMUM_STRING_SIZE);
	return strnlen(s, size);
}

char* csstrnzcpy(char* s1, const char* s2, size_t size)
{
	ASSERT(s1 && s2);
	ASSERT(size > 0 && size <= MAXIMUM_STRING_SIZE);
	return strncpy(s1, s2, size);
}

char* csstrnzcat(char* s1, char const* s2, size_t size)
{
	ASSERT(s1 && s2);
	ASSERT(size > 0 && size <= MAXIMUM_STRING_SIZE);
	return strncat(s1, s2, size);
}

int csstricmp(const char* s1, const char* s2)
{
	ASSERT(s1 && s2);
	return strcmp(s1, s2);
}

int32 cvsnzprintf(char* buffer, size_t size, char const* format, ...)
{
	ASSERT(buffer);
	ASSERT(format);
	ASSERT(size > 0);

	va_list va_args;
	va_start(va_args, format);
	int32 result = vsnprintf(buffer, size, format, va_args);
	buffer[size - 1] = '\0';
	
	va_end(va_args);
	return result;
}