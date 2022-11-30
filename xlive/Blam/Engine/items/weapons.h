#pragma once
#include "Blam/Common/Common.h"
#include "Blam/Engine/objects/objects.h"

enum e_unit_weapons
{
	PrimaryWeapon,
	SecondaryWeapon,
	DualWeildWeapon
};

enum e_weapon_index : unsigned short
{
	Primary = 0xFF00,
	Secondary = 0xFF01,
	DualWeild = 0x0201
};

struct s_weapon_data_definition : s_object_data_definition
{
	PAD(0x25C - sizeof(s_object_data_definition));
};
CHECK_STRUCT_SIZE(s_weapon_data_definition, 0x25C);
