#include "stdafx.h"
#include "following_camera.h"

#include "camera_track_definition.h"
#include "director.h"
#include "observer.h"
#include "game/game_globals.h"
#include "game/player_control.h"
#include "saved_games/cartographer_player_profile.h"

//void following_camera_update_apply_camera_track(s_unit_camera* unit_camera, real32 delta, real_vector3d* forward_out, 
//	real_point3d* position_out, bool tight_camera_track)
//{
//	s_camera_track_definition* camera_track;
//
//	// if the unit camera doesn't have any tracks, get the default unit track from game globals.
//	if (!unit_camera->camera_tracks.count)
//	{
//		camera_track = game_globals_get_default_camera_track();
//	}
//	else
//	{
//		// if tight camera track is required the unit must have an additional camera track added
//		// in index 1 that is used for tight tracking if it doesn't have one then default back to the base track
//		int8 track_index = 0;
//		if(tight_camera_track && unit_camera->camera_tracks.count > 1)
//			track_index = 1;
//
//		camera_track = (s_camera_track_definition*)tag_get_fast(unit_camera->camera_tracks[track_index]->track.index);
//
//		if(!camera_track)
//			camera_track = game_globals_get_default_camera_track();
//	}
//
//	if(camera_track->control_points.count < 4)
//	{
//		*forward_out = global_forward3d;
//		position_out->x = 0;
//		position_out->y = 0;
//		position_out->z = 0;
//	}
//
//
//}
//
//// todo: someone figure out what this is, doing I just guessed names
//void following_camera_update_pitch(s_unit_camera* unit_camera, real32 desired_pitch, bool tight_camera_track)
//{
//	real32 pitch_accum_1 = 0.f;
//	real32 pitch_accum_2 = 1.f;
//
//	do
//	{
//		real32 accum = (pitch_accum_1 + pitch_accum_2) * 0.5f;
//
//
//	} while (pitch_accum_2 - pitch_accum_1 > 0.001f);
//}
//
//typedef void(__cdecl* t_following_camera_update)(s_following_camera* camera, s_director_update* director_update, s_observer_command* observer_command);
//t_following_camera_update p_following_camera_update;
//
//void __cdecl following_camera_update(s_following_camera* camera, s_director_update* director_update, s_observer_command* observer_command)
//{
//	s_player_control_camera_info camera_info;
//	player_control_get_camera_info(director_update->user_index, &camera_info);
//
//	observer_command->position.position = camera_info.location;
//	observer_command->timer = 0.f;
//	observer_command->flags = 0;
//
//	s_saved_game_cartographer_player_profile* player_profile = cartographer_player_profile_get_by_user_index(director_update->user_index);
//
//	observer_command->field_of_view = player_profile->vehicle_field_of_view;
//
//	if(camera->valid)
//	{
//		if(camera_info.slave_object != camera->slave_object || camera_info.parent_seat_index != camera->parent_seat_index)
//		{
//			observer_command->timer = PIN(observer_command->timer, 0, 0.5);
//		}
//	}
//
//	camera->slave_object = camera_info.slave_object;
//	camera->parent_seat_index = camera_info.parent_seat_index;
//
//	if (camera_info.unit_camera)
//	{
//		unit_datum* unit = (unit_datum*)object_get_fast_unsafe(camera->slave_object);
//
//		const bool flag_test = (unit->control_flags.test(_unit_control_bit_1) || unit->control_flags.test(_unit_control_bit_2));
//		if (flag_test != camera->unit_control_flags_test)
//		{
//			observer_command->observer_command_flags[1] = _observer_command_bit_1;
//			observer_command->observer_command_values[1] = PIN(observer_command->observer_command_values[1], 0, 0.5);
//			camera->unit_control_flags_test = flag_test;
//		}
//
//		const bool tight_camera_desired = unit_desires_tight_camera_track(camera->slave_object);
//		if (tight_camera_desired != camera->desires_tight_camera_track)
//		{
//			observer_command->timer = PIN(observer_command->timer, 0, 0.5);
//			camera->desires_tight_camera_track = tight_camera_desired;
//		}
//
//		if (director_update->unk_1)
//		{
//			camera->facing.yaw += director_update->facing.yaw;
//			camera->facing.pitch += director_update->facing.pitch;
//
//			observer_command->observer_command_flags[5] = _observer_command_bit_1;
//			observer_command->observer_command_values[5] = PIN(observer_command->observer_command_values[5], 0, 0.4f);
//		}
//		else if (camera->facing.yaw != 0.0f || camera->facing.pitch != 0.0f)
//		{
//			camera->facing.yaw = 0.f;
//			camera->facing.pitch = 0.f;
//		}
//
//		// guessed name might not even be what it does
//		real32 turn_scale_rate = camera->turn_scale_maybe - (director_update->turn_scale * 0.05f);
//		turn_scale_rate = PIN(turn_scale_rate, 0, 5.0f);
//		camera->turn_scale_maybe = turn_scale_rate;
//
//		if (director_update->turn_scale != 0.f)
//		{
//			observer_command->observer_command_flags[3] = _observer_command_bit_1;
//			observer_command->observer_command_values[3] = PIN(observer_command->observer_command_values[3], 0, 0.4f);
//			observer_command->observer_command_flags[1] = _observer_command_bit_1;
//			observer_command->observer_command_values[1] = PIN(observer_command->observer_command_values[1], 0, 0.4f);
//		}
//
//		real_euler_angles2d* player_facing = player_control_get_facing(director_update->user_index);
//
//		real32 pinned_pitch = PIN(camera->facing.pitch + player_facing->pitch, DEGREES_TO_RADIANS(-90), DEGREES_TO_RADIANS(90));
//
//		following_camera_update_pitch(camera_info.unit_camera, pinned_pitch, camera->desires_tight_camera_track);
//	}
//}

// Temporary following camera fov implementation until the rewrite is done
// replace two calls in the update function to capture the user index and use it to grab the fov for that user
int32 updating_user_index = 0;

void __cdecl following_camera_player_control_get_camera_info(int32 user_index, s_player_control_camera_info* camera_info)
{
	updating_user_index = user_index;
	player_control_get_camera_info(user_index, camera_info);
}

real32 following_camera_get_default_camera_field_of_view()
{
	s_saved_game_cartographer_player_profile* player_profile = cartographer_player_profile_get_by_user_index(updating_user_index);
	return DEGREES_TO_RADIANS(player_profile->vehicle_field_of_view);
}

void following_camera_apply_patches()
{
	// Temporary following camera fov implementation until the rewrite is done
	// replace two calls in the update function to capture the user index and use it to grab the fov for that user
	PatchCall(Memory::GetAddress(0xCD157), following_camera_player_control_get_camera_info);
	PatchCall(Memory::GetAddress(0xCD189), following_camera_get_default_camera_field_of_view);

	//DETOUR_ATTACH(p_following_camera_update, Memory::GetAddress<t_following_camera_update>(0xCD144), following_camera_update);
}
