#pragma once

// TODO add asserts in the future
size_t ustrnlen(const wchar_t* string, size_t max_count);

// TODO add asserts in the future
wchar_t* ustrncpy(wchar_t* dst, const wchar_t* src, size_t count);

// TODO add asserts in the future
wchar_t* ustrnzcpy(wchar_t* dst, const wchar_t* src, size_t count);

// TODO add asserts in the future
wchar_t* ustrnzcat(wchar_t* dst, const wchar_t* src, size_t count);
