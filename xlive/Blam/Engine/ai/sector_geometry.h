#pragma once



// max count: UNSIGNED_SHORT_MAX 65535
struct sector_vertex
{
	real_point3d point;
};
ASSERT_STRUCT_SIZE(sector_vertex, 12);
