#pragma once



// max count: UNSIGNED_SHORT_MAX 65535
struct sector_vertex
{
	real_point3d point;
};
TAG_BLOCK_SIZE_ASSERT(sector_vertex, 12);
