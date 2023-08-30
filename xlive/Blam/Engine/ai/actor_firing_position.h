#pragma once
#include "Blam/Cache/DataTypes/BlamPrimitiveType.h"

// Unsure about the size for this struct...
struct firing_position_ref
{
	datum dynamic_firing_set_index;
	datum field_3F8;
	__int16 field_3FC;
	byte gap_3F8[26];
	int current_position_index;
};
