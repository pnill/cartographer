#include "stdafx.h"
#include "object_types.h"
#include "objects.h"

#include "H2MOD/Tags/TagInterface.h"

object_type_definition** get_object_type_definitions()
{
	return Memory::GetAddress<object_type_definition**>(0x41F560, 0x0);
}

object_type_definition* get_game_object_type_definition(datum object_datum)
{
	return get_object_type_definitions()[object_get_fast_unsafe<s_object_data_definition>(object_datum)->object_type];
}
int object_type_from_group_tag(datum tag_index)
{
	int result;

	result = 0;
	while (get_object_type_definitions()[result]->group_tag != tags::get_tag_group_fast(tag_index))
	{
		if (++result >= 13)
			return -1;
	}
	return result;
}
