#include "stdafx.h"
#include "first_person_camera.h"

#include "user_interface_controller.h"
#include "cache/cache_files.h"
#include "game/player_control.h"
#include "objects/objects.h"
#include "units/units.h"
#include "units/unit_definitions.h"

#include "H2MOD/Modules/CustomVariantSettings/CustomVariantSettings.h"
#include "H2MOD/Modules/Shell/Config.h"
#include "saved_games/cartographer_player_profile.h"


bool player_control_fov_overridden = false;
float overridden_fov_radians;

float observer_suggested_field_of_view()
{
	return *Memory::GetAddress<float*>(0x413780, 0x3B5300);
}

void observer_set_suggested_field_of_view(float fov)
{
	// Don't change the fov if it's 0 or greater than 110
	if (fov <= 0 || fov > 110) return;

	float final_fov_rad;
	if (currentVariantSettings.forced_fov == 0)
	{
		final_fov_rad = DEGREES_TO_RADIANS(fov);
	}
	else
	{
		final_fov_rad = DEGREES_TO_RADIANS(currentVariantSettings.forced_fov);
	}
	*Memory::GetAddress<float*>(0x413780, 0x3B5300) = final_fov_rad;
}


void player_control_set_field_of_view(float fov) 
{
	// Don't change the fov if it's 0 or greater than 110
	if (fov <= 0 || fov > 110) return;

	player_control_fov_overridden = true;
	overridden_fov_radians = DEGREES_TO_RADIANS(fov);
}

real32 __cdecl player_control_get_field_of_view(uint32 user_index)
{
	const s_player_control* player_control_info = player_control_get(user_index);

	float result = observer_suggested_field_of_view();
	
	if (player_control_info->unit_datum_index != NONE)
	{
		float fov;
		const s_saved_game_cartographer_player_profile_v1* profile_settings = cartographer_player_profile_get_by_user_index(user_index);

		if (currentVariantSettings.forced_fov != 0)
		{
			fov = DEGREES_TO_RADIANS(currentVariantSettings.forced_fov);
		}
		else if (profile_settings)
		{
			fov = DEGREES_TO_RADIANS(profile_settings->field_of_view);
		}
		else
		{
			_unit_definition* unit = (_unit_definition*)tag_get_fast(object_get_fast_unsafe(player_control_info->unit_datum_index)->tag_definition_index);
			fov = unit->camera_field_of_view;
		}

		result = unit_get_field_of_view(player_control_info->unit_datum_index, fov, player_control_info->actions.zoom_level);
	}
	return result;
}

void first_person_camera_apply_patches(void)
{
	// Set the custom FOV's from our config beforehand
	player_control_set_field_of_view(H2Config_field_of_view);
	observer_set_suggested_field_of_view(H2Config_vehicle_field_of_view);

	// Patch this call for our custom fov implementation
	PatchCall(Memory::GetAddress(0xCD880, 0xB8BF9), player_control_get_field_of_view);
	return;
}