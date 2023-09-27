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