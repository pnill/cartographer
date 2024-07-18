#pragma once

// TODO add asserts in the future
int32 ustrncmp(const wchar_t* string1, const wchar_t* string2, size_t max_count);

// TODO add asserts in the future
size_t ustrnlen(const wchar_t* string, size_t max_count);

// TODO add asserts in the future
wchar_t* ustrncpy(wchar_t* dst, const wchar_t* src, size_t count);

// TODO add asserts in the future
wchar_t* ustrnzcpy(wchar_t* dst, const wchar_t* src, size_t count);

// TODO add asserts in the future
wchar_t* ustrnzcat(wchar_t* dst, const wchar_t* src, size_t count);

int32 usnzprintf(wchar_t* string, size_t size, const wchar_t* format, ...);

// Converts a wide char string to a utf8 string with the specified length
void wchar_string_to_utf8_string(const wchar_t* src, utf8* dst, int buffer_count);

// Converts a utf8 string to a wide char string with the specified length
void utf8_string_to_wchar_string(const utf8* src, wchar_t* dst, int buffer_count);

// Returns length (in characters) of utf8 string
size_t utf8_string_length(const utf8* src, size_t size);