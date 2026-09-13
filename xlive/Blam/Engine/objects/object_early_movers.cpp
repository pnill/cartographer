#include "stdafx.h"
#include "object_early_movers.h"

#include "objects.h"
#include "object_definition.h"

#include "cache/cache_files.h"

/* public code */

s_object_early_movers_globals* object_early_movers_globals_get(void)
{
	return *Memory::GetAddress<s_object_early_movers_globals**>(0x4E6654, 0x50E928);
}

void object_early_mover_new(
	datum object_index)
{
	struct object_datum* object = object_get(object_index);
	struct object_definition const* object_definition = object_definition_get(object->definition_index);

	s_object_early_movers_globals* object_early_movers_globals = object_early_movers_globals_get();

	if (object_definition->object.flags.test(_object_definition_early_mover_bit) && object_early_movers_globals->object_index_count < MAXIMUM_OBJECT_EARLY_MOVERS_PER_MAP)
	{
		object->object.physics_flags.set(_object_is_early_mover_bit, true);
		object_early_movers_globals->early_mover_objects[object_early_movers_globals->object_index_count++] = object_index;
	}

	return;
}
