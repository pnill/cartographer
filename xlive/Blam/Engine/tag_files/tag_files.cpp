#include "stdafx.h"
#include "tag_files.h"

bool __cdecl tag_files_open(void)
{
	return INVOKE(0x30D58, 0x24C08, tag_files_open);
}

const char* tag_name_strip_path(const char* name)
{
    ASSERT(name);

    const char* result = strrchr(name, '\\');
    return result ? result + 1 : name;
}
