#pragma once

struct tag_reference
{
	tag_group group;
	datum index;
};
ASSERT_STRUCT_SIZE(tag_reference, 8);