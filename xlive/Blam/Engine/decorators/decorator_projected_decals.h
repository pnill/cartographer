#pragma once

#include "Blam/Engine/math/color_math.h"


#define k_maximum_projected_decal_count 32768

// max count: 64*k_kilo 65536
struct rasterizer_vertex_decorator_decal
{
    real_point3d position;
    real_point2d texcoord0;
    real_point2d texcoord1;
    pixel32 color;
};
TAG_BLOCK_SIZE_ASSERT(rasterizer_vertex_decorator_decal, 32);

// max count: k_maximum_projected_decal_count 32768
struct s_decorator_projected_decal
{
    byte decorator_set;         // Block Index: scenario_decorator_set_palette_entry 
    byte decorator_class;
    byte decorator_permutation;
    byte sprite_index;
    real_point3d position;
    real_vector3d left;
    real_vector3d up;
    real_vector3d extents;
    real_point3d previous_position;
};
TAG_BLOCK_SIZE_ASSERT(s_decorator_projected_decal, 64);
