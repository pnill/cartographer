#include "stdafx.h"
#include "object_types.h"

object_type_definition** get_object_type_definitions()
{
	return Memory::GetAddress<object_type_definition**>(0x41F560, 0x3C2A70);
}

object_type_definition* object_type_definition_get(e_object_type object_type)
{
	return get_object_type_definitions()[object_type];
}
