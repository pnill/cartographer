#pragma once

#define k_number_of_vertex_shader_constants k_kilo

struct s_vertex_shader_constant
{
    uint8 register_index;
    uint8 parameter_index;
    uint8 destination_mask;
    uint8 scale_by_texture_stage;
};
ASSERT_STRUCT_SIZE(s_vertex_shader_constant, 4)
