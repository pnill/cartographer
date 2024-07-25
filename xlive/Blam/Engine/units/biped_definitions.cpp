#include "stdafx.h"
#include "biped_definitions.h"

/**
 * \brief attaches the havok vtables to the classes contained within, and initializes runtime values.
 * \param biped_datum tag datum of the biped
 */
void __cdecl biped_definitions_fixup(datum biped_datum)
{
	INVOKE(0x1389B0, 0x134618, biped_definitions_fixup, biped_datum);
}
