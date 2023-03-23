#include "stdafx.h"
#include "convex_hulls.h"

bool convex_hull2d_test_circle(short verticies_size, real_point2d* p2d_array, real_point2d* p2d, float radius)
{
    int index;
    float magnitude;
    float product;

    real_vector2d out;
    real_vector2d out2;

    if (verticies_size <= 0)
    {
        return true;
    }

    for (short i = 0; i < verticies_size; ++i)
    {
        index = i + 1 >= verticies_size ? 0 : i + 1;

        vector_from_points2d(&p2d_array[i], &p2d_array[index], &out);
        vector_from_points2d(&p2d_array[i], p2d, &out2);
        magnitude = magnitude_squared2d(&p2d_array[index]);
        if (magnitude != 0.0f)
        {
            product = cross_product2d(&out2, &out);
            if (product > 0.0f && (product * product) > (radius * radius) * magnitude)
            {
                return false;
            }
        }
    }
    return true;
}
