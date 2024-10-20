#pragma once
#include "bitmaps/bitmap_group.h"

/* prototypes */

void rasterizer_main_apply_patches(void);

void rasterizer_present_frame_wrapper(bitmap_data* bitmap);

void rasterizer_present_frame_screenshot_wrapper(bitmap_data* bitmap);
