#pragma once
#include "tag_files/tag_groups.h"

struct tag_reference
{
	tag_group group;
	datum index;
};
ASSERT_STRUCT_SIZE(tag_reference, 8);