#include "stdafx.h"
#include "simulation_game_units.h"

#include "simulation_game_objects.h"

#include "Blam/Engine/game/game_engine.h"
#include "Blam/Engine/math/color_math.h"
#include "Blam/Engine/units/units.h"

#include "Util/Hooks/Hook.h"

datum __stdcall c_simulation_unit_entity_definition__create_object(void* _this,
    int32 creation_data_size,
    s_simulation_unit_creation_data* creation_data,
    uint32* flags,
    int32 internal_state_data_size,
    s_simulation_unit_state_data* initial_state_data)
{
    real_color_rgb change_colors[4]; // [esp+18h] [ebp-F8h] BYREF
    
    object_placement_data placement_data;
    object_placement_data_new(&placement_data, creation_data->object.object_definition_index, -1, 0);
    c_simulation_object_entity_definition__object_setup_placement_data(_this, &creation_data->object, &initial_state_data->object_state_data, flags, &placement_data);

    // Check if the unit is controlled by a player and change color is available before we override it
    if (initial_state_data->controlling_player_index != NONE && game_engine_get_change_colors(&creation_data->profile_traits.profile, creation_data->team, change_colors))
    {
        placement_data.active_change_colors_mask |= 15u;
        memcpy(placement_data.change_colors, change_colors, sizeof(placement_data.change_colors));
    }

    datum unit_index = c_simulation_object_entity_definition__object_create_object(_this, &creation_data->object, &initial_state_data->object_state_data, flags, &placement_data);
    if (unit_index != NONE)
    {
        unit_delete_all_weapons(unit_index);
    }

    return unit_index;
}

void simulation_game_units_apply_patches(void)
{
    // Replace calls to this function so units that aren't players don't have their colors overridden
    DetourClassFunc(Memory::GetAddress<uint8*>(0x1F9DCB, 0x1E3B33), (uint8*)c_simulation_unit_entity_definition__create_object, 11);
	return;
}