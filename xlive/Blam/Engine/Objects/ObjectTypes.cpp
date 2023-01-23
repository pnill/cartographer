#include "stdafx.h"
#include "ObjectTypes.h"

#include "Objects.h"

c_object_type_definition** c_object_type_definition::get_object_type_definitions()
{
	return Memory::GetAddress<c_object_type_definition**>(0x41F560, 0x0);
}

c_object_type_definition* c_object_type_definition::get_game_object_type_definition(datum object_datum)
{
	return get_object_type_definitions()[object_get_fast_unsafe<s_object_data_definition>(object_datum)->object_type];
}
