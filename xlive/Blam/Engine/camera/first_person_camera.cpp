#include "stdafx.h"
#include "first_person_camera.h"

#include "director.h"
#include "observer.h"
#include "game/game_globals.h"
#include "game/player_control.h"
#include "saved_games/cartographer_player_profile.h"
#include "units/units.h"
#include "units/unit_definitions.h"
#include "units/vehicle_definitions.h"

/* private code */

void __cdecl first_person_camera_build_observer_command(datum player_unit_index, real_vector3d* unit_facing, s_observer_command* observer_command)
{
	ASSERT(unit_facing);
	ASSERT(observer_command);

	observer_command->timer = 0;
	observer_command->flags = 0;
	observer_command->position.orientation = global_zero_vector3d;
	observer_command->focus_distance = 0.f;
	observer_command->forward = *unit_facing;
	observer_command->field_of_view = DEGREES_TO_RADIANS(70.f);
	generate_up_vector3d(&observer_command->forward, &observer_command->up);
	if(player_unit_index != NONE)
	{
		unit_datum* player_unit = (unit_datum*)object_get_fast_unsafe(player_unit_index);
		unit_get_camera_position(player_unit_index, &observer_command->position.position);
		object_get_velocities(player_unit_index, &observer_command->velocity, nullptr);
		if(player_unit->object.parent_object_index)
		{
			unit_datum* vehicle_unit = (unit_datum*)object_try_and_get_and_verify_type(player_unit->object.parent_object_index, _object_type_vehicle);
			if(vehicle_unit)
			{
				_vehicle_definition* vehicle_definition = (_vehicle_definition*)tag_get_fast(vehicle_unit->object.tag_definition_index);

				if(vehicle_definition->unit.seats[player_unit->parent_seat_index]->flags.test(_unit_seat_definition_first_person_camera_slaved_to_gun))
				{
					object_marker vehicle_marker{};
					if(object_get_markers_by_string_id(player_unit->object.parent_object_index, (string_id)0xF0000DBu, &vehicle_marker, 1))
					{
						observer_command->position.position = vehicle_marker.matrix.position;
						observer_command->forward = vehicle_marker.matrix.vectors.forward;
						observer_command->up = vehicle_marker.matrix.vectors.up;
					}
					else
					{
						real_point3d parent_position{};
						real_matrix4x3 parent_matrix{};
						object_get_origin_interpolated(player_unit->object.parent_object_index, &parent_position);
						matrix4x3_from_point_and_vectors(&parent_matrix, &parent_position, &vehicle_unit->object.forward, &vehicle_unit->object.up);
						matrix4x3_inverse_transform_normal(&parent_matrix, &observer_command->forward, &observer_command->forward);
						generate_up_vector3d(&observer_command->forward, &observer_command->up);
						matrix4x3_transform_vector(&parent_matrix, &observer_command->forward, &observer_command->forward);
						matrix4x3_transform_vector(&parent_matrix, &observer_command->up, &observer_command->up);
					}
				}
			}
		}
		observer_command->flags = 1;
	}
	observer_validate_camera_command(observer_command);
}
typedef void(__cdecl* t_first_person_camera_update)(int8* camera, s_director_update* director_update, s_observer_command* observer_command);
t_first_person_camera_update p_first_person_camera_update;

void __cdecl first_person_camera_update(int8* camera, s_director_update* director_update, s_observer_command* observer_command)
{
	ASSERT(camera);
	ASSERT(director_update);
	ASSERT(observer_command);

	datum player_unit_index = NONE;
	if (players_user_is_active(director_update->user_index))
		player_unit_index = s_player::get_unit_index(player_index_from_user_index(director_update->user_index));

	real_vector3d unit_facing{};

	player_control_get_facing_direction(director_update->user_index, &unit_facing);
	first_person_camera_build_observer_command(player_unit_index, &unit_facing, observer_command);

	s_game_globals* globals = scenario_get_game_globals();
	s_saved_game_cartographer_player_profile* player_profile = cartographer_player_profile_get_by_user_index(director_update->user_index);
	observer_command->crosshair_position.x = 0.f;
	observer_command->crosshair_position.y = player_profile->crosshair_offset;
	real32 field_of_view = player_control_get_field_of_view(director_update->user_index) * 1.f;

	field_of_view = PIN(field_of_view, DEGREES_TO_RADIANS(1.f), DEGREES_TO_RADIANS(110.f));

	observer_command->field_of_view = field_of_view;

	if(*(real32*)camera != field_of_view)
	{
		observer_command->observer_command_values[4] = 0.18f;
		observer_command->observer_command_flags[4] = _observer_command_bit_1;
		*(real32*)camera = field_of_view;
	}
}

/* public code */

void first_person_camera_apply_patches(void)
{
	// Patch this call for our custom fov implementation
	DETOUR_ATTACH(p_first_person_camera_update, Memory::GetAddress<t_first_person_camera_update>(0xCD7E1), first_person_camera_update);
	return;
}