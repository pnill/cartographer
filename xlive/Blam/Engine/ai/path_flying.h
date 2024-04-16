#pragma once


#define k_max_flight_references_per_area 10

// max count: k_max_flight_references_per_area 10
struct flight_hint_reference
{
    short flight_hint_index;
    short point_index;
};
ASSERT_STRUCT_SIZE(flight_hint_reference, 4);
