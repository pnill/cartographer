#pragma once
#include "items.h"

#define MAXIMUM_NUMBER_OF_MAGAZINES_PER_WEAPON 2
#define MAXIMUM_NUMBER_OF_MAGAZINE_OBJECTS_PER_MAGAZINE 8
#define k_weapon_trigger_count 2
#define k_weapon_barrel_count 2
#define k_weapon_barrel_effect_count 3

struct s_weapon_data_definition
{
	s_item_data_definition item;
	char field_0[240];
};
CHECK_STRUCT_SIZE(s_weapon_data_definition, 0x25C);
