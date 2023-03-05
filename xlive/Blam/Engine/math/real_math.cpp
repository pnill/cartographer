#include "stdafx.h"
#include "real_math.h"

float plane3d_distance_to_point(const real_plane3d* plane, const real_point3d* point)
{
    return ((((point->x * plane->normal.i) + (point->y * plane->normal.j)) + (point->z * plane->normal.k)) - plane->distance);
}
