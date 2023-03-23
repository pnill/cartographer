#pragma once
#include "Blam/Engine/math/real_math.h"

struct s_early_mover_data
{
	real_point3d origin;
	real_point3d position;
	real_vector3d vector0;
	real_vector3d vector1;
	real_vector3d vector2;
	real_vector3d angular_velocity;
	real_matrix4x3 matrix0;
	real_matrix4x3 matrix1;
	real_matrix4x3 matrix2;
	bool some_bool;
	bool another_bool;
};

struct s_object_early_movers_globals
{
	s_early_mover_data early_mover_data[32];
	datum early_mover_objects[32];
	int object_index_count;
	bool map_initialized;

	static s_object_early_movers_globals* get()
	{
		return *Memory::GetAddress<s_object_early_movers_globals**>(0x4E6654, 0x50E928);
	}
};

void object_early_mover_new(datum object_index);
