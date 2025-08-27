#pragma once
#include "game/game_allegiance.h"
#include "objects/object_location.h"
#include "tag_files/string_id.h"
#include "tag_files/tag_block.h"

/* enums */

// Unverified, taken from H1
enum e_damage_data_flags : uint32
{
	_damage_area_of_effect_bit = 0x0,
	_damage_create_localized_effect_bit = 0x1,
	_damage_kill_instantly_bit = 0x2,
	_damage_from_weapon_bit = 0x3,
	_damage_silent_bit = 0x4,
	_damage_bypasses_shields_bit = 0x5,
	_damage_damaged_one_object_bit = 0x6,
	_damage_no_statistics_bit = 0x7,
	_damage_can_affect_physics = 0x8, // H2V verified
	NUMBER_OF_DAMAGE_DATA_FLAGS = 0x9,
};

/* structures */

struct s_damage_owner
{
	datum owner_player_index;
	datum owner_object_index;
	e_game_team owner_team_index;
	int16 pad;
};
ASSERT_STRUCT_SIZE(s_damage_owner, 12);

struct s_damage_data
{
	datum definition_index;
	c_flags_no_init<e_damage_data_flags, uint32, NUMBER_OF_DAMAGE_DATA_FLAGS> flags;
	s_damage_owner owner;
	int32 field_1C;
	datum inventory_owner_unit_index;
	s_location location;
	real_point3d origin;
	real_point3d epicenter;
	real_vector3d direction;
	real32 field_48;
	int32 field_4C[2];
	real32 scale;
	int32 field_58;
	real32 multiplier;
	int8 gap_60[8];
	bool damage_disabled;
	int8 pad0[3];
	real_vector3d projectile_direction;
	real32 material_effect_scale;
	int16 material_type;
	int16 field_7E;
	int8 gap_80[4];
	bool valid_damage_reporting_type;
	int8 pad[3];
};
ASSERT_STRUCT_SIZE(s_damage_data, 0x88);

// max_count: LONG_MAX
struct s_armor_modifier_definition
{
	string_id name;
	real32 damage_multiplier;
};
ASSERT_STRUCT_SIZE(s_armor_modifier_definition, 8);

// max_count: LONG_MAX
struct s_damage_group_definition
{
	string_id name;
	tag_block<s_armor_modifier_definition> armor_modifiers;
	
	// Runtime field?
	int32 pad;
};
ASSERT_STRUCT_SIZE(s_damage_group_definition, 16);

/* prototypes */

void damage_apply_patches(void);

void damage_initialize_for_new_map(void);

void __cdecl object_cause_damage(s_damage_data* damage_data, datum object_index, int16 node_index, int16 region_index, int16 material_index, real_vector3d* object_normal);

void __cdecl object_apply_damage_aftermath(datum object_index, s_damage_data* damage_data);