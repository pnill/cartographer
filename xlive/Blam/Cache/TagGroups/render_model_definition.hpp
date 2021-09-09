#pragma once
#include "Blam\Cache\DataTypes\BlamDataTypes.h"
#include "Blam/Cache/DataTypes/Strings.h"
#include "Blam/Math/BlamMath.h"

/*********************************************************************
* name: model
* group_tag : hlmt
* header size : 252
* *********************************************************************/

#pragma pack(push,1)
struct s_render_model_group_definition
{
	string_id name_old_string_id;//0x0
	enum class e_flags : __int16
	{
		NUM__render_model_force_third_person_bit = FLAG(0),
		force_carmackreverse = FLAG(1),
		force_node_maps = FLAG(2),
		geometry_postprocessed = FLAG(3),
	};
	e_flags flags;//0x4
	PAD(0x6);//0x6
	struct s_import_info_block
	{
		__int32 build;//0x0
		tag_string256 version;//0x4
		tag_string32 import_date;//0x104
		tag_string32 culprit;//0x124
		PAD(0x60);//0x144
		tag_string32 import_time;//0x1A4
		PAD(0x4);//0x1C4
		struct s_files_block
		{
			tag_string256 path;//0x0
			tag_string32 modification_date;//0x100
			PAD(0x60);//0x120
			__int32 checksum;//0x180
			__int32 size;//0x184
			data_block zipped_data;//0x188
			PAD(0x80);//0x190
		};
		TAG_BLOCK_SIZE_ASSERT(s_files_block, 0x210);
		tag_block<s_files_block> files;//0x1C8
		PAD(0x80);//0x1D0
	};
	TAG_BLOCK_SIZE_ASSERT(s_import_info_block, 0x250);
	tag_block<s_import_info_block> import_info;//0xC
	struct s_compression_info_block
	{
		float position_bounds_x_lower;//0x0
		float position_bounds_x_upper;//0x4
		float position_bounds_y_lower;//0x8
		float position_bounds_y_upper;//0xC
		float position_bounds_z_lower;//0x10
		float position_bounds_z_upper;//0x14
		float texcoord_bounds_x_lower;//0x18
		float texcoord_bounds_x_upper;//0x1C
		float texcoord_bounds_y_lower;//0x20
		float texcoord_bounds_y_upper;//0x24
		float secondary_texcoord_bounds_x_lower;//0x28
		float secondary_texcoord_bounds_x_upper;//0x2C
		float secondary_texcoord_bounds_y_lower;//0x30
		float secondary_texcoord_bounds_y_upper;//0x34
	};
	TAG_BLOCK_SIZE_ASSERT(s_compression_info_block, 0x38);
	tag_block<s_compression_info_block> compression_info;//0x14
	struct s_regions_block
	{
		string_id name_old_string_id;//0x0
		__int16 node_map_offset_old;//0x4
		__int16 node_map_size_old;//0x6
		struct s_permutations_block
		{
			string_id name_old_string_id;//0x0
			__int16 l1_section_index;//0x4
			__int16 l2_section_index;//0x6
			__int16 l3_section_index;//0x8
			__int16 l4_section_index;//0xA
			__int16 l5_section_index;//0xC
			__int16 l6_section_index;//0xE
		};
		TAG_BLOCK_SIZE_ASSERT(s_permutations_block, 0x10);
		tag_block<s_permutations_block> permutations;//0x8
	};
	TAG_BLOCK_SIZE_ASSERT(s_regions_block, 0x10);
	tag_block<s_regions_block> regions;//0x1C
	struct s_sections_block
	{
		enum class e_global_geometry_classification_enum_definition : __int16
		{
			worldspace = 0,
			rigid = 1,
			rigid_boned = 2,
			skinned = 3,
			unsupported_reimport = 4,
		};
		e_global_geometry_classification_enum_definition global_geometry_classification_enum_definition;//0x0
		PAD(0x2);//0x2
		__int16 total_vertex_count;//0x4
		__int16 total_triangle_count;//0x6
		__int16 total_part_count;//0x8
		__int16 shadowcasting_triangle_count;//0xA
		__int16 shadowcasting_part_count;//0xC
		__int16 opaque_point_count;//0xE
		__int16 opaque_vertex_count;//0x10
		__int16 opaque_part_count;//0x12
		__int8 opaque_max_nodesvertex;//0x14
		__int8 transparent_max_nodesvertex;//0x15
		__int16 shadowcasting_rigid_triangle_count;//0x16
		enum class e_geometry_classification : __int16
		{
			worldspace = 0,
			rigid = 1,
			rigid_boned = 2,
			skinned = 3,
			unsupported_reimport = 4,
		};
		e_geometry_classification geometry_classification;//0x18
		enum class e_geometry_compression_flags : __int16
		{
			compressed_position = FLAG(0),
			compressed_texcoord = FLAG(1),
			compressed_secondary_texcoord = FLAG(2),
		};
		e_geometry_compression_flags geometry_compression_flags;//0x1A
		struct s_NUM__block
		{
			float position_bounds_x_lower;//0x0
			float position_bounds_x_upper;//0x4
			float position_bounds_y_lower;//0x8
			float position_bounds_y_upper;//0xC
			float position_bounds_z_lower;//0x10
			float position_bounds_z_upper;//0x14
			float texcoord_bounds_x_lower;//0x18
			float texcoord_bounds_x_upper;//0x1C
			float texcoord_bounds_y_lower;//0x20
			float texcoord_bounds_y_upper;//0x24
			float secondary_texcoord_bounds_x_lower;//0x28
			float secondary_texcoord_bounds_x_upper;//0x2C
			float secondary_texcoord_bounds_y_lower;//0x30
			float secondary_texcoord_bounds_y_upper;//0x34
		};
		TAG_BLOCK_SIZE_ASSERT(s_NUM__block, 0x38);
		tag_block<s_NUM__block> NUM_;//0x1C
		__int8 hardware_node_count;//0x24
		__int8 node_map_size;//0x25
		__int16 software_plane_count;//0x26
		__int16 total_subpart_cont;//0x28
		enum class e_section_lighting_flags : __int16
		{
			has_lm_texcoords = FLAG(0),
			has_lm_inc_rad = FLAG(1),
			has_lm_colors = FLAG(2),
			has_lm_prt = FLAG(3),
		};
		e_section_lighting_flags section_lighting_flags;//0x2A
		__int16 rigid_node;//0x2C
		enum class e_flags : __int16
		{
			geometry_postprocessed = FLAG(0),
		};
		e_flags flags;//0x2E
		struct s_section_data_block
		{
			struct s_parts_block
			{
				enum class e_type : __int16
				{
					not_drawn = 0,
					opaque_shadow_only = 1,
					opaque_shadowcasting = 2,
					opaque_nonshadowing = 3,
					transparent = 4,
					lightmap_only = 5,
				};
				e_type type;//0x0
				enum class e_flags : __int16
				{
					decalable = FLAG(0),
					new_part_types = FLAG(1),
					dislikes_photons = FLAG(2),
					override_triangle_list = FLAG(3),
					ignored_by_lightmapper = FLAG(4),
				};
				e_flags flags;//0x2
				__int16 material;//0x4
				__int16 strip_start_index;//0x6
				__int16 strip_length;//0x8
				__int16 first_subpart_index;//0xA
				__int16 subpart_count;//0xC
				__int8 max_nodesvertex;//0xE
				__int8 contributing_compound_node_count;//0xF
				float position_x;//0x10
				float position_y;//0x14
				float position_z;//0x18
				__int8 node_index_1;//0x1C
				__int8 node_index_2;//0x1D
				__int8 node_index_3;//0x1E
				__int8 node_index_4;//0x1F
				float node_weight_1;//0x20
				float node_weight_2;//0x24
				float node_weight_3;//0x28
				float lod_mipmap_magic_number;//0x2C
				PAD(0x18);//0x30
			};
			TAG_BLOCK_SIZE_ASSERT(s_parts_block, 0x48);
			tag_block<s_parts_block> parts;//0x0
			struct s_subparts_block
			{
				__int16 indices_start_index;//0x0
				__int16 indices_length;//0x2
				__int16 visibility_bounds_index;//0x4
				__int16 part_index;//0x6
			};
			TAG_BLOCK_SIZE_ASSERT(s_subparts_block, 0x8);
			tag_block<s_subparts_block> subparts;//0x8
			struct s_visibility_bounds_block
			{
				float position_x;//0x0
				float position_y;//0x4
				float position_z;//0x8
				float radius;//0xC
				__int8 node_0;//0x10
				PAD(0x3);//0x11
			};
			TAG_BLOCK_SIZE_ASSERT(s_visibility_bounds_block, 0x14);
			tag_block<s_visibility_bounds_block> visibility_bounds;//0x10
			struct s_raw_vertices_block
			{
				float position_x;//0x0
				float position_y;//0x4
				float position_z;//0x8
				__int32 node_index_old_1;//0xC
				__int32 node_index_old_2;//0x10
				__int32 node_index_old_3;//0x14
				__int32 node_index_old_4;//0x18
				float node_weight_1;//0x1C
				float node_weight_2;//0x20
				float node_weight_3;//0x24
				float node_weight_4;//0x28
				__int32 node_index_new_1;//0x2C
				__int32 node_index_new_2;//0x30
				__int32 node_index_new_3;//0x34
				__int32 node_index_new_4;//0x38
				__int32 use_new_node_indices;//0x3C
				__int32 adjusted_compound_node_index;//0x40
				float texcoord_x;//0x44
				float texcoord_y;//0x48
				PAD(0x30);//0x4C
				float secondary_texcoord_x;//0x7C
				float secondary_texcoord_y;//0x80
				real_color_rgb primary_lightmap_color;//0x84
				float primary_lightmap_texcoord_x;//0x90
				float primary_lightmap_texcoord_y;//0x94
				PAD(0x2C);//0x98
			};
			TAG_BLOCK_SIZE_ASSERT(s_raw_vertices_block, 0xC4);
			tag_block<s_raw_vertices_block> raw_vertices;//0x18
			struct s_strip_indices_block
			{
				__int16 index;//0x0
			};
			TAG_BLOCK_SIZE_ASSERT(s_strip_indices_block, 0x2);
			tag_block<s_strip_indices_block> strip_indices;//0x20
			data_block visibility_mopp_code;//0x28
			struct s_mopp_reorder_table_block
			{
				__int16 index;//0x0
			};
			TAG_BLOCK_SIZE_ASSERT(s_mopp_reorder_table_block, 0x2);
			tag_block<s_mopp_reorder_table_block> mopp_reorder_table;//0x30
			struct s_vertex_buffers_block
			{
				PAD(0x20);//0x0
			};
			TAG_BLOCK_SIZE_ASSERT(s_vertex_buffers_block, 0x20);
			tag_block<s_vertex_buffers_block> vertex_buffers;//0x38
			PAD(0x4);//0x40
			struct s_raw_points_block
			{
				float position_x;//0x0
				float position_y;//0x4
				float position_z;//0x8
				__int32 node_index_old_1;//0xC
				__int32 node_index_old_2;//0x10
				__int32 node_index_old_3;//0x14
				__int32 node_index_old_4;//0x18
				float node_weight_1;//0x1C
				float node_weight_2;//0x20
				float node_weight_3;//0x24
				float node_weight_4;//0x28
				__int32 node_index_new_1;//0x2C
				__int32 node_index_new_2;//0x30
				__int32 node_index_new_3;//0x34
				__int32 node_index_new_4;//0x38
				__int32 use_new_node_indices;//0x3C
				__int32 adjusted_compound_node_index;//0x40
			};
			TAG_BLOCK_SIZE_ASSERT(s_raw_points_block, 0x44);
			tag_block<s_raw_points_block> raw_points;//0x44
			data_block runtime_point_data;//0x4C
			struct s_rigid_point_groups_block
			{
				__int8 rigid_node_index;//0x0
				__int8 nodespoint;//0x1
				__int16 point_count;//0x2
			};
			TAG_BLOCK_SIZE_ASSERT(s_rigid_point_groups_block, 0x4);
			tag_block<s_rigid_point_groups_block> rigid_point_groups;//0x54
			struct s_vertexpoint_indices_block
			{
				__int16 index;//0x0
			};
			TAG_BLOCK_SIZE_ASSERT(s_vertexpoint_indices_block, 0x2);
			tag_block<s_vertexpoint_indices_block> vertexpoint_indices;//0x5C
			struct s_node_map_block
			{
				__int8 node_index;//0x0
			};
			TAG_BLOCK_SIZE_ASSERT(s_node_map_block, 0x1);
			tag_block<s_node_map_block> node_map;//0x64
			PAD(0x4);//0x6C
		};
		TAG_BLOCK_SIZE_ASSERT(s_section_data_block, 0x70);
		tag_block<s_section_data_block> section_data;//0x30
		__int32 block_offset;//0x38
		__int32 block_size;//0x3C
		__int32 section_data_size;//0x40
		__int32 resource_data_size;//0x44
		struct s_resources_block
		{
			enum class e_type : __int8
			{
				tag_block = 0,
				tag_data = 1,
				vertex_buffer = 2,
			};
			e_type type;//0x0
			PAD(0x3);//0x1
			__int16 primary_locator;//0x4
			__int16 secondary_locator;//0x6
			__int32 resource_data_size;//0x8
			__int32 resource_data_offset;//0xC
		};
		TAG_BLOCK_SIZE_ASSERT(s_resources_block, 0x10);
		tag_block<s_resources_block> resources;//0x48
		PAD(0x4);//0x50
		__int16 owner_tag_section_offset;//0x54
		PAD(0x6);//0x56
	};
	TAG_BLOCK_SIZE_ASSERT(s_sections_block, 0x5C);
	tag_block<s_sections_block> sections;//0x24
	struct s_invalid_section_pair_bits_block
	{
		__int32 bits;//0x0
	};
	TAG_BLOCK_SIZE_ASSERT(s_invalid_section_pair_bits_block, 0x4);
	tag_block<s_invalid_section_pair_bits_block> invalid_section_pair_bits;//0x2C
	struct s_section_groups_block
	{
		enum class e_detail_levels : __int16
		{
			l1_super_low = FLAG(0),
			l2_low = FLAG(1),
			l3_medium = FLAG(2),
			l4_high = FLAG(3),
			l5_super_high = FLAG(4),
			l6_hollywood = FLAG(5),
		};
		e_detail_levels detail_levels;//0x0
		PAD(0x2);//0x2
		struct s_compound_nodes_block
		{
			__int8 node_index_1;//0x0
			__int8 node_index_2;//0x1
			__int8 node_index_3;//0x2
			__int8 node_index_4;//0x3
			float node_weight_1;//0x4
			float node_weight_2;//0x8
			float node_weight_3;//0xC
		};
		TAG_BLOCK_SIZE_ASSERT(s_compound_nodes_block, 0x10);
		tag_block<s_compound_nodes_block> compound_nodes;//0x4
	};
	TAG_BLOCK_SIZE_ASSERT(s_section_groups_block, 0xC);
	tag_block<s_section_groups_block> section_groups;//0x34
	__int8 l1_section_group_index;//0x3C
	__int8 l2_section_group_index;//0x3D
	__int8 l3_section_group_index;//0x3E
	__int8 l4_section_group_index;//0x3F
	__int8 l5_section_group_index;//0x40
	__int8 l6_section_group_index;//0x41
	PAD(0x2);//0x42
	__int32 node_list_checksum;//0x44
	struct s_nodes_block
	{
		string_id name_old_string_id;//0x0
		__int16 parent_node;//0x4
		__int16 first_child_node;//0x6
		__int16 next_sibling_node;//0x8
		__int16 import_node_index;//0xA
		float default_translation_x;//0xC
		float default_translation_y;//0x10
		float default_translation_z;//0x14
		float default_rotation_i;//0x18
		float default_rotation_j;//0x1C
		float default_rotation_k;//0x20
		float default_rotation_w;//0x24
		float default_scale;//0x28
		float inverse_forward_i;//0x2c
		float inverse_forward_j;//0x30
		float inverse_forward_k;//0x34
		float inverse_left_i;//0x38
		float inverse_left_j;//0x3c
		float inverse_left_k;//0x40
		float inverse_up_i;//0x44
		float inverse_up_j;//0x48
		float inverse_up_k;//
		float inverse_position_x;//0x4C
		float inverse_position_y;//0x50
		float inverse_position_z;//0x54
		float distance_from_parent;//0x5C
	};
	TAG_BLOCK_SIZE_ASSERT(s_nodes_block, 0x60);
	tag_block<s_nodes_block> nodes;//0x48
	struct s_node_map_old_block
	{
		__int8 node_index;//0x0
	};
	TAG_BLOCK_SIZE_ASSERT(s_node_map_old_block, 0x1);
	tag_block<s_node_map_old_block> node_map_old;//0x50
	struct s_marker_groups_block
	{
		string_id name_old_string_id;//0x0
		struct s_markers_block
		{
			__int8 region_index;//0x0
			__int8 permutation_index;//0x1
			__int8 node_index;//0x2
			PAD(0x1);//0x3
			float translation_x;//0x4
			float translation_y;//0x8
			float translation_z;//0xC
			float rotation_i;
			float rotation_j;
			float roatation_k;
			float rotation_w;//0x1C
			float scale;//0x20
		};
		TAG_BLOCK_SIZE_ASSERT(s_markers_block, 0x24);
		tag_block<s_markers_block> markers;//0x4
	};
	TAG_BLOCK_SIZE_ASSERT(s_marker_groups_block, 0xC);
	tag_block<s_marker_groups_block> marker_groups;//0x58
	struct s_materials_block
	{
		tag_reference old_shader;//0x0
		tag_reference shader;//0x8
		struct s_properties_block
		{
			enum class e_type : __int16
			{
				lightmap_resolution = 0,
				lightmap_power = 1,
				lightmap_half_life = 2,
				lightmap_diffuse_scale = 3,
			};
			e_type type;//0x0
			__int16 int_value;//0x2
			float real_value;//0x4
		};
		TAG_BLOCK_SIZE_ASSERT(s_properties_block, 0x8);
		tag_block<s_properties_block> properties;//0x10
		PAD(0x4);//0x18
		__int8 breakable_surface_index;//0x1C
		PAD(0x3);//0x1D
	};
	TAG_BLOCK_SIZE_ASSERT(s_materials_block, 0x20);
	tag_block<s_materials_block> materials;//0x60
	struct s_errors_block
	{
		tag_string256 name;//0x0
		enum class e_report_type : __int16
		{
			silent = 0,
			comment = 1,
			warning = 2,
			error = 3,
		};
		e_report_type report_type;//0x100
		enum class e_flags : __int16
		{
			rendered = FLAG(0),
			tangent_space = FLAG(1),
			noncritical = FLAG(2),
			lightmap_light = FLAG(3),
			report_key_is_valid = FLAG(4),
		};
		e_flags flags;//0x102
		PAD(0x198);//0x104
		struct s_reports_block
		{
			enum class e_type : __int16
			{
				silent = 0,
				comment = 1,
				warning = 2,
				error = 3,
			};
			e_type type;//0x0
			enum class e_flags : __int16
			{
				rendered = FLAG(0),
				tangent_space = FLAG(1),
				noncritical = FLAG(2),
				lightmap_light = FLAG(3),
				report_key_is_valid = FLAG(4),
			};
			e_flags flags;//0x2
			data_block text;//0x4
			tag_string32 source_filename;//0xC
			__int32 source_line_number;//0x2C
			struct s_vertices_block
			{
				float position_x;//0x0
				float position_y;//0x4
				float position_z;//0x8
				__int8 node_index_1;//0xC
				__int8 node_index_2;//0xD
				__int8 node_index_3;//0xE
				__int8 node_index_4;//0xF
				float node_weight_1;//0x10
				float node_weight_2;//0x14
				float node_weight_3;//0x18
				float node_weight_4;//0x1C
				real_color_argb color;//0x20
				float screen_size;//0x30
			};
			TAG_BLOCK_SIZE_ASSERT(s_vertices_block, 0x34);
			tag_block<s_vertices_block> vertices;//0x30
			struct s_vectors_block
			{
				float position_x;//0x0
				float position_y;//0x4
				float position_z;//0x8
				__int8 node_index_1;//0xC
				__int8 node_index_2;//0xD
				__int8 node_index_3;//0xE
				__int8 node_index_4;//0xF
				float node_weight_1;//0x10
				float node_weight_2;//0x14
				float node_weight_3;//0x18
				float node_weight_4;//0x1C
				real_color_argb color;//0x20
				PAD(0xC);//0x30
				float screen_length;//0x3C
			};
			TAG_BLOCK_SIZE_ASSERT(s_vectors_block, 0x40);
			tag_block<s_vectors_block> vectors;//0x38
			struct s_lines_block
			{
				float position_x_1;//0x0
				float position_y_1;//0x4
				float position_z_1;//0x8
				__int8 a_node_index_1;//0xC
				__int8 a_node_index_2;//0xD
				__int8 a_node_index_3;//0xE
				__int8 a_node_index_4;//0xF
				float a_node_weight_1;//0x10
				float a_node_weight_2;//0x14
				float a_node_weight_3;//0x18
				float a_node_weight_4;//0x1C
				float position_x_2;//0x20
				float position_y_2;//0x24
				float position_z_2;//0x28
				__int8 b_node_index_1;//0x2C
				__int8 b_node_index_2;//0x2D
				__int8 b_node_index_3;//0x2E
				__int8 b_node_index_4;//0x2F
				float b_node_weight_1;//0x30
				float b_node_weight_2;//0x34
				float b_node_weight_3;//0x38
				float b_node_weight_4;//0x3C
				float c_node_weight_1;//0x40
				float c_node_weight_2;//0x44
				float c_node_weight_3;//0x48
				float c_node_weight_4;//0x4C
			};
			TAG_BLOCK_SIZE_ASSERT(s_lines_block, 0x50);
			tag_block<s_lines_block> lines;//0x40
			struct s_triangles_block
			{
				float a_position_x_1;//0x0
				float a_position_y_1;//0x4
				float a_position_z_1;//0x8
				__int8 a_node_index_1;//0xC
				__int8 a_node_index_2;//0xD
				__int8 a_node_index_3;//0xE
				__int8 a_node_index_4;//0xF
				float a_node_weight_1;//0x10
				float a_node_weight_2;//0x14
				float a_node_weight_3;//0x18
				float a_node_weight_4;//0x1C
				float b_position_x_2;//0x20
				float b_position_y_2;//0x24
				float b_position_z_2;//0x28
				__int8 b_node_index_1;//0x2C
				__int8 b_node_index_2;//0x2D
				__int8 b_node_index_3;//0x2E
				__int8 b_node_index_4;//0x2F
				float b_node_weight_1;//0x30
				float b_node_weight_2;//0x34
				float b_node_weight_3;//0x38
				float b_node_weight_4;//0x3C
				float c_position_x_3;//0x40
				float c_position_y_3;//0x44
				float c_position_z_3;//0x48
				__int8 c_node_index_1;//0x4C
				__int8 c_node_index_2;//0x4D
				__int8 c_node_index_3;//0x4E
				__int8 c_node_index_4;//0x4F
				float c_node_weight_1;//0x50
				float c_node_weight_2;//0x54
				float c_node_weight_3;//0x58
				float c_node_weight_4;//0x5C
				float d_node_weight_1;//0x60
				float d_node_weight_2;//0x64
				float d_node_weight_3;//0x68
				float d_node_weight_4;//0x6C
			};
			TAG_BLOCK_SIZE_ASSERT(s_triangles_block, 0x70);
			tag_block<s_triangles_block> triangles;//0x48
			struct s_quads_block
			{
				float a_position_x_1;//0x0
				float a_position_y_1;//0x4
				float a_position_z_1;//0x8
				__int8 a_node_index_1;//0xC
				__int8 a_node_index_2;//0xD
				__int8 a_node_index_3;//0xE
				__int8 a_node_index_4;//0xF
				float a_node_weight_1;//0x10
				float a_node_weight_2;//0x14
				float a_node_weight_3;//0x18
				float a_node_weight_4;//0x1C
				float b_position_x_2;//0x20
				float b_position_y_2;//0x24
				float b_position_z_2;//0x28
				__int8 b_node_index_1;//0x2C
				__int8 b_node_index_2;//0x2D
				__int8 b_node_index_3;//0x2E
				__int8 b_node_index_4;//0x2F
				float b_node_weight_1;//0x30
				float b_node_weight_2;//0x34
				float b_node_weight_3;//0x38
				float b_node_weight_4;//0x3C
				float c_position_x_3;//0x40
				float c_position_y_3;//0x44
				float c_position_z_3;//0x48
				__int8 c_node_index_1;//0x4C
				__int8 c_node_index_2;//0x4D
				__int8 c_node_index_3;//0x4E
				__int8 c_node_index_4;//0x4F
				float c_node_weight_1;//0x50
				float c_node_weight_2;//0x54
				float c_node_weight_3;//0x58
				float c_node_weight_4;//0x5C
				float d_position_x_4;//0x60
				float d_position_y_4;//0x64
				float d_position_z_4;//0x68
				__int8 d_node_index_1;//0x6C
				__int8 d_node_index_2;//0x6D
				__int8 d_node_index_3;//0x6E
				__int8 d_node_index_4;//0x6F
				float d_node_weight_1;//0x70
				float d_node_weight_2;//0x74
				float d_node_weight_3;//0x78
				float d_node_weight_4;//0x7C
				float e_node_weight_1;//0x80
				float e_node_weight_2;//0x84
				float e_node_weight_3;//0x88
				float e_node_weight_4;//0x8C
			};
			TAG_BLOCK_SIZE_ASSERT(s_quads_block, 0x90);
			tag_block<s_quads_block> quads;//0x50
			struct s_comments_block
			{
				data_block text;//0x0
				float position_x;//0x8
				float position_y;//0xC
				float position_z;//0x10
				__int8 node_index_1;//0x14
				__int8 node_index_2;//0x15
				__int8 node_index_3;//0x16
				__int8 node_index_4;//0x17
				float node_weight_1;//0x18
				float node_weight_2;//0x1C
				float node_weight_3;//0x20
				float node_weight_4;//0x24
				real_color_argb color;//0x28
			};
			TAG_BLOCK_SIZE_ASSERT(s_comments_block, 0x38);
			tag_block<s_comments_block> comments;//0x58
			PAD(0x17C);//0x60
			__int32 report_key;//0x1DC
			__int32 node_index;//0x1E0
			float bounds_x_lower;//0x1E4
			float bounds_x_upper;//0x1E8
			float bounds_y_lower;//0x1EC
			float bounds_y_upper;//0x1F0
			float bounds_z_lower;//0x1F4
			float bounds_z_upper;//0x1F8
			real_color_argb color;//0x1FC
			PAD(0x54);//0x20C
		};
		TAG_BLOCK_SIZE_ASSERT(s_reports_block, 0x260);
		tag_block<s_reports_block> reports;//0x29C
	};
	TAG_BLOCK_SIZE_ASSERT(s_errors_block, 0x2A4);
	tag_block<s_errors_block> errors;//0x68
	float dont_draw_over_camera_cosine_angle;//0x70
	struct s_prt_info_block
	{
		__int16 sh_order;//0x0
		__int16 num_of_clusters;//0x2
		__int16 pca_vectors_per_cluster;//0x4
		__int16 number_of_rays;//0x6
		__int16 number_of_bounces;//0x8
		__int16 mat_index_for_sbsfc_scattering;//0xA
		float length_scale;//0xC
		__int16 number_of_lods_in_model;//0x10
		PAD(0x2);//0x12
		struct s_lod_info_block
		{
			__int32 cluster_offset;//0x0
			struct s_section_info_block
			{
				__int32 section_index;//0x0
				__int32 pca_data_offset;//0x4
			};
			TAG_BLOCK_SIZE_ASSERT(s_section_info_block, 0x8);
			tag_block<s_section_info_block> section_info;//0x4
		};
		TAG_BLOCK_SIZE_ASSERT(s_lod_info_block, 0xC);
		tag_block<s_lod_info_block> lod_info;//0x14
		struct s_cluster_basis_block
		{
			float basis_data;//0x0
		};
		TAG_BLOCK_SIZE_ASSERT(s_cluster_basis_block, 0x4);
		tag_block<s_cluster_basis_block> cluster_basis;//0x1C
		struct s_raw_pca_data_block
		{
			float raw_pca_data;//0x0
		};
		TAG_BLOCK_SIZE_ASSERT(s_raw_pca_data_block, 0x4);
		tag_block<s_raw_pca_data_block> raw_pca_data;//0x24
		struct s_vertex_buffers_block
		{
			PAD(0x20);//0x0
		};
		TAG_BLOCK_SIZE_ASSERT(s_vertex_buffers_block, 0x20);
		tag_block<s_vertex_buffers_block> vertex_buffers;//0x2C
		__int32 block_offset;//0x34
		__int32 block_size;//0x38
		__int32 section_data_size;//0x3C
		__int32 resource_data_size;//0x40
		struct s_resources_block
		{
			enum class e_type : __int8
			{
				tag_block = 0,
				tag_data = 1,
				vertex_buffer = 2,
			};
			e_type type;//0x0
			PAD(0x3);//0x1
			__int16 primary_locator;//0x4
			__int16 secondary_locator;//0x6
			__int32 resource_data_size;//0x8
			__int32 resource_data_offset;//0xC
		};
		TAG_BLOCK_SIZE_ASSERT(s_resources_block, 0x10);
		tag_block<s_resources_block> resources;//0x44
		PAD(0x4);//0x4C
		__int16 owner_tag_section_offset;//0x50
		PAD(0x6);//0x52
	};
	TAG_BLOCK_SIZE_ASSERT(s_prt_info_block, 0x58);
	tag_block<s_prt_info_block> prt_info;//0x74
	struct s_section_render_leaves_block
	{
		struct s_node_render_leaves_block
		{
			struct s_collision_leaves_block
			{
				__int16 cluster;//0x0
				__int16 surface_reference_count;//0x2
				__int32 first_surface_reference_index;//0x4
			};
			TAG_BLOCK_SIZE_ASSERT(s_collision_leaves_block, 0x8);
			tag_block<s_collision_leaves_block> collision_leaves;//0x0
			struct s_surface_references_block
			{
				__int16 strip_index;//0x0
				__int16 lightmap_triangle_index;//0x2
				__int32 bsp_node_index;//0x4
			};
			TAG_BLOCK_SIZE_ASSERT(s_surface_references_block, 0x8);
			tag_block<s_surface_references_block> surface_references;//0x8
		};
		TAG_BLOCK_SIZE_ASSERT(s_node_render_leaves_block, 0x10);
		tag_block<s_node_render_leaves_block> node_render_leaves;//0x0
	};
	TAG_BLOCK_SIZE_ASSERT(s_section_render_leaves_block, 0x8);
	tag_block<s_section_render_leaves_block> section_render_leaves;//0x7C

};
TAG_GROUP_SIZE_ASSERT(s_render_model_group_definition, 0x84);
#pragma pack(pop)