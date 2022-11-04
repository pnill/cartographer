#pragma once
#include "Blam\Cache\DataTypes\BlamDataTypes.h"
#include "Blam\Cache\DataTypes\BlamPrimitiveType.h"
#include "Blam\Common\Common.h"
#include "Blam\Math\real_math.h"

/*********************************************************************
* name: scenario_structure_bsp
* group_tag : sbsp
* header size : 572
* *********************************************************************/
struct s_scenario_structure_bsp_group_definition :TagGroup<'sbsp'>
{
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
	tag_block<s_import_info_block> import_info;//0x0
	__int32 NUM_;//0x8
	struct s_collision_materials_block
	{
		tag_reference old_shader;//0x0
		PAD(0x2);//0x8
		__int16 conveyor_surface_index;//0xA
		tag_reference new_shader;//0xC
	};
	TAG_BLOCK_SIZE_ASSERT(s_collision_materials_block, 0x14);
	tag_block<s_collision_materials_block> collision_materials;//0xC
	struct s_collision_bsp_block
	{
		struct s_bsp_3d_nodes_block
		{
			PAD(0x8);//0x0
		};
		TAG_BLOCK_SIZE_ASSERT(s_bsp_3d_nodes_block, 0x8);
		tag_block<s_bsp_3d_nodes_block> bsp_3d_nodes;//0x0
		struct s_planes_block
		{
			real_plane3d plane_distance;
		};
		TAG_BLOCK_SIZE_ASSERT(s_planes_block, 0x10);
		tag_block<s_planes_block> planes;//0x8
		struct s_leaves_block
		{
			enum class e_flags : __int8
			{
				contains_doublesided_surfaces = FLAG(0),
			};
			e_flags flags;//0x0
			__int8 bsp_2d_reference_count;//0x1
			__int16 first_bsp_2d_reference;//0x2
		};
		TAG_BLOCK_SIZE_ASSERT(s_leaves_block, 0x4);
		tag_block<s_leaves_block> leaves;//0x10
		struct s_bsp_2d_references_block
		{
			__int16 plane;//0x0
			__int16 bsp_2d_node;//0x2
		};
		TAG_BLOCK_SIZE_ASSERT(s_bsp_2d_references_block, 0x4);
		tag_block<s_bsp_2d_references_block> bsp_2d_references;//0x18
		struct s_bsp_2d_nodes_block
		{
			float plane_normal_i;//0x0
			float plane_normal_j;//0x4
			float plane_distance;//0x8
			__int16 left_child;//0xC
			__int16 right_child;//0xE
		};
		TAG_BLOCK_SIZE_ASSERT(s_bsp_2d_nodes_block, 0x10);
		tag_block<s_bsp_2d_nodes_block> bsp_2d_nodes;//0x20
		struct s_surfaces_block
		{
			__int16 plane;//0x0
			__int16 first_edge;//0x2
			enum class e_flags : __int8
			{
				twosided = FLAG(0),
				invisible = FLAG(1),
				climbable = FLAG(2),
				breakable = FLAG(3),
				invalid = FLAG(4),
				conveyor = FLAG(5),
			};
			e_flags flags;//0x4
			__int8 breakable_surface;//0x5
			__int16 material;//0x6
		};
		TAG_BLOCK_SIZE_ASSERT(s_surfaces_block, 0x8);
		tag_block<s_surfaces_block> surfaces;//0x28
		struct s_edges_block
		{
			__int16 start_vertex;//0x0
			__int16 end_vertex;//0x2
			__int16 forward_edge;//0x4
			__int16 reverse_edge;//0x6
			__int16 left_surface;//0x8
			__int16 right_surface;//0xA
		};
		TAG_BLOCK_SIZE_ASSERT(s_edges_block, 0xC);
		tag_block<s_edges_block> edges;//0x30
		struct s_vertices_block
		{
			real_point3d point;
			__int16 first_edge;//0xC
			PAD(0x2);//0xE
		};
		TAG_BLOCK_SIZE_ASSERT(s_vertices_block, 0x10);
		tag_block<s_vertices_block> vertices;//0x38
	};
	TAG_BLOCK_SIZE_ASSERT(s_collision_bsp_block, 0x40);
	tag_block<s_collision_bsp_block> collision_bsp;//0x14
	float vehicle_floor;//0x1C
	float vehicle_ceiling;//0x20
	struct s_unused_nodes_block
	{
		PAD(0x6);//0x0
	};
	TAG_BLOCK_SIZE_ASSERT(s_unused_nodes_block, 0x6);
	tag_block<s_unused_nodes_block> unused_nodes;//0x24
	struct s_leaves_block
	{
		__int16 cluster;//0x0
		__int16 surface_reference_count;//0x2
		__int32 first_surface_reference_index;//0x4
	};
	TAG_BLOCK_SIZE_ASSERT(s_leaves_block, 0x8);
	tag_block<s_leaves_block> leaves;//0x2C
	float world_bounds_x_lower;//0x34
	float world_bounds_x_upper;//0x38
	float world_bounds_y_lower;//0x3C
	float world_bounds_y_upper;//0x40
	float world_bounds_z_lower;//0x44
	float world_bounds_z_upper;//0x48
	struct s_surface_references_block
	{
		__int16 strip_index;//0x0
		__int16 lightmap_triangle_index;//0x2
		__int32 bsp_node_index;//0x4
	};
	TAG_BLOCK_SIZE_ASSERT(s_surface_references_block, 0x8);
	tag_block<s_surface_references_block> surface_references;//0x4C
	data_block cluster_data;//0x54
	struct s_cluster_portals_block
	{
		__int16 back_cluster;//0x0
		__int16 front_cluster;//0x2
		__int32 plane_index;//0x4
		float centroid_x;//0x8
		float centroid_y;//0xC
		float centroid_z;//0x10
		float bounding_radius;//0x14
		enum class e_flags : __int32
		{
			ai_cannot_hear_through_this = FLAG(0),
			oneway = FLAG(1),
			door = FLAG(2),
			noway = FLAG(3),
			oneway_reversed = FLAG(4),
			no_one_can_hear_through_this = FLAG(5),
		};
		e_flags flags;//0x18
		struct s_vertices_block
		{
			float point_x;//0x0
			float point_y;//0x4
			float point_z;//0x8
		};
		TAG_BLOCK_SIZE_ASSERT(s_vertices_block, 0xC);
		tag_block<s_vertices_block> vertices;//0x1C
	};
	TAG_BLOCK_SIZE_ASSERT(s_cluster_portals_block, 0x24);
	tag_block<s_cluster_portals_block> cluster_portals;//0x5C
	struct s_fog_planes_block
	{
		__int16 scenario_planar_fog_index;//0x0
		PAD(0xE);//0x2
		float plane_distance;//0x10
		enum class e_flags : __int16
		{
			extend_infinitely_while_visible = FLAG(0),
			do_not_floodfill = FLAG(1),
			aggressive_floodfill = FLAG(2),
		};
		e_flags flags;//0x14
		__int16 priority;//0x16
	};
	TAG_BLOCK_SIZE_ASSERT(s_fog_planes_block, 0x18);
	tag_block<s_fog_planes_block> fog_planes;//0x64
	PAD(0x18);//0x6C
	struct s_weather_palette_block
	{
		tag_string32 name;//0x0
		tag_reference weather_system;//0x20
		PAD(0x24);//0x28
		tag_reference wind;//0x4C
		PAD(0xC);//0x54
		float wind_magnitude;//0x60
		PAD(0x4);//0x64
		tag_string32 wind_scale_function;//0x68
	};
	TAG_BLOCK_SIZE_ASSERT(s_weather_palette_block, 0x88);
	tag_block<s_weather_palette_block> weather_palette;//0x84
	struct s_weather_polyhedra_block
	{
		float bounding_sphere_center_x;//0x0
		float bounding_sphere_center_y;//0x4
		float bounding_sphere_center_z;//0x8
		float bounding_sphere_radius;//0xC
		struct s_planes_block
		{
			PAD(0xC);//0x0
			float plane_distance;//0xC
		};
		TAG_BLOCK_SIZE_ASSERT(s_planes_block, 0x10);
		tag_block<s_planes_block> planes;//0x10
	};
	TAG_BLOCK_SIZE_ASSERT(s_weather_polyhedra_block, 0x18);
	tag_block<s_weather_polyhedra_block> weather_polyhedra;//0x8C
	struct s_detail_objects_block
	{
		struct s_cells_block
		{
			__int16 NUM_1;//0x0
			__int16 NUM_2;//0x2
			__int16 NUM_3;//0x4
			__int16 NUM_4;//0x6
			__int32 NUM_5;//0x8
			__int32 NUM_6;//0xC
			__int32 NUM_7;//0x10
			PAD(0xC);//0x14
		};
		TAG_BLOCK_SIZE_ASSERT(s_cells_block, 0x20);
		tag_block<s_cells_block> cells;//0x0
		struct s_instances_block
		{
			__int8 NUM_1;//0x0
			__int8 NUM_2;//0x1
			__int8 NUM_3;//0x2
			__int8 NUM_4;//0x3
			__int16 NUM_5;//0x4
		};
		TAG_BLOCK_SIZE_ASSERT(s_instances_block, 0x6);
		tag_block<s_instances_block> instances;//0x8
		struct s_counts_block
		{
			__int16 NUM_;//0x0
		};
		TAG_BLOCK_SIZE_ASSERT(s_counts_block, 0x2);
		tag_block<s_counts_block> counts;//0x10
		struct s_z_reference_vectors_block
		{
			float NUM_1;//0x0
			float NUM_2;//0x4
			float NUM_3;//0x8
			float NUM_4;//0xC
		};
		TAG_BLOCK_SIZE_ASSERT(s_z_reference_vectors_block, 0x10);
		tag_block<s_z_reference_vectors_block> z_reference_vectors;//0x18
		PAD(0x4);//0x20
	};
	TAG_BLOCK_SIZE_ASSERT(s_detail_objects_block, 0x24);
	tag_block<s_detail_objects_block> detail_objects;//0x94
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
		struct s_cluster_data_block
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
		TAG_BLOCK_SIZE_ASSERT(s_cluster_data_block, 0x44);
		tag_block<s_cluster_data_block> cluster_data;//0x4C
		float bounds_x_lower;//0x54
		float bounds_x_upper;//0x58
		float bounds_y_lower;//0x5C
		float bounds_y_upper;//0x60
		float bounds_z_lower;//0x64
		float bounds_z_upper;//0x68
		__int8 scenario_sky_index;//0x6C
		__int8 media_index;//0x6D
		__int8 scenario_visible_sky_index;//0x6E
		__int8 scenario_atmospheric_fog_index;//0x6F
		__int8 planar_fog_designator;//0x70
		__int8 visible_fog_plane_index;//0x71
		__int16 background_sound;//0x72
		__int16 sound_environment;//0x74
		__int16 weather;//0x76
		__int16 transition_structure_bsp;//0x78
		PAD(0x6);//0x7A
		enum class e_flags : __int16
		{
			oneway_portal = FLAG(0),
			door_portal = FLAG(1),
			postprocessed_geometry = FLAG(2),
			is_the_sky = FLAG(3),
		};
		e_flags flags;//0x80
		PAD(0x2);//0x82
		struct s_predicted_resources_block
		{
			enum class e_type : __int16
			{
				bitmap = 0,
				sound = 1,
				render_model_geometry = 2,
				cluster_geometry = 3,
				cluster_instanced_geometry = 4,
				lightmap_geometry_object_buckets = 5,
				lightmap_geometry_instance_buckets = 6,
				lightmap_cluster_bitmaps = 7,
				lightmap_instance_bitmaps = 8,
			};
			e_type type;//0x0
			__int16 resource_index;//0x2
			__int32 tag_index;//0x4
		};
		TAG_BLOCK_SIZE_ASSERT(s_predicted_resources_block, 0x8);
		tag_block<s_predicted_resources_block> predicted_resources;//0x84
		struct s_portals_block
		{
			__int16 portal_index;//0x0
		};
		TAG_BLOCK_SIZE_ASSERT(s_portals_block, 0x2);
		tag_block<s_portals_block> portals;//0x8C
		__int32 checksum_from_structure;//0x94
		struct s_instanced_geometry_indices_block
		{
			__int16 instanced_geometry_index;//0x0
		};
		TAG_BLOCK_SIZE_ASSERT(s_instanced_geometry_indices_block, 0x2);
		tag_block<s_instanced_geometry_indices_block> instanced_geometry_indices;//0x98
		struct s_index_reorder_table_block
		{
			__int16 index;//0x0
		};
		TAG_BLOCK_SIZE_ASSERT(s_index_reorder_table_block, 0x2);
		tag_block<s_index_reorder_table_block> index_reorder_table;//0xA0
		data_block collision_mopp_code;//0xA8
	};
	TAG_BLOCK_SIZE_ASSERT(s_clusters_block, 0xB0);
	tag_block<s_clusters_block> clusters;//0x9C
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
	tag_block<s_materials_block> materials;//0xA4
	struct s_sky_owner_cluster_block
	{
		__int16 cluster_owner;//0x0
	};
	TAG_BLOCK_SIZE_ASSERT(s_sky_owner_cluster_block, 0x2);
	tag_block<s_sky_owner_cluster_block> sky_owner_cluster;//0xAC
	struct s_conveyor_surfaces_block
	{
		PAD(0x18);//0x0
	};
	TAG_BLOCK_SIZE_ASSERT(s_conveyor_surfaces_block, 0x18);
	tag_block<s_conveyor_surfaces_block> conveyor_surfaces;//0xB4
	struct s_breakable_surfaces_block
	{
		__int16 instanced_geometry_instance;//0x0
		__int16 breakable_surface_index;//0x2
		float centroid_x;//0x4
		float centroid_y;//0x8
		float centroid_z;//0xC
		float radius;//0x10
		__int32 collision_surface_index;//0x14
	};
	TAG_BLOCK_SIZE_ASSERT(s_breakable_surfaces_block, 0x18);
	tag_block<s_breakable_surfaces_block> breakable_surfaces;//0xBC
	struct s_pathfinding_data_block
	{
		struct s_sectors_block
		{
			enum class e_pathfinding_sector_flags : __int16
			{
				sector_walkable = FLAG(0),
				sector_breakable = FLAG(1),
				sector_mobile = FLAG(2),
				sector_bsp_source = FLAG(3),
				floor = FLAG(4),
				ceiling = FLAG(5),
				wall_north = FLAG(6),
				wall_south = FLAG(7),
				wall_east = FLAG(8),
				wall_west = FLAG(9),
				crouchable = FLAG(10),
				aligned = FLAG(11),
				sector_step = FLAG(12),
				sector_interior = FLAG(13),
			};
			e_pathfinding_sector_flags pathfinding_sector_flags;//0x0
			__int16 hint_index;//0x2
			__int32 first_link_do_not_set_manually;//0x4
		};
		TAG_BLOCK_SIZE_ASSERT(s_sectors_block, 0x8);
		tag_block<s_sectors_block> sectors;//0x0
		struct s_links_block
		{
			__int16 vertex_1;//0x0
			__int16 vertex_2;//0x2
			enum class e_link_flags : __int16
			{
				sector_link_from_collision_edge = FLAG(0),
				sector_intersection_link = FLAG(1),
				sector_link_bsp2d_creation_error = FLAG(2),
				sector_link_topology_error = FLAG(3),
				sector_link_chain_error = FLAG(4),
				sector_link_both_sectors_walkable = FLAG(5),
				sector_link_magic_hanging_link = FLAG(6),
				sector_link_threshold = FLAG(7),
				sector_link_crouchable = FLAG(8),
				sector_link_wall_base = FLAG(9),
				sector_link_ledge = FLAG(10),
				sector_link_leanable = FLAG(11),
				sector_link_start_corner = FLAG(12),
				sector_link_end_corner = FLAG(13),
			};
			e_link_flags link_flags;//0x4
			__int16 hint_index;//0x6
			__int16 forward_link;//0x8
			__int16 reverse_link;//0xA
			__int16 left_sector;//0xC
			__int16 right_sector;//0xE
		};
		TAG_BLOCK_SIZE_ASSERT(s_links_block, 0x10);
		tag_block<s_links_block> links;//0x8
		struct s_refs_block
		{
			__int32 node_ref_or_sector_ref;//0x0
		};
		TAG_BLOCK_SIZE_ASSERT(s_refs_block, 0x4);
		tag_block<s_refs_block> refs;//0x10
		struct s_bsp2d_nodes_block
		{
			float plane_normal_i;//0x0
			float plane_normal_j;//0x4
			float plane_distance;//0x8
			__int32 left_child;//0xC
			__int32 right_child;//0x10
		};
		TAG_BLOCK_SIZE_ASSERT(s_bsp2d_nodes_block, 0x14);
		tag_block<s_bsp2d_nodes_block> bsp2d_nodes;//0x18
		struct s_surface_flags_block
		{
			__int32 flags;//0x0
		};
		TAG_BLOCK_SIZE_ASSERT(s_surface_flags_block, 0x4);
		tag_block<s_surface_flags_block> surface_flags;//0x20
		struct s_vertices_block
		{
			float point_x;//0x0
			float point_y;//0x4
			float point_z;//0x8
		};
		TAG_BLOCK_SIZE_ASSERT(s_vertices_block, 0xC);
		tag_block<s_vertices_block> vertices;//0x28
		struct s_object_refs_block
		{
			enum class e_flags : __int16
			{
				mobile = FLAG(0),
			};
			e_flags flags;//0x0
			PAD(0x2);//0x2
			__int32 first_sector;//0x4
			__int32 last_sector;//0x8
			struct s_bsps_block
			{
				__int32 bsp_reference;//0x0
				__int32 first_sector;//0x4
				__int32 last_sector;//0x8
				__int16 node_index;//0xC
				PAD(0x2);//0xE
			};
			TAG_BLOCK_SIZE_ASSERT(s_bsps_block, 0x10);
			tag_block<s_bsps_block> bsps;//0xC
			struct s_nodes_block
			{
				__int16 reference_frame_index;//0x0
				__int8 projection_axis;//0x2
				enum class e_projection_sign : __int8
				{
					projection_sign = FLAG(0),
				};
				e_projection_sign projection_sign;//0x3
			};
			TAG_BLOCK_SIZE_ASSERT(s_nodes_block, 0x4);
			tag_block<s_nodes_block> nodes;//0x14
		};
		TAG_BLOCK_SIZE_ASSERT(s_object_refs_block, 0x1C);
		tag_block<s_object_refs_block> object_refs;//0x30
		struct s_pathfinding_hints_block
		{
			enum class e_hint_type : __int16
			{
				intersection_link = 0,
				jump_link = 1,
				climb_link = 2,
				vault_link = 3,
				mount_link = 4,
				hoist_link = 5,
				wall_jump_link = 6,
				breakable_floor = 7,
			};
			e_hint_type hint_type;//0x0
			__int16 next_hint_index;//0x2
			__int16 hint_data_0;//0x4
			__int16 hint_data_1;//0x6
			__int16 hint_data_2;//0x8
			__int16 hint_data_3;//0xA
			__int16 hint_data_4;//0xC
			__int16 hint_data_5;//0xE
			__int16 hint_data_6;//0x10
			__int16 hint_data_7;//0x12
		};
		TAG_BLOCK_SIZE_ASSERT(s_pathfinding_hints_block, 0x14);
		tag_block<s_pathfinding_hints_block> pathfinding_hints;//0x38
		struct s_instanced_geometry_refs_block
		{
			__int16 pathfinding_object_index;//0x0
			PAD(0x2);//0x2
		};
		TAG_BLOCK_SIZE_ASSERT(s_instanced_geometry_refs_block, 0x4);
		tag_block<s_instanced_geometry_refs_block> instanced_geometry_refs;//0x40
		__int32 structure_checksum;//0x48
		PAD(0x20);//0x4C
		struct s_userplaced_hints_block
		{
			struct s_point_geometry_block
			{
				float point_x;//0x0
				float point_y;//0x4
				float point_z;//0x8
				__int16 reference_frame;//0xC
				PAD(0x2);//0xE
			};
			TAG_BLOCK_SIZE_ASSERT(s_point_geometry_block, 0x10);
			tag_block<s_point_geometry_block> point_geometry;//0x0
			struct s_ray_geometry_block
			{
				float point_x;//0x0
				float point_y;//0x4
				float point_z;//0x8
				__int16 reference_frame;//0xC
				PAD(0xE);//0xE
			};
			TAG_BLOCK_SIZE_ASSERT(s_ray_geometry_block, 0x1C);
			tag_block<s_ray_geometry_block> ray_geometry;//0x8
			struct s_line_segment_geometry_block
			{
				enum class e_flags : __int32
				{
					bidirectional = FLAG(0),
					closed = FLAG(1),
				};
				e_flags flags;//0x0
				float point_0_x;//0x4
				float point_0_y;//0x8
				float point_0_z;//0xC
				__int16 reference_frame;//0x10
				PAD(0x2);//0x12
				float point_1_x;//0x14
				float point_1_y;//0x18
				float point_1_z;//0x1C
				__int16 reference_frame_1;//0x20
				PAD(0x2);//0x22
			};
			TAG_BLOCK_SIZE_ASSERT(s_line_segment_geometry_block, 0x24);
			tag_block<s_line_segment_geometry_block> line_segment_geometry;//0x10
			struct s_parallelogram_geometry_block
			{
				enum class e_flags : __int32
				{
					bidirectional = FLAG(0),
					closed = FLAG(1),
				};
				e_flags flags;//0x0
				float point_0_x;//0x4
				float point_0_y;//0x8
				float point_0_z;//0xC
				__int16 reference_frame_0;//0x10
				PAD(0x2);//0x12
				float point_1_x;//0x14
				float point_1_y;//0x18
				float point_1_z;//0x1C
				__int16 reference_frame_1;//0x20
				PAD(0x2);//0x22
				float point_2_x;//0x24
				float point_2_y;//0x28
				float point_2_z;//0x2C
				__int16 reference_frame_2;//0x30
				PAD(0x2);//0x32
				float point_3_x;//0x34
				float point_3_y;//0x38
				float point_3_z;//0x3C
				__int16 reference_frame_3;//0x40
				PAD(0x2);//0x42
			};
			TAG_BLOCK_SIZE_ASSERT(s_parallelogram_geometry_block, 0x44);
			tag_block<s_parallelogram_geometry_block> parallelogram_geometry;//0x18
			struct s_polygon_geometry_block
			{
				enum class e_flags : __int32
				{
					bidirectional = FLAG(0),
					closed = FLAG(1),
				};
				e_flags flags;//0x0
				struct s_points_block
				{
					float point_x;//0x0
					float point_y;//0x4
					float point_z;//0x8
					__int16 reference_frame;//0xC
					PAD(0x2);//0xE
				};
				TAG_BLOCK_SIZE_ASSERT(s_points_block, 0x10);
				tag_block<s_points_block> points;//0x4
			};
			TAG_BLOCK_SIZE_ASSERT(s_polygon_geometry_block, 0xC);
			tag_block<s_polygon_geometry_block> polygon_geometry;//0x20
			struct s_jump_hints_block
			{
				enum class e_flags : __int16
				{
					bidirectional = FLAG(0),
					closed = FLAG(1),
				};
				e_flags flags;//0x0
				__int16 geometry_index;//0x2
				enum class e_force_jump_height : __int16
				{
					none = 0,
					down = 1,
					step = 2,
					crouch = 3,
					stand = 4,
					storey = 5,
					tower = 6,
					infinite = 7,
				};
				e_force_jump_height force_jump_height;//0x4
				enum class e_control_flags : __int16
				{
					magic_lift = FLAG(0),
				};
				e_control_flags control_flags;//0x6
			};
			TAG_BLOCK_SIZE_ASSERT(s_jump_hints_block, 0x8);
			tag_block<s_jump_hints_block> jump_hints;//0x28
			struct s_climb_hints_block
			{
				enum class e_flags : __int16
				{
					bidirectional = FLAG(0),
					closed = FLAG(1),
				};
				e_flags flags;//0x0
				__int16 geometry_index;//0x2
			};
			TAG_BLOCK_SIZE_ASSERT(s_climb_hints_block, 0x4);
			tag_block<s_climb_hints_block> climb_hints;//0x30
			struct s_well_hints_block
			{
				enum class e_flags : __int32
				{
					bidirectional = FLAG(0),
				};
				e_flags flags;//0x0
				struct s_points_block
				{
					enum class e_type : __int16
					{
						jump = 0,
						climb = 1,
						hoist = 2,
					};
					e_type type;//0x0
					PAD(0xE);//0x2
					__int16 reference_frame;//0x10
					PAD(0x2);//0x12
					__int32 sector_index;//0x14
					angle normal_yaw;//0x18
					angle normal_pitch;//0x1C
				};
				TAG_BLOCK_SIZE_ASSERT(s_points_block, 0x20);
				tag_block<s_points_block> points;//0x4
			};
			TAG_BLOCK_SIZE_ASSERT(s_well_hints_block, 0xC);
			tag_block<s_well_hints_block> well_hints;//0x38
			struct s_flight_hints_block
			{
				struct s_points_block
				{
					PAD(0xC);//0x0
				};
				TAG_BLOCK_SIZE_ASSERT(s_points_block, 0xC);
				tag_block<s_points_block> points;//0x0
			};
			TAG_BLOCK_SIZE_ASSERT(s_flight_hints_block, 0x8);
			tag_block<s_flight_hints_block> flight_hints;//0x40
		};
		TAG_BLOCK_SIZE_ASSERT(s_userplaced_hints_block, 0x48);
		tag_block<s_userplaced_hints_block> userplaced_hints;//0x6C
	};
	TAG_BLOCK_SIZE_ASSERT(s_pathfinding_data_block, 0x74);
	tag_block<s_pathfinding_data_block> pathfinding_data;//0xC4
	struct s_pathfinding_edges_block
	{
		__int8 midpoint;//0x0
	};
	TAG_BLOCK_SIZE_ASSERT(s_pathfinding_edges_block, 0x1);
	tag_block<s_pathfinding_edges_block> pathfinding_edges;//0xCC
	struct s_background_sound_palette_block
	{
		tag_string32 name;//0x0
		tag_reference background_sound;//0x20
		tag_reference inside_cluster_sound;//0x28
		PAD(0x14);//0x30
		float cutoff_distance;//0x44
		enum class e_scale_flags : __int32
		{
			override_default_scale = FLAG(0),
			use_adjacent_cluster_as_portal_scale = FLAG(1),
			use_adjacent_cluster_as_exterior_scale = FLAG(2),
			scale_with_weather_intensity = FLAG(3),
		};
		e_scale_flags scale_flags;//0x48
		float interior_scale;//0x4C
		float portal_scale;//0x50
		float exterior_scale;//0x54
		float interpolation_speed;//0x58
		PAD(0x8);//0x5C
	};
	TAG_BLOCK_SIZE_ASSERT(s_background_sound_palette_block, 0x64);
	tag_block<s_background_sound_palette_block> background_sound_palette;//0xD4
	struct s_sound_environment_palette_block
	{
		tag_string32 name;//0x0
		tag_reference sound_environment;//0x20
		float cutoff_distance;//0x28
		float interpolation_speed;//0x2C
		PAD(0x18);//0x30
	};
	TAG_BLOCK_SIZE_ASSERT(s_sound_environment_palette_block, 0x48);
	tag_block<s_sound_environment_palette_block> sound_environment_palette;//0xDC
	data_block sound_pas_data;//0xE4
	struct s_markers_block
	{
		tag_string32 name;//0x0
		PAD(0xC);//0x20
		float rotation_w;//0x2C
		float position_x;//0x30
		float position_y;//0x34
		float position_z;//0x38
	};
	TAG_BLOCK_SIZE_ASSERT(s_markers_block, 0x3C);
	tag_block<s_markers_block> markers;//0xEC
	struct s_runtime_decals_block
	{
		PAD(0x10);//0x0
	};
	TAG_BLOCK_SIZE_ASSERT(s_runtime_decals_block, 0x10);
	tag_block<s_runtime_decals_block> runtime_decals;//0xF4
	struct s_environment_object_palette_block
	{
		tag_reference definition;//0x0
		tag_reference model;//0x8
		PAD(0x4);//0x10
	};
	TAG_BLOCK_SIZE_ASSERT(s_environment_object_palette_block, 0x14);
	tag_block<s_environment_object_palette_block> environment_object_palette;//0xFC
	struct s_environment_objects_block
	{
		tag_string32 name;//0x0
		PAD(0xC);//0x20
		float rotation_w;//0x2C
		float translation_x;//0x30
		float translation_y;//0x34
		float translation_z;//0x38
		__int16 palette_index;//0x3C
		PAD(0x2);//0x3E
		__int32 unique_id;//0x40
		unsigned __int32 exported_object_type;//0x44
		tag_string32 scenario_object_name;//0x48
	};
	TAG_BLOCK_SIZE_ASSERT(s_environment_objects_block, 0x68);
	tag_block<s_environment_objects_block> environment_objects;//0x104
	struct s_lightmaps_block
	{
		tag_reference bitmap_group;//0x0
	};
	TAG_BLOCK_SIZE_ASSERT(s_lightmaps_block, 0x8);
	tag_block<s_lightmaps_block> lightmaps;//0x10C
	PAD(0x4);//0x114
	struct s_leaf_map_leaves_block
	{
		struct s_faces_block
		{
			__int32 node_index;//0x0
			struct s_vertices_block
			{
				float vertex_x;//0x0
				float vertex_y;//0x4
				float vertex_z;//0x8
			};
			TAG_BLOCK_SIZE_ASSERT(s_vertices_block, 0xC);
			tag_block<s_vertices_block> vertices;//0x4
		};
		TAG_BLOCK_SIZE_ASSERT(s_faces_block, 0xC);
		tag_block<s_faces_block> faces;//0x0
		struct s_connection_indices_block
		{
			__int32 connection_index;//0x0
		};
		TAG_BLOCK_SIZE_ASSERT(s_connection_indices_block, 0x4);
		tag_block<s_connection_indices_block> connection_indices;//0x8
	};
	TAG_BLOCK_SIZE_ASSERT(s_leaf_map_leaves_block, 0x10);
	tag_block<s_leaf_map_leaves_block> leaf_map_leaves;//0x118
	struct s_leaf_map_connections_block
	{
		__int32 plane_index;//0x0
		__int32 back_leaf_index;//0x4
		__int32 front_leaf_index;//0x8
		struct s_vertices_block
		{
			float vertex_x;//0x0
			float vertex_y;//0x4
			float vertex_z;//0x8
		};
		TAG_BLOCK_SIZE_ASSERT(s_vertices_block, 0xC);
		tag_block<s_vertices_block> vertices;//0xC
		float area;//0x14
	};
	TAG_BLOCK_SIZE_ASSERT(s_leaf_map_connections_block, 0x18);
	tag_block<s_leaf_map_connections_block> leaf_map_connections;//0x120
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
				float position_x_0;//0x0
				float position_y_0;//0x4
				float position_z_0;//0x8
				__int8 node_index_0;//0xC
				__int8 node_index_1;//0xD
				__int8 node_index_2;//0xE
				__int8 node_index_3;//0xF
				float node_weight_0;//0x10
				float node_weight_1;//0x14
				float node_weight_2;//0x18
				float node_weight_3;//0x1C
				float position_x_1;//0x20
				float position_y_1;//0x24
				float position_z_1;//0x28
				__int8 node_index_0b;//0x2C
				__int8 node_index_1b;//0x2D
				__int8 node_index_2b;//0x2E
				__int8 node_index_3b;//0x2F
				float node_weight_0b;//0x30
				float node_weight_1b;//0x34
				float node_weight_2b;//0x38
				float node_weight_3b;//0x3C
				float node_weight_0c;//0x40
				float node_weight_1c;//0x44
				float node_weight_2c;//0x48
				float node_weight_3c;//0x4C
			};
			TAG_BLOCK_SIZE_ASSERT(s_lines_block, 0x50);
			tag_block<s_lines_block> lines;//0x40
			struct s_triangles_block
			{
				float position_x_0;//0x0
				float position_y_0;//0x4
				float position_z_0;//0x8
				__int8 node_index_0;//0xC
				__int8 node_index_1;//0xD
				__int8 node_index_2;//0xE
				__int8 node_index_3;//0xF
				float node_weight_0;//0x10
				float node_weight_1;//0x14
				float node_weight_2;//0x18
				float node_weight_3;//0x1C
				float position_x_1;//0x20
				float position_y_1;//0x24
				float position_z_1;//0x28
				__int8 node_index_0a;//0x2C
				__int8 node_index_1a;//0x2D
				__int8 node_index_2a;//0x2E
				__int8 node_index_3a;//0x2F
				float node_weight_0a;//0x30
				float node_weight_1a;//0x34
				float node_weight_2a;//0x38
				float node_weight_3a;//0x3C
				float position_x_2;//0x40
				float position_y_2;//0x44
				float position_z_2;//0x48
				__int8 node_index_0b;//0x4C
				__int8 node_index_1b;//0x4D
				__int8 node_index_2b;//0x4E
				__int8 node_index_3b;//0x4F
				float node_weight_0b;//0x50
				float node_weight_1b;//0x54
				float node_weight_2b;//0x58
				float node_weight_3b;//0x5C
				float node_weight_0c;//0x60
				float node_weight_1c;//0x64
				float node_weight_2c;//0x68
				float node_weight_3c;//0x6C
			};
			TAG_BLOCK_SIZE_ASSERT(s_triangles_block, 0x70);
			tag_block<s_triangles_block> triangles;//0x48
			struct s_quads_block
			{
				float position_x_0;//0x0
				float position_y_0;//0x4
				float position_z_0;//0x8
				__int8 node_index_0;//0xC
				__int8 node_index_1;//0xD
				__int8 node_index_2;//0xE
				__int8 node_index_3;//0xF
				float node_weight_0;//0x10
				float node_weight_1;//0x14
				float node_weight_2;//0x18
				float node_weight_3;//0x1C
				float position_x_1;//0x20
				float position_y_1;//0x24
				float position_z_1;//0x28
				__int8 node_index_0a;//0x2C
				__int8 node_index_1a;//0x2D
				__int8 node_index_2a;//0x2E
				__int8 node_index_3a;//0x2F
				float node_weight_0a;//0x30
				float node_weight_1a;//0x34
				float node_weight_2a;//0x38
				float node_weight_3a;//0x3C
				float position_x_2;//0x40
				float position_y_2;//0x44
				float position_z_2;//0x48
				__int8 node_index_0b;//0x4C
				__int8 node_index_1b;//0x4D
				__int8 node_index_2b;//0x4E
				__int8 node_index_3b;//0x4F
				float node_weight_0b;//0x50
				float node_weight_1b;//0x54
				float node_weight_2b;//0x58
				float node_weight_3b;//0x5C
				float position_x_3;//0x60
				float position_y_3;//0x64
				float position_z_3;//0x68
				__int8 node_index_0c;//0x6C
				__int8 node_index_1c;//0x6D
				__int8 node_index_2c;//0x6E
				__int8 node_index_3c;//0x6F
				float node_weight_0c;//0x70
				float node_weight_1c;//0x74
				float node_weight_2c;//0x78
				float node_weight_3c;//0x7C
				float node_weight_0d;//0x80
				float node_weight_1d;//0x84
				float node_weight_2d;//0x88
				float node_weight_3d;//0x8C
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
	tag_block<s_errors_block> errors;//0x128
	struct s_precomputed_lighting_block
	{
		__int32 index;//0x0
		enum class e_light_type : __int16
		{
			free_standing = 0,
			attached_to_editor_object = 1,
			attached_to_structure_object = 2,
		};
		e_light_type light_type;//0x4
		__int8 attachment_index;//0x6
		__int8 object_type;//0x7
		__int16 projection_count;//0x8
		__int16 cluster_count;//0xA
		__int16 volume_count;//0xC
		PAD(0x2);//0xE
		data_block projections;//0x10
		data_block visibility_clusters;//0x18
		data_block cluster_remap_table;//0x20
		data_block visibility_volumes;//0x28
	};
	TAG_BLOCK_SIZE_ASSERT(s_precomputed_lighting_block, 0x30);
	tag_block<s_precomputed_lighting_block> precomputed_lighting;//0x130
	struct s_instanced_geometries_definitions_block
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
		struct s_render_data_block
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
		TAG_BLOCK_SIZE_ASSERT(s_render_data_block, 0x44);
		tag_block<s_render_data_block> render_data;//0x4C
		struct s_index_reorder_table_block
		{
			__int16 index;//0x0
		};
		TAG_BLOCK_SIZE_ASSERT(s_index_reorder_table_block, 0x2);
		tag_block<s_index_reorder_table_block> index_reorder_table;//0x54
		__int32 checksum;//0x5C
		float bounding_sphere_center_x;//0x60
		float bounding_sphere_center_y;//0x64
		float bounding_sphere_center_z;//0x68
		float bounding_sphere_radius;//0x6C
		struct s_bsp_3d_nodes_block
		{
			PAD(0x8);//0x0
		};
		TAG_BLOCK_SIZE_ASSERT(s_bsp_3d_nodes_block, 0x8);
		tag_block<s_bsp_3d_nodes_block> bsp_3d_nodes;//0x70
		struct s_planes_block
		{
			PAD(0xC);//0x0
			float plane_distance;//0xC
		};
		TAG_BLOCK_SIZE_ASSERT(s_planes_block, 0x10);
		tag_block<s_planes_block> planes;//0x78
		struct s_leaves_block
		{
			enum class e_flags : __int8
			{
				contains_doublesided_surfaces = FLAG(0),
			};
			e_flags flags;//0x0
			__int8 bsp_2d_reference_count;//0x1
			__int16 first_bsp_2d_reference;//0x2
		};
		TAG_BLOCK_SIZE_ASSERT(s_leaves_block, 0x4);
		tag_block<s_leaves_block> leaves;//0x80
		struct s_bsp_2d_references_block
		{
			__int16 plane;//0x0
			__int16 bsp_2d_node;//0x2
		};
		TAG_BLOCK_SIZE_ASSERT(s_bsp_2d_references_block, 0x4);
		tag_block<s_bsp_2d_references_block> bsp_2d_references;//0x88
		struct s_bsp_2d_nodes_block
		{
			float plane__normal_i;//0x0
			float plane__normal_j;//0x4
			float plane__distance;//0x8
			__int16 left_child;//0xC
			__int16 right_child;//0xE
		};
		TAG_BLOCK_SIZE_ASSERT(s_bsp_2d_nodes_block, 0x10);
		tag_block<s_bsp_2d_nodes_block> bsp_2d_nodes;//0x90
		struct s_surfaces_block
		{
			__int16 plane;//0x0
			__int16 first_edge;//0x2
			enum class e_flags : __int8
			{
				twosided = FLAG(0),
				invisible = FLAG(1),
				climbable = FLAG(2),
				breakable = FLAG(3),
				invalid = FLAG(4),
				conveyor = FLAG(5),
			};
			e_flags flags;//0x4
			__int8 breakable_surface;//0x5
			__int16 material;//0x6
		};
		TAG_BLOCK_SIZE_ASSERT(s_surfaces_block, 0x8);
		tag_block<s_surfaces_block> surfaces;//0x98
		struct s_edges_block
		{
			__int16 start_vertex;//0x0
			__int16 end_vertex;//0x2
			__int16 forward_edge;//0x4
			__int16 reverse_edge;//0x6
			__int16 left_surface;//0x8
			__int16 right_surface;//0xA
		};
		TAG_BLOCK_SIZE_ASSERT(s_edges_block, 0xC);
		tag_block<s_edges_block> edges;//0xA0
		struct s_vertices_block
		{
			float point_x;//0x0
			float point_y;//0x4
			float point_z;//0x8
			__int16 first_edge;//0xC
			PAD(0x2);//0xE
		};
		TAG_BLOCK_SIZE_ASSERT(s_vertices_block, 0x10);
		tag_block<s_vertices_block> vertices;//0xA8
		struct s_bsp_physics_block
		{
			PAD(0x4);//0x0
			__int16 size;//0x4
			__int16 count;//0x6
			PAD(0x3C);//0x8
			__int16 sizea;//0x44
			__int16 counta;//0x46
			PAD(0xC);//0x48
			__int16 sizeb;//0x54
			__int16 countb;//0x56
			PAD(0xC);//0x58
			data_block mopp_code_data;//0x64
			PAD(0x8);//0x6C
		};
		TAG_BLOCK_SIZE_ASSERT(s_bsp_physics_block, 0x74);
		tag_block<s_bsp_physics_block> bsp_physics;//0xB0
		struct s_render_leaves_block
		{
			__int16 cluster;//0x0
			__int16 surface_reference_count;//0x2
			__int32 first_surface_reference_index;//0x4
		};
		TAG_BLOCK_SIZE_ASSERT(s_render_leaves_block, 0x8);
		tag_block<s_render_leaves_block> render_leaves;//0xB8
		struct s_surface_references_block
		{
			__int16 strip_index;//0x0
			__int16 lightmap_triangle_index;//0x2
			__int32 bsp_node_index;//0x4
		};
		TAG_BLOCK_SIZE_ASSERT(s_surface_references_block, 0x8);
		tag_block<s_surface_references_block> surface_references;//0xC0
	};
	TAG_BLOCK_SIZE_ASSERT(s_instanced_geometries_definitions_block, 0xC8);
	tag_block<s_instanced_geometries_definitions_block> instanced_geometries_definitions;//0x138
	struct s_instanced_geometry_instances_block
	{
		float scale;//0x0
		PAD(0x24);//0x4
		float position_x;//0x28
		float position_y;//0x2C
		float position_z;//0x30
		__int16 instance_definition;//0x34
		enum class e_flags : __int16
		{
			not_in_lightprobes = FLAG(0),
		};
		e_flags flags;//0x36
		PAD(0x14);//0x38
		__int32 checksum;//0x4C
		string_id name;//0x50
		enum class e_pathfinding_policy : __int16
		{
			cutout = 0,
			e_static = 1,
			none = 2,
		};
		e_pathfinding_policy pathfinding_policy;//0x54
		enum class e_lightmapping_policy : __int16
		{
			per_pixel = 0,
			per_vertex = 1,
		};
		e_lightmapping_policy lightmapping_policy;//0x56
	};
	TAG_BLOCK_SIZE_ASSERT(s_instanced_geometry_instances_block, 0x58);
	tag_block<s_instanced_geometry_instances_block> instanced_geometry_instances;//0x140
	struct s_ambience_sound_clusters_block
	{
		PAD(0x4);//0x0
		struct s_enclosing_portal_designators_block
		{
			__int16 portal_designator;//0x0
		};
		TAG_BLOCK_SIZE_ASSERT(s_enclosing_portal_designators_block, 0x2);
		tag_block<s_enclosing_portal_designators_block> enclosing_portal_designators;//0x4
		struct s_interior_cluster_indices_block
		{
			__int16 interior_cluster_index;//0x0
		};
		TAG_BLOCK_SIZE_ASSERT(s_interior_cluster_indices_block, 0x2);
		tag_block<s_interior_cluster_indices_block> interior_cluster_indices;//0xC
	};
	TAG_BLOCK_SIZE_ASSERT(s_ambience_sound_clusters_block, 0x14);
	tag_block<s_ambience_sound_clusters_block> ambience_sound_clusters;//0x148
	struct s_reverb_sound_clusters_block
	{
		PAD(0x4);//0x0
		struct s_enclosing_portal_designators_block
		{
			__int16 portal_designator;//0x0
		};
		TAG_BLOCK_SIZE_ASSERT(s_enclosing_portal_designators_block, 0x2);
		tag_block<s_enclosing_portal_designators_block> enclosing_portal_designators;//0x4
		struct s_interior_cluster_indices_block
		{
			__int16 interior_cluster_index;//0x0
		};
		TAG_BLOCK_SIZE_ASSERT(s_interior_cluster_indices_block, 0x2);
		tag_block<s_interior_cluster_indices_block> interior_cluster_indices;//0xC
	};
	TAG_BLOCK_SIZE_ASSERT(s_reverb_sound_clusters_block, 0x14);
	tag_block<s_reverb_sound_clusters_block> reverb_sound_clusters;//0x150
	struct s_transparent_planes_block
	{
		__int16 section_index;//0x0
		__int16 part_index;//0x2
		PAD(0xC);//0x4
		float plane_distance;//0x10
	};
	TAG_BLOCK_SIZE_ASSERT(s_transparent_planes_block, 0x14);
	tag_block<s_transparent_planes_block> transparent_planes;//0x158
	PAD(0x60);//0x160
	float vehicle_sperical_limit_radius;//0x1C0
	float vehicle_sperical_limit_center_x;//0x1C4
	float vehicle_sperical_limit_center_y;//0x1C8
	float vehicle_sperical_limit_center_z;//0x1CC
	struct s_debug_info_block
	{
		PAD(0x40);//0x0
		struct s_clusters_block
		{
			enum class e_errors : __int16
			{
				multiple_fog_planes = FLAG(0),
				fog_zone_collision = FLAG(1),
				fog_zone_immersion = FLAG(2),
			};
			e_errors errors;//0x0
			enum class e_warnings : __int16
			{
				multiple_visible_fog_planes = FLAG(0),
				visible_fog_cluster_omission = FLAG(1),
				fog_plane_missed_renderbsp = FLAG(2),
			};
			e_warnings warnings;//0x2
			PAD(0x1C);//0x4
			struct s_lines_block
			{
				enum class e_type : __int16
				{
					fog_plane_boundary_edge = 0,
					fog_plane_internal_edge = 1,
					fog_zone_floodfill = 2,
					fog_zone_cluster_centroid = 3,
					fog_zone_cluster_geometry = 4,
					fog_zone_portal_centroid = 5,
					fog_zone_portal_geometry = 6,
				};
				e_type type;//0x0
				__int16 code;//0x2
				__int16 pad_thai;//0x4
				PAD(0x2);//0x6
				float point_0_x;//0x8
				float point_0_y;//0xC
				float point_0_z;//0x10
				float point_1_x;//0x14
				float point_1_y;//0x18
				float point_1_z;//0x1C
			};
			TAG_BLOCK_SIZE_ASSERT(s_lines_block, 0x20);
			tag_block<s_lines_block> lines;//0x20
			struct s_fog_plane_indices_block
			{
				__int32 index;//0x0
			};
			TAG_BLOCK_SIZE_ASSERT(s_fog_plane_indices_block, 0x4);
			tag_block<s_fog_plane_indices_block> fog_plane_indices;//0x28
			struct s_visible_fog_plane_indices_block
			{
				__int32 index;//0x0
			};
			TAG_BLOCK_SIZE_ASSERT(s_visible_fog_plane_indices_block, 0x4);
			tag_block<s_visible_fog_plane_indices_block> visible_fog_plane_indices;//0x30
			struct s_vis_fog_omission_cluster_indices_block
			{
				__int32 index;//0x0
			};
			TAG_BLOCK_SIZE_ASSERT(s_vis_fog_omission_cluster_indices_block, 0x4);
			tag_block<s_vis_fog_omission_cluster_indices_block> vis_fog_omission_cluster_indices;//0x38
			struct s_containing_fog_zone_indices_block
			{
				__int32 index;//0x0
			};
			TAG_BLOCK_SIZE_ASSERT(s_containing_fog_zone_indices_block, 0x4);
			tag_block<s_containing_fog_zone_indices_block> containing_fog_zone_indices;//0x40
		};
		TAG_BLOCK_SIZE_ASSERT(s_clusters_block, 0x48);
		tag_block<s_clusters_block> clusters;//0x40
		struct s_fog_planes_block
		{
			__int32 fog_zone_index;//0x0
			PAD(0x18);//0x4
			__int32 connected_plane_designator;//0x1C
			struct s_lines_block
			{
				enum class e_type : __int16
				{
					fog_plane_boundary_edge = 0,
					fog_plane_internal_edge = 1,
					fog_zone_floodfill = 2,
					fog_zone_cluster_centroid = 3,
					fog_zone_cluster_geometry = 4,
					fog_zone_portal_centroid = 5,
					fog_zone_portal_geometry = 6,
				};
				e_type type;//0x0
				__int16 code;//0x2
				__int16 pad_thai;//0x4
				PAD(0x2);//0x6
				float point_0_x;//0x8
				float point_0_y;//0xC
				float point_0_z;//0x10
				float point_1_x;//0x14
				float point_1_y;//0x18
				float point_1_z;//0x1C
			};
			TAG_BLOCK_SIZE_ASSERT(s_lines_block, 0x20);
			tag_block<s_lines_block> lines;//0x20
			struct s_intersected_cluster_indices_block
			{
				__int32 index;//0x0
			};
			TAG_BLOCK_SIZE_ASSERT(s_intersected_cluster_indices_block, 0x4);
			tag_block<s_intersected_cluster_indices_block> intersected_cluster_indices;//0x28
			struct s_inf_extent_cluster_indices_block
			{
				__int32 index;//0x0
			};
			TAG_BLOCK_SIZE_ASSERT(s_inf_extent_cluster_indices_block, 0x4);
			tag_block<s_inf_extent_cluster_indices_block> inf_extent_cluster_indices;//0x30
		};
		TAG_BLOCK_SIZE_ASSERT(s_fog_planes_block, 0x38);
		tag_block<s_fog_planes_block> fog_planes;//0x48
		struct s_fog_zones_block
		{
			__int32 media_index;//0x0
			__int32 base_fog_plane_index;//0x4
			PAD(0x18);//0x8
			struct s_lines_block
			{
				enum class e_type : __int16
				{
					fog_plane_boundary_edge = 0,
					fog_plane_internal_edge = 1,
					fog_zone_floodfill = 2,
					fog_zone_cluster_centroid = 3,
					fog_zone_cluster_geometry = 4,
					fog_zone_portal_centroid = 5,
					fog_zone_portal_geometry = 6,
				};
				e_type type;//0x0
				__int16 code;//0x2
				__int16 pad_thai;//0x4
				PAD(0x2);//0x6
				float point_0_x;//0x8
				float point_0_y;//0xC
				float point_0_z;//0x10
				float point_1_x;//0x14
				float point_1_y;//0x18
				float point_1_z;//0x1C
			};
			TAG_BLOCK_SIZE_ASSERT(s_lines_block, 0x20);
			tag_block<s_lines_block> lines;//0x20
			struct s_immersed_cluster_indices_block
			{
				__int32 index;//0x0
			};
			TAG_BLOCK_SIZE_ASSERT(s_immersed_cluster_indices_block, 0x4);
			tag_block<s_immersed_cluster_indices_block> immersed_cluster_indices;//0x28
			struct s_bounding_fog_plane_indices_block
			{
				__int32 index;//0x0
			};
			TAG_BLOCK_SIZE_ASSERT(s_bounding_fog_plane_indices_block, 0x4);
			tag_block<s_bounding_fog_plane_indices_block> bounding_fog_plane_indices;//0x30
			struct s_collision_fog_plane_indices_block
			{
				__int32 index;//0x0
			};
			TAG_BLOCK_SIZE_ASSERT(s_collision_fog_plane_indices_block, 0x4);
			tag_block<s_collision_fog_plane_indices_block> collision_fog_plane_indices;//0x38
		};
		TAG_BLOCK_SIZE_ASSERT(s_fog_zones_block, 0x40);
		tag_block<s_fog_zones_block> fog_zones;//0x50
	};
	TAG_BLOCK_SIZE_ASSERT(s_debug_info_block, 0x58);
	tag_block<s_debug_info_block> debug_info;//0x1D0
	tag_reference decorators;//0x1D8
	data_block mopp_code;//0x1E0
	PAD(0x4);//0x1E8
	float mopp_bounds_min_x;//0x1EC
	float mopp_bounds_min_y;//0x1F0
	float mopp_bounds_min_z;//0x1F4
	float mopp_bounds_max_x;//0x1F8
	float mopp_bounds_max_y;//0x1FC
	float mopp_bounds_max_z;//0x200
	data_block breakable_surfaces_mopp_code;//0x204
	struct s_breakable_surface_key_table_block
	{
		__int16 instanced_geometry_index;//0x0
		__int16 breakable_surface_index;//0x2
		__int32 seed_surface_index;//0x4
		float x0;//0x8
		float x1;//0xC
		float y0;//0x10
		float y1;//0x14
		float z0;//0x18
		float z1;//0x1C
	};
	TAG_BLOCK_SIZE_ASSERT(s_breakable_surface_key_table_block, 0x20);
	tag_block<s_breakable_surface_key_table_block> breakable_surface_key_table;//0x20C
	struct s_water_definitions_block
	{
		tag_reference shader;//0x0
		struct s_section_block
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
		TAG_BLOCK_SIZE_ASSERT(s_section_block, 0x44);
		tag_block<s_section_block> section;//0x8
		__int32 block_offset;//0x10
		__int32 block_size;//0x14
		__int32 section_data_size;//0x18
		__int32 resource_data_size;//0x1C
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
		tag_block<s_resources_block> resources;//0x20
		PAD(0x4);//0x28
		__int16 owner_tag_section_offset;//0x2C
		PAD(0x6);//0x2E
		real_color_rgb sun_spot_color;//0x34
		real_color_rgb reflection_tint;//0x40
		real_color_rgb refraction_tint;//0x4C
		real_color_rgb horizon_color;//0x58
		float sun_specular_power;//0x64
		float reflection_bump_scale;//0x68
		float refraction_bump_scale;//0x6C
		float fresnel_scale;//0x70
		float sun_dir_heading;//0x74
		float sun_dir_pitch;//0x78
		float fov;//0x7C
		float aspect;//0x80
		float height;//0x84
		float farz;//0x88
		float rotate_offset;//0x8C
		float center_i;//0x90
		float center_j;//0x94
		float extents_i;//0x98
		float extents_j;//0x9C
		float fog_near;//0xA0
		float fog_far;//0xA4
		float dynamic_height_bias;//0xA8
	};
	TAG_BLOCK_SIZE_ASSERT(s_water_definitions_block, 0xAC);
	tag_block<s_water_definitions_block> water_definitions;//0x214
	struct s_portal
	{
		struct s_device_portal_associations_block
		{
			__int32 unique_id;//0x0
			__int16 origin_bsp_index;//0x4
			enum class e_type : __int8
			{
				biped = 0,
				vehicle = 1,
				weapon = 2,
				equipment = 3,
				garbage = 4,
				projectile = 5,
				scenery = 6,
				machine = 7,
				control = 8,
				light_fixture = 9,
				sound_scenery = 10,
				crate = 11,
				creature = 12,
			};
			e_type type;//0x6
			enum class e_source : __int8
			{
				structure = 0,
				editor = 1,
				dynamic = 2,
				legacy = 3,
			};
			e_source source;//0x7
			__int16 first_game_portal_index;//0x8
			__int16 game_portal_count;//0xA
		};
		TAG_BLOCK_SIZE_ASSERT(s_device_portal_associations_block, 0xC);
		tag_block<s_device_portal_associations_block> device_portal_associations;//0x0
		struct s_game_portal_to_portal_map_block
		{
			__int16 portal_index;//0x0
		};
		TAG_BLOCK_SIZE_ASSERT(s_game_portal_to_portal_map_block, 0x2);
		tag_block<s_game_portal_to_portal_map_block> game_portal_to_portal_map;//0x8
	};
	TAG_BLOCK_SIZE_ASSERT(s_portal, 0x10);
	tag_block<s_portal> portal;//0x21C
	struct s_audibility_block
	{
		__int32 door_portal_count;//0x0
		float cluster_distance_bounds_lower;//0x4
		float cluster_distance_bounds_upper;//0x8
		struct s_encoded_door_pas_block
		{
			__int32 NUM_;//0x0
		};
		TAG_BLOCK_SIZE_ASSERT(s_encoded_door_pas_block, 0x4);
		tag_block<s_encoded_door_pas_block> encoded_door_pas;//0xC
		struct s_cluster_door_portal_encoded_pas_block
		{
			__int32 NUM_;//0x0
		};
		TAG_BLOCK_SIZE_ASSERT(s_cluster_door_portal_encoded_pas_block, 0x4);
		tag_block<s_cluster_door_portal_encoded_pas_block> cluster_door_portal_encoded_pas;//0x14
		struct s_ai_deafening_pas_block
		{
			__int32 NUM_;//0x0
		};
		TAG_BLOCK_SIZE_ASSERT(s_ai_deafening_pas_block, 0x4);
		tag_block<s_ai_deafening_pas_block> ai_deafening_pas;//0x1C
		struct s_cluster_distances_block
		{
			__int8 NUM_;//0x0
		};
		TAG_BLOCK_SIZE_ASSERT(s_cluster_distances_block, 0x1);
		tag_block<s_cluster_distances_block> cluster_distances;//0x24
		struct s_machine_door_mapping_block
		{
			__int8 machine_door_index;//0x0
		};
		TAG_BLOCK_SIZE_ASSERT(s_machine_door_mapping_block, 0x1);
		tag_block<s_machine_door_mapping_block> machine_door_mapping;//0x2C
	};
	TAG_BLOCK_SIZE_ASSERT(s_audibility_block, 0x34);
	tag_block<s_audibility_block> audibility;//0x224
	struct s_object_fake_lightprobes_block
	{
		__int32 unique_id;//0x0
		__int16 origin_bsp_index;//0x4
		enum class e_type : __int8
		{
			biped = 0,
			vehicle = 1,
			weapon = 2,
			equipment = 3,
			garbage = 4,
			projectile = 5,
			scenery = 6,
			machine = 7,
			control = 8,
			light_fixture = 9,
			sound_scenery = 10,
			crate = 11,
			creature = 12,
		};
		e_type type;//0x6
		enum class e_source : __int8
		{
			structure = 0,
			editor = 1,
			dynamic = 2,
			legacy = 3,
		};
		e_source source;//0x7
		real_color_rgb ambient;//0x8
		PAD(0xC);//0x14
		float lighting_accuracy;//0x20
		float shadow_opacity;//0x24
		real_color_rgb primary_direction_color;//0x28
		PAD(0xC);//0x34
		real_color_rgb secondary_direction_color;//0x40
		PAD(0xC);//0x4C
		__int16 sh_index;//0x58
		PAD(0x2);//0x5A
	};
	TAG_BLOCK_SIZE_ASSERT(s_object_fake_lightprobes_block, 0x5C);
	tag_block<s_object_fake_lightprobes_block> object_fake_lightprobes;//0x22C
	struct s_decorators_block
	{
		float grid_origin_x;//0x0
		float grid_origin_y;//0x4
		float grid_origin_z;//0x8
		__int32 cell_count_per_dimension;//0xC
		struct s_cache_blocks_block
		{
			__int32 block_offset;//0x0
			__int32 block_size;//0x4
			__int32 section_data_size;//0x8
			__int32 resource_data_size;//0xC
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
			tag_block<s_resources_block> resources;//0x10
			PAD(0x4);//0x18
			__int16 owner_tag_section_offset;//0x1C
			PAD(0x6);//0x1E
			struct s_cache_block_data_block
			{
				struct s_placements_block
				{
					__int32 internal_data_1;//0x0
					__int32 compressed_position;//0x4
					PAD(0x8);//0x8
					__int32 compressed_light_direction;//0x10
					__int32 compressed_light_2_direction;//0x14
				};
				TAG_BLOCK_SIZE_ASSERT(s_placements_block, 0x18);
				tag_block<s_placements_block> placements;//0x0
				struct s_decal_vertices_block
				{
					float position_x;//0x0
					float position_y;//0x4
					float position_z;//0x8
					float texcoord_0_x;//0xC
					float texcoord_0_y;//0x10
					float texcoord_1_x;//0x14
					float texcoord_1_y;//0x18
					PAD(0x4);//0x1C
				};
				TAG_BLOCK_SIZE_ASSERT(s_decal_vertices_block, 0x20);
				tag_block<s_decal_vertices_block> decal_vertices;//0x8
				struct s_decal_indices_block
				{
					__int16 index;//0x0
				};
				TAG_BLOCK_SIZE_ASSERT(s_decal_indices_block, 0x2);
				tag_block<s_decal_indices_block> decal_indices;//0x10
				PAD(0x30);//0x18
				struct s_sprite_vertices_block
				{
					float position_x;//0x0
					float position_y;//0x4
					float position_z;//0x8
					PAD(0x18);//0xC
					float texcoord_x;//0x24
					float texcoord_y;//0x28
					PAD(0x4);//0x2C
				};
				TAG_BLOCK_SIZE_ASSERT(s_sprite_vertices_block, 0x30);
				tag_block<s_sprite_vertices_block> sprite_vertices;//0x48
				struct s_sprite_indices_block
				{
					__int16 index;//0x0
				};
				TAG_BLOCK_SIZE_ASSERT(s_sprite_indices_block, 0x2);
				tag_block<s_sprite_indices_block> sprite_indices;//0x50
				PAD(0x30);//0x58
			};
			TAG_BLOCK_SIZE_ASSERT(s_cache_block_data_block, 0x88);
			tag_block<s_cache_block_data_block> cache_block_data;//0x24
			PAD(0x8);//0x2C
		};
		TAG_BLOCK_SIZE_ASSERT(s_cache_blocks_block, 0x34);
		tag_block<s_cache_blocks_block> cache_blocks;//0x10
		struct s_groups_block
		{
			__int8 decorator_set;//0x0
			enum class e_decorator_type : __int8
			{
				model = 0,
				floating_decal = 1,
				projected_decal = 2,
				screen_facing_quad = 3,
				axis_rotating_quad = 4,
				cross_quad = 5,
			};
			e_decorator_type decorator_type;//0x1
			__int8 shader_index;//0x2
			__int8 compressed_radius;//0x3
			__int16 cluster;//0x4
			__int16 cache_block;//0x6
			__int16 decorator_start_index;//0x8
			__int16 decorator_count;//0xA
			__int16 vertex_start_offset;//0xC
			__int16 vertex_count;//0xE
			__int16 index_start_offset;//0x10
			__int16 index_count;//0x12
			__int32 compressed_bounding_center;//0x14
		};
		TAG_BLOCK_SIZE_ASSERT(s_groups_block, 0x18);
		tag_block<s_groups_block> groups;//0x18
		struct s_cells_block
		{
			__int16 child_index_0;//0x0
			__int16 child_index_1;//0x2
			__int16 child_index_2;//0x4
			__int16 child_index_3;//0x6
			__int16 child_index_4;//0x8
			__int16 child_index_5;//0xA
			__int16 child_index_6;//0xC
			__int16 child_index_7;//0xE
			__int16 cache_block_index;//0x10
			__int16 group_count;//0x12
			__int32 group_start_index;//0x14
		};
		TAG_BLOCK_SIZE_ASSERT(s_cells_block, 0x18);
		tag_block<s_cells_block> cells;//0x20
		struct s_decals_block
		{
			__int8 decorator_set;//0x0
			__int8 decorator_class;//0x1
			__int8 decorator_permutation;//0x2
			__int8 sprite_index;//0x3
			float position_x;//0x4
			float position_y;//0x8
			float position_z;//0xC
			PAD(0x24);//0x10
			float previous_position_x;//0x34
			float previous_position_y;//0x38
			float previous_position_z;//0x3C
		};
		TAG_BLOCK_SIZE_ASSERT(s_decals_block, 0x40);
		tag_block<s_decals_block> decals;//0x28
	};
	TAG_BLOCK_SIZE_ASSERT(s_decorators_block, 0x30);
	tag_block<s_decorators_block> decorators_block;//0x234
};
TAG_GROUP_SIZE_ASSERT(s_scenario_structure_bsp_group_definition, 0x23C);
