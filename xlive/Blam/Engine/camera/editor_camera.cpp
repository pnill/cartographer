#include "stdafx.h"
#include "editor_camera.h"

#include "director.h"
#include "observer.h"

#include "game/player_control.h"
#include "input/input_abstraction.h"
#include "input/input_windows.h"
#include "objects/objects.h"
#include "saved_games/cartographer_player_profile/cartographer_player_profile.h"

/* constants */

constexpr real32 k_editor_camera_maximum_pitch = 1.5676548f;

constexpr real32 k_editor_camera_look_sensitivity = 0.0031415927f;

constexpr real32 k_flying_camera_default_speed = 2.f;
constexpr real32 k_flying_camera_minimum_speed = 0.125f;
constexpr real32 k_flying_camera_maximum_speed = 128.f;
constexpr real32 k_flying_camera_speed_wheel_step = 1.25f;
constexpr real32 k_flying_camera_boost_multiplier = 4.f;

constexpr real32 k_flying_camera_velocity_blend_rate = 8.f;

constexpr real32 k_flying_camera_interpolation_time = 0.3f;
constexpr real32 k_orbiting_camera_interpolation_time = 0.5f;

constexpr real32 k_orbiting_camera_zoom_rate = 0.3f;
constexpr real32 k_orbiting_camera_maximum_focus_distance = 0.6f;

/* typedefs */

typedef void(__cdecl* t_editor_camera_orbiting_update)(s_editor_camera* camera, s_director_update* director_update, s_observer_command* observer_command);
typedef void(__cdecl* t_editor_camera_flying_update)(s_editor_camera* camera, s_director_update* director_update, s_observer_command* observer_command);

/* globals */

t_editor_camera_orbiting_update p_editor_camera_orbiting_update;
t_editor_camera_flying_update p_editor_camera_flying_update;

static real_vector3d g_flying_camera_velocity = {};
static real32 g_flying_camera_speed = k_flying_camera_default_speed;

/* prototypes */

// this replaces rewritng director_update_controls, and just implementing it for the editor camera.
// when that function is rewritten this can be absorbed into it.
static void editor_camera_update_controls(s_director_update* director_update);
static void __cdecl editor_camera_orbiting_update(s_editor_camera* camera, s_director_update* director_update, s_observer_command* observer_command);
static void __cdecl editor_camera_flying_update(s_editor_camera* camera, s_director_update* director_update, s_observer_command* observer_command);

/* public code */

void editor_camera_apply_patches()
{
	DETOUR_ATTACH(p_editor_camera_orbiting_update, Memory::GetAddress<t_editor_camera_orbiting_update>(0xCC58A), editor_camera_orbiting_update);
	DETOUR_ATTACH(p_editor_camera_flying_update, Memory::GetAddress<t_editor_camera_flying_update>(0xCC87C), editor_camera_flying_update);
}

s_editor_camera_globals* editor_camera_globals_get(void)
{
	return Memory::GetAddress<s_editor_camera_globals*>(0x4D8480);
}

void editor_camera_set_position(const real_point3d* point, const real_euler_angles2d* angles)
{
	s_editor_camera_globals* globals = editor_camera_globals_get();

	if (globals->user_camera != NULL)
	{
		globals->user_camera->position = *point;
		globals->user_camera->facing = *angles;
	}
	else
	{
		globals->saved_position = *point;
		globals->saved_facing = *angles;
		globals->state_valid = true;
	}
}

void editor_camera_flying_reset(void)
{
	g_flying_camera_velocity = {};
}

/* private code */

static void editor_camera_update_controls(s_director_update* director_update)
{
	const uint16* mouse_buttons = input_get_mouse_button_state();
	const mouse_state* mouse = input_get_mouse_state();

	real_vector3d target_velocity = {};

	if (mouse_buttons != NULL && mouse_buttons[_mouse_button_right] != 0)
	{
		director_update->facing.yaw = (real32)mouse->state.lX * -k_editor_camera_look_sensitivity;
		director_update->facing.pitch = (real32)mouse->state.lY * -k_editor_camera_look_sensitivity;

		if (mouse->state.lZ != 0)
		{
			g_flying_camera_speed = PIN(
				g_flying_camera_speed * powf(k_flying_camera_speed_wheel_step, 
				(real32)mouse->state.lZ / (real32)WHEEL_DELTA),
				k_flying_camera_minimum_speed, 
				k_flying_camera_maximum_speed);
		}

		real32 speed = g_flying_camera_speed;

		if (input_key_frames_down(_key_left_shift) != 0 || input_key_frames_down(_key_right_shift) != 0)
		{
			speed *= k_flying_camera_boost_multiplier;
		}

		target_velocity.i = (real32)((input_key_frames_down(_key_w) != 0) - (input_key_frames_down(_key_s) != 0)) * speed;
		target_velocity.j = (real32)((input_key_frames_down(_key_a) != 0) - (input_key_frames_down(_key_d) != 0)) * speed;
		target_velocity.k = (real32)((input_key_frames_down(_key_r) != 0) - (input_key_frames_down(_key_f) != 0)) * speed;
	}

	const real32 dt = director_update->delta;
	const real32 blend = PIN(dt * k_flying_camera_velocity_blend_rate, 0.f, 1.f);

	g_flying_camera_velocity.i += (target_velocity.i - g_flying_camera_velocity.i) * blend;
	g_flying_camera_velocity.j += (target_velocity.j - g_flying_camera_velocity.j) * blend;
	g_flying_camera_velocity.k += (target_velocity.k - g_flying_camera_velocity.k) * blend;

	director_update->movement_forward = g_flying_camera_velocity.i * dt;
	director_update->movement_side = g_flying_camera_velocity.j * dt;
	director_update->movement_up = g_flying_camera_velocity.k * dt;
	director_update->has_input = true;
}

static void __cdecl editor_camera_orbiting_update(s_editor_camera* camera, s_director_update* director_update, s_observer_command* observer_command)
{
	s_player_control_camera_info camera_info;
	player_control_get_camera_info(director_update->user_index, &camera_info);

	observer_command->position.position = camera_info.location;

	if (director_update->has_input)
	{
		camera->facing.yaw += director_update->facing.yaw;
		camera->facing.pitch = PIN(camera->facing.pitch + director_update->facing.pitch, DEGREES_TO_RADIANS(-90), DEGREES_TO_RADIANS(90));
		director_inhibit_input(director_update->user_index);
	}

	camera->focus_distance = PIN(camera->focus_distance - (director_update->turn_scale * k_orbiting_camera_zoom_rate), 0, k_orbiting_camera_maximum_focus_distance);

	if (camera_info.slave_object != NONE)
	{
		vector3d_from_euler_angles2d(&observer_command->forward, &camera->facing);
		generate_up_vector3d(&observer_command->forward, &observer_command->up);
		object_get_velocities(camera_info.slave_object, &observer_command->velocity, 0);
		observer_command->flags = 1;
	}

	observer_command->position.orientation = *global_zero_vector3d;
	observer_command->focus_distance = camera->focus_distance;

	s_saved_game_cartographer_player_profile* player_profile = cartographer_player_profile_get_by_user_index(director_update->user_index);

	observer_command->field_of_view = player_profile->vehicle_field_of_view;
	observer_command->timer = k_orbiting_camera_interpolation_time;
	observer_validate_camera_command(observer_command);
}

static void __cdecl editor_camera_flying_update(s_editor_camera* camera, s_director_update* director_update, s_observer_command* observer_command)
{
	editor_camera_update_controls(director_update);

	director_inhibit_facing(director_update->user_index);
	director_inhibit_input(director_update->user_index);

	if (director_update->has_input)
	{
		camera->facing.yaw += director_update->facing.yaw;
		camera->facing.pitch = PIN(camera->facing.pitch + director_update->facing.pitch, -k_editor_camera_maximum_pitch, k_editor_camera_maximum_pitch);
		camera->roll = 0.f;
	}

	observer_command->timer = k_flying_camera_interpolation_time;
	vector3d_from_euler_angles2d(&observer_command->forward, &camera->facing);
	generate_up_vector3d(&observer_command->forward, &observer_command->up);

	const real32 cos_yaw = cosf(camera->facing.yaw);
	const real32 sin_yaw = sinf(camera->facing.yaw);

	camera->position.x += (director_update->movement_forward * cos_yaw) - (director_update->movement_side * sin_yaw);
	camera->position.y += (director_update->movement_side * cos_yaw) + (director_update->movement_forward * sin_yaw);
	camera->position.z += director_update->movement_up;

	observer_command->position.position = camera->position;
	observer_command->position.orientation = *global_zero_vector3d;
	observer_command->focus_distance = 0.f;

	s_saved_game_cartographer_player_profile* player_profile = cartographer_player_profile_get_by_user_index(director_update->user_index);
	
	observer_command->field_of_view = player_profile->vehicle_field_of_view;

	observer_command->flags = 1;
	observer_validate_camera_command(observer_command);
}
