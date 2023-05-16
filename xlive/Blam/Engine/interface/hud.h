#pragma once

void set_primary_hud_scale(float size);
void set_secondary_hud_scale(float size);
float* get_secondary_hud_scale();
void set_crosshair_scale(float size);
void set_crosshair_offset(float offset);
void hud_patches_on_map_load();
void hud_apply_patches();
void hud_apply_pre_winmain_patches();
