#include "stdafx.h"
#include "units.h"

#include "unit_control.h"
#include "unit_definitions.h"

#include "items/weapons.h"

#include "cache/cache_files.h"
#include "simulation/game_interface/simulation_game_action.h"


/* prototypes */

// Replace calls to use interpolated functions
static void unit_get_camera_position_patch_mass_functions(void);

// Replace calls to use interpolated functions
static void unit_get_camera_position_patch_marker_functions(void);

// Replace calls to use interpolated functions
static void unit_get_head_position_patch_functions(void);

static void unit_apply_interpolation_patches(void);

static void __cdecl unit_set_weapon_type(
	datum unit_index,
	datum inventory_index,
	datum weapon_definition_index,
	int16 multiplayer_weapon_identifier,
	void* a5);

/* public code */

void unit_apply_patches(void)
{
	unit_apply_interpolation_patches();

	// TODO FIXME: doesn't fix it completely
	// PatchCall(Memory::GetAddress(0x1F8362, 0x1E20C8), unit_set_weapon_type);

	// don't update the weapon state if we didn't actually received an update
	// for some reason someone at bungie thought it was a good idea to apply an weapon ammo update
	// even if we received just the weapon definition
	NopFill(Memory::GetAddress(0x1F836A, 0x1E20D0), 4);
	return;
}

void __cdecl unit_delete_all_weapons(datum unit_datum_index)
{
	INVOKE(0x1441E0, 0x133030, unit_delete_all_weapons, unit_datum_index);
	return;
}

datum __cdecl unit_inventory_next_weapon(datum unit_datum_index)
{
	return INVOKE(0x139E04, 0x128C53, unit_inventory_next_weapon, unit_datum_index);
}

datum __cdecl unit_inventory_get_weapon(datum unit_index, int16 weapon_slot)
{
	//todo get server offset
	return INVOKE(0x13D9AD, 0x12C7FC, unit_inventory_get_weapon, unit_index, weapon_slot);
}

bool __cdecl unit_add_weapon_to_inventory(datum unit_datum_index, datum weapon_datum_index, e_weapon_addition_method weapon_addition_method)
{
	return INVOKE(0x1442AA, 0x1330FA, unit_add_weapon_to_inventory, unit_datum_index, weapon_datum_index, weapon_addition_method);
}

real32 __cdecl unit_get_field_of_view(datum unit_datum_index, real32 unit_camera_field_of_view, int16 zoom_level)
{
	return INVOKE(0x1414AC, 0x1302FB, unit_get_field_of_view, unit_datum_index, unit_camera_field_of_view, zoom_level);
}

bool unit_is_dual_wielding(datum unit_index)
{
	unit_datum* unit = unit_get(unit_index);
	return unit->unit.weapon_indices[0] != NONE && unit->unit.weapon_indices[1] != NONE;
}

bool unit_in_vehicle(datum unit_index) 
{
	unit_datum* unit = unit_get(unit_index);
	return unit->unit.parent_seat_index != NONE;
}

datum player_index_from_unit_index(datum unit_index)
{
	const unit_datum* unit = unit_try_and_get(unit_index);
	return (unit ? unit->unit.player_index : NONE);
}

void __cdecl unit_get_head_position_interpolated(datum unit_index, real_point3d* position)
{
	object_marker marker;
	object_get_markers_by_string_id(unit_index, _string_id_head, &marker, 1);
	*position = marker.matrix.position;
	return;
}

void __cdecl unit_get_camera_position(datum unit_index, real_point3d* out_point)
{
	INVOKE(0x13D2EC, 0, unit_get_camera_position, unit_index, out_point);
}

void unit_get_aiming_vector(datum unit_index, real_vector3d* out_vector)
{
	unit_datum* unit = unit_try_and_get(unit_index);

	ASSERT(unit);
	ASSERT(out_vector);

	*out_vector = unit->unit.aiming_vector;
}

void __cdecl unit_control(datum unit_index, const unit_control_data* control_data)
{
	INVOKE(0x138B75, 0x1279C4, unit_control, unit_index, control_data);
	return;
}

e_game_team unit_get_team_index(datum unit_index)
{
	const unit_datum* unit = unit_try_and_get(unit_index);
	return (unit ? unit->unit.unit_team : _game_team_observer);
}

bool __cdecl unit_desires_tight_camera_track(datum unit_index)
{
	return INVOKE(0x13F63B, 0, unit_desires_tight_camera_track, unit_index);
}

bool unit_does_not_show_readied_weapon(datum unit_index)
{
	unit_datum const* unit = unit_get(unit_index);
	struct unit_definition const* unit_definition = unit_definition_get(unit->definition_index);

	ASSERT(unit_definition);

	return unit_definition->unit.flags.test(_unit_definition_doesnt_show_readied_weapon_bit);
}

void unit_set_desired_grenade_type(datum unit_index, int16 grenade_type)
{
	unit_datum* unit = unit_try_and_get(unit_index);
	ASSERT((grenade_type >= 0) && (grenade_type < k_unit_grenade_types_count));
	unit->unit.current_grenade_index = (int8)grenade_type;
	unit->unit.desired_grenade_index = (int8)grenade_type;
	return;
}

void unit_add_grenade_type_to_inventory(datum unit_index, int16 grenade_type, int16 grenade_count)
{
	unit_datum* unit = unit_try_and_get(unit_index);
	ASSERT(grenade_count >= 0);
	ASSERT((grenade_type >= 0) && (grenade_type < k_unit_grenade_types_count));

	unit->unit.grenade_counts[grenade_type] += (int8)grenade_count;
	unit_set_desired_grenade_type(unit_index, grenade_type);

	// Originally the simulation did not update the grenade count when a grenade type was added to the unit's inventory
	// We want this to be updated across the simulation
	simulation_action_object_update(unit_index, FLAG(_simulation_action_update_grenade_count_bit));
	return;
}

/* private code */

static void __cdecl unit_set_weapon_type(
	datum unit_index,
	datum inventory_index,
	datum weapon_definition_index,
	int16 multiplayer_weapon_identifier,
	void* a5)
{
	INVOKE(0x144E38, 0x133C88, unit_set_weapon_type, unit_index, inventory_index, weapon_definition_index, multiplayer_weapon_identifier, a5);

	/* hacky but it'll mirror original functionality */
	/* the weapon state sync (ammo counter) will not apply if *only* the weapon type flag is set, as the code for that has been no-op'd */
	/* to avoid applying empty state updates to an weapon */
	/* but if the unit set weapon type flag is set, and the weapon picked up has no ammo, just set the weapon state to 0 */
	/* empty dual wieldable weapons do not receive state updates on pickup for some reason offhost */
	/* and if the weapon doesn't have an empty magazine, its state will be updates exactly after this fn call concludes */
	// TODO FIXME: doesn't fix it completely
	/*
	unit_datum* unit = unit_get(unit_index);
	datum weapon_index = unit->unit.weapon_object_indices[inventory_index];
	if (weapon_index != NONE)
	{
		weapon_datum* weapon = weapon_get(weapon_index);
		weapon->weapon.age = 0.0f;
		weapon->weapon.magazines[0].rounds_loaded = 0;
		weapon->weapon.magazines[0].rounds_inventory = 0;
	}*/
}

// Replace calls to use interpolated functions
static void unit_get_camera_position_patch_mass_functions(void)
{
	PatchCall(Memory::GetAddress(0x90C98, 0x48F98), object_get_center_of_mass_interpolated);
	PatchCall(Memory::GetAddress(0x13D406, 0x12C255), object_get_center_of_mass_interpolated);
	return;
}

// Replace calls to use interpolated functions
static void unit_get_camera_position_patch_marker_functions(void)
{
	PatchCall(Memory::GetAddress(0x13D3CF, 0x12C21E), object_get_markers_by_string_id);
	PatchCall(Memory::GetAddress(0x13D48D, 0x12C2DC), object_get_markers_by_string_id);
	return;
}

// Replace calls to use interpolated functions
static void unit_get_head_position_patch_functions(void)
{
	PatchCall(Memory::GetAddress(0x6C759), unit_get_head_position_interpolated);
	PatchCall(Memory::GetAddress(0x6EAB6), unit_get_head_position_interpolated);
	PatchCall(Memory::GetAddress(0x220E50), unit_get_head_position_interpolated);
	return;
}

static void unit_apply_interpolation_patches(void)
{
	unit_get_camera_position_patch_mass_functions();
	unit_get_camera_position_patch_marker_functions();
	unit_get_head_position_patch_functions();
	return;
}
