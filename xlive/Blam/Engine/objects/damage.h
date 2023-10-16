#pragma once
#include "Blam/Cache/DataTypes/BlamDataTypes.h"
#include "Blam/Math/real_math.h"

struct s_damage_owner
{
	int32 unk1;
	int32 unk2;
	int32 unk3;
};
CHECK_STRUCT_SIZE(s_damage_owner, 12);

#pragma pack(push, 1)
struct s_damage_data
{
	DWORD damage_tag_index;			// 0x0
	DWORD flags;					// 0x4
	DWORD field_8;					// 0x8
	datum creator_datum;			// 0xC
	signed __int16 field_10;		// 0x10
	BYTE gap_12[2];					// 0x12
	int field_14;			// 0x14
	int field_18;			// 0x18
	int field_1C;					// 0x1C
	BYTE gap_20[4];					// 0x20
	DWORD field_24;					// 0x24
	DWORD field_28;					// 0x28
	DWORD field_2C;					// 0x2C
	DWORD field_30;					// 0x30
	DWORD field_34;					// 0x34
	DWORD field_38;					// 0x38
	real_point3d field_3C;			// 0x3C
	BYTE gap48[12];					// 0x48
	real_point3d field_54;			// 0x54
	BYTE gap_60[24];				// 0x60
	float field_78;					// 0x78
	__int16 field_7C;				// 0x7C
	signed __int16 field_7E;		// 0x7E
	BYTE gap_80[4];					// 0x80
	char field_84;					// 0x84
};
#pragma pack(pop)
CHECK_STRUCT_SIZE(s_damage_data, 0x85);

// max_count: LONG_MAX
struct s_armor_modifier_definition
{
	string_id name;
	real32 damage_multiplier;
};
TAG_BLOCK_SIZE_ASSERT(s_armor_modifier_definition, 8);

// max_count: LONG_MAX
struct s_damage_group_definition
{
	string_id name;
	tag_block<s_armor_modifier_definition> armor_modifiers;
	
	// Runtime field?
	int32 pad;
};
TAG_BLOCK_SIZE_ASSERT(s_damage_group_definition, 16);
