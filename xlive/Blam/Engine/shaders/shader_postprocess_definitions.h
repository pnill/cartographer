#pragma once
#include "pixel_shader.h"
#include "vertex_shader.h"

#include "math/color_math.h"
#include "math/function_definitions.h"

#include "tag_files/string_id.h"
#include "tag_files/tag_block.h"
#include "tag_files/tag_reference.h"

/* constants */

#define k_number_of_render_states k_kilo
#define k_number_of_render_state_parameters k_kilo
#define k_number_of_extern_references k_kilo
#define k_number_of_texture_stage_states k_kilo
#define k_number_of_texture_stage_state_parameters k_kilo
#define k_number_of_textures k_kilo

/* structures */

struct s_shader_postprocess_value_property
{
	real32 value;
};
ASSERT_STRUCT_SIZE(s_shader_postprocess_value_property, 4);

struct s_shader_postprocess_color_property
{
	real_rgb_color color;
};
ASSERT_STRUCT_SIZE(s_shader_postprocess_color_property, 12);

struct s_shader_postprocess_bitmap_property
{
	int16 bitmap_index;
	int16 animated_parameter_index;
};
ASSERT_STRUCT_SIZE(s_shader_postprocess_bitmap_property, 4);

struct s_shader_postprocess_animated_parameter_reference
{
	int8 pad[3];
	int8 parameter_index;
};
ASSERT_STRUCT_SIZE(s_shader_postprocess_animated_parameter_reference, 4);

struct s_shader_postprocess_animated_parameter
{
	int16 overlay_references;
};
ASSERT_STRUCT_SIZE(s_shader_postprocess_animated_parameter, 2);

struct s_shader_postprocess_overlay_reference
{
	int16 overlay_index;
	int16 transform_index;
};

struct s_shader_postprocess_overlay_new
{
	string_id input_name;
	string_id range_name;
	real32 time_period_in_seconds;
	c_function_definition function;
};

struct s_shader_postprocess_implementation_new
{
	int16 bitmap_transforms;
	int16 render_states;
	int16 texture_states;
	int16 pixel_constants;
	int16 vertex_constants;
};

// max count: k_kilo
#pragma pack(push,1)
struct s_shader_postprocess_level_of_detail_new
{
	int32 available_layer_flags;
	int16 layers;
};
ASSERT_STRUCT_SIZE(s_shader_postprocess_level_of_detail_new, 6);
#pragma pack(pop)

// max count: k_kilo
struct s_shader_postprocess_bitmap_new
{
	datum bitmap_group;		// This is the actual index in cache builds? (YES)
	int32 bitmap_index;
	real32 log_bitmap_dimension;
};
ASSERT_STRUCT_SIZE(s_shader_postprocess_bitmap_new, 12);

// This is a class but has the naming of a structure: thanks Bungie
struct s_shader_postprocess_definition_new
{
	datum shader_template_index;
	tag_block<s_shader_postprocess_bitmap_new> bitmaps;
	tag_block<pixel32> pixel_constants;
	tag_block<real_vector4d> vertex_constants;
	tag_block<s_shader_postprocess_level_of_detail_new> levels_of_detail;
	tag_block<int16> layers;
	tag_block<int16> passes;
	tag_block<s_shader_postprocess_implementation_new> implementations;
	tag_block<s_shader_postprocess_overlay_new> overlays;
	tag_block<s_shader_postprocess_overlay_reference> overlay_references;
	tag_block<s_shader_postprocess_animated_parameter> animated_parameters;
	tag_block<s_shader_postprocess_animated_parameter_reference> animated_parameter_references;
	tag_block<s_shader_postprocess_bitmap_property> bitmap_properties;
	tag_block<s_shader_postprocess_color_property> color_properties;
	tag_block<s_shader_postprocess_value_property> value_properties;
	tag_block<s_shader_postprocess_level_of_detail_new> old_levels_of_detail;

	s_shader_postprocess_bitmap_property* bitmap_property_get(int32 index) const
	{
		return (s_shader_postprocess_bitmap_property*)tag_block_get_element_with_size(
			(s_tag_block*)&this->bitmap_properties,
			index,
			sizeof(s_shader_postprocess_bitmap_property));
	}

	s_shader_postprocess_bitmap_new* bitmap_get(int16 index) const
	{
		return (s_shader_postprocess_bitmap_new*)tag_block_get_element_with_size(
			(s_tag_block*)&this->bitmaps,
			index,
			sizeof(s_shader_postprocess_bitmap_new));
	}
};
ASSERT_STRUCT_SIZE(s_shader_postprocess_definition_new, 124)

#pragma pack(push,1)
// max count: k_kilo
struct s_shader_template_postprocess_level_of_detail_new
{
	s_tag_block_index layers;
	int32 available_layers;
	real32 projected_height_percentage;
};
ASSERT_STRUCT_SIZE(s_shader_template_postprocess_level_of_detail_new, 10);

// max count: k_kilo
struct s_shader_template_postprocess_pass_new
{
	tag_reference pass;
	s_tag_block_index implementations;
};
ASSERT_STRUCT_SIZE(s_shader_template_postprocess_pass_new, 10);

// max count: k_kilo
struct s_shader_template_postprocess_implementation_new
{
	s_tag_block_index bitmaps;
	s_tag_block_index pixel_constants;
	s_tag_block_index vertex_constants;
};
ASSERT_STRUCT_SIZE(s_shader_template_postprocess_implementation_new, 6);
#pragma pack(pop)

// max count: k_kilo
struct s_shader_template_postprocess_remapping_new
{
	int16 field_0;
	int8 field_2;
	uint8 source_index;
};
ASSERT_STRUCT_SIZE(s_shader_template_postprocess_remapping_new, 4);

// max count: 1
struct s_shader_template_postprocess_definition_new
{
	tag_block<s_shader_template_postprocess_level_of_detail_new> levels_of_detail;
	tag_block<s_tag_block_index> layers;
	tag_block<s_shader_template_postprocess_pass_new> passes;
	tag_block<s_shader_template_postprocess_implementation_new> implementations;
	tag_block<s_shader_template_postprocess_remapping_new> remappings;
};
ASSERT_STRUCT_SIZE(s_shader_template_postprocess_definition_new, 40);

#pragma pack(push,1)
// max count: k_kilo
struct s_shader_pass_postprocess_implementation_new
{
	s_tag_block_index textures;
	s_tag_block_index render_states;
	s_tag_block_index texture_states;

	int8 pad[240];

	s_tag_block_index ps_fragments;
	s_tag_block_index ps_permutations;
	s_tag_block_index ps_combiners;

	tag_reference vertex_shader;	// vrtx
	int32 unk_0;
	int32 unk_1;
	int32 unk_2;
	int32 unk_3;
	int32 unk_4;
	int32 unk_5;

	s_tag_block_index default_render_states;
	s_tag_block_index render_state_externs;
	s_tag_block_index texture_state_externs;
	s_tag_block_index pixel_constant_externs;
	s_tag_block_index vertex_constant_externs;
	s_tag_block_index ps_constants;
	s_tag_block_index vs_constants;
	s_tag_block_index pixel_constant_info;
	s_tag_block_index vertex_constant_info;
	s_tag_block_index render_state_info;
	s_tag_block_index texture_state_info;

	tag_block<s_pixel_shader> pixel_shader;
	tag_block<s_pixel_shader_extern_map> pixel_shader_switch_extern_map;
	tag_block<s_pixel_shader_index> pixel_shader_index_block;
};
ASSERT_STRUCT_SIZE(s_shader_pass_postprocess_implementation_new, 330)

// max count: k_number_of_render_states
struct s_render_state
{
	uint8 state_index;
	int32 state_value;
};
ASSERT_STRUCT_SIZE(s_render_state, 5)

struct s_render_state_extern
{
	uint8 render_state;		// D3DRENDERSTATETYPE
	uint8 parameter_type;	// e_shader_parameter_type
	uint8 pad;
};
ASSERT_STRUCT_SIZE(s_render_state_extern, 3)

// max count: k_kilo
struct s_shader_pass_postprocess_constant_new
{
	string_id parameter_name;
	uint8 component_mask;
	uint8 scale_by_texture_stage;
	uint8 function_index;
};
ASSERT_STRUCT_SIZE(s_shader_pass_postprocess_constant_new, 7)

struct s_shader_pass_postprocess_constant_info_new
{
	string_id parameter_name;
	uint8 pad[3];
};
ASSERT_STRUCT_SIZE(s_shader_pass_postprocess_constant_info_new, 7)

// max count: k_kilo
struct s_shader_pass_postprocess_texture_new
{
	int8 bitmap_parameter_index;
	int8 bitmap_extern_index;
	uint8 texture_stage_index;
	uint8 flags;
};
ASSERT_STRUCT_SIZE(s_shader_pass_postprocess_texture_new, 4)

// max count: k_kilo
struct s_shader_pass_postprocess_texture_state
{
	// low 4 bits: unknown
	// top 4 bits: D3DSAMP_MINFILTER
	uint8 unknown_and_min_filter;

	// low 4 bits: D3DSAMP_MAGFILTER
	// top 4 bits: D3DSAMP_MIPFILTER
	uint8 mag_filter_and_mip_filter;

	// low 4 bits: D3DSAMP_MAXANISOTROPY
	// top 4 bits: D3DSAMP_MAXMIPLEVEL
	uint8 max_anisotropy_and_max_mip_level;

	uint8 address_u;	// D3DSAMP_ADDRESSU
	uint8 address_v;	// D3DSAMP_ADDRESSV
	uint8 address_w;	// D3DSAMP_ADDRESSW

	int8 field_6;
	pixel32 border_color;		// D3DSAMP_BORDERCOLOR
	uint32 mip_map_lod_bias;	// D3DSAMP_MIPMAPLODBIAS
	int8 field_F[9];
};
ASSERT_STRUCT_SIZE(s_shader_pass_postprocess_texture_state, 24)
#pragma pack(pop)

// max count: k_kilo
struct s_shader_pass_postprocess_extern_new
{
	s_render_state_extern render_state;
	uint8 extern_index;
};
ASSERT_STRUCT_SIZE(s_shader_pass_postprocess_extern_new, 4)

#pragma pack(push,1)
// max count: k_number_of_texture_stage_states
struct s_texture_stage_state
{
	uint8 state_index;
	uint8 stage_index;
	int32 state_value;
};
ASSERT_STRUCT_SIZE(s_texture_stage_state, 6)

// max count: k_number_of_render_state_parameters
struct s_render_state_parameter
{
	uint8 parameter_index;
	uint8 parameter_type;
	uint8 state_index;
};
ASSERT_STRUCT_SIZE(s_render_state_parameter, 3)
#pragma pack(pop)

// max count: k_number_of_texture_stage_state_parameters
struct s_texture_stage_state_parameter
{
	uint8 parameter_index;
	uint8 parameter_type;
	uint8 state_index;
	uint8 stage_index;
};
ASSERT_STRUCT_SIZE(s_texture_stage_state_parameter, 4)

// max count: k_number_of_textures
struct s_texture
{
	uint8 stage_index;
	uint8 parameter_index;
	uint8 global_texture_index;
	uint8 flags;
};
ASSERT_STRUCT_SIZE(s_texture, 4)

struct s_shader_gpu_state
{
	tag_block<s_render_state> render_states;
	tag_block<s_texture_stage_state> texture_stage_states;
	tag_block<s_render_state_parameter> renderStateParameters;
	tag_block<s_texture_stage_state_parameter> textureStageParameters;
	tag_block<s_texture> textures;
	tag_block<s_vertex_shader_constant> vn_constants;
	tag_block<s_vertex_shader_constant> cn_constants;
};
ASSERT_STRUCT_SIZE(s_shader_gpu_state, 56)

struct s_shader_gpu_state_reference
{
	s_tag_block_index render_states;
	s_tag_block_index texture_stage_states;
	s_tag_block_index render_state_parameters;
	s_tag_block_index texture_stage_parameters;
	s_tag_block_index textures;
	s_tag_block_index vn_constants;
	s_tag_block_index cn_constants;
};
ASSERT_STRUCT_SIZE(s_shader_gpu_state_reference, 14)

//  max count: k_number_of_extern_references
struct s_extern_reference
{
	uint8 parameter_index;
	uint8 extern_index;
};
ASSERT_STRUCT_SIZE(s_extern_reference, 2)

// max count: k_number_of_shader_pass_implementations
struct s_shader_pass_postprocess_implementation
{
	s_shader_gpu_state gpu_state;
	s_shader_gpu_state_reference gpu_constant_state;
	s_shader_gpu_state_reference gpu_volatile_state;
	s_shader_gpu_state_reference gpu_default_state;
	
	tag_reference vertex_shader;	// vrtx
	int32 unk_0;
	int32 unk_1;
	int32 unk_2;
	int32 unk_3;
	int32 unk_4;
	int32 unk_5;

	tag_block<s_extern_reference> value_externs;
	tag_block<s_extern_reference> color_externs;
	tag_block<s_extern_reference> switch_externs;

	int16 bitmap_parameter_count;
	int16 pad;

	int16 gap[240];

	tag_block<s_pixel_shader_fragment> pixel_shader_fragments;
	tag_block<s_pixel_shader_permutation> pixel_shader_permutations;
	tag_block<s_pixel_shader_combiner> pixel_shader_combiners;
	tag_block<s_pixel_shader_constant> pixel_shader_constants;

	int8 pad1[8];
};
ASSERT_STRUCT_SIZE(s_shader_pass_postprocess_implementation, 680)

// max count: 1
struct s_shader_pass_postprocess_definition_new
{
	tag_block<s_shader_pass_postprocess_implementation_new> implementations;
	tag_block<s_shader_pass_postprocess_texture_new> textures;
	tag_block<s_render_state> render_states;
	tag_block<s_shader_pass_postprocess_texture_state> texture_states;
	tag_block<s_pixel_shader_fragment> ps_fragments;
	tag_block<s_pixel_shader_permutation_new> ps_permutations;
	tag_block<s_pixel_shader_combiner> ps_combiners;
	tag_block<s_shader_pass_postprocess_extern_new> externs;
	tag_block<s_shader_pass_postprocess_constant_new> constants;
	tag_block<s_shader_pass_postprocess_constant_info_new> constant_info;
	tag_block<s_shader_pass_postprocess_implementation> old_implementations;
};
ASSERT_STRUCT_SIZE(s_shader_pass_postprocess_definition_new, 88)

