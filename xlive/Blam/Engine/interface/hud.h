#pragma once

void set_hud_size(float size);
void set_crosshair_and_text_size(float size);
float* get_crosshair_and_text_size();
void set_crosshair_size(float size);
void set_crosshair_offset(float offset);
void hud_patches_on_map_load();
void hud_apply_patches();
void hud_apply_pre_winmain_patches();
