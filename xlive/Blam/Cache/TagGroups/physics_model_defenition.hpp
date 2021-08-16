#pragma once
#include "Blam\Cache\DataTypes\BlamDataTypes.h"
#include "Blam/Cache/TagGroups.hpp"
#include "Blam/Math/BlamMath.h"

/*********************************************************************
* name: physics_model
* group_tag : phmo
* header size : 272
* *********************************************************************/
#pragma pack(push,1)
struct s_physics_model_group_definition : TagGroup<'phmo'>
{
	enum class e_flags : __int32
	{
		unused = FLAG(0),
	};
	e_flags flags;//0x0
	float mass;//0x4
	float low_freq_deactivation_scale;//0x8
	float high_freq_deactivation_scale;//0xC
	PAD(0x18);//0x10
	struct s_phantom_types_block
	{
		enum class e_flags : __int32
		{
			generates_effects = FLAG(0),
			use_acc_as_force = FLAG(1),
			negates_gravity = FLAG(2),
			ignores_players = FLAG(3),
			ignores_nonplayers = FLAG(4),
			ignores_bipeds = FLAG(5),
			ignores_vehicles = FLAG(6),
			ignores_weapons = FLAG(7),
			ignores_equipment = FLAG(8),
			ignores_garbage = FLAG(9),
			ignores_projectiles = FLAG(10),
			ignores_scenery = FLAG(11),
			ignores_machines = FLAG(12),
			ignores_controls = FLAG(13),
			ignores_light_fixtures = FLAG(14),
			ignores_sound_scenery = FLAG(15),
			ignores_crates = FLAG(16),
			ignores_creatures = FLAG(17),
			NUM_1 = FLAG(18),
			NUM_2 = FLAG(19),
			NUM_3 = FLAG(20),
			NUM_4 = FLAG(21),
			NUM_5 = FLAG(22),
			NUM_6 = FLAG(23),
			localizes_physics = FLAG(24),
			disable_linear_damping = FLAG(25),
			disable_angular_damping = FLAG(26),
			ignores_dead_bipeds = FLAG(27),
		};
		e_flags flags;//0x0
		enum class e_minimum_size : __int8
		{
			default = 0,
			tiny = 1,
			small = 2,
			medium = 3,
			large = 4,
			huge = 5,
			extra_huge = 6,
		};
		e_minimum_size minimum_size;//0x4
		enum class e_maximum_size : __int8
		{
			default = 0,
			tiny = 1,
			small = 2,
			medium = 3,
			large = 4,
			huge = 5,
			extra_huge = 6,
		};
		e_maximum_size maximum_size;//0x5
		PAD(0x2);//0x6
		string_id marker_name;//0x8
		string_id alignment_marker_name;//0xC
		PAD(0x8);//0x10
		float hookes_law_e;//0x18
		float linear_dead_radius;//0x1C
		float center_acc;//0x20
		float center_max_vel;//0x24
		float axis_acc;//0x28
		float axis_max_vel;//0x2C
		float direction_acc;//0x30
		float direction_max_vel;//0x34
		PAD(0x1C);//0x38
		float alignment_hookes_law_e;//0x54
		float alignment_acc;//0x58
		float alignment_max_vel;//0x5C
		PAD(0x8);//0x60
	};
	TAG_BLOCK_SIZE_ASSERT(s_phantom_types_block, 0x68);
	tag_block<s_phantom_types_block> phantom_types;//0x28
	struct s_node_edges_block
	{
		PAD(0x4);//0x0
		__int16 node_a;//0x4
		__int16 node_b;//0x6
		struct s_constraints_block
		{
			enum class e_type : __int16
			{
				hinge = 0,
				limited_hinge = 1,
				ragdoll = 2,
				stiff_spring = 3,
				ball_and_socket = 4,
				prismatic = 5,
			};
			e_type type;//0x0
			__int16 index;//0x2
			enum class e_flags : __int32
			{
				is_rigid = FLAG(0),
				disable_effects = FLAG(1),
			};
			e_flags flags;//0x4
			float friction;//0x8
		};
		TAG_BLOCK_SIZE_ASSERT(s_constraints_block, 0xC);
		tag_block<s_constraints_block> constraints;//0x8
		string_id node_a_material;//0x10
		string_id node_b_material;//0x14
	};
	TAG_BLOCK_SIZE_ASSERT(s_node_edges_block, 0x18);
	tag_block<s_node_edges_block> node_edges;//0x30
	struct s_rigid_bodies_block
	{
		__int16 node;//0x0
		__int16 region;//0x2
		__int16 permutattion;//0x4
		PAD(0x2);//0x6
		float bouding_sphere_offset_x;//0x8
		float bouding_sphere_offset_y;//0xC
		float bouding_sphere_offset_z;//0x10
		float bounding_sphere_radius;//0x14
		enum class e_flags : __int16
		{
			no_collisions_wself = FLAG(0),
			only_collide_wenv = FLAG(1),
			disable_effects = FLAG(2),
			does_not_interact_wenvironment = FLAG(3),
			best_early_mover_body = FLAG(4),
			has_no_phantom_power_version = FLAG(5),
		};
		e_flags flags;//0x18
		enum class e_motion_type : __int16
		{
			sphere = 0,
			stabilized_sphere = 1,
			box = 2,
			stabilized_box = 3,
			keyframed = 4,
			fixed = 5,
		};
		e_motion_type motion_type;//0x1A
		__int16 no_phantom_power_alt;//0x1C
		enum class e_size : __int16
		{
			default = 0,
			tiny = 1,
			small = 2,
			medium = 3,
			large = 4,
			huge = 5,
			extra_huge = 6,
		};
		e_size size;//0x1E
		float inertia_tensor_scale;//0x20
		float linear_damping;//0x24
		float angular_damping;//0x28
		PAD(0xC);//0x2C
		enum class e_shape_type : __int16
		{
			sphere = 0,
			pill = 1,
			box = 2,
			triangle = 3,
			polyhedron = 4,
			multi_sphere = 5,
			unused_0 = 6,
			unused_1 = 7,
			unused_2 = 8,
			unused_3 = 9,
			unused_4 = 10,
			unused_5 = 11,
			unused_6 = 12,
			unused_7 = 13,
			list = 14,
			mopp = 15,
		};
		e_shape_type shape_type;//0x38
		__int16 shape;//0x3A
		float mass;//0x3C
		PAD(0x40);//0x40
		float bounding_sphere_pad;//0x80
		PAD(0xC);//0x84
	};
	TAG_BLOCK_SIZE_ASSERT(s_rigid_bodies_block, 0x90);
	tag_block<s_rigid_bodies_block> rigid_bodies;//0x38
	struct s_materials_block
	{
		string_id name;//0x0
		string_id global_material_name;//0x4
		__int16 phantom_type;//0x8
		enum class e_flags : __int16
		{
			does_not_collide_with_fixed_bodies = FLAG(0),
		};
		e_flags flags;//0xA
	};
	TAG_BLOCK_SIZE_ASSERT(s_materials_block, 0xC);
	tag_block<s_materials_block> materials;//0x40
	struct s_spheres_block
	{
		string_id name;//0x0
		__int16 material;//0x4
		enum class e_flags : __int16
		{
			unused = FLAG(0),
		};
		e_flags flags;//0x6
		float relative_mass_scale;//0x8
		float friction;//0xC
		float restitution;//0x10
		float volume;//0x14
		float mass;//0x18
		PAD(0x2);//0x1C
		__int16 phantom;//0x1E
		PAD(0x4);//0x20
		__int16 size_1;//0x24
		__int16 count_1;//0x26
		PAD(0x4);//0x28
		float radius;//0x2C
		PAD(0x4);//0x30
		__int16 size_2;//0x34
		__int16 count_2;//0x36
		PAD(0x48);//0x38
	};
	TAG_BLOCK_SIZE_ASSERT(s_spheres_block, 0x80);
	tag_block<s_spheres_block> spheres;//0x48
	struct s_multi_spheres_block
	{
		string_id name;//0x0
		__int16 material;//0x4
		enum class e_flags : __int16
		{
			unused = FLAG(0),
		};
		e_flags flags;//0x6
		float relative_mass_scale;//0x8
		float friction;//0xC
		float restitution;//0x10
		float volume;//0x14
		float mass;//0x18
		PAD(0x2);//0x1C
		__int16 phantom;//0x1E
		PAD(0x4);//0x20
		__int16 size;//0x24
		__int16 count;//0x26
		PAD(0x4);//0x28
		__int32 num_spheres;//0x2C
		PAD(0x80);//0x30
	};
	TAG_BLOCK_SIZE_ASSERT(s_multi_spheres_block, 0xB0);
	tag_block<s_multi_spheres_block> multi_spheres;//0x50
	struct s_pills_block
	{
		string_id name;//0x0
		__int16 material;//0x4
		enum class e_flags : __int16
		{
			unused = FLAG(0),
		};
		e_flags flags;//0x6
		float relative_mass_scale;//0x8
		float friction;//0xC
		float restitution;//0x10
		float volume;//0x14
		float mass;//0x18
		PAD(0x2);//0x1C
		__int16 phantom;//0x1E
		PAD(0x4);//0x20
		__int16 size;//0x24
		__int16 count;//0x26
		PAD(0x4);//0x28
		float radius;//0x2C
		PAD(0x20);//0x30
	};
	TAG_BLOCK_SIZE_ASSERT(s_pills_block, 0x50);
	tag_block<s_pills_block> pills;//0x58
	struct s_boxes_block
	{
		string_id name;//0x0
		__int16 material;//0x4
		enum class e_flags : __int16
		{
			unused = FLAG(0),
		};
		e_flags flags;//0x6
		float relative_mass_scale;//0x8
		float friction;//0xC
		float restitution;//0x10
		float volume;//0x14
		float mass;//0x18
		PAD(0x2);//0x1C
		__int16 phantom;//0x1E
		PAD(0x4);//0x20
		__int16 size_1;//0x24
		__int16 count_1;//0x26
		PAD(0x4);//0x28
		float radius;//0x2C
		PAD(0x14);//0x30
		__int16 size_2;//0x44
		__int16 count_2;//0x46
		PAD(0x48);//0x48
	};
	TAG_BLOCK_SIZE_ASSERT(s_boxes_block, 0x90);
	tag_block<s_boxes_block> boxes;//0x60
	struct s_triangles_block
	{
		string_id name;//0x0
		__int16 material;//0x4
		enum class e_flags : __int16
		{
			unused = FLAG(0),
		};
		e_flags flags;//0x6
		float relative_mass_scale;//0x8
		float friction;//0xC
		float restitution;//0x10
		float volume;//0x14
		float mass;//0x18
		PAD(0x2);//0x1C
		__int16 phantom;//0x1E
		PAD(0x4);//0x20
		__int16 size;//0x24
		__int16 count;//0x26
		PAD(0x4);//0x28
		float radius;//0x2C
		PAD(0x30);//0x30
	};
	TAG_BLOCK_SIZE_ASSERT(s_triangles_block, 0x60);
	tag_block<s_triangles_block> triangles;//0x68
	struct s_polyhedra_block
	{
		string_id name;//0x0
		__int16 material;//0x4
		enum class e_flags : __int16
		{
			unused = FLAG(0),
		};
		e_flags flags;//0x6
		float relative_mass_scale;//0x8
		float friction;//0xC
		float restitution;//0x10
		float volume;//0x14
		float mass;//0x18
		PAD(0x2);//0x1C
		__int16 phantom;//0x1E
		PAD(0x4);//0x20
		__int16 size;//0x24
		__int16 count;//0x26
		PAD(0x4);//0x28
		float radius;//0x2C
		PAD(0x24);//0x30
		__int32 four_vectors_size;//0x54
		__int32 four_vectors_capacity;//0x58
		__int32 num_vertices;//0x5C
		PAD(0x94);//0x60
		__int32 plane_equations_size;//0xF4
		__int32 plane_equations_capacity;//0xF8
		PAD(0x4);//0xFC
	};
	TAG_BLOCK_SIZE_ASSERT(s_polyhedra_block, 0x100);
	tag_block<s_polyhedra_block> polyhedra;//0x70
	struct s_polyhedron_four_vectors_block
	{
		PAD(0x30);//0x0
	};
	TAG_BLOCK_SIZE_ASSERT(s_polyhedron_four_vectors_block, 0x30);
	tag_block<s_polyhedron_four_vectors_block> polyhedron_four_vectors;//0x78
	struct s_polyhedron_plane_equations_block
	{
		PAD(0x10);//0x0
	};
	TAG_BLOCK_SIZE_ASSERT(s_polyhedron_plane_equations_block, 0x10);
	tag_block<s_polyhedron_plane_equations_block> polyhedron_plane_equations;//0x80
	struct s_mass_distributions_block
	{
		PAD(0x40);//0x0
	};
	TAG_BLOCK_SIZE_ASSERT(s_mass_distributions_block, 0x40);
	tag_block<s_mass_distributions_block> mass_distributions;//0x88
	struct s_lists_block
	{
		PAD(0x4);//0x0
		__int16 size;//0x4
		__int16 count;//0x6
		PAD(0x8);//0x8
		__int32 child_shapes_size;//0x10
		__int32 child_shapes_capacity;//0x14
		enum class e_shape_type : __int16
		{
			sphere = 0,
			pill = 1,
			box = 2,
			triangle = 3,
			polyhedron = 4,
			multi_sphere = 5,
			unused_0 = 6,
			unused_1 = 7,
			unused_2 = 8,
			unused_3 = 9,
			unused_4 = 10,
			unused_5 = 11,
			unused_6 = 12,
			unused_7 = 13,
			list = 14,
			mopp = 15,
		};
		e_shape_type shape_type_1;//0x18
		__int16 shape_1;//0x1A
		__int32 collision_filter_1;//0x1C

		e_shape_type shape_type_2;//0x20
		__int16 shape_2;//0x22
		__int32 collision_filter_2;//0x24

		e_shape_type shape_type_3;//0x28
		__int16 shape_3;//0x2A
		__int32 collision_filter_3;//0x2C

		e_shape_type shape_type_4;//0x30
		__int16 shape_4;//0x32
		__int32 collision_filter_4;//0x34
	};
	TAG_BLOCK_SIZE_ASSERT(s_lists_block, 0x38);
	tag_block<s_lists_block> lists;//0x90
	struct s_list_shapes_block
	{
		enum class e_shape_type : __int16
		{
			sphere = 0,
			pill = 1,
			box = 2,
			triangle = 3,
			polyhedron = 4,
			multi_sphere = 5,
			unused_0 = 6,
			unused_1 = 7,
			unused_2 = 8,
			unused_3 = 9,
			unused_4 = 10,
			unused_5 = 11,
			unused_6 = 12,
			unused_7 = 13,
			list = 14,
			mopp = 15,
		};
		e_shape_type shape_type;//0x0
		__int16 shape;//0x2
		__int32 collision_filter;//0x4
	};
	TAG_BLOCK_SIZE_ASSERT(s_list_shapes_block, 0x8);
	tag_block<s_list_shapes_block> list_shapes;//0x98
	struct s_mopps_block
	{
		PAD(0x4);//0x0
		__int16 size;//0x4
		__int16 count;//0x6
		PAD(0x6);//0x8
		__int16 list;//0xE
		__int32 code_offset;//0x10
	};
	TAG_BLOCK_SIZE_ASSERT(s_mopps_block, 0x14);
	tag_block<s_mopps_block> mopps;//0xA0
	data_block mopp_codes;//0xA8
	struct s_hinge_constraints_block
	{
		string_id name;//0x0
		__int16 node_a;//0x4
		__int16 node_b;//0x6
		float a_scale;//0x8
		PAD(0x24);//0xC
		float a_position_x;//0x30
		float a_position_y;//0x34
		float a_position_z;//0x38
		float b_scale;//0x3C
		PAD(0x24);//0x40
		float b_position_x;//0x64
		float b_position_y;//0x68
		float b_position_z;//0x6C
		__int16 edge_index;//0x70
		PAD(0x6);//0x72
	};
	TAG_BLOCK_SIZE_ASSERT(s_hinge_constraints_block, 0x78);
	tag_block<s_hinge_constraints_block> hinge_constraints;//0xB0
	struct s_ragdoll_constraints_block
	{
		string_id name;//0x0
		__int16 node_a;//0x4
		__int16 node_b;//0x6
		float a_scale;//0x8
		PAD(0x24);//0xC
		float a_position_x;//0x30
		float a_position_y;//0x34
		float a_position_z;//0x38
		float b_scale;//0x3C
		PAD(0x24);//0x40
		float b_position_x;//0x64
		float b_position_y;//0x68
		float b_position_z;//0x6C
		__int16 edge_index;//0x70
		PAD(0x6);//0x72
		float min_twist;//0x78
		float max_twist;//0x7C
		float min_cone;//0x80
		float max_cone;//0x84
		float min_plane;//0x88
		float max_plane;//0x8C
		float max_friciton_torque;//0x90
	};
	TAG_BLOCK_SIZE_ASSERT(s_ragdoll_constraints_block, 0x94);
	tag_block<s_ragdoll_constraints_block> ragdoll_constraints;//0xB8
	struct s_regions_block
	{
		string_id name;//0x0
		struct s_permutations_block
		{
			string_id name;//0x0
			struct s_rigid_bodies_block
			{
				__int16 rigid_body;//0x0
			};
			TAG_BLOCK_SIZE_ASSERT(s_rigid_bodies_block, 0x2);
			tag_block<s_rigid_bodies_block> rigid_bodies;//0x4
		};
		TAG_BLOCK_SIZE_ASSERT(s_permutations_block, 0xC);
		tag_block<s_permutations_block> permutations;//0x4
	};
	TAG_BLOCK_SIZE_ASSERT(s_regions_block, 0xC);
	tag_block<s_regions_block> regions;//0xC0
	struct s_nodes_block
	{
		string_id name;//0x0
		enum class e_flags : __int16
		{
			does_not_animate = FLAG(0),
		};
		e_flags flags;//0x4
		__int16 parent;//0x6
		__int16 sibling;//0x8
		__int16 child;//0xA
	};
	TAG_BLOCK_SIZE_ASSERT(s_nodes_block, 0xC);
	tag_block<s_nodes_block> nodes;//0xC8
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
	tag_block<s_import_info_block> import_info;//0xD0
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
				__int8 node_index_1;//0xC
				__int8 node_index_2;//0xD
				__int8 node_index_3;//0xE
				__int8 node_index_4;//0xF
				float node_weight_1;//0x10
				float node_weight_2;//0x14
				float node_weight_3;//0x18
				float node_weight_4;//0x1C
				float position_x_2;//0x20
				float position_y_2;//0x24
				float position_z_2;//0x28
				__int8 node_index_5;//0x2C
				__int8 node_index_6;//0x2D
				__int8 node_index_7;//0x2E
				__int8 node_index_8;//0x2F
				float node_weight_5;//0x30
				float node_weight_6;//0x34
				float node_weight_7;//0x38
				float node_weight_8;//0x3C
				float node_weight_9;//0x40
				float node_weight_10;//0x44
				float node_weight_11;//0x48
				float node_weight_12;//0x4C
			};
			TAG_BLOCK_SIZE_ASSERT(s_lines_block, 0x50);
			tag_block<s_lines_block> lines;//0x40
			struct s_triangles_block
			{
				//float position_x[0];//0x0
				//float position_y[0];//0x4
				//float position_z[0];//0x8
				//__int8 node_index[0];//0xC
				//__int8 node_index[1];//0xD
				//__int8 node_index[2];//0xE
				//__int8 node_index[3];//0xF
				//float node_weight[0];//0x10
				//float node_weight[1];//0x14
				//float node_weight[2];//0x18
				//float node_weight[3];//0x1C
				//float position_x[1];//0x20
				//float position_y[1];//0x24
				//float position_z[1];//0x28
				//__int8 node_index[0];//0x2C
				//__int8 node_index[1];//0x2D
				//__int8 node_index[2];//0x2E
				//__int8 node_index[3];//0x2F
				//float node_weight[0];//0x30
				//float node_weight[1];//0x34
				//float node_weight[2];//0x38
				//float node_weight[3];//0x3C
				//float position_x[2];//0x40
				//float position_y[2];//0x44
				//float position_z[2];//0x48
				//__int8 node_index[0];//0x4C
				//__int8 node_index[1];//0x4D
				//__int8 node_index[2];//0x4E
				//__int8 node_index[3];//0x4F
				//float node_weight[0];//0x50
				//float node_weight[1];//0x54
				//float node_weight[2];//0x58
				//float node_weight[3];//0x5C
				//float node_weight[0];//0x60
				//float node_weight[1];//0x64
				//float node_weight[2];//0x68
				//float node_weight[3];//0x6C
				PAD(0x70);
			};
			TAG_BLOCK_SIZE_ASSERT(s_triangles_block, 0x70);
			tag_block<s_triangles_block> triangles;//0x48
			struct s_quads_block
			{
				//float position_x[0];//0x0
				//float position_y[0];//0x4
				//float position_z[0];//0x8
				//__int8 node_index[0];//0xC
				//__int8 node_index[1];//0xD
				//__int8 node_index[2];//0xE
				//__int8 node_index[3];//0xF
				//float node_weight[0];//0x10
				//float node_weight[1];//0x14
				//float node_weight[2];//0x18
				//float node_weight[3];//0x1C
				//float position_x[1];//0x20
				//float position_y[1];//0x24
				//float position_z[1];//0x28
				//__int8 node_index[0];//0x2C
				//__int8 node_index[1];//0x2D
				//__int8 node_index[2];//0x2E
				//__int8 node_index[3];//0x2F
				//float node_weight[0];//0x30
				//float node_weight[1];//0x34
				//float node_weight[2];//0x38
				//float node_weight[3];//0x3C
				//float position_x[2];//0x40
				//float position_y[2];//0x44
				//float position_z[2];//0x48
				//__int8 node_index[0];//0x4C
				//__int8 node_index[1];//0x4D
				//__int8 node_index[2];//0x4E
				//__int8 node_index[3];//0x4F
				//float node_weight[0];//0x50
				//float node_weight[1];//0x54
				//float node_weight[2];//0x58
				//float node_weight[3];//0x5C
				//float position_x[3];//0x60
				//float position_y[3];//0x64
				//float position_z[3];//0x68
				//__int8 node_index[0];//0x6C
				//__int8 node_index[1];//0x6D
				//__int8 node_index[2];//0x6E
				//__int8 node_index[3];//0x6F
				//float node_weight[0];//0x70
				//float node_weight[1];//0x74
				//float node_weight[2];//0x78
				//float node_weight[3];//0x7C
				//float node_weight[0];//0x80
				//float node_weight[1];//0x84
				//float node_weight[2];//0x88
				//float node_weight[3];//0x8C
				PAD(0x90);
			};
			TAG_BLOCK_SIZE_ASSERT(s_quads_block, 0x90);
			tag_block<s_quads_block> quads;//0x50
			struct s_comments_block
			{
				//data_block text;//0x0
				//float position_x;//0x8
				//float position_y;//0xC
				//float position_z;//0x10
				//__int8 node_index[0];//0x14
				//__int8 node_index[1];//0x15
				//__int8 node_index[2];//0x16
				//__int8 node_index[3];//0x17
				//float node_weight[0];//0x18
				//float node_weight[1];//0x1C
				//float node_weight[2];//0x20
				//float node_weight[3];//0x24
				//real_color_argb color;//0x28
				PAD(0x38);
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
	tag_block<s_errors_block> errors;//0xD8
	struct s_point_to_path_curves_block
	{
		string_id name;//0x0
		__int16 node_index;//0x4
		PAD(0x2);//0x6
		struct s_points_block
		{
			float position_x;//0x0
			float position_y;//0x4
			float position_z;//0x8
			float t_value;//0xC
		};
		TAG_BLOCK_SIZE_ASSERT(s_points_block, 0x10);
		tag_block<s_points_block> points;//0x8
	};
	TAG_BLOCK_SIZE_ASSERT(s_point_to_path_curves_block, 0x10);
	tag_block<s_point_to_path_curves_block> point_to_path_curves;//0xE0
	struct s_limited_hinge_constraints_block
	{
		string_id name;//0x0
		__int16 node_a;//0x4
		__int16 node_b;//0x6
		float a_scale;//0x8
		PAD(0x24);//0xC
		float a_position_x;//0x30
		float a_position_y;//0x34
		float a_position_z;//0x38
		float b_scale;//0x3C
		PAD(0x24);//0x40
		float b_position_x;//0x64
		float b_position_y;//0x68
		float b_position_z;//0x6C
		__int16 edge_index;//0x70
		PAD(0x6);//0x72
		float limit_friction;//0x78
		float limit_min_angle;//0x7C
		float limit_max_angle;//0x80
	};
	TAG_BLOCK_SIZE_ASSERT(s_limited_hinge_constraints_block, 0x84);
	tag_block<s_limited_hinge_constraints_block> limited_hinge_constraints;//0xE8
	struct s_ball_and_socket_constraints_block
	{
		string_id name;//0x0
		__int16 node_a;//0x4
		__int16 node_b;//0x6
		float a_scale;//0x8
		PAD(0x24);//0xC
		float a_position_x;//0x30
		float a_position_y;//0x34
		float a_position_z;//0x38
		float b_scale;//0x3C
		PAD(0x24);//0x40
		float b_position_x;//0x64
		float b_position_y;//0x68
		float b_position_z;//0x6C
		__int16 edge_index;//0x70
		PAD(0x6);//0x72
	};
	TAG_BLOCK_SIZE_ASSERT(s_ball_and_socket_constraints_block, 0x78);
	tag_block<s_ball_and_socket_constraints_block> ball_and_socket_constraints;//0xF0
	struct s_stiff_spring_constraints_block
	{
		string_id name;//0x0
		__int16 node_a;//0x4
		__int16 node_b;//0x6
		float a_scale;//0x8
		PAD(0x24);//0xC
		float a_position_x;//0x30
		float a_position_y;//0x34
		float a_position_z;//0x38
		float b_scale;//0x3C
		PAD(0x24);//0x40
		float b_position_x;//0x64
		float b_position_y;//0x68
		float b_position_z;//0x6C
		__int16 edge_index;//0x70
		PAD(0x6);//0x72
		float spring_length;//0x78
	};
	TAG_BLOCK_SIZE_ASSERT(s_stiff_spring_constraints_block, 0x7C);
	tag_block<s_stiff_spring_constraints_block> stiff_spring_constraints;//0xF8
	struct s_prismatic_constraints_block
	{
		string_id name;//0x0
		__int16 node_a;//0x4
		__int16 node_b;//0x6
		float a_scale;//0x8
		PAD(0x24);//0xC
		float a_position_x;//0x30
		float a_position_y;//0x34
		float a_position_z;//0x38
		float b_scale;//0x3C
		PAD(0x24);//0x40
		float b_position_x;//0x64
		float b_position_y;//0x68
		float b_position_z;//0x6C
		__int16 edge_index;//0x70
		PAD(0x6);//0x72
		float min_limit;//0x78
		float max_limit;//0x7C
		float max_friction_force;//0x80
	};
	TAG_BLOCK_SIZE_ASSERT(s_prismatic_constraints_block, 0x84);
	tag_block<s_prismatic_constraints_block> prismatic_constraints;//0x100
	struct s_phantoms_block
	{
		PAD(0x4);//0x0
		__int16 size_1;//0x4
		__int16 count_1;//0x6
		PAD(0x10);//0x8
		__int16 size_2;//0x18
		__int16 count_2;//0x1A
		PAD(0x4);//0x1C
	};
	TAG_BLOCK_SIZE_ASSERT(s_phantoms_block, 0x20);
	tag_block<s_phantoms_block> phantoms;//0x108
};
TAG_GROUP_SIZE_ASSERT(s_physics_model_group_definition, 0x110);
#pragma pack(pop)