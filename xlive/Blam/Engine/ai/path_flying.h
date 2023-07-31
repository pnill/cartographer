#pragma once
#include "Blam/Common/Common.h"

#define k_max_flight_references_per_area 10

// max count: k_max_flight_references_per_area 10
struct flight_hint_reference
{
    short flight_hint_index;
    short point_index;
};
TAG_BLOCK_SIZE_ASSERT(flight_hint_reference, 4);
