#pragma once
#include "Blam/Engine/math/real_math.h"
#include "Blam/Engine/tag_files/string_id.h"

#define MAXIMUM_POSTURES_PER_UNIT 20

// max count: MAXIMUM_POSTURES_PER_UNIT 20
struct s_posture_definition
{
    string_id name;
    real_vector3d pill_offset;
};
TAG_BLOCK_SIZE_ASSERT(s_posture_definition, 16);

