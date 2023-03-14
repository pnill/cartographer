#pragma once
#include "Blam\Cache\DataTypes\BlamDataTypes.h"
#include "Blam\Cache\TagGroups.h"
#include "Blam\Math\real_math.h"

/*********************************************************************
* name: scenario_structure_lightmap
* group_tag : ltmp
* header size : 260
* *********************************************************************/
struct s_scenario_structure_lightmap_group_definition :TagGroup<'ltmp'>
{
	float search_distance_lower_bound;//0x0
	float search_distance_upper_bound;//0x4
	float luminels_per_world_unit;//0x8
	float output_white_reference;//0xC
	float output_black_reference;//0x10
	float output_schlick_parameter;//0x14
	float diffuse_map_scale;//0x18
	float sun_scale;//0x1C
	float sky_scale;//0x20
	float indirect_scale;//0x24
	float prt_scale;//0x28
	float surface_light_scale;//0x2C
	float scenario_light_scale;//0x30
	float lightprobe_interpolation_overide;//0x34
	PAD(0x48);//0x38
	struct s_lightmap_groups_block
	{
		enum class e_type : __int16
		{
			normal = 0,
		};
		e_type type;//0x0
		enum class e_flags : __int16
		{
			unused = FLAG(0),
		};
		e_flags flags;//0x2
		__int32 structure_checksum;//0x4
		struct s_section_palette_block
		{
			__int32 first_palette_color;//0x0
			PAD(0x3FC);//0x4
		};
		TAG_BLOCK_SIZE_ASSERT(s_section_palette_block, 0x400);
		tag_block<s_section_palette_block> section_palette;//0x8
		struct s_writable_palettes_block
		{
			__int32 first_palette_color;//0x0
			PAD(0x3FC);//0x4
		};
		TAG_BLOCK_SIZE_ASSERT(s_writable_palettes_block, 0x400);
		tag_block<s_writable_palettes_block> writable_palettes;//0x10
		tag_reference bitmap_group;//0x18
		struct s_clusters_block
		{
			__int16 total_vertex_count;//0x0
			__int16 total_triangle_count;//0x2
			__int16 total_part_count;//0x4
			__int16 shadowcasting_triangle_count;//0x6
			__int16 shadowcasting_part_count;//0x8
			__int16 opaque_point_count;//0xA
			__int16 opaque_vertex_count;//0xC
			__int16 opaque_part_count;//0xE
			__int8 opaque_max_nodesvertex;//0x10
			__int8 transparent_max_nodesvertex;//0x11
			__int16 shadowcasting_rigid_triangle_count;//0x12
			enum class e_geometry_classification : __int16
			{
				worldspace = 0,
				rigid = 1,
				rigid_boned = 2,
				skinned = 3,
				unsupported_reimport = 4,
			};
			e_geometry_classification geometry_classification;//0x14
			enum class e_geometry_compression_flags : __int16
			{
				compressed_position = FLAG(0),
				compressed_texcoord = FLAG(1),
				compressed_secondary_texcoord = FLAG(2),
			};
			e_geometry_compression_flags geometry_compression_flags;//0x16
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
			tag_block<s_NUM__block> NUM_;//0x18
			__int8 hardware_node_count;//0x20
			__int8 node_map_size;//0x21
			__int16 software_plane_count;//0x22
			__int16 total_subpart_cont;//0x24
			enum class e_section_lighting_flags : __int16
			{
				has_lm_texcoords = FLAG(0),
				has_lm_inc_rad = FLAG(1),
				has_lm_colors = FLAG(2),
				has_lm_prt = FLAG(3),
			};
			e_section_lighting_flags section_lighting_flags;//0x26
			__int32 block_offset;//0x28
			__int32 block_size;//0x2C
			__int32 section_data_size;//0x30
			__int32 resource_data_size;//0x34
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
			tag_block<s_resources_block> resources;//0x38
			PAD(0x4);//0x40
			__int16 owner_tag_section_offset;//0x44
			PAD(0x6);//0x46
			struct s_cache_data_block
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
					__int8 node_index_0;//0x1C
					__int8 node_index_1;//0x1D
					__int8 node_index_2;//0x1E
					__int8 node_index_3;//0x1F
					float node_weight_0;//0x20
					float node_weight_1;//0x24
					float node_weight_2;//0x28
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
					__int32 node_index_old_0;//0xC
					__int32 node_index_old_1;//0x10
					__int32 node_index_old_2;//0x14
					__int32 node_index_old_3;//0x18
					float node_weight_0;//0x1C
					float node_weight_1;//0x20
					float node_weight_2;//0x24
					float node_weight_3;//0x28
					__int32 node_index_new_0;//0x2C
					__int32 node_index_new_1;//0x30
					__int32 node_index_new_2;//0x34
					__int32 node_index_new_3;//0x38
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
			};
			TAG_BLOCK_SIZE_ASSERT(s_cache_data_block, 0x44);
			tag_block<s_cache_data_block> cache_data;//0x4C
		};
		TAG_BLOCK_SIZE_ASSERT(s_clusters_block, 0x54);
		tag_block<s_clusters_block> clusters;//0x20
		struct s_cluster_render_info_block
		{
			__int16 bitmap_index;//0x0
			__int8 palette_index;//0x2
			PAD(0x1);//0x3
		};
		TAG_BLOCK_SIZE_ASSERT(s_cluster_render_info_block, 0x4);
		tag_block<s_cluster_render_info_block> cluster_render_info;//0x28
		struct s_poop_definitions_block
		{
			__int16 total_vertex_count;//0x0
			__int16 total_triangle_count;//0x2
			__int16 total_part_count;//0x4
			__int16 shadowcasting_triangle_count;//0x6
			__int16 shadowcasting_part_count;//0x8
			__int16 opaque_point_count;//0xA
			__int16 opaque_vertex_count;//0xC
			__int16 opaque_part_count;//0xE
			__int8 opaque_max_nodesvertex;//0x10
			__int8 transparent_max_nodesvertex;//0x11
			__int16 shadowcasting_rigid_triangle_count;//0x12
			enum class e_geometry_classification : __int16
			{
				worldspace = 0,
				rigid = 1,
				rigid_boned = 2,
				skinned = 3,
				unsupported_reimport = 4,
			};
			e_geometry_classification geometry_classification;//0x14
			enum class e_geometry_compression_flags : __int16
			{
				compressed_position = FLAG(0),
				compressed_texcoord = FLAG(1),
				compressed_secondary_texcoord = FLAG(2),
			};
			e_geometry_compression_flags geometry_compression_flags;//0x16
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
			tag_block<s_NUM__block> NUM_;//0x18
			__int8 hardware_node_count;//0x20
			__int8 node_map_size;//0x21
			__int16 software_plane_count;//0x22
			__int16 total_subpart_cont;//0x24
			enum class e_section_lighting_flags : __int16
			{
				has_lm_texcoords = FLAG(0),
				has_lm_inc_rad = FLAG(1),
				has_lm_colors = FLAG(2),
				has_lm_prt = FLAG(3),
			};
			e_section_lighting_flags section_lighting_flags;//0x26
			__int32 block_offset;//0x28
			__int32 block_size;//0x2C
			__int32 section_data_size;//0x30
			__int32 resource_data_size;//0x34
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
			tag_block<s_resources_block> resources;//0x38
			PAD(0x4);//0x40
			__int16 owner_tag_section_offset;//0x44
			PAD(0x6);//0x46
			struct s_cache_data_block
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
					__int8 node_index_0;//0x1C
					__int8 node_index_1;//0x1D
					__int8 node_index_2;//0x1E
					__int8 node_index_3;//0x1F
					float node_weight_0;//0x20
					float node_weight_1;//0x24
					float node_weight_2;//0x28
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
					__int32 node_index_old_0;//0xC
					__int32 node_index_old_1;//0x10
					__int32 node_index_old_2;//0x14
					__int32 node_index_old_3;//0x18
					float node_weight_0;//0x1C
					float node_weight_1;//0x20
					float node_weight_2;//0x24
					float node_weight_3;//0x28
					__int32 node_index_new_0;//0x2C
					__int32 node_index_new_1;//0x30
					__int32 node_index_new_2;//0x34
					__int32 node_index_new_3;//0x38
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
			};
			TAG_BLOCK_SIZE_ASSERT(s_cache_data_block, 0x44);
			tag_block<s_cache_data_block> cache_data;//0x4C
		};
		TAG_BLOCK_SIZE_ASSERT(s_poop_definitions_block, 0x54);
		tag_block<s_poop_definitions_block> poop_definitions;//0x30
		struct s_lighting_environments_block
		{
			float sample_point_x;//0x0
			float sample_point_y;//0x4
			float sample_point_z;//0x8
			float red_coefficient_0;//0xC
			float red_coefficient_1;//0x10
			float red_coefficient_2;//0x14
			float red_coefficient_3;//0x18
			float red_coefficient_4;//0x1C
			float red_coefficient_5;//0x20
			float red_coefficient_6;//0x24
			float red_coefficient_7;//0x28
			float red_coefficient_8;//0x2C
			float green_coefficient_0;//0x30
			float green_coefficient_1;//0x34
			float green_coefficient_2;//0x38
			float green_coefficient_3;//0x3C
			float green_coefficient_4;//0x40
			float green_coefficient_5;//0x44
			float green_coefficient_6;//0x48
			float green_coefficient_7;//0x4C
			float green_coefficient_8;//0x50
			float blue_coefficient_0;//0x54
			float blue_coefficient_1;//0x58
			float blue_coefficient_2;//0x5C
			float blue_coefficient_3;//0x60
			float blue_coefficient_4;//0x64
			float blue_coefficient_5;//0x68
			float blue_coefficient_6;//0x6C
			float blue_coefficient_7;//0x70
			float blue_coefficient_8;//0x74
			PAD(0xC);//0x78
			float incoming_light_intensity_x;//0x84
			float incoming_light_intensity_y;//0x88
			float incoming_light_intensity_z;//0x8C
			__int32 specular_bitmap_index;//0x90
			PAD(0xC);//0x94
			float rotation_speed;//0xA0
			PAD(0xC);//0xA4
			real_color_rgb color_tint;//0xB0
			enum class e_procedural_overide : __int16
			{
				no_overide = 0,
				cie_clear_sky = 1,
				cie_partly_cloudy = 2,
				cie_cloudy = 3,
				directional_light = 4,
				cone_light = 5,
				sphere_light = 6,
				hemisphere_light = 7,
			};
			e_procedural_overide procedural_overide;//0xBC
			enum class e_flags : __int16
			{
				leave_me_alone_please = FLAG(0),
			};
			e_flags flags;//0xBE
			PAD(0x18);//0xC0
			float procedural_param1w;//0xD8
		};
		TAG_BLOCK_SIZE_ASSERT(s_lighting_environments_block, 0xDC);
		tag_block<s_lighting_environments_block> lighting_environments;//0x38
		struct s_geometry_buckets_block
		{
			enum class e_flags : __int16
			{
				incident_direction = FLAG(0),
				color = FLAG(1),
			};
			e_flags flags;//0x0
			PAD(0x2);//0x2
			struct s_raw_vertices_block
			{
				real_color_rgb primary_lightmap_color;//0x0
				PAD(0xC);//0xC
			};
			TAG_BLOCK_SIZE_ASSERT(s_raw_vertices_block, 0x18);
			tag_block<s_raw_vertices_block> raw_vertices;//0x4
			__int32 block_offset;//0xC
			__int32 block_size;//0x10
			__int32 section_data_size;//0x14
			__int32 resource_data_size;//0x18
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
			tag_block<s_resources_block> resources;//0x1C
			PAD(0x4);//0x24
			__int16 owner_tag_section_offset;//0x28
			PAD(0x6);//0x2A
			struct s_cache_data_block
			{
				struct s_vertex_buffers_block
				{
					PAD(0x20);//0x0
				};
				TAG_BLOCK_SIZE_ASSERT(s_vertex_buffers_block, 0x20);
				tag_block<s_vertex_buffers_block> vertex_buffers;//0x0
			};
			TAG_BLOCK_SIZE_ASSERT(s_cache_data_block, 0x8);
			tag_block<s_cache_data_block> cache_data;//0x30
		};
		TAG_BLOCK_SIZE_ASSERT(s_geometry_buckets_block, 0x38);
		tag_block<s_geometry_buckets_block> geometry_buckets;//0x40
		struct s_instance_render_info_block
		{
			__int16 bitmap_index;//0x0
			__int8 palette_index;//0x2
			PAD(0x1);//0x3
		};
		TAG_BLOCK_SIZE_ASSERT(s_instance_render_info_block, 0x4);
		tag_block<s_instance_render_info_block> instance_render_info;//0x48
		struct s_instance_bucket_refs_block
		{
			__int16 flags;//0x0
			__int16 bucket_index;//0x2
			struct s_section_offsets_block
			{
				__int16 section_offset;//0x0
			};
			TAG_BLOCK_SIZE_ASSERT(s_section_offsets_block, 0x2);
			tag_block<s_section_offsets_block> section_offsets;//0x4
		};
		TAG_BLOCK_SIZE_ASSERT(s_instance_bucket_refs_block, 0xC);
		tag_block<s_instance_bucket_refs_block> instance_bucket_refs;//0x50
		struct s_scenery_object_info_block
		{
			__int32 unique_id;//0x0
			__int16 origin_bsp_index;//0x4
			__int8 type;//0x6
			__int8 source;//0x7
			__int32 render_model_checksum;//0x8
		};
		TAG_BLOCK_SIZE_ASSERT(s_scenery_object_info_block, 0xC);
		tag_block<s_scenery_object_info_block> scenery_object_info;//0x58
		struct s_scenery_object_bucket_refs_block
		{
			__int16 flags;//0x0
			__int16 bucket_index;//0x2
			struct s_section_offsets_block
			{
				__int16 section_offset;//0x0
			};
			TAG_BLOCK_SIZE_ASSERT(s_section_offsets_block, 0x2);
			tag_block<s_section_offsets_block> section_offsets;//0x4
		};
		TAG_BLOCK_SIZE_ASSERT(s_scenery_object_bucket_refs_block, 0xC);
		tag_block<s_scenery_object_bucket_refs_block> scenery_object_bucket_refs;//0x60
	};
	TAG_BLOCK_SIZE_ASSERT(s_lightmap_groups_block, 0x68);
	tag_block<s_lightmap_groups_block> lightmap_groups;//0x80
	PAD(0xC);//0x88
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
				__int8 node_index_0;//0xC
				__int8 node_index_1;//0xD
				__int8 node_index_2;//0xE
				__int8 node_index_3;//0xF
				float node_weight_0;//0x10
				float node_weight_1;//0x14
				float node_weight_2;//0x18
				float node_weight_3;//0x1C
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
				__int8 node_index_0;//0xC
				__int8 node_index_1;//0xD
				__int8 node_index_2;//0xE
				__int8 node_index_3;//0xF
				float node_weight_0;//0x10
				float node_weight_1;//0x14
				float node_weight_2;//0x18
				float node_weight_3;//0x1C
				real_color_argb color;//0x20
				PAD(0xC);//0x30
				float screen_length;//0x3C
			};
			TAG_BLOCK_SIZE_ASSERT(s_vectors_block, 0x40);
			tag_block<s_vectors_block> vectors;//0x38
			struct s_lines_block
			{
				struct s_point_block
				{
					real_point3d position;
					__int8 node_0;
					__int8 node_1;
					__int8 node_2;
					__int8 node_3;
					float node_weight_1;
					float node_weight_2;
					float node_weight_3;
					float node_weight_4;
				};
				s_point_block point_1;
				s_point_block point_2;
				float node_weight_0_C;//0x40
				float node_weight_1_C;//0x44
				float node_weight_2_C;//0x48
				float node_weight_3_C;//0x4C
			};
			TAG_BLOCK_SIZE_ASSERT(s_lines_block, 0x50);
			tag_block<s_lines_block> lines;//0x40
			struct s_triangles_block
			{
				struct s_point_block
				{
					real_point3d position;
					__int8 node_0;
					__int8 node_1;
					__int8 node_2;
					__int8 node_3;
					float node_weight_1;
					float node_weight_2;
					float node_weight_3;
					float node_weight_4;
				};
				s_point_block point_1;
				s_point_block point_2;
				s_point_block point_3;
				float node_weight_0;//0x60
				float node_weight_1;//0x64
				float node_weight_2;//0x68
				float node_weight_3;//0x6C
			};
			TAG_BLOCK_SIZE_ASSERT(s_triangles_block, 0x70);
			tag_block<s_triangles_block> triangles;//0x48
			struct s_quads_block
			{
				struct s_point_block
				{
					real_point3d position;
					__int8 node_0;
					__int8 node_1;
					__int8 node_2;
					__int8 node_3;
					float node_weight_1;
					float node_weight_2;
					float node_weight_3;
					float node_weight_4;
				};
				s_point_block point_1;
				s_point_block point_2;
				s_point_block point_3;
				s_point_block point_4;
				float node_weight_0;//0x80
				float node_weight_1;//0x84
				float node_weight_2;//0x88
				float node_weight_3;//0x8C
			};
			TAG_BLOCK_SIZE_ASSERT(s_quads_block, 0x90);
			tag_block<s_quads_block> quads;//0x50
			struct s_comments_block
			{
				data_block text;//0x0
				float position_x;//0x8
				float position_y;//0xC
				float position_z;//0x10
				__int8 node_index_0;//0x14
				__int8 node_index_1;//0x15
				__int8 node_index_2;//0x16
				__int8 node_index_3;//0x17
				float node_weight_0;//0x18
				float node_weight_1;//0x1C
				float node_weight_2;//0x20
				float node_weight_3;//0x24
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
	tag_block<s_errors_block> errors;//0x94
	PAD(0x68);//0x9C
};
TAG_GROUP_SIZE_ASSERT(s_scenario_structure_lightmap_group_definition, 0x104);
