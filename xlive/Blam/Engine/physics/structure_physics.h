#pragma once
#include "tag_files/data_reference.h"
#include "tag_files/tag_block.h"

/* structures */

// max: (8*1024) 8192
struct s_structure_physics_breakable_surface_key_value
{
	uint16 instanced_geometry_index;
	uint16 breakable_surface_index;
	int32 seed_surface_index;
	real_bounds x_bounds;
	real_bounds y_bounds;
	real_bounds z_bounds;
};
ASSERT_STRUCT_SIZE(s_structure_physics_breakable_surface_key_value, 32);

// max: 1
struct s_structure_physics
{
    /****************************************
	* definition_name: mopp_code_data
	* flags: 0
	* alignment_bit: 16
	* byteswap_proc: 0x00531b20
	****************************************/
	// DataSize(1048576)
    data_reference mopp_code;

	int32 pad;

	real_point3d mopp_bounds_min;
	real_point3d mopp_bounds_max;

	/****************************************
	* definition_name: mopp_code_data
	* flags: 0
	* alignment_bit: 16
	* byteswap_proc: 0x00531b20
	****************************************/
	// DataSize(1048576)
	data_reference breakable_surfaces_mopp_code;
	tag_block<s_structure_physics_breakable_surface_key_value> breakable_surface_keys;
};
ASSERT_STRUCT_SIZE(s_structure_physics, 52);