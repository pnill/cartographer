#pragma once
#include "tag_files/tag_reference.h"
#include "tag_files/string_id.h"

#define NUMBER_OF_MATERIAL_TYPES 33

enum e_global_material_definition_flags : int16
{
	_global_material_definition_flag_flammable = FLAG(0),
	_global_material_definition_flag_biomass = FLAG(1)
};

enum e_material_type : int16
{
	_material_type_dirt = 0,
	_material_type_sand = 1,
	_material_type_stone = 2,
	_material_type_snow = 3,
	_material_type_wood = 4,
	_material_type_metal_hollow = 5,
	_material_type_metal_thin = 6,
	_material_type_metal_thick = 7,
	_material_type_rubber = 8,
	_material_type_glass = 9,
	_material_type_force_field = 10,
	_material_type_grunt = 11,
	_material_type_hunter_armor = 12,
	_material_type_hunter_skin = 13,
	_material_type_elite = 14,
	_material_type_jackal = 15,
	_material_type_jackal_energy_shield = 16,
	_material_type_engineer_skin = 17,
	_material_type_engineer_force_field = 18,
	_material_type_flood_combat_form = 19,
	_material_type_flood_carrier_form = 20,
	_material_type_cyborg_armor = 21,
	_material_type_cyborg_energy_shield = 22,
	_material_type_human_armor = 23,
	_material_type_human_skin = 24,
	_material_type_sentinel = 25,
	_material_type_monitor = 26,
	_material_type_plastic = 27,
	_material_type_water = 28,
	_material_type_leaves = 29,
	_material_type_elite_energy_shield = 30,
	_material_type_ice = 31,
	_material_type_hunter_shield = 32
};

enum e_sweetener_inheritance_flags : int32
{
    _sweetener_inheritance_sound_small = FLAG(0),
    _sweetener_inheritance_sound_medium = FLAG(1),
    _sweetener_inheritance_sound_large = FLAG(2),
    _sweetener_inheritance_sound_rolling = FLAG(3),
    _sweetener_inheritance_sound_grinding = FLAG(4),
    _sweetener_inheritance_sound_melee = FLAG(5),
	_sweetener_inheritance_flag_6 = FLAG(6),
    _sweetener_inheritance_effect_small = FLAG(7),
	_sweetener_inheritance_effect_medium = FLAG(8),
	_sweetener_inheritance_effect_large = FLAG(9),
	_sweetener_inheritance_effect_rolling = FLAG(10),
	_sweetener_inheritance_effect_grinding = FLAG(11),
	_sweetener_inheritance_effect_melee = FLAG(12),
	_sweetener_inheritance_flag_13 = FLAG(13)
};


// max count: NUMBER_OF_MATERIAL_TYPES
struct material_definition
{
    string_id new_material_name;
    string_id new_general_material_name;

    // Explaination("vehicle terrain parameters", "the following fields modify the way a vehicle drives over terrain of this material type.")
    real32 ground_friction_scale;           // fraction of original velocity parallel to the ground after one tick
    real32 ground_friction_normal_k1_scale; // cosine of angle at which friction falls off
    real32 ground_friction_normal_k0_scale; // cosine of angle at which friction is zero
    real32 ground_depth_scale;              // depth a point mass rests in the ground
    real32 ground_damp_fraction_scale;      // fraction of original velocity perpendicular to the ground after one tick
    tag_reference melee_hit_sound;          // snd!
};
TAG_BLOCK_SIZE_ASSERT(material_definition, 36);

// max count: 1
struct s_material_physics_properties
{
	int32 pad;
	real32 friction;
	real32 restitution;
	real32 density;			// Kilograms per meter cubed
};
TAG_BLOCK_SIZE_ASSERT(s_material_physics_properties, 16);

// max count: 1
struct s_global_material_sweeteners_definition
{
    tag_reference sound_sweetener_small;		// snd!
    tag_reference sound_sweetener_medium;		// snd!
    tag_reference sound_sweetener_large;		// snd!
    tag_reference sound_sweetener_rolling;		// lsnd
    tag_reference sound_sweetener_grinding;		// lsnd
    tag_reference sound_sweetener_melee;		// snd!
    tag_reference unknown;
    tag_reference effect_sweetener_small;		// effe
    tag_reference effect_sweetener_medium;		// effe
    tag_reference effect_sweetener_large;		// effe
    tag_reference effect_sweetener_rolling;		// effe
    tag_reference effect_sweetener_grinding;	// effe
    tag_reference effect_sweetener_melee;		// effe
    tag_reference unknown1;

    // Explaination("sweetener inheritance flags", "when a sweetener inheritance flag is set the sound\effect is not inherited from the parent material.  If you leave the sweetener blank and set the flag than no effect\sound will play")
	e_sweetener_inheritance_flags sweetener_inheritance_flags;
};

// max count: k_maximum_material_types
struct s_global_material_definition
{
	string_id name;
	string_id parent_name;
	int16 pad;
	e_global_material_definition_flags flags;
	e_material_type old_material_type;
	int16 pad1;
	string_id general_armor;
	string_id specific_armor;
	s_material_physics_properties physics_properties;

	tag_reference old_material_physics; // mpdt
	tag_reference breakable_surface;    // bsdt
	s_global_material_sweeteners_definition sweeteners;
	tag_reference material_effects;     // foot
};
TAG_BLOCK_SIZE_ASSERT(s_global_material_definition, 0xB4);
