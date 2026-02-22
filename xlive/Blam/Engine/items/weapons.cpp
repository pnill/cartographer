#include "stdafx.h"
#include "weapons.h"

#include "weapon_definitions.h"
#include "cache/cache_files.h"
#include "H2MOD/Modules/CustomVariantSettings/CustomVariantSettings.h"
#include "objects/damage_reporting.h"

/* prototypes */

static void weapon_barrel_idle(uint32 weapon_index, uint16 barrel_index);

/* private code */

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

void weapon_barrel_idle(uint32 weapon_index, uint16 barrel_index)
{
    weapon_datum* weapon = (weapon_datum*)object_get(weapon_index);
    weapon_definition* weapon_def = (weapon_definition*)tag_get_fast(weapon->definition_index);

    ASSERT(weapon);
    ASSERT(weapon_def);

    weapon_barrel* weapon_barrel = &weapon->weapon.barrels[barrel_index];
    weapon_barrel_definition* barrel_def = weapon_def->weapon.barrels[barrel_index];

    weapon_barrel->firing_idle_ticks = 0;
    weapon_barrel->fire_count = 0;
    weapon_barrel->state = _weapon_barrel_state_idle;

    bool dub_shot_test = barrel_def->damage_effect_reporting_type == _damage_reporting_type_battle_rifle && currentVariantSettings.disable_dub_shot;

    if (!barrel_def->flags.test(_weapon_barrel_definition_dont_clear_fire_bit_after_recovering) || dub_shot_test)
    {
        weapon_barrel->flags.set(_weapon_barrel_fire_bit, false);
    }
}

/* public code */

void weapons_apply_patches()
{
    PatchCall(Memory::GetAddress(0x15C60C), weapon_barrel_idle_usercall_to_rewritten);
    PatchCall(Memory::GetAddress(0x1611AD), weapon_barrel_idle_usercall_to_rewritten);
    PatchCall(Memory::GetAddress(0x162B38), weapon_barrel_idle_usercall_to_rewritten);
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
