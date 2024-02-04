#pragma once

// This controls the scale for hud when we implement the redrawn hud
// ex. a value of 4.0 would allow us to use hud bitmaps that are 4x larger (resolution wise) than the old ones
#define k_primary_upscale_size 1.0f
#define k_secondary_upscale_size 1.0f
#define k_crosshair_upscale_size 1.0f

void set_primary_hud_scale(float scale);
void set_secondary_hud_scale(float scale);
float* get_ui_scale();
float* get_primary_hud_scale();
float* get_secondary_hud_scale();
void set_crosshair_offset(float offset);
void hud_patches_on_map_load();
void hud_apply_pre_winmain_patches();
