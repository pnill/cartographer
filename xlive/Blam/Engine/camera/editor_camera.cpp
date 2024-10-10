#include "stdafx.h"
#include "editor_camera.h"

#include "director.h"
#include "observer.h"
#include "game/player_control.h"
#include "saved_games/cartographer_player_profile.h"

typedef void(__cdecl* t_editor_camera_orbiting_update)(s_editor_camera* camera, s_director_update* director_update, s_observer_command* observer_command);
t_editor_camera_orbiting_update p_editor_camera_orbiting_update;

void __cdecl editor_camera_orbiting_update(s_editor_camera* camera, s_director_update* director_update, s_observer_command* observer_command)
{
	s_player_control_camera_info camera_info;
	player_control_get_camera_info(director_update->user_index, &camera_info);

	observer_command->position.position = camera_info.location;

	if (director_update->unk_1)
	{
		camera->facing.yaw += director_update->facing.yaw;
		camera->facing.pitch = PIN(camera->facing.pitch + director_update->facing.pitch, DEGREES_TO_RADIANS(-90), DEGREES_TO_RADIANS(90));
		director_set_unknown_flag(director_update->user_index);
	}

	camera->focus_distance = PIN(camera->focus_distance - (director_update->turn_scale * 0.3f), 0, 0.6f);

	if(camera_info.slave_object != NONE)
	{
		vector3d_from_euler_angles2d(&observer_command->forward, &camera->facing);
		generate_up_vector3d(&observer_command->forward, &observer_command->up);
		object_get_velocities(camera_info.slave_object, &observer_command->velocity, 0);
		observer_command->flags = 1;
	}

	observer_command->position.orientation = global_zero_vector3d;
	observer_command->focus_distance = camera->focus_distance;

	s_saved_game_cartographer_player_profile* player_profile = cartographer_player_profile_get_by_user_index(director_update->user_index);

	observer_command->field_of_view = player_profile->vehicle_field_of_view;
	observer_command->timer = 0.5f;
	observer_validate_camera_command(observer_command);
}


void editor_camera_apply_patches()
{
	DETOUR_ATTACH(p_editor_camera_orbiting_update, Memory::GetAddress<t_editor_camera_orbiting_update>(0xCC58A), editor_camera_orbiting_update);
}