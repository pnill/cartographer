#pragma once
#include "shader_definitions.h"
#include "shader_postprocess_definitions.h"

#include "math/color_math.h"
#include "memory/static_arrays.h"
#include "tag_files/data_reference.h"
#include "tag_files/string_id.h"
#include "tag_files/tag_block.h"
#include "tag_files/tag_reference.h"

/* constants */

#define k_number_of_shader_template_properties 14
#define k_maximum_categories_per_shader_template 16
#define k_maximum_parameters_per_shader_template_category 64
#define k_shader_level_of_detail_count 8
#define k_maximum_pass_references_per_shader_template 16

/* enums */

enum e_shader_template_flags : uint16
{
	_shader_template_force_active_camo_bit = 0,
	_shader_template_water_bit = 1,
	_shader_template_foliage_bit = 2,
	_shader_template_hide_standard_parameters_bit = 3,
	k_shader_template_flag_count
};

enum e_shader_template_property_flags : int16
{
    _shader_template_property_unused = 0,
    _shader_template_property_diffuse_map = 1,
    _shader_template_property_lightmap_emissive_map = 2,
    _shader_template_property_lightmap_emissive_color = 3,
    _shader_template_property_lightmap_emissive_power = 4,
    _shader_template_property_lightmap_resolution_scale = 5,
    _shader_template_property_lightmap_half_life = 6,
    _shader_template_property_lightmap_diffuse_scale = 7,
    _shader_template_property_lightmap_alpha_test_map = 8,
    _shader_template_property_lightmap_translucent_map = 9,
    _shader_template_property_lightmap_translucent_color = 10,
    _shader_template_property_lightmap_translucent_alpha = 11,
    _shader_template_property_active_camo_map = 12,
    _shader_template_property_lightmap_foliage_scale = 13
};

enum e_shader_template_parameter_flags : uint16
{
    _shader_template_parameter_animated_bit = 0,
    _shader_template_parameter_hide_bitmap_reference_bit = 1,
    k_shader_template_parameter_flag_count
};

enum e_bitmap_animation_flags : uint16
{
    _bitmap_animation_scale_uniform_bit = 0,
    _bitmap_animation_scale_bit = 1,
    _bitmap_animation_translation_bit = 2,
    _bitmap_animation_rotation_bit = 3,
    _bitmap_animation_index_bit = 4,
    k_bitmap_animation_flag_count
};

enum e_shader_template_layer : int16
{
    _shader_template_layer_texaccum = 0,
    _shader_template_layer_environment_map = 1,
    _shader_template_layer_self_illumination = 2,
    _shader_template_layer_overlay = 3,
    _shader_template_layer_transparent = 4,
    _shader_template_layer_lightmap_indirect = 5,
    _shader_template_layer_diffuse = 6,
    _shader_template_layer_specular = 7,
    _shader_template_layer_shadow_generate = 8,
    _shader_template_layer_shadow_apply = 9,
    _shader_template_layer_boom = 10,
    _shader_template_layer_fog = 11,
    _shader_template_layer_sh_prt = 12,
    _shader_template_layer_active_camo = 13,
    _shader_template_layer_water_edge_blend = 14,
    _shader_template_layer_decal = 15,
    _shader_template_layer_active_camo_stencil_modulate = 16,
    _shader_template_layer_hologram = 17,
    _shader_template_layer_light_albedo = 18
};

/* structures */

// max count: k_number_of_shader_template_properties
struct s_shader_template_property
{
    e_shader_template_property_flags property;
    int16 pad;
    string_id parameter_name;
};
ASSERT_STRUCT_SIZE(s_shader_template_property, 8);

// max count: k_maximum_parameters_per_shader_template_category
struct s_shader_template_parameter
{
    string_id name;

    /****************************************
    * definition_name: shader_text_definition
    * flags: 6
    * alignment_bit: 0
    ****************************************/
    // DataSize(65535)
    data_reference explanation;
    e_shader_parameter_type type;
    c_flags_no_init<e_shader_template_parameter_flags, uint16, k_shader_template_parameter_flag_count> flags;

    tag_reference default_bitmap;   // bitm
    real32 default_const_value;
    real_rgb_color default_const_color;

    e_bitmap_type bitmap_type;
    int16 pad;

    c_flags_no_init<e_bitmap_animation_flags, uint16, k_bitmap_animation_flag_count> bitmap_animation_flags;
    int16 pad1;
    real32 bitmap_scale;
};
ASSERT_STRUCT_SIZE(s_shader_template_parameter, 52);

// max count: k_maximum_categories_per_shader_template
struct s_shader_template_category
{
	string_id name;
	tag_block<s_shader_template_parameter> parameters;
};
ASSERT_STRUCT_SIZE(s_shader_template_category, 12);

// max count: k_maximum_pass_references_per_shader_template
struct s_shader_template_pass_reference
{
    e_shader_template_layer layer;
    int16 pad;
    tag_reference pass; // spas
    int32 pad1[3];
};
ASSERT_STRUCT_SIZE(s_shader_template_pass_reference, 24);

// max count: k_shader_level_of_detail_count
struct s_shader_template_level_of_detail
{
    real32 projected_diameter_pixels;
    tag_block<s_shader_template_pass_reference> pass;
};
ASSERT_STRUCT_SIZE(s_shader_template_level_of_detail, 12);

struct s_shader_template_definition
{
    /****************************************
    * definition_name: shader_text_definition
    * flags: 6
    * alignment_bit: 0
    ****************************************/
    // DataSize(65535)
	data_reference documentation;

	string_id default_material_name;

	/* Explaination("FLAGS", "* Force Active Camo: Should be used with cautuion, as this causes a backbuffer copy when this shader is rendered.
	* Water: ? ? ? .
	* Foliage : Used with lightmapped foliage(two - sided lighting) shaders.It affects importing but not rendering.")
	*/

	c_flags_no_init<e_shader_template_flags, uint16, k_shader_template_flag_count> flags;
	tag_block<s_shader_template_property> properties;
	tag_block<s_shader_template_category> categories;

    // Explaination("LIGHT RESPONSE", "Not used anymore.")

    tag_reference light_response;   // slit

    tag_block<s_shader_template_level_of_detail> lods;

    tag_block<int32> light_response_0;    // unknown name | max count: UNSIGNED_SHORT_MAX
    tag_block<int32> light_response_1;    // unknown name | max count: UNSIGNED_SHORT_MAX

    // Explaination("RECURSIVE RENDERING", "Really cool stuff.")
    
    tag_reference aux_1_shader;             // shad
    e_shader_template_layer aux_1_layer;
    tag_reference aux_2_shader;             // shad
    e_shader_template_layer aux_2_layer;

    int16 pad;
    tag_block<s_shader_template_postprocess_definition_new> postprocess_definition;
};
ASSERT_STRUCT_SIZE(s_shader_template_definition, 96);
