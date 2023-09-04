#include "stdafx.h"
#include "unicode.h"

size_t ustrnlen(const wchar_t* string, size_t max_count)
{
	return wcsnlen(string, max_count);
}

wchar_t* ustrncpy(wchar_t* dst, const wchar_t* src, size_t count)
{
	return wcsncpy(dst, src, count);
}

wchar_t* ustrnzcpy(wchar_t* dst, const wchar_t* src, size_t count)
{
	wchar_t* result = ustrncpy(dst, src, count - 1);
	dst[count - 1] = 0;
	return result;
}

wchar_t* ustrnzcat(wchar_t* dst, const wchar_t* src, size_t count)
{
	return wcsncat(dst, src, count);
}
