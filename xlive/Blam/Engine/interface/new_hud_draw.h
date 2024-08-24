#pragma once

struct s_draw_hud_bitmap_widget_input_results
{
	int32 result_1;
	int32 result_2;
	int32 result_3;
	int32 result_4;
};


/* public code */

datum hud_bitmap_tag_index_get(void);

int32 hud_bitmap_data_index_get(void);

void __cdecl draw_hud_layer(void);

void new_hud_draw_apply_patches();