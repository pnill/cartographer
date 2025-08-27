#include "stdafx.h"
#include "weapons.h"

#include "cache/cache_files.h"
#include "game/game.h"
#include "saved_games/game_variant.h"
#include "units/units.h"

int32 __cdecl weapon_get_rounds_total(datum object_index, int32 magazine_index, bool a3)
{
	return INVOKE(0x15F313, 0x1435D3, weapon_get_rounds_total, object_index, magazine_index, a3);
}

int32 __cdecl weapon_get_rounds_available(datum weapon_index, int32 magazine_index, bool a3)
{
	return INVOKE(0x15F1AF, 0x14346F, weapon_get_rounds_available, weapon_index, magazine_index, a3);
}

void __cdecl weapons_fire_barrels(void)
{
	INVOKE(0x160AB7, 0x144D77, weapons_fire_barrels);
	return;
}

void weapon_take_inventory_rounds(datum weapon_index, int32 magazine_index, int32 round_count)
{
	weapon_datum* weapon = (weapon_datum*)object_get_fast_unsafe(weapon_index);
	weapon_definition* weapon_def = (weapon_definition*)tag_get_fast(weapon->definition_index);

	int32 available_rounds = weapon_get_rounds_available(weapon_index, magazine_index, false);

	s_game_variant* variant = get_game_variant();

	if (game_is_multiplayer() && variant)
		if (variant->cartographer_settings.flags.test(_cartographer_variant_infinite_ammo))
			return;

	if (available_rounds >= round_count)
	{
		int32 rounds_total = 0;
		if (magazine_index >= 0 && magazine_index < weapon_def->weapon.magazines.count)
		{
			int32 max_rounds = weapon->weapon.magazines[magazine_index].rounds_inventory_maximum;

			rounds_total = max_rounds;
			if (round_count < max_rounds)
				rounds_total = round_count;

			weapon->weapon.magazines[magazine_index].rounds_inventory_maximum = (int16)(max_rounds - rounds_total);

			if (rounds_total > 0)
				object_wake(weapon_index);
		}

		// if the desired round count has not been reached check other weapons in the unit's inventory for any matching weapons
		// and take the ammo from that (dual wielding)
		if (weapon->item.inventory_owner_unit_index != NONE && rounds_total < round_count)
		{
			unit_datum* owning_unit = (unit_datum*)object_get_fast_unsafe(weapon->item.inventory_owner_unit_index);

			if (owning_unit)
			{
				for (uint32 index = 0; index < NUMBEROF(owning_unit->unit.weapon_object_indices); ++index)
				{
					weapon_datum* unit_weapon = (weapon_datum*)object_get_fast_unsafe(owning_unit->unit.weapon_object_indices[index]);

					if (unit_weapon && unit_weapon->definition_index == weapon->definition_index)
					{
						int32 max_rounds = unit_weapon->weapon.magazines[magazine_index].rounds_inventory_maximum;

						int32 rounds_taken = round_count - rounds_total;

						if (round_count - rounds_total > max_rounds)
							rounds_taken = max_rounds;

						if (rounds_taken > 0)
						{
							unit_weapon->weapon.magazines[magazine_index].rounds_inventory_maximum = (int16)(max_rounds - rounds_taken);
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

void weapons_apply_patches()
{
	PatchCall(Memory::GetAddress(0x15FB9D, 0x143E5D), weapon_take_inventory_rounds);
}
