#pragma once
#include "Blam\Cache\DataTypes\BlamDataTypes.h"
#include "Blam\Math\BlamMath.h"

struct s_object_placement_data
{
	datum tag_index;//0
	DWORD unique_id;//0x4
	DWORD origin_bsp_index;//0x8
	string_id variant_name;//0xC
	DWORD unk_10;//0x10
	BYTE placement_policy;//0x14
	BYTE unk_15;//0x15
	WORD unk_16;//0x16
	DWORD object_placement_flags;//0x18
	real_point3d position;//0x1C
	real_vector3d orientation;//0x28
	real_vector3d up;//0x34
	real_point3d translational_velocity;//0x40
	real_vector3d angular_velocity;//0x4C
	float scale;//0x58
	datum player_index;
	DWORD field_60;
	int team_index;
	DWORD field_68;
	DWORD field_6C;
	DWORD field_70;
	DWORD active_change_colors_mask;
	real_color_rgb change_colors[4];
	PAD(8);
	WORD destroyed_constraints_flag;
	WORD loosened_constraints_flag;
	WORD field_B4;
	WORD field_B6;
	DWORD field_B8;
	PAD(8);
};
CHECK_STRUCT_SIZE(s_object_placement_data, 0xC4);