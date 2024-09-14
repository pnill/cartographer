#include "stdafx.h"
#include "rasterizer_lens_flares.h"

#include "rasterizer/dx9/rasterizer_dx9_lens_flares.h"
#include "rasterizer/dx9/rasterizer_dx9_targets.h"

/* structures */

struct s_ray_of_buddha_data
{
    datum tag_index;
    real_point3d point;
};
ASSERT_STRUCT_SIZE(s_ray_of_buddha_data, 16);

/* globals */

bool g_hs_rasterizer_ray_of_buddha = true;

/* prototypes */

s_ray_of_buddha_data* global_ray_of_buddha_data_get(void);
uint32* global_sun_count_get(void);

/* public code */

void rasterizer_lens_flares_apply_patches(void)
{
	//rasterizer_near_clip_distance <real>
	//Changed from game default of 0.06 to 0.0601
	real32* rasterizer_near_clip_distance = Memory::GetAddress<real32*>(0x468150, 0x0);
	*rasterizer_near_clip_distance = 0.0601f;

    PatchCall(Memory::GetAddress(0x1913AD), rasterizer_suns_glow_occlude);
	return;
}

// TODO: fix the sun count so we can render more than one sun at once

e_rasterizer_target __cdecl rasterizer_suns_glow_occlude(e_rasterizer_target rasterizer_target)
{
    if (g_hs_rasterizer_ray_of_buddha)
    {
        uint32 g_sun_count = *global_sun_count_get();

        for (uint32 i = 0; i < g_sun_count; i++)
        {
            s_ray_of_buddha_data* data = &global_ray_of_buddha_data_get()[i];
            rasterizer_dx9_sun_glow_occlude(data->tag_index, &data->point, rasterizer_target);
        }
    }

    return rasterizer_target;
}

e_rasterizer_target rasterizer_suns_glow_draw(e_rasterizer_target rasterizer_target)
{
    if (g_hs_rasterizer_ray_of_buddha)
    {
        uint32 g_sun_count = *global_sun_count_get();

        for (uint32 i = 0; i < g_sun_count; i++)
        {
            s_ray_of_buddha_data* data = &global_ray_of_buddha_data_get()[i];
            rasterizer_dx9_sun_glow_draw(data->tag_index, &data->point, rasterizer_target);
        }
    }

    return rasterizer_target;	
}

e_rasterizer_target __cdecl lens_flares_draw(e_rasterizer_target rasterizer_target)
{
    return INVOKE(0x26E5AB, 0x0, lens_flares_draw, rasterizer_target);
}

/* private code */

s_ray_of_buddha_data* global_ray_of_buddha_data_get(void)
{
    return Memory::GetAddress<s_ray_of_buddha_data*>(0xA4B020);
}

uint32* global_sun_count_get(void)
{
    return Memory::GetAddress<uint32*>(0xA4B030);
}
