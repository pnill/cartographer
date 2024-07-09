#include "stdafx.h"
#include "physics_model_definitions.h"

/**
 * \brief attaches the havok vtables to the classes contained within, and initializes runtime values.
 * \param phsyics_model_datum tag datum of the collision model
 * \param unk_havok_flag unknown havok flag that changes how the objects are initialized
 */
void __cdecl physics_model_definitions_fixup(datum phsyics_model_datum, bool unk_havok_flag)
{
	INVOKE(0x7B844, 0, physics_model_definitions_fixup, phsyics_model_datum, unk_havok_flag);
}
