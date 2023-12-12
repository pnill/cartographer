#pragma once
#include "Blam/Cache/DataTypes/BlamDataTypes.h"

#define k_maximum_effect_locations_count 32

// max count: k_maximum_effect_locations_count 32
struct effect_location_definition
{
    /*
    Explaination("MARKER NAMES", "In addition to the marker in the render model there are several special marker names:
    replace
    Replace allows you to use the same effect with different markers. Damage transition effects support this for example.

    gravity, up
    The direction of gravity (down) and the opposite direction (up).  Always supplied

    normal
    Vector pointing directly away from the surface you collided with. Supplied for effects from collision.

    forward
    The 'negative incident' vector i.e. the direction the object is moving in. Most commonly used to generated decals. Supplied for effects from collision.

    backward
    The 'incident' vector i.e. the opposite of the direction the object is moving in. Supplied for effects from collision.

    reflection
    The way the effect would reflect off the surface it hit. Supplied for effects from collision.

    root
    The object root (pivot). These can used for all effects which are associated with an object.

    impact
    The location of a havok impact.
     */
    string_id marker_name;
};
TAG_BLOCK_SIZE_ASSERT(effect_location_definition, 0x4);
