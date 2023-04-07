#pragma once
#include "Blam\Cache\DataTypes\BlamPrimitiveType.h"

struct s_aim_assist_targetting_data
{
	datum target_object;
	datum model_target;
	datum target_player;
	char gap_C[12];
	signed __int16 auto_aim_flags;
	char gap_1A[2];
	float primary_auto_aim_level;
	float secondary_auto_aim_level;
};
CHECK_STRUCT_SIZE(s_aim_assist_targetting_data, 0x24);
