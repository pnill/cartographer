#pragma once

#include "Blam/Math/real_math.h"

enum e_special_movement : int
{
	special_movement_jump = FLAG(0),
	special_movement_climb = FLAG(1),
	special_movement_vault = FLAG(2),
	special_movement_mount = FLAG(3),
	special_movement_hoist = FLAG(4),
	special_movement_wall_jump = FLAG(5),
	special_movement_na = FLAG(6)
};

// max_count: 1
struct special_movement_definition
{
	e_special_movement special_movement;
};
TAG_BLOCK_SIZE_ASSERT(special_movement_definition, 4);

// Unsure about the size for this struct...
struct actor_input
{
	bool unk_bool_264;
	bool underwater;
	bool driver;
	bool passenger;
	bool unk_bool_268;
	bool unk_bool_269;
	BYTE gap_26A[2];
	datum root_vehicle_index;
	__int16 vehicle_driver_type;
	BYTE gap_272[2];
	int some_object_index_274;
	bool unk_bool_278;
	BYTE gap_279[19];
	datum field_28C;
	real_vector3d facing_vector;
	real_vector3d aiming_vector;
	real_vector3d looking_vector;
	real_vector3d vector_2B4;
	real_vector3d vector_2C0;
	BYTE gap_2CC[4];
};
