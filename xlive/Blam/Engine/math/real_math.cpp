#include "stdafx.h"
#include "real_math.h"

#include "Blam/Engine/structures/structures.h"

float plane3d_distance_to_point(const real_plane3d* plane, const real_point3d* point)
{
    return ((((point->x * plane->normal.i) + (point->y * plane->normal.j)) + (point->z * plane->normal.k)) - plane->distance);
}

__int16 projection_from_vector3d(const real_vector3d* vector)
{
    real_vector3d abs_vector;

    abs_vector.i = fabs(vector->i);
    abs_vector.j = fabs(vector->j);
    abs_vector.k = fabs(vector->k);
    if (abs_vector.k >= abs_vector.j)
    {
        if (abs_vector.k >= abs_vector.i)
        {
            return 2;
        }
    }
    return abs_vector.j >= abs_vector.i;
}

bool projection_sign_from_vector3d(const real_vector3d* vector, short projection)
{
    return *(&vector->i + projection) > 0.0;
}

void project_point3d(const real_point3d* point, const short projection, const bool a3, real_point2d* out)
{
    int index = 3 * (a3 + 2 * projection);
    out->x = *(&point->x + global_projection3d_mappings[index]);
    out->y = *(&point->x + global_projection3d_mappings[index + 1]);
}

float magnitude_squared2d(const real_point2d* point)
{
    return ((point->x * point->x) + (point->y * point->y));
}

float cross_product2d(const real_vector2d *p1, const real_vector2d* p2)
{
    return ((p1->i * p2->j) - (p1->j * p2->i));
}

void vector_from_points2d(const real_point2d* initial_point, const real_point2d* new_point, real_vector2d* vector)
{
    vector->i = new_point->x - initial_point->x;
    vector->j = new_point->y - initial_point->y;
}