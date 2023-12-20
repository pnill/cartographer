#pragma once
#include "items.h"
#include "weapon_definitions.h"

#define MAXIMUM_NUMBER_OF_MAGAZINES_PER_WEAPON 2
#define MAXIMUM_NUMBER_OF_MAGAZINE_OBJECTS_PER_MAGAZINE 8
#define k_weapon_trigger_count 2
#define k_weapon_barrel_count 2
#define k_weapon_barrel_effect_count 3

struct s_weapon_magazine
{
	int16 weapon_state;
	int16 field_2;
	int16 field_4;
	int16 rounds_inventory_maximum;
	int16 rounds_loaded_maximum;
	int16 field_A;
	int16 field_C;
	int16 field_E;
};
CHECK_STRUCT_SIZE(s_weapon_magazine, 16);

struct weapon_datum
{
	s_item_data_definition item;
	int16 weapon_flags;
	int16 weapon_control_flags;
	int8 field_16C[12];
	int16 first_person_animation_duration_ticks;
	int8 field_17E[6];
	real32 field_184;
	real32 field_188;
	int8 field_18C[18];
	int16 turn_on_time_ticks;
	int8 field_202[130];
	int16 field_222;
	s_weapon_magazine magazines[2];
	int8 field_22E[24];
};
CHECK_STRUCT_SIZE(weapon_datum, 604);

int32 __cdecl weapon_get_rounds_total(datum object_index, int32 magazine_index, bool a3);
void __cdecl weapons_fire_barrels();