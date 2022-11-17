#include "stdafx.h"
#include "damage.h"

#include "Blam/Engine/Objects/Objects.h"
#include "Blam/Engine/Objects/Objects.h"
#include "H2MOD/Variants/VariantSystem.h"
#include "Util/Hooks/Hook.h"
#include "H2MOD/Utils/Utils.h"

// Used to get damage on any object
typedef void(__cdecl* object_cause_damage_t)(s_damage_data* damage_data, int damaged_object_indexes, __int16 a4, __int16 a5, __int16 a6, int a7);
object_cause_damage_t p_object_cause_damage;

// Engine call to set damage applied on an object by a projectile
void __cdecl carto_projectile_collision_object_cause_damage(s_damage_data* damage_data, int damaged_object_indexes, __int16 a4, __int16 a5, __int16 a6, int a7)
{
	// Hook on call to prevent guardian glitching on Infection gametype
	if (CustomVariantHandler::VariantEnabled(_id_infection)) {
		if (!DATUM_IS_NONE(damage_data->creator_datum) && damage_data->field_10 != -1)
		{
			LOG_TRACE_GAME(
				"{} {} {} {} {} {} {} {}",
				__FUNCTION__,
				damage_data->flags,
				IntToString<int>(damage_data->damage_tag_index, std::hex),
				IntToString<int>(damage_data->creator_datum, std::hex),
				IntToString<int>(damage_data->field_10, std::hex), //TODO reverse what field_10 is
				IntToString<int>(damage_data->field_14, std::hex),
				IntToString<int>(damage_data->field_18, std::hex),
				IntToString<int>(damage_data->field_1C, std::hex),
				IntToString<int>(damage_data->field_24, std::hex),
				IntToString<int>(damage_data->field_28, std::hex)
			);
			p_object_cause_damage(damage_data, damaged_object_indexes, a4, a5, a6, a7);
		}
		else
		{
			LOG_TRACE_GAME("GUARDIAN GLITCH PREVENTED");
		}
	}
	else
	{
		//Calls basic engine function when not in zombies game
		p_object_cause_damage(damage_data, damaged_object_indexes, a4, a5, a6, a7);
	}
}

/*This is to get the datum of the last player who damaged the datum/unit provided */
int get_damage_owner(datum damaged_unit_index)
{
	char* damaged_player_ptr = (char*)object_try_and_get_and_verify_type(damaged_unit_index, FLAG(e_object_type::biped) | FLAG(e_object_type::vehicle));
	if (damaged_player_ptr)
	{
		return *(int*)(damaged_player_ptr + 0xC8); // player_ptr/unit_ptr + 0xC8 = damaging player this works on vehicles/AI and such too.
	}

	return NONE;
}

namespace damage
{
	void ApplyPatches()
	{
		//Guardian Patch
		p_object_cause_damage = Memory::GetAddress<object_cause_damage_t>(0x17AD81, 0x1525E1);
		PatchCall(Memory::GetAddress(0x147DB8, 0x172D55), carto_projectile_collision_object_cause_damage);
	}
}
