#pragma once

/* constants */

#define k_render_visible_section_model_group_count 31
#define k_render_visible_section_model_group_none 32

/* structures */
struct s_model_group_submit_data
{
	uint8 count;
	uint8 model_group_index[k_render_visible_section_model_group_count];
};
ASSERT_STRUCT_SIZE(s_model_group_submit_data, 32);

/* public code */

void render_submit_apply_patches(void);
