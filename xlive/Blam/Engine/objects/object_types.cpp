#include "stdafx.h"
#include "object_types.h"

#include "Objects.h"

c_object_type_definition** c_object_type_definition::get_object_type_definitions()
{
	return Memory::GetAddress<c_object_type_definition**>(0x41F560, 0x0);
}

c_object_type_definition* c_object_type_definition::get_game_object_type_definition(datum object_datum)
{
	return get_object_type_definitions()[object_get_fast_unsafe<s_object_data_definition>(object_datum)->object_type];
}
c_object_type_definition* c_object_type_definition::get_tag_type_definition(datum tag_datum)
{
	typedef int(__cdecl* get_tag_type_definition_t)(datum tag_datum);
	get_tag_type_definition_t p_get_tag_type_definition = Memory::GetAddress<get_tag_type_definition_t>(0x1863BB, 0);
	auto b = p_get_tag_type_definition(tag_datum);
	auto a =  get_object_type_definitions()[b];
	return a;
}
