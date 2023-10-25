#pragma once
#include "Blam/Engine/tag_files/string_id.h"
#include "Blam/Math/real_math.h"

#define MAXIMUM_REGIONS_PER_MODEL 16
#define MAXIMUM_NODES_PER_MODEL 255

// max count: MAXIMUM_NODES_PER_MODEL
struct s_model_node
{
    string_id name;
    int16 parent_node;          // Block index: model_node
    int16 first_child_node;     // Block index: model_node
    int16 next_sibling_node;    // Block index: model_node
    int16 pad;

    real_point3d default_translation;
    real_quaternion default_rotation;
    real_matrix4x3 default_inverse_matrix;
};
CHECK_STRUCT_SIZE(s_model_node, 92);
