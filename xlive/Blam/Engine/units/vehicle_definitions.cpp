#include "stdafx.h"
#include "vehicle_definitions.h"

/**
 * \brief attaches the havok vtables to the classes contained within, and initializes runtime values.
 * \param vehicle_datum tag datum of the vehicle
 */
void __cdecl vehicle_definitions_fixup(datum vehicle_datum)
{
	INVOKE(0x13895A, 0x1345C2, vehicle_definitions_fixup, vehicle_datum);
}
