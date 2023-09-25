#pragma once
#include "items.h"

struct s_weapon_data_definition
{
	s_item_data_definition item;
	char field_0[240];
};
CHECK_STRUCT_SIZE(s_weapon_data_definition, 0x25C);
