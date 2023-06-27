#pragma once
#include "Blam/Common/Common.h"

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