#include "stdafx.h"
#include "unicode.h"

int32 ustrncmp(const wchar_t* string1, const wchar_t* string2, size_t max_count)
{
	return wcsncmp(string1, string2, max_count);
}

size_t ustrnlen(const wchar_t* string, size_t max_count)
{
	return wcsnlen(string, max_count);
}

wchar_t* ustrncpy(wchar_t* dest, const wchar_t* src, size_t count)
{
	ASSERT(dest);
	ASSERT(src);
	ASSERT(count > 0);
	wcsncpy_s(dest, count, src, UINT_MAX);
	return dest;
}

wchar_t* ustrnzcpy(wchar_t* dest, const wchar_t* src, size_t count)
{
	wchar_t* result = ustrncpy(dest, src, count - 1);
	dest[count - 1] = 0;
	return result;
}

wchar_t* ustrnzcat(wchar_t* dst, const wchar_t* src, size_t count)
{
	return wcsncat(dst, src, count);
}

int32 usnzprintf(wchar_t* string, size_t size, const wchar_t* format, ...)
{
	va_list va_args;
	va_start(va_args, format);

	ASSERT(string != NULL);
	ASSERT(size > 0);

	int32 result = _vsnwprintf_s(string, size - 1, UINT_MAX, format, va_args);
	va_end(va_args);
	return result;
}

void wchar_string_to_utf8_string(const wchar_t* src, utf8* dst, int buffer_count)
{
	typedef void (__cdecl* wchar_string_to_utf8_string_t)(const wchar_t*, utf8*, int);
	auto p_wchar_string_to_utf8_string = Memory::GetAddress<wchar_string_to_utf8_string_t>(0x4C9F7, 0x31DD7);
	p_wchar_string_to_utf8_string(src, dst, buffer_count);
	return;
}

void utf8_string_to_wchar_string(const utf8* src, wchar_t* dst, int buffer_count)
{
	typedef void(__cdecl* utf8_string_to_wchar_string_t)(const utf8*, wchar_t*, int);
	auto p_utf8_string_to_wchar_string = Memory::GetAddress<utf8_string_to_wchar_string_t>(0x4C801, 0x31BE1);
	p_utf8_string_to_wchar_string(src, dst, buffer_count);
	return;
}

size_t utf8_string_length(const utf8* src, size_t size)
{
	return MultiByteToWideChar(CP_UTF8, 0, src, size, NULL, 0);
}