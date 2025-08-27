#pragma once

/* prototypes */

int32 ustrcmp(const wchar_t* string1, const wchar_t* string2);

size_t ustrlen(const wchar_t* string);

size_t ustrnlen(const wchar_t* string, size_t max_count);

int32 ustrcoll(const wchar_t* string1, const wchar_t* string2);

size_t ustrcspn(const wchar_t* string, const wchar_t* character_set);

wchar_t* ustrncat(wchar_t* dest, const wchar_t* src, size_t count);

int32 ustrncmp(const wchar_t* string1, const wchar_t* string2, size_t max_count);

wchar_t* ustrncpy(wchar_t* dst, const wchar_t* src, size_t count);

const wchar_t* ustrpbrk(const wchar_t* string, const wchar_t* character_set);

const wchar_t* ustrrchr(const wchar_t* string, wchar_t c);

size_t ustrspn(const wchar_t* string, const wchar_t* character_set);

const wchar_t* ustrstr(const wchar_t* string, const wchar_t* character_set);

size_t ustrxfrm(wchar_t* dest, wchar_t* src, size_t count);

wchar_t* ustrnlwr(wchar_t* string, int32 count);

wchar_t* ustrnupr(wchar_t* string, int32 count);

int32 ustricmp(const wchar_t* string1, const wchar_t* string2);

int32 ustrnicmp(const wchar_t* string1, const wchar_t* string2, size_t count);

int32 uisalpha(wchar_t c);

int32 uisupper(wchar_t c);

int32 uislower(wchar_t c);

int32 uiswdigit(wchar_t c);

int32 uisxdigit(wchar_t c);

int32 uisspace(wchar_t c);

int32 uispunct(wchar_t c);

int32 uisalnum(wchar_t c);

int32 uisprint(wchar_t c);

int32 uisgraph(wchar_t c);

int32 uiscntrl(wchar_t c);

wchar_t utoupper(wchar_t c);

wchar_t utolower(wchar_t c);

wchar_t ufgetc(_iobuf* stream);

wchar_t ufputc(wchar_t c, _iobuf* stream);

wchar_t uungetc(wchar_t c, _iobuf* stream);

wchar_t* ufgets(wchar_t* string, int32 size, _iobuf* stream);

int32 ufputs(const wchar_t* string, _iobuf* stream);

wchar_t* ugets(wchar_t* string, size_t count);

int32 uputs(const wchar_t* string);

int32 ufprintf(_iobuf* stream, const wchar_t* format, ...);

int32 uprintf(const wchar_t* format, ...);

int32 usnprintf(wchar_t* string, size_t size, const wchar_t* format, ...);

int32 uvfprintf(_iobuf* stream, const wchar_t* format, char* ap);

int32 uvprintf(const wchar_t* format, char* ap);

int32 uvsnprintf(wchar_t* string, size_t size, const wchar_t* format, char* ap);

_iobuf* ufdopen(int32 fd, const wchar_t* path);

_iobuf* ufopen(const wchar_t* path, const wchar_t* mode);

int32 ufclose(_iobuf* stream);

_iobuf* ufreopen(const wchar_t* path, const wchar_t* mode, _iobuf* stream);

void uperror(const wchar_t* string);

_iobuf* upopen(const wchar_t* command, const wchar_t* mode);

int32 uremove(const wchar_t* path);

bool utmpnam(wchar_t* Buffer, size_t count);

int32 ustrtol(const wchar_t* nptr, wchar_t** endptr, int32 base);

int32 ustrtoul(const wchar_t* nptr, wchar_t** endptr, int32 base);

real64 ustrtod(const wchar_t* nptr, wchar_t** endptr);

int32 utol(const wchar_t* string);

void wchar_string_to_ascii_string(const wchar_t* src, char* dst, int32 count);

void __cdecl ascii_string_to_wchar_string(const char* src, wchar_t* dst, int32 count);

// Converts a utf8 string to a wide char string with the specified length
void __cdecl utf8_string_to_wchar_string(const utf8* src, wchar_t* dst, int32 count);

// Converts a wide char string to a utf8 string with the specified length
void __cdecl wchar_string_to_utf8_string(const wchar_t* src, utf8* dst, int32 count);

// Returns length (in characters) of utf8 string
size_t utf8_string_length(const utf8* src, size_t size);

bool ugetenv(wchar_t* buffer, size_t count, const wchar_t* var_name);

bool __cdecl uniswcntrl(wchar_t* string);

int32 usnzprintf(wchar_t* string, size_t size, const wchar_t* format, ...);

bool __cdecl validate_wchar_characters(wchar_t* string);

bool __cdecl wchar_has_double_pipe(wchar_t* string);