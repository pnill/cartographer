#pragma once

void set_hud_size(const float size);
void set_crosshair_and_text_size(const float size);
void set_crosshair_size(float size, bool map_load_context = false);
void set_crosshair_offset(const float offset);
void hud_patches_on_map_load();
void hud_apply_patches();
