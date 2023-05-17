#include "stdafx.h"
#include "first_person_camera.h"

#include "Blam/Cache/TagGroups/unit_definition.hpp"
#include "Blam/Engine/game/player_control.h"
#include "Blam/Engine/objects/objects.h"
#include "Blam/Engine/units/units.h"

#include "H2MOD/Modules/CustomVariantSettings/CustomVariantSettings.h"
#include "H2MOD/Modules/Shell/Config.h"
#include "H2MOD/Tags/TagInterface.h"
#include "Util/Hooks/Hook.h"

#define _USE_MATH_DEFINES
#include <math.h>

bool fov_overridden = false;
float overridden_fov_radians;


float observer_suggested_field_of_view()
{
	return *Memory::GetAddress<float*>(0x413780, 0x3B5300);
}

void observer_set_suggested_field_of_view(float fov)
{
	float calculated_radians_FOV;
	if (currentVariantSettings.forcedFOV == 0)
	{
		calculated_radians_FOV = fov * M_PI / 180.0f;
	}
	else
	{
		calculated_radians_FOV = currentVariantSettings.forcedFOV * M_PI / 180.0f;
	}
	*Memory::GetAddress<float*>(0x413780, 0x3B5300) = calculated_radians_FOV;
}


void player_control_set_field_of_view(float fov) 
{
	fov_overridden = true;
	overridden_fov_radians = fov * M_PI / 180.0f;
}

float __cdecl player_control_get_field_of_view(int controller_index)
{
	const s_player_control* player_control_info = &s_player_control_globals::get()->local_players[controller_index];

	float result = observer_suggested_field_of_view();
	if (player_control_info->unit_datum_index != DATUM_INDEX_NONE)
	{
		float fov;
		
		if (currentVariantSettings.forcedFOV != 0)
		{
			fov = currentVariantSettings.forcedFOV;
		}
		else if (fov_overridden)
		{
			fov = overridden_fov_radians;
		}
		else
		{
			fov = tags::get_tag_fast<s_unit_group_definition>(object_get_fast_unsafe(player_control_info->unit_datum_index)->tag_definition_index)->camera_field_of_view.as_rad();
		}

		result = unit_get_field_of_view(player_control_info->unit_datum_index, fov, player_control_info->actions.weapon_indexes);
	}
	return result;
}

void first_person_camera_apply_patches()
{
	// Set the custom FOV's from our config beforehand
	player_control_set_field_of_view(H2Config_field_of_view);
	observer_set_suggested_field_of_view(H2Config_vehicle_field_of_view);

	// Patch this call for our custom fov implementation
	PatchCall(Memory::GetAddress(0xCD880, 0xB8BF9), player_control_get_field_of_view);
}