#pragma once
#include "math/color_math.h"
#include "memory/static_arrays.h"
#include "tag_files/data_reference.h"
#include "tag_files/tag_block.h"

/* constants */

#define k_number_of_pixel_shader_fragments k_kilo
#define k_number_of_pixel_shader_combiners k_kilo
#define k_number_of_pixel_shader_permutations k_kilo
#define k_number_of_pixel_shader_constants k_kilo

/* enums */

enum e_ps_component_mask : int8
{
    _ps_component_mask__xvalue = 0,
    _ps_component_mask_y_value = 1,
    _ps_component_mask_z_value = 2,
    _ps_component_mask_w_value = 3,
    _ps_component_mask_xyzrgb_color = 4
};

enum e_ps_parameter_type : int8
{
    _ps_parameter_bitmap = 0,
    _ps_parameter_value = 1,
    _ps_parameter_color = 2,
    _ps_parameter_switch = 3
};

/* structures */

// max count: 32
struct s_pixel_shader_constant_defaults
{
    int32 defaults;
};
ASSERT_STRUCT_SIZE(s_pixel_shader_constant_defaults, 4)

// max count: 100
struct s_pixel_shader
{
    void* pixel_shader_handle_runtime[3];
    tag_block<s_pixel_shader_constant_defaults> constant_register_defaults;

    /****************************************
    * definition_name: shader_postprocess_compiled_pixel_shader_data
    * flags: 0
    * alignment_bit: 0
    ****************************************/
    // DataSize(32768)
    data_reference compiled_shader[3];
};
ASSERT_STRUCT_SIZE(s_pixel_shader, 44)

// max count: 6
struct s_pixel_shader_extern_map
{
    uint8 switch_parameter;
    uint8 case_scalar;
};
ASSERT_STRUCT_SIZE(s_pixel_shader_extern_map, 2)

// max count: 1
struct s_pixel_shader_index
{
    uint8 pixel_shader_index;
};
ASSERT_STRUCT_SIZE(s_pixel_shader_index, 1)

#pragma pack(push,1)
// max count: k_number_of_pixel_shader_fragments
struct s_pixel_shader_fragment
{
    uint8 switch_parameter_index;
    s_tag_block_index permutations_index;
};
ASSERT_STRUCT_SIZE(s_pixel_shader_fragment, 3)
#pragma pack(pop)

// max count: k_kilo
struct s_pixel_shader_permutation_new
{
    int16 enum_index;
    uint16 flags;
    s_tag_block_index combiners;
};
ASSERT_STRUCT_SIZE(s_pixel_shader_permutation_new, 6)

// max count: k_number_of_pixel_shader_combiners
struct s_pixel_shader_combiner
{
    int8 pad[16];
    pixel32 constant_color_0;
    pixel32 constant_color_1;
    uint8 color_a_register_ptr_index;
    uint8 color_b_register_ptr_index;
    uint8 color_c_register_ptr_index;
    uint8 color_d_register_ptr_index;
    uint8 alpha_a_register_ptr_index;
    uint8 alpha_b_register_ptr_index;
    uint8 alpha_c_register_ptr_index;
    uint8 alpha_d_register_ptr_index;
};
ASSERT_STRUCT_SIZE(s_pixel_shader_combiner, 32)

enum e_pixel_shader_permutation_flags : uint16
{
    _pixel_shader_permutation_has_final_combiner_bit = 0,
    k_pixel_shader_permutation_flag_count
};

// max count: k_number_of_pixel_shader_permutations
struct s_pixel_shader_permutation
{
    int16 enum_index;
    c_flags_no_init<e_pixel_shader_permutation_flags, uint16, k_pixel_shader_permutation_flag_count> flags;
    s_tag_block_index constants;
    s_tag_block_index combiners;
    int8 pad[8];
};
ASSERT_STRUCT_SIZE(s_pixel_shader_combiner, 32)

// max count: k_number_of_pixel_shader_constants
struct s_pixel_shader_constant
{
    e_ps_parameter_type parameter_type;
    uint8 combiner_index;
    uint8 register_index;
    e_ps_component_mask component_mask;
    int16 pad;
};
ASSERT_STRUCT_SIZE(s_pixel_shader_constant, 6)
