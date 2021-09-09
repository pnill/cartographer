//#pragma once
//#include "Blam\Cache\DataTypes\BlamDataTypes.h"
//#include "Blam/Math/BlamMath.h"
//
///*********************************************************************
//* name: shader
//* group_tag : shad
//* header size : 92
//* *********************************************************************/
//struct s_shader_group_definition
//{
//	tag_reference template_;//0x0
//	string_id material_name;//0x8
//	struct s_runtime_properties_block
//	{
//		tag_reference diffuse_map;//0x0
//		tag_reference lightmap_emissive_map;//0x8
//		real_color_rgb lightmap_emissive_color;//0x10
//		float lightmap_emissive_power;//0x1C
//		float lightmap_resolution_scale;//0x20
//		float lightmap_half_life;//0x24
//		float lightmap_diffuse_scale;//0x28
//		tag_reference alpha_test_map;//0x2C
//		tag_reference translucent_map;//0x34
//		real_color_rgb lightmap_transparent_color;//0x3C
//		float lightmap_transparent_alpha;//0x48
//		float lightmap_foliage_scale;//0x4C
//	};
//	TAG_BLOCK_SIZE_ASSERT(s_runtime_properties_block, 0x50);
//	tag_block<s_runtime_properties_block> runtime_properties;//0xC
//	PAD(0x2);//0x14
//	enum class e_flags : __int16
//	{
//		water = FLAG(0),
//		sort_first = FLAG(1),
//		no_active_camo = FLAG(2),
//	};
//	e_flags flags;//0x16
//	struct s_parameters_block
//	{
//		string_id name;//0x0
//		enum class e_type : __int16
//		{
//			bitmap = 0,
//			value = 1,
//			color = 2,
//			switch_ = 3,
//		};
//		e_type type;//0x4
//		PAD(0x2);//0x6
//		tag_reference bitmap;//0x8
//		float const_value;//0x10
//		real_color_rgb const_color;//0x14
//		struct s_animation_properties_block
//		{
//			enum class e_type : __int16
//			{
//				bitmap_scale_uniform = 0,
//				bitmap_scale_x = 1,
//				bitmap_scale_y = 2,
//				bitmap_scale_z = 3,
//				bitmap_translation_x = 4,
//				bitmap_translation_y = 5,
//				bitmap_translation_z = 6,
//				bitmap_rotation_angle = 7,
//				bitmap_rotation_axis_x = 8,
//				bitmap_rotation_axis_y = 9,
//				bitmap_rotation_axis_z = 10,
//				value = 11,
//				color = 12,
//				bitmap_index = 13,
//			};
//			e_type type;//0x0
//			PAD(0x2);//0x2
//			string_id input_name;//0x4
//			string_id range_name;//0x8
//			float time_period;//0xC
//			struct s_data_block
//			{
//				__int8 value;//0x0
//			};
//			TAG_BLOCK_SIZE_ASSERT(s_data_block, 0x1);
//			tag_block<s_data_block> data;//0x10
//		};
//		TAG_BLOCK_SIZE_ASSERT(s_animation_properties_block, 0x18);
//		tag_block<s_animation_properties_block> animation_properties;//0x20
//	};
//	TAG_BLOCK_SIZE_ASSERT(s_parameters_block, 0x28);
//	tag_block<s_parameters_block> parameters;//0x18
//	struct s_postprocess_definition_block
//	{
//		tag_reference shader_template_index;//0x0
//		struct s_bitmaps_block
//		{
//			datum bitmap_group;//0x0
//			__int32 bitmap_index;//0x4
//			float log_bitmap_dimension;//0x8
//		};
//		TAG_BLOCK_SIZE_ASSERT(s_bitmaps_block, 0xC);
//		tag_block<s_bitmaps_block> bitmaps;//0x4
//		struct s_pixel_constants_block
//		{
//			PAD(0x4);//0x0
//		};
//		TAG_BLOCK_SIZE_ASSERT(s_pixel_constants_block, 0x4);
//		tag_block<s_pixel_constants_block> pixel_constants;//0xC
//		struct s_vertex_constants_block
//		{
//			PAD(0xC);//0x0
//			float w;//0xC
//		};
//		TAG_BLOCK_SIZE_ASSERT(s_vertex_constants_block, 0x10);
//		tag_block<s_vertex_constants_block> vertex_constants;//0x14
//		#pragma pack(push, 1)
//		struct s_levels_of_detail_block_a
//		{
//			int available_layer_flags;//0x0
//			short block_index_data;//0x4
//		};
//		#pragma pack(pop) 
//		TAG_BLOCK_SIZE_ASSERT(s_levels_of_detail_block_a, 0x6);
//		size_t a = sizeof(s_levels_of_detail_block_a);
//		tag_block<s_levels_of_detail_block_a> levels_of_detail;//0x1C
//		struct s_layers_block
//		{
//			__int16 block_index_data;//0x0
//		};
//		TAG_BLOCK_SIZE_ASSERT(s_layers_block, 0x2);
//		tag_block<s_layers_block> layers;//0x24
//		struct s_passes_block
//		{
//			__int16 block_index_data;//0x0
//		};
//		TAG_BLOCK_SIZE_ASSERT(s_passes_block, 0x2);
//		tag_block<s_passes_block> passes;//0x2C
//		struct s_implementations_block
//		{
//			__int16 block_index_data;//0x0
//			__int16 block_index_data;//0x2
//			__int16 block_index_data;//0x4
//			__int16 block_index_data;//0x6
//			__int16 block_index_data;//0x8
//		};
//		TAG_BLOCK_SIZE_ASSERT(s_implementations_block, 0xA);
//		tag_block<s_implementations_block> implementations;//0x34
//		struct s_overlays_block
//		{
//			string_id input_name;//0x0
//			string_id range_name;//0x4
//			float time_period_in_seconds;//0x8
//			struct s_data_block
//			{
//				__int8 value;//0x0
//			};
//			TAG_BLOCK_SIZE_ASSERT(s_data_block, 0x1);
//			tag_block<s_data_block> data;//0xC
//		};
//		TAG_BLOCK_SIZE_ASSERT(s_overlays_block, 0x14);
//		tag_block<s_overlays_block> overlays;//0x3C
//		struct s_overlay_references_block
//		{
//			__int16 overlay_index;//0x0
//			__int16 transform_index;//0x2
//		};
//		TAG_BLOCK_SIZE_ASSERT(s_overlay_references_block, 0x4);
//		tag_block<s_overlay_references_block> overlay_references;//0x44
//		struct s_animated_parameters_block
//		{
//			__int16 block_index_data;//0x0
//		};
//		TAG_BLOCK_SIZE_ASSERT(s_animated_parameters_block, 0x2);
//		tag_block<s_animated_parameters_block> animated_parameters;//0x4C
//		struct s_animated_parameter_references_block
//		{
//			PAD(0x3);//0x0
//			__int8 parameter_index;//0x3
//		};
//		TAG_BLOCK_SIZE_ASSERT(s_animated_parameter_references_block, 0x4);
//		tag_block<s_animated_parameter_references_block> animated_parameter_references;//0x54
//		struct s_bitmap_properties_block
//		{
//			__int16 bitmap_index;//0x0
//			__int16 animated_parameter_index;//0x2
//		};
//		TAG_BLOCK_SIZE_ASSERT(s_bitmap_properties_block, 0x4);
//		tag_block<s_bitmap_properties_block> bitmap_properties;//0x5C
//		struct s_color_properties_block
//		{
//			real_color_rgb color;//0x0
//		};
//		TAG_BLOCK_SIZE_ASSERT(s_color_properties_block, 0xC);
//		tag_block<s_color_properties_block> color_properties;//0x64
//		struct s_value_properties_block
//		{
//			float value;//0x0
//		};
//		TAG_BLOCK_SIZE_ASSERT(s_value_properties_block, 0x4);
//		tag_block<s_value_properties_block> value_properties;//0x6C
//		struct s_old_levels_of_detail_block
//		{
//			float projected_height_percentage;//0x0
//			__int32 available_layers;//0x4
//			struct s_layers_block
//			{
//				__int16 block_index_data;//0x0
//			};
//			TAG_BLOCK_SIZE_ASSERT(s_layers_block, 0x2);
//			tag_block<s_layers_block> layers;//0x8
//			struct s_passes_block
//			{
//				tag_reference shader_pass;//0x0
//				__int16 block_index_data;//0x8
//			};
//			TAG_BLOCK_SIZE_ASSERT(s_passes_block, 0xA);
//			tag_block<s_passes_block> passes;//0x10
//			struct s_implementations_block
//			{
//				__int16 block_index_data;//0x0
//				__int16 block_index_data;//0x2
//				__int16 block_index_data;//0x4
//				__int16 block_index_data;//0x6
//				__int16 block_index_data;//0x8
//				__int16 block_index_data;//0xA
//				__int16 block_index_data;//0xC
//				__int16 block_index_data;//0xE
//				__int16 block_index_data;//0x10
//				__int16 block_index_data;//0x12
//				__int16 block_index_data;//0x14
//				__int16 block_index_data;//0x16
//				__int16 block_index_data;//0x18
//				__int16 block_index_data;//0x1A
//				__int16 block_index_data;//0x1C
//				__int16 block_index_data;//0x1E
//				__int16 block_index_data;//0x20
//				__int16 block_index_data;//0x22
//				__int16 block_index_data;//0x24
//				__int16 block_index_data;//0x26
//				__int16 block_index_data;//0x28
//				__int16 block_index_data;//0x2A
//			};
//			TAG_BLOCK_SIZE_ASSERT(s_implementations_block, 0x2C);
//			tag_block<s_implementations_block> implementations;//0x18
//			struct s_bitmaps_block
//			{
//				__int8 parameter_index;//0x0
//				__int8 flags;//0x1
//				__int32 bitmap_group_index;//0x2
//				float log_bitmap_dimension;//0x6
//			};
//			TAG_BLOCK_SIZE_ASSERT(s_bitmaps_block, 0xA);
//			tag_block<s_bitmaps_block> bitmaps;//0x20
//			struct s_bitmap_transforms_block
//			{
//				__int8 parameter_index;//0x0
//				__int8 bitmap_transform_index;//0x1
//				float value;//0x2
//			};
//			TAG_BLOCK_SIZE_ASSERT(s_bitmap_transforms_block, 0x6);
//			tag_block<s_bitmap_transforms_block> bitmap_transforms;//0x28
//			struct s_values_block
//			{
//				__int8 parameter_index;//0x0
//				float value;//0x1
//			};
//			TAG_BLOCK_SIZE_ASSERT(s_values_block, 0x5);
//			tag_block<s_values_block> values;//0x30
//			struct s_colors_block
//			{
//				__int8 parameter_index;//0x0
//				real_color_rgb color;//0x1
//			};
//			TAG_BLOCK_SIZE_ASSERT(s_colors_block, 0xD);
//			tag_block<s_colors_block> colors;//0x38
//			struct s_bitmap_transform_overlays_block
//			{
//				__int8 parameter_index;//0x0
//				__int8 transform_index;//0x1
//				__int8 animation_property_type;//0x2
//				string_id input_name;//0x3
//				string_id range_name;//0x7
//				float time_period_in_seconds;//0xB
//				struct s_data_block
//				{
//					__int8 value;//0x0
//				};
//				TAG_BLOCK_SIZE_ASSERT(s_data_block, 0x1);
//				tag_block<s_data_block> data;//0xF
//			};
//			TAG_BLOCK_SIZE_ASSERT(s_bitmap_transform_overlays_block, 0x17);
//			tag_block<s_bitmap_transform_overlays_block> bitmap_transform_overlays;//0x40
//			struct s_value_overlays_block
//			{
//				__int8 parameter_index;//0x0
//				string_id input_name;//0x1
//				string_id range_name;//0x5
//				float time_period_in_seconds;//0x9
//				struct s_data_block
//				{
//					__int8 value;//0x0
//				};
//				TAG_BLOCK_SIZE_ASSERT(s_data_block, 0x1);
//				tag_block<s_data_block> data;//0xD
//			};
//			TAG_BLOCK_SIZE_ASSERT(s_value_overlays_block, 0x15);
//			tag_block<s_value_overlays_block> value_overlays;//0x48
//			struct s_color_overlays_block
//			{
//				__int8 parameter_index;//0x0
//				string_id input_name;//0x1
//				string_id range_name;//0x5
//				float time_period_in_seconds;//0x9
//				struct s_data_block
//				{
//					__int8 value;//0x0
//				};
//				TAG_BLOCK_SIZE_ASSERT(s_data_block, 0x1);
//				tag_block<s_data_block> data;//0xD
//			};
//			TAG_BLOCK_SIZE_ASSERT(s_color_overlays_block, 0x15);
//			tag_block<s_color_overlays_block> color_overlays;//0x50
//			struct s_vertex_shader_constants_block
//			{
//				__int8 register_index;//0x0
//				__int8 register_bank;//0x1
//				float data;//0x2
//				float data;//0x6
//				float data;//0xA
//				float data;//0xE
//			};
//			TAG_BLOCK_SIZE_ASSERT(s_vertex_shader_constants_block, 0x12);
//			tag_block<s_vertex_shader_constants_block> vertex_shader_constants;//0x58
//			struct s_render_states_block
//			{
//				__int8 state_index;//0x0
//				__int32 state_value;//0x1
//			};
//			TAG_BLOCK_SIZE_ASSERT(s_render_states_block, 0x5);
//			tag_block<s_render_states_block> render_states;//0x60
//			struct s_texture_stage_states_block
//			{
//				__int8 state_index;//0x0
//				__int8 stage_index;//0x1
//				__int32 state_value;//0x2
//			};
//			TAG_BLOCK_SIZE_ASSERT(s_texture_stage_states_block, 0x6);
//			tag_block<s_texture_stage_states_block> texture_stage_states;//0x68
//			struct s_render_state_parameters_block
//			{
//				__int8 parameter_index;//0x0
//				__int8 parameter_type;//0x1
//				__int8 state_index;//0x2
//			};
//			TAG_BLOCK_SIZE_ASSERT(s_render_state_parameters_block, 0x3);
//			tag_block<s_render_state_parameters_block> render_state_parameters;//0x70
//			struct s_texture_stage_parameters_block
//			{
//				__int8 parameter_index;//0x0
//				__int8 parameter_type;//0x1
//				__int8 state_index;//0x2
//				__int8 stage_index;//0x3
//			};
//			TAG_BLOCK_SIZE_ASSERT(s_texture_stage_parameters_block, 0x4);
//			tag_block<s_texture_stage_parameters_block> texture_stage_parameters;//0x78
//			struct s_textures_block
//			{
//				__int8 stage_index;//0x0
//				__int8 parameter_index;//0x1
//				__int8 global_texture_index;//0x2
//				__int8 flags;//0x3
//			};
//			TAG_BLOCK_SIZE_ASSERT(s_textures_block, 0x4);
//			tag_block<s_textures_block> textures;//0x80
//			struct s_vn_constants_block
//			{
//				__int8 register_index;//0x0
//				__int8 parameter_index;//0x1
//				__int8 destination_mask;//0x2
//				__int8 scale_by_texture_stage;//0x3
//			};
//			TAG_BLOCK_SIZE_ASSERT(s_vn_constants_block, 0x4);
//			tag_block<s_vn_constants_block> vn_constants;//0x88
//			struct s_cn_constants_block
//			{
//				__int8 register_index;//0x0
//				__int8 parameter_index;//0x1
//				__int8 destination_mask;//0x2
//				__int8 scale_by_texture_stage;//0x3
//			};
//			TAG_BLOCK_SIZE_ASSERT(s_cn_constants_block, 0x4);
//			tag_block<s_cn_constants_block> cn_constants;//0x90
//		};
//		TAG_BLOCK_SIZE_ASSERT(s_old_levels_of_detail_block, 0x98);
//		tag_block<s_old_levels_of_detail_block> old_levels_of_detail;//0x74
//	};
//	TAG_BLOCK_SIZE_ASSERT(s_postprocess_definition_block, 0x7C);
//	tag_block<s_postprocess_definition_block> postprocess_definition;//0x20
//	PAD(0x4);//0x28
//	struct s_predicted_resources_block
//	{
//		enum class e_type : __int16
//		{
//			bitmap = 0,
//			sound = 1,
//			render_model_geometry = 2,
//			cluster_geometry = 3,
//			cluster_instanced_geometry = 4,
//			lightmap_geometry_object_buckets = 5,
//			lightmap_geometry_instance_buckets = 6,
//			lightmap_cluster_bitmaps = 7,
//			lightmap_instance_bitmaps = 8,
//		};
//		e_type type;//0x0
//		__int16 resource_index;//0x2
//		__int32 tag_index;//0x4
//	};
//	TAG_BLOCK_SIZE_ASSERT(s_predicted_resources_block, 0x8);
//	tag_block<s_predicted_resources_block> predicted_resources;//0x2C
//	tag_reference light_response;//0x34
//	enum class e_shader_lod_bias : __int16
//	{
//		none = 0,
//		NUM_4x_size = 1,
//		NUM_2x_size = 2,
//		NUM_12_size = 3,
//		NUM_14_size = 4,
//		never = 5,
//		cinematic = 6,
//	};
//	e_shader_lod_bias shader_lod_bias;//0x3C
//	enum class e_specular_type : __int16
//	{
//		none = 0,
//		default = 1,
//		dull = 2,
//		shiny = 3,
//	};
//	e_specular_type specular_type;//0x3E
//	enum class e_lightmap_type : __int16
//	{
//		diffuse = 0,
//		default_specular = 1,
//		dull_specular = 2,
//		shiny_specular = 3,
//	};
//	e_lightmap_type lightmap_type;//0x40
//	PAD(0x2);//0x42
//	float lightmap_specular_brightness;//0x44
//	float lightmap_ambient_bias : [11];//0x48
//	struct s_postprocess_properties_block
//	{
//		__int32 bitmap_group_index;//0x0
//	};
//	TAG_BLOCK_SIZE_ASSERT(s_postprocess_properties_block, 0x4);
//	tag_block<s_postprocess_properties_block> postprocess_properties;//0x4C
//	float added_depth_bias_offset;//0x54
//	float added_depth_bias_slope_scale;//0x58
//};
//TAG_GROUP_SIZE_ASSERT(s_shader_group_definition, 0x5C);
