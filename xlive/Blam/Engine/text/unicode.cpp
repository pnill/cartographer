#include "stdafx.h"
#include "unicode.h"

// TODO reimplement this properly
size_t ustrnlen(wchar_t* string, size_t size)
{
	return wcsnlen(string, size);
}
