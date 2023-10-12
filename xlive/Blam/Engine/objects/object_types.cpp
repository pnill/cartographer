#include "stdafx.h"
#include "object_types.h"

#include "object_placement.h"


object_type_definition** get_object_type_definitions(void)
{
	return Memory::GetAddress<object_type_definition**>(0x41F560, 0x3C2A70);
}

object_type_definition* object_type_definition_get(e_object_type object_type)
{
	return get_object_type_definitions()[object_type];
}

void __cdecl object_type_adjust_placement(object_placement_data* placement_data)
{
	INVOKE(0x1863EA, 0x170486, object_type_adjust_placement, placement_data);
	return;
}