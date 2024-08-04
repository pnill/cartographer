#include "stdafx.h"
#include "creature_definitions.h"

/**
 * \brief attaches the havok vtables to the classes contained within, and initializes runtime values.
 * \param creature_datum tag datum of the creature
 */
void __cdecl creature_definitions_fixup(datum creature_datum)
{
	INVOKE(0x138985, 0x1345ED, creature_definitions_fixup, creature_datum);
}
