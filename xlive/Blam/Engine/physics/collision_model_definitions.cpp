#include "stdafx.h"
#include "collision_model_definitions.h"

/**
 * \brief attaches the havok vtables to the classes contained within, and initializes runtime values.
 * \param collision_model_datum tag datum of the collision model
 */
void __cdecl collision_model_definitions_fixup(datum collision_model_datum)
{
	INVOKE(0x7BE5C, 0x858BC, collision_model_definitions_fixup, collision_model_datum);
}
