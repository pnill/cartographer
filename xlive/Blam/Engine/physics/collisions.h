#pragma once
#include "Blam/Engine/math/real_math.h"
#include "Blam/Engine/objects/objects.h"

enum e_collision_test_flags : uint32
{
	_collision_test_structure_bit = 0,
	_collision_test_media_bit = 1,
	_collision_test_instanced_geometry_bit = 2,
	_collision_test_objects_bit = 3,
	_collision_test_bit_4 = 4,
	_collision_test_bit_5 = 5,
	_collision_test_bit_6 = 6,
	_collision_test_bit_7 = 7,
	_collision_test_bit_8 = 8,
	_collision_test_bit_9 = 9,
	_collision_test_bit_10 = 10,
	_collision_test_bit_11 = 11,
	_collision_test_bit_12 = 12,
	_collision_test_bit_13 = 13,
	_collision_test_bit_14 = 14,
	_collision_test_bit_15 = 15,
	_collision_test_bit_16 = 16,
	_collision_test_bit_17 = 17,
	_collision_test_bit_18 = 18,
	_collision_test_bit_19 = 19,
	_collision_test_bit_20 = 20,
	_collision_test_bit_21 = 21,
	_collision_test_bit_22 = 22,
	_collision_test_bit_23 = 23,
	_collision_test_bit_24 = 24,
	_collision_test_bit_25 = 25,
	_collision_test_bit_26 = 26,
	_collision_test_bit_27 = 27,
	_collision_test_bit_28 = 28,
	_collision_test_try_to_keep_location_valid_bit = 29,
	_collision_test_bit_30 = 30,
	_collision_test_bit_31 = 31,
};

struct collision_result
{
	int32 type;
	real32 t;
	real_point3d point;
	s_location locations[2];
	int16 global_material_index;
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
	int16 field_58;
	int16 field_5A;
};
CHECK_STRUCT_SIZE(collision_result, 0x5C);

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
	datum first_ignore_object_index,
	datum second_ignore_object_index,
	collision_result* collision);
