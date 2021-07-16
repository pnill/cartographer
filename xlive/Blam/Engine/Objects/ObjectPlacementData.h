#pragma once
#include "Blam\Maths\Maths.h"
#include "Blam\Cache\DataTypes.h"

//0xC4

struct __declspec(align(4)) ObjectPlacementData
{
	datum tag_index;
	datum unique_id;
	DWORD origin_bsp_index;
	DWORD variant_index;
	DWORD unk_14;
	BYTE placement_policy;
	BYTE unk_16;
	WORD unk_17;
	DWORD object_placement_flags;
	real_point3d position;
	real_vector3d forward;
	real_vector3d up;
	real_point3d translation_velocity;
	real_vector3d angular_velocity;
	float scale;
	datum player_index;
	DWORD field_60;
	DWORD field_64;
	DWORD field_68;
	DWORD field_6C;
	DWORD field_70;
	DWORD active_change_colors_mask;
	real_color_rgb change_colors[4];
	BYTE gap_A8[8];
	WORD destroyed_constraints_flag;
	WORD loosened_constraints_flag;
	WORD field_B4;
	WORD field_B6;
	DWORD field_B8;
	BYTE gap_BC[8];
};

static_assert(sizeof(ObjectPlacementData) == 0xC4, "Invalid ObjectPlacementData size");
