#include "stdafx.h"
#include "unicode.h"

/* public code */

int32 ustrcmp(const wchar_t* string1, const wchar_t* string2)
{
	ASSERT(string1);
	ASSERT(string2);
	return wcscmp(string1, string2);
}

size_t ustrlen(const wchar_t* string)
{
	ASSERT(string != NULL);
	return wcslen(string);
}

size_t ustrnlen(const wchar_t* string, size_t count)
{
	ASSERT(string != NULL);
	size_t result = 0;
	for (; result < count; ++result)
	{
		if (!string[result])
			break;
	}
	return result;
}

int32 ustrcoll(const wchar_t* string1, const wchar_t* string2)
{
	ASSERT(string1 != NULL);
	ASSERT(string2 != NULL);
	return wcscoll(string1, string2);
}

size_t ustrcspn(const wchar_t* string, const wchar_t* character_set)
{
	ASSERT(string != NULL);
	ASSERT(character_set != NULL);
	return wcscspn(string, character_set);
}

wchar_t* ustrncat(wchar_t* dest, const wchar_t* src, size_t count)
{
	ASSERT(dest != NULL);
	ASSERT(src != NULL);

	// Originally used the unsafe version in h2v, adjusted to use the safe version
	const errno_t error = wcsncat_s(dest, count, src, _TRUNCATE);
	return error == 0 ? dest : NULL;
}

int32 ustrncmp(const wchar_t* string1, const wchar_t* string2, size_t count)
{
	ASSERT(string1 != NULL);
	ASSERT(string2 != NULL);
	return wcsncmp(string1, string2, count);
}

wchar_t* ustrncpy(wchar_t* dest, const wchar_t* src, size_t count)
{
	ASSERT(dest != NULL);
	ASSERT(src != NULL);
	ASSERT(count > 0);
	wcsncpy_s(dest, count, src, UINT_MAX);
	return dest;
}

const wchar_t* ustrpbrk(const wchar_t* string, const wchar_t* character_set)
{
	ASSERT(string != NULL);
	ASSERT(character_set != NULL);
	return wcspbrk(string, character_set);
}

const wchar_t* ustrrchr(const wchar_t* string, wchar_t c)
{
	ASSERT(string != NULL);
	return wcsrchr(string, c);
}

size_t ustrspn(const wchar_t* string, const wchar_t* character_set)
{
	ASSERT(string != NULL);
	ASSERT(character_set != NULL);
	return wcsspn(string, character_set);
}

const wchar_t* ustrstr(const wchar_t* string, const wchar_t* character_set)
{
	ASSERT(string != NULL);
	ASSERT(character_set != NULL);
	return wcsstr(string, character_set);
}

size_t ustrxfrm(wchar_t* dest, wchar_t* src, size_t count)
{
	ASSERT(dest != NULL);
	ASSERT(src != NULL);
	return wcsxfrm(dest, src, count);
}

wchar_t* ustrnlwr(wchar_t* string, int32 count)
{
	ASSERT(string != NULL);

	for (int32 i = 0; i < count && string[i] != '\0'; ++i)
	{
		string[i] = utolower(string[i]);
	}
	return string;
}

wchar_t* ustrnupr(wchar_t* string, int32 count)
{
	ASSERT(string != NULL);

	for (int32 i = 0; i < count && string[i] != '\0'; ++i)
	{
		string[i] = utoupper(string[i]);
	}
	return string;
}

int32 ustricmp(const wchar_t* string1, const wchar_t* string2)
{
	ASSERT(string1 != NULL);
	ASSERT(string2 != NULL);
	return _wcsicmp(string1, string2);
}

int32 ustrnicmp(const wchar_t* string1, const wchar_t* string2, size_t count)
{
	ASSERT(string1 != NULL);
	ASSERT(string2 != NULL);
	return _wcsnicmp(string1, string2, count);
}

int32 uisalpha(wchar_t c)
{
	return iswalpha(c);
}

int32 uisupper(wchar_t c)
{
	return iswupper(c);
}

int32 uislower(wchar_t c)
{
	return iswlower(c);
}

int32 uiswdigit(wchar_t c)
{
	return iswdigit(c);
}

int32 uisxdigit(wchar_t c)
{
	return iswxdigit(c);
}

int32 uisspace(wchar_t c)
{
	return iswspace(c);
}

int32 uispunct(wchar_t c)
{
	return iswpunct(c);
}

int32 uisalnum(wchar_t c)
{
	return iswalnum(c);
}

int32 uisprint(wchar_t c)
{
	return iswprint(c);
}

int32 uisgraph(wchar_t c)
{
	return iswgraph(c);
}

int32 uiscntrl(wchar_t c)
{
	return iswcntrl(c);
}

wchar_t utoupper(wchar_t c)
{
	return towupper(c);
}

wchar_t utolower(wchar_t c)
{
	return towlower(c);
}

wchar_t ufgetc(_iobuf* stream)
{
	ASSERT(stream != NULL);
	return fgetwc(stream);
}

wchar_t ufputc(wchar_t c, _iobuf* stream)
{
	ASSERT(stream != NULL);
	return fputwc(c, stream);
}

wchar_t uungetc(wchar_t c, _iobuf* stream)
{
	ASSERT(stream != NULL);
	return ungetwc(c, stream);
}

wchar_t* ufgets(wchar_t* string, int32 size, _iobuf* stream)
{
	ASSERT(string != NULL);
	return fgetws(string, size, stream);
}

int32 ufputs(const wchar_t* string, _iobuf* stream)
{
	ASSERT(string != NULL);
	return fputws(string, stream);
}

wchar_t* ugets(wchar_t* string, size_t count)
{
	ASSERT(string != NULL);
	return _getws_s(string, count);
}

int32 uputs(const wchar_t* string)
{
	ASSERT(string != NULL);
	return _putws(string);
}

int32 ufprintf(_iobuf* stream, const wchar_t* format, ...)
{
	va_list va_args;
	va_start(va_args, format);
	ASSERT(stream != NULL);
	ASSERT(format != NULL);
	const int32 result = vfwprintf(stream, format, va_args);
	va_end(va_args);
	return result;
}

int32 uprintf(const wchar_t* format, ...)
{
	va_list va_args;
	va_start(va_args, format);
	ASSERT(format != NULL);
	const int32 result = vwprintf(format, va_args);
	va_end(va_args);
	return result;
}

int32 usnprintf(wchar_t* string, size_t size, const wchar_t* format, ...)
{
	va_list va_args;
	va_start(va_args, format);

	ASSERT(string != NULL);
	ASSERT(size > 0);

	int32 result = _vsnwprintf_s(string, size - 1, UINT_MAX, format, va_args);
	va_end(va_args);
	return result;
}

int32 uvfprintf(_iobuf* stream, const wchar_t* format, char* ap)
{
	ASSERT(stream != NULL);
	ASSERT(format != NULL);
	return vfwprintf(stream, format, ap);
}

int32 uvprintf(const wchar_t* format, char* ap)
{
	ASSERT(format != NULL);
	return vwprintf(format, ap);
}

int32 uvsnprintf(wchar_t* string, size_t size, const wchar_t* format, char* ap)
{
	ASSERT(string && format);
	ASSERT(size > 0);

	return _vsnwprintf_s(string, size - 1, UINT_MAX, format, ap);
}

_iobuf* ufdopen(int32 fd, const wchar_t* path)
{
	ASSERT(path != NULL);
	return _wfdopen(fd, path);
}

_iobuf* ufopen(const wchar_t* path, const wchar_t* mode)
{
	ASSERT(path != NULL);
	ASSERT(mode != NULL);

	_iobuf* result;
	const errno_t error = _wfopen_s(&result, path, mode);
	return error == 0 ? result : NULL;
}

int32 ufclose(_iobuf* stream)
{
	ASSERT(stream != NULL);
	return fclose(stream);
}

_iobuf* ufreopen(const wchar_t* path, const wchar_t* mode, _iobuf* stream)
{
	ASSERT(path != NULL);
	ASSERT(mode != NULL);

	_iobuf* result;
	const errno_t error = _wfreopen_s(&result, path, mode, stream);
	return error == 0 ? result : NULL;
}

void uperror(const wchar_t* string)
{
	ASSERT(string != NULL);
	_wperror(string);
	return;
}

_iobuf* upopen(const wchar_t* command, const wchar_t* mode)
{
	ASSERT(command != NULL);
	ASSERT(mode != NULL);
	return _wpopen(command, mode);
}

int32 uremove(const wchar_t* path)
{
	ASSERT(path != NULL);
	return _wremove(path);
}

bool utmpnam(wchar_t* Buffer, size_t count)
{
	return _wtmpnam_s(Buffer, count) == 0;
}

int32 ustrtol(const wchar_t* nptr, wchar_t** endptr, int32 base)
{
	ASSERT(nptr != NULL);
	return wcstol(nptr, endptr, base);
}

int32 ustrtoul(const wchar_t* nptr, wchar_t** endptr, int32 base)
{
	ASSERT(nptr != NULL);
	return wcstoul(nptr, endptr, base);
}

real64 ustrtod(const wchar_t* nptr, wchar_t** endptr)
{
	ASSERT(nptr != NULL);
	return wcstod(nptr, endptr);
}

int32 utol(const wchar_t* string)
{
	ASSERT(string != NULL);
	return _wtol(string);
}

void wchar_string_to_ascii_string(const wchar_t* src, char* dst, int32 count)
{
	ASSERT(((void*)src) != ((void*)dst));
	for (int32 i = 0; i < count; ++i)
	{
		const wchar_t wide_character = src[i];
		// Final character
		if (i + 1 == count)
		{
			dst[i] = '\0';
		}
		// Characters that aren't ascii
		else if (wide_character > 127)
		{
			dst[i] = '?';
		}
		// Wide characters that are valid ascii characters
		else
		{
			dst[i] = (char)wide_character;
		}
	}
	return;
}

void __cdecl ascii_string_to_wchar_string(const char* src, wchar_t* dst, int32 count)
{
	INVOKE(0x4BF9E, 0x3137D, ascii_string_to_wchar_string, src, dst, count);
	return;
}

void __cdecl utf8_string_to_wchar_string(const utf8* src, wchar_t* dst, int32 count)
{
	INVOKE(0x4C801, 0x31BE1, utf8_string_to_wchar_string, src, dst, count);
	return;
}

void __cdecl wchar_string_to_utf8_string(const wchar_t* src, utf8* dst, int32 count)
{
	INVOKE(0x4C9F7, 0x31DD7, wchar_string_to_utf8_string, src, dst, count);
	return;
}

size_t utf8_string_length(const utf8* src, size_t size)
{
	return MultiByteToWideChar(CP_UTF8, 0, src, size, NULL, 0);
}

bool ugetenv(wchar_t* buffer, size_t count, const wchar_t* var_name)
{
	ASSERT(var_name);
	size_t required_count;
	const errno_t err = _wgetenv_s(&required_count, buffer, count, var_name);

	if (required_count > count)
	{
		error(_error_log, "%s: increase the number of elements passed to the buffer. Passed: %d | Required: %d", __FUNCTION__, count, required_count);
	}


	return err == ERROR_SUCCESS;
}

bool __cdecl uniswcntrl(wchar_t* string)
{
	ASSERT(string);

	return INVOKE(0x4C792, 0x31B72, uniswcntrl, string);
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

bool __cdecl validate_wchar_characters(wchar_t* string)
{
	return INVOKE(0x4C706, 0x31AE6, validate_wchar_characters, string);
}

bool wchar_has_double_pipe(wchar_t* string)
{
	return INVOKE(0x4BF26, 0, wchar_has_double_pipe, string);
}