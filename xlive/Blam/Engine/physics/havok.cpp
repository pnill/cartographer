#include "stdafx.h"
#include "havok.h"

bool __cdecl havok_can_allocate_space_for_instance_of_object_definition(datum tag_index)
{
    return INVOKE(0x9FE55, 0x920B5, havok_can_allocate_space_for_instance_of_object_definition, tag_index);
}
