#pragma once
#include "Blam/Engine/math/real_math.h"
#include "Blam/Engine/objects/objects.h"

enum e_collision_test_flags : uint32
{
	_collision_test_structure_bit = 0,
	_collision_test_media_bit = 1,
	_collision_test_instanced_geometry_bit = 2,
	_collision_test_objects_bit = 3,
};

struct collision_result
{
	int32 type;
	real32 t;
	real_point3d point;
	s_location location_0;
	s_location location_1;
	int16 global_material_index;
	int16 pad;
	real_plane3d fog_plane;
	int32 field_38;
	int32 instanced_geometry_instance_index;
	datum object_index;
	int16 field_44;
	int16 matrix_index;
	int32 field_48;
	int32 field_4C;
	int32 field_50;
	int32 flags;
	int32 field_58;
};

uint16 collision_compute_object_cull_flags(datum object_index);

bool __cdecl collision_test_line(e_collision_test_flags flags,
	const real_point3d* placement,
	const real_point3d* position,
	datum first_ignore_object_index,
	datum second_ignore_object_index,
	collision_result* collision);

bool __cdecl collision_test_vector(e_collision_test_flags flags,
	const real_point3d* placement,
	const real_vector3d* direction,
	int32 first_ignore_object_index,
	datum second_ignore_object_index,
	collision_result* collision);
