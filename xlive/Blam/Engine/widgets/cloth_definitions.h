#pragma once

/* macros */

#define cloth_definition_get(index)	((struct cloth_definition*)tag_get(_tag_group_cloth, (index)))

/* enums */

enum e_cloth_integration_type : int16
{
	_cloth_integration_type_verlet
};

enum e_cloth_flags
{
	_cloth_flag_doesnt_use_wind,
	_cloth_flag_uses_grid_attach_top,

	k_cloth_flag_count
};
typedef c_flags_no_init<e_cloth_flags, uint32, k_cloth_flag_count> c_cloth_flags;


/* structures */

struct cloth_link_definition
{
	int32 attachment_bits;
	int16 index_1;
	int16 index_2;
	real32 default_distance;
	real32 damping_multiplier;
};
ASSERT_STRUCT_SIZE(cloth_link_definition, 16);

struct cloth_index_definition
{
	int16 index;
};
ASSERT_STRUCT_SIZE(cloth_index_definition, 2);

struct cloth_vertex_definition
{
	real_point3d initial_position;
	real_vector2d uvs;
};
ASSERT_STRUCT_SIZE(cloth_vertex_definition, 20);

struct cloth_properties
{
	e_cloth_integration_type type;

	int16 number_iterations;
	real32 gravity_multiplier;
	real32 drag;
	real32 wind_scale;
	real32 wind_flappiness_scale;
	real32 longest_relative_rod;

	int8 pad[24];
};
ASSERT_STRUCT_SIZE(cloth_properties, 48);

struct cloth_definition
{
	c_cloth_flags flags;

	string_id marker_attachment_name;

	tag_reference shader; //shad

	// Explaination("Default cloth vertices", "if not importing from a render model, type a grid size")
	short_bounds grid_dimension;
	real_bounds grid_spacing;


	// Explaination("Cloth Properties", "")
	cloth_properties properties;

	// Explaination("Import or grid data", "")
	s_tag_block verticies; //cloth_vertex_definition
	s_tag_block indices; //cloth_index_definition
	s_tag_block strip_indices; // cloth_index_definition
	s_tag_block links; //cloth_link_definition
};
ASSERT_STRUCT_SIZE(cloth_definition, 108);
