#include "stdafx.h"
#include "weapons.h"

#include "weapon_definitions.h"
#include "cache/cache_files.h"
#include "game/game.h"
#include "objects/damage_reporting.h"
#include "saved_games/game_variant.h"
#include "shell/shell.h"
#include "units/units.h"

/* prototypes */

static void weapon_barrel_idle(uint32 weapon_index, uint16 barrel_index);
static void weapon_take_inventory_rounds(datum weapon_index, int32 magazine_index, int32 round_count);
static __declspec(naked) void weapon_barrel_idle_usercall_to_rewritten()
{
	__asm
	{
		push ebx

		mov ebx, [esp + 0x4 + 0x4]

		push ebx
		push eax

		call weapon_barrel_idle

		add esp, 2 * 4

		pop ebx

		retn
	}
}


/* public code */

void weapons_apply_patches()
{
	PatchCall(Memory::GetAddress(0x15C60C, 0x1408CC), weapon_barrel_idle_usercall_to_rewritten);
	PatchCall(Memory::GetAddress(0x1611AD, 0x14546D), weapon_barrel_idle_usercall_to_rewritten);
	PatchCall(Memory::GetAddress(0x162B38, 0x146DF8), weapon_barrel_idle_usercall_to_rewritten);

	PatchCall(Memory::GetAddress(0x15FB9D, 0x143E5D), weapon_take_inventory_rounds);
}

int32 __cdecl weapon_get_rounds_total(datum object_index, int32 magazine_index, bool a3)
{
	return INVOKE(0x15F313, 0x1435D3, weapon_get_rounds_total, object_index, magazine_index, a3);
}

void __cdecl weapons_fire_barrels(void)
{
	INVOKE(0x160AB7, 0x144D77, weapons_fire_barrels);
	return;
}

int32 __cdecl weapon_get_rounds_available(datum weapon_index, int32 magazine_index, bool a3)
{
	return INVOKE(0x15F1AF, 0x14346F, weapon_get_rounds_available, weapon_index, magazine_index, a3);
}

/* private code */

static void weapon_barrel_idle(uint32 weapon_index, uint16 barrel_index)
{
	weapon_datum* weapon = (weapon_datum*)object_get(weapon_index);
	struct weapon_definition* weapon_definition = weapon_definition_get(weapon->definition_index);

	ASSERT(weapon);
	ASSERT(weapon_definition);

	weapon_barrel* weapon_barrel = &weapon->weapon.barrels[barrel_index];
	weapon_barrel_definition* barrel_def = TAG_BLOCK_GET_ELEMENT(&weapon_definition->weapon.barrels, barrel_index, weapon_barrel_definition);

	weapon_barrel->firing_idle_ticks = 0;
	weapon_barrel->fire_count = 0;
	weapon_barrel->state = _weapon_barrel_state_idle;

	s_game_variant* variant = get_game_variant();

	bool force_idle = false;

	if (game_is_multiplayer() && variant)
		force_idle = barrel_def->damage_effect_reporting_type == _damage_reporting_type_battle_rifle && variant->cartographer_settings.flags.test(_cartographer_variant_disable_dub_shot);

	if (!barrel_def->flags.test(_weapon_barrel_definition_dont_clear_fire_bit_after_recovering) || force_idle)
		weapon_barrel->flags.set(_weapon_barrel_fire_bit, false);
}

static void weapon_take_inventory_rounds(datum weapon_index, int32 magazine_index, int32 round_count)
{
	weapon_datum* weapon = weapon_get(weapon_index);
	struct weapon_definition* weapon_definition = weapon_definition_get(weapon->definition_index);

	int32 available_rounds = weapon_get_rounds_available(weapon_index, magazine_index, false);

	s_game_variant* variant = get_game_variant();

	if (game_is_multiplayer() && variant)
		if (variant->cartographer_settings.flags.test(_cartographer_variant_infinite_ammo))
			return;

	if (available_rounds >= round_count)
	{
		int32 rounds_total = 0;
		if (magazine_index >= 0 && magazine_index < weapon_definition->weapon.magazines.count)
		{
			int32 max_rounds = weapon->weapon.magazines[magazine_index].rounds_inventory;

			rounds_total = MIN(round_count, max_rounds);

			weapon->weapon.magazines[magazine_index].rounds_inventory = (int16)(max_rounds - rounds_total);

			if (rounds_total > 0)
				object_wake(weapon_index);
		}

		// if the desired round count has not been reached check other weapons in the unit's inventory for any matching weapons
		// and take the ammo from that (dual wielding)
		if (weapon->item.inventory_owner_unit_index != NONE && rounds_total < round_count)
		{
			unit_datum* owning_unit = unit_get(weapon->item.inventory_owner_unit_index);

			if (owning_unit)
			{
				for (uint32 index = 0; index < NUMBEROF(owning_unit->unit.weapon_object_indices); ++index)
				{
					weapon_datum* unit_weapon = weapon_get(owning_unit->unit.weapon_object_indices[index]);

					if (unit_weapon && unit_weapon->definition_index == weapon->definition_index)
					{
						int32 max_rounds = unit_weapon->weapon.magazines[magazine_index].rounds_inventory;

						int32 rounds_taken = MIN(round_count - rounds_total, max_rounds);

						if (rounds_taken > 0)
						{
							unit_weapon->weapon.magazines[magazine_index].rounds_inventory = (int16)(max_rounds - rounds_taken);
							rounds_total += rounds_taken;

							if (rounds_total < round_count)
								break;
						}
					}
				}
			}
		}
	}
}
