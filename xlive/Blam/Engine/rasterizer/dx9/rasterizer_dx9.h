#pragma once
#include "math/real_math.h"

/* classes */

template <size_t size>
class c_rasterizer_constant_4f_cache
{
    real_vector4d values[size];
public:
    bool test_cache(int32 index, real_vector4d* constants, int32 count)
    {
        ASSERT(index < size && index + count <= size);

        bool result = false;

        if (count)
        {
            bool contents_match = false;

            // Loop through values and constants and xor to determine if values are different
            int32 i = 0;
            for (;
                !(
                    (int32)values[index].i ^ (int32)constants[i].i |
                    (int32)values[index].j ^ (int32)constants[i].j |
                    (int32)values[index].k ^ (int32)constants[i].k |
                    (int32)values[index].w ^ (int32)constants[i].w
                    );
                i++)
            {
                if (i >= count)
                {
                    contents_match = true;
                    break;
                }
            }

            // Copy over the rest of the constants if the values don't match
            if (!contents_match)
            {
                if (count != i)
                {
                    for (int32 j = i + index; j < size; j++)
                    {
                        values[j] = constants[j];
                    }

                }
                result = true;
            }
        }

        return result;
    }
};

/* public code */

bool* rasterizer_dx9_disable_stencil_get(void);

c_rasterizer_constant_4f_cache<256>* rasterizer_get_main_vertex_shader_cache(void);

c_rasterizer_constant_4f_cache<32>* rasterizer_get_main_pixel_shader_cache(void);
