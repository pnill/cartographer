#include "stdafx.h"
#include "first_person_camera.h"

#include "director.h"
#include "observer.h"

#include "cache/cache_files.h"
#include "game/game.h"
#include "game/game_globals.h"
#include "game/players.h"
#include "game/player_control.h"
#include "math/matrix_math.h"
#include "saved_games/game_variant.h"
#include "saved_games/cartographer_player_profile/cartographer_player_profile.h"
#include "units/unit_definitions.h"
#include "units/vehicles.h"
#include "units/vehicle_definitions.h"

/* typedefs */
typedef void(__cdecl* t_first_person_camera_update)(int8* camera, s_director_update* director_update, s_observer_command* observer_command);

/* globals */

real32 g_camera_scale = 1.f;
real32 g_observer_suggested_field_of_view = DEGREES_TO_RADIANS(70.f);

t_first_person_camera_update p_first_person_camera_update;

/* prototypes */

static void __cdecl first_person_camera_build_observer_command(datum player_unit_index, real_vector3d* unit_facing, s_observer_command* observer_command);

static void __cdecl first_person_camera_update(int8* camera, s_director_update* director_update, s_observer_command* result);

/* public code */

void first_person_camera_apply_patches(void)
{
	// Patch this call for our custom fov implementation
	DETOUR_ATTACH(p_first_person_camera_update, Memory::GetAddress<t_first_person_camera_update>(0xCD7E1), first_person_camera_update);
	return;
}

/* private code */

static void __cdecl first_person_camera_build_observer_command(
	datum player_unit_index, 
	real_vector3d* unit_facing,
	s_observer_command* result)
{
	result->timer = 0;
	result->flags = 0;
	result->position.orientation = *global_zero_vector3d;
	result->focus_distance = 0.f;
	result->forward = *unit_facing;
	result->field_of_view = g_observer_suggested_field_of_view;

	generate_up_vector3d(&result->forward, &result->up);
	assert_valid_real_vector3d_axes2(&result->forward, &result->up);

	if (player_unit_index != NONE)
	{
		unit_datum* player_unit = unit_get(player_unit_index);
		
		unit_get_camera_position(player_unit_index, &result->position.position);
		object_get_velocities(player_unit_index, &result->velocity, nullptr);
		
		if (player_unit->object.parent_object_index != NONE)
		{
			vehicle_datum* vehicle_unit = vehicle_get(player_unit->object.parent_object_index);
			if (vehicle_unit)
			{
				struct vehicle_definition const* vehicle_definition = vehicle_definition_get(vehicle_unit->definition_index);

				ASSERT(vehicle_definition);

				unit_seat const* seat = TAG_BLOCK_GET_ELEMENT(&vehicle_definition->unit.seats, player_unit->unit.parent_seat_index, unit_seat);

				if (seat->flags.test(_unit_seat_definition_first_person_camera_slaved_to_gun))
				{
					object_marker vehicle_marker;

					if (object_get_markers_by_string_id(player_unit->object.parent_object_index, (string_id)0xF0000DBu, &vehicle_marker, 1))
					{
						result->position.position = vehicle_marker.matrix.position;
						result->forward = vehicle_marker.matrix.vectors.forward;
						result->up = vehicle_marker.matrix.vectors.up;
					}
					else
					{
						real_point3d parent_position;
						real_matrix4x3 parent_matrix;
						object_get_origin_interpolated(player_unit->object.parent_object_index, &parent_position);
						matrix4x3_from_point_and_vectors(&parent_matrix, &parent_position, &vehicle_unit->object.forward, &vehicle_unit->object.up);
						matrix4x3_inverse_transform_normal(&parent_matrix, &result->forward, &result->forward);
						generate_up_vector3d(&result->forward, &result->up);
						matrix4x3_transform_vector(&parent_matrix, &result->forward, &result->forward);
						matrix4x3_transform_vector(&parent_matrix, &result->up, &result->up);
					}
				}
			}
		}

		result->flags = 1;
	}
	
	observer_validate_camera_command(result);

	return;
}

static void __cdecl first_person_camera_update(int8* camera, s_director_update* director_update, s_observer_command* result)
{
	ASSERT(camera);
	ASSERT(director_update);
	ASSERT(result);

	datum player_unit_index = NONE;
	if (players_user_is_active(director_update->user_index))
	{
		const datum player_index = player_index_from_user_index(director_update->user_index);
		player_unit_index = player_get(player_index)->unit_index;
	}

	real_vector3d unit_facing;
	player_control_get_facing_direction(director_update->user_index, &unit_facing);
	first_person_camera_build_observer_command(player_unit_index, &unit_facing, result);
	
	// New method to override the crosshair offset using carto profiles
	if (true)
	{
		s_game_variant* variant = get_game_variant();

		if (game_is_multiplayer() && variant && variant->cartographer_settings.flags.test(_cartographer_variant_force_default_cross_hair_offset))
		{
			result->crosshair_position = k_observer_default_cross_hair_position;
		}
		else
		{
			s_saved_game_cartographer_player_profile* player_profile = cartographer_player_profile_get_by_user_index(director_update->user_index);

			result->crosshair_position.x = k_observer_default_cross_hair_position.x;
			result->crosshair_position.y = player_profile->crosshair_offset;
		}
	}
	// Original method of setting the crosshair position
	else
	{
		s_game_globals const* globals = scenario_get_game_globals();
		s_game_globals_player_control const* player_control = TAG_BLOCK_GET_ELEMENT(&globals->player_control, 0, s_game_globals_player_control);

		result->crosshair_position = player_control->crosshair_location;
	}

	real32 field_of_view = player_control_get_field_of_view(director_update->user_index) * g_camera_scale;
	field_of_view = PIN(field_of_view, DEGREES_TO_RADIANS(1.f), DEGREES_TO_RADIANS(110.f));

	result->field_of_view = field_of_view;

	if(*(real32*)camera != field_of_view)
	{
		result->observer_command_values[4] = 0.18f;
		result->observer_command_flags[4] = _observer_command_bit_1;
		*(real32*)camera = field_of_view;
	}
	return;
}
