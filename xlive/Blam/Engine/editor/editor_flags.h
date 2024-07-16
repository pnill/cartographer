#pragma once
#include "cseries/cseries_strings.h"


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
ASSERT_STRUCT_SIZE(editor_comment_definition, 304);
