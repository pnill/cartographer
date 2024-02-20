#include "stdafx.h"
#include "simulation_game_units.h"

#include "simulation_game_objects.h"

#include "Blam/Engine/game/game_globals.h"
#include "Blam/Engine/game/game_engine.h"
#include "Blam/Engine/math/color_math.h"
#include "Blam/Engine/units/units.h"



datum __stdcall c_simulation_unit_entity_definition__create_object(void* _this,
    int32 creation_data_size,
    s_simulation_unit_creation_data* creation_data,
    uint32* flags,
    int32 internal_state_data_size,
    s_simulation_unit_state_data* initial_state_data)
{
    real_rgb_color change_colors[4];
    
    object_placement_data placement_data;
    object_placement_data_new(&placement_data, creation_data->object.object_definition_index, -1, 0);

    // Hacky hack for player variants
    // TODO Remove this once we get tag injection working on servers
    if (initial_state_data->controlling_player_index != NONE)
    {
        if (datum unit_rep_tag_index = game_globals_get_representation(creation_data->profile_traits.profile.player_character_type)->third_person_unit.index;
            unit_rep_tag_index != NONE)
        {
            placement_data.tag_index = unit_rep_tag_index;
            creation_data->object.object_definition_index = unit_rep_tag_index;
        }
    }
    c_simulation_object_entity_definition__object_setup_placement_data(_this, &creation_data->object, &initial_state_data->object_state_data, flags, &placement_data);

    // We check the following in order to force the player colour
    // The unit is not controlled by an actor
    // The function game_engine_get_change_colors is able to retrieve the colours for the engine mode
    if ((initial_state_data->controlling_actor_index == NONE) &&
        game_engine_get_change_colors(&creation_data->profile_traits.profile, creation_data->team, change_colors))
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