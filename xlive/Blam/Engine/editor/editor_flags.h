#pragma once

#include "Blam/Cache/DataTypes/TagBlock.h"
#include "Blam/Engine/cseries/cseries_strings.h"
#include "Blam/Engine/math/real_math.h"

#define MAXIMUM_EDITOR_COMMENTS 65536

enum e_comment_type : int
{
    comment_type_generic = 0
};

// max count: MAXIMUM_EDITOR_COMMENTS 65536
struct editor_comment_definition
{
    real_point3d position;
    e_comment_type type;
    static_string32 name;
    static_string256 comment;
};
TAG_BLOCK_SIZE_ASSERT(editor_comment_definition, 304);
