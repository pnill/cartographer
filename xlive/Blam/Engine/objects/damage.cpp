#include "stdafx.h"
#include "damage.h"

#include "game/game.h"
#include "game/game_options.h"
#include "networking/network_event.h"
#include "units/units.h"

#include "shell/shell.h"

#include "H2MOD/Modules/CustomVariantSettings/CustomVariantSettings.h"

/* public code */

void damage_apply_patches(void)
{
	// Hook on call to prevent guardian glitching
	// Used to disable it in Infection only, became a bigger problem so now we have to disable it globally.....
	PatchCall(Memory::GetAddress(0x147DB8, 0x172D55), object_cause_damage);

	if (!shell_is_dedicated_server()) 
	{
		PatchCall(Memory::GetAddress(0x1FD293), object_apply_damage_aftermath);
	}
	return;
}

void damage_initialize_for_new_map(void)
{
	// TODO: implement debug functionality
	return;
}

void __cdecl object_cause_damage(s_damage_data* damage_data, datum object_index, int16 node_index, int16 region_index, int16 material_index, real_vector3d* object_normal)
{
	bool prevent_guardian_glitch = false;

	// Only run the patch if we're in multiplayer and friendly fire is off
	if (game_is_multiplayer() && TEST_BIT(game_options_get()->game_variant.game_engine_flags, _game_engine_friendly_fire_bit) == 0)
	{
		// Obtain the actor index so we can determine whether or not to disable damage
		// If object is a unit then we can grab the actor index
		const unit_datum* unit = unit_try_and_get(damage_data->owner.owner_object_index);
		const datum actor_index = unit != NULL ? unit->unit.actor_index : NONE;

		// Disable damage if all are true:
		// 1. Not coming from an actor (actor index is none)
		// 2. Does not have a valid player index or team
		if (actor_index == NONE && (damage_data->owner.owner_player_index == NONE || damage_data->owner.owner_team_index == _game_team_observer))
		{
			event(_event_verbose, "objects:damage: GUARDIAN GLITCH PREVENTED");
			prevent_guardian_glitch = true;
		}
	}

	if (!prevent_guardian_glitch)
	{
		INVOKE(0x17AD81, 0x1525E1, object_cause_damage, damage_data, object_index, node_index, region_index, material_index, object_normal);
	}
	return;
}

void __cdecl object_apply_damage_aftermath(datum object_index, s_damage_aftermath_data* aftermath_data)
{
	SET_BIT(aftermath_data->flags, _damage_aftermath_has_explosion_physics, currentVariantSettings.explosionPhysics);
	INVOKE(0x17A25D, 0x0, object_apply_damage_aftermath, object_index, aftermath_data);
	return;
}