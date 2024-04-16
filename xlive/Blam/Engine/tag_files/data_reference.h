#pragma once

struct data_reference
{
	int32 size;
	int32 data;
};
ASSERT_STRUCT_SIZE(data_reference, 8);