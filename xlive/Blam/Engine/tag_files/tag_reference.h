#pragma once
#include "tag_files/tag_groups.h"

struct tag_reference
{
	tag_group TagGroup;
	datum TagIndex;
};
CHECK_STRUCT_SIZE(tag_reference, 8);