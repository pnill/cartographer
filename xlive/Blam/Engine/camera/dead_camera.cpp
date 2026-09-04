#include "stdafx.h"
#include "dead_camera.h"

#include "director.h"

#include "game/game.h"
#include "game/game_time.h"
#include "game/players.h"
#include "input/input_abstraction.h"
#include "math/random_math.h"
#include "objects/objects.h"
#include "units/bipeds.h"

/* constants */

constexpr real32 k_dead_camera_field_of_view = 78.f;
constexpr real32 k_dead_camera_field_of_view_radians = DEGREES_TO_RADIANS(k_dead_camera_field_of_view);

constexpr real32 k_dead_camera_minimum_focus_distance = 2.f;
constexpr real32 k_dead_camera_maximum_focus_distance = 6.f;
constexpr real32 k_dead_camera_minimum_pitch = DEGREES_TO_RADIANS(27.f);
constexpr real32 k_dead_camera_maximum_pitch = DEGREES_TO_RADIANS(63.f);

constexpr real32 k_dead_camera_transition_time = 3.f;

constexpr real32 k_dead_camera_transition_focus_distance = 0.5f;

constexpr real32 k_dead_camera_death_transition_time = 0.33f;

constexpr real32 k_dead_camera_switch_time_multiplayer = 15.f;
constexpr real32 k_dead_camera_switch_time_campaign = 3.f;

constexpr real32 k_dead_camera_manual_switch_delay = 1.f;

/* typedefs */

typedef void(__cdecl* t_dead_camera_new)(s_dead_camera* camera, int32 user_index, datum target_object_index);
typedef void(__cdecl* t_dead_camera_update)(s_dead_camera* camera, s_director_update* director_update, s_observer_command* observer_command);

/* globals */

static t_dead_camera_new p_dead_camera_new;
static t_dead_camera_update p_dead_camera_update;

/* prototypes */

static void __cdecl dead_camera_get_next_player_view(s_dead_camera* camera);

static datum __cdecl dead_camera_get_campaign_view(void);

/* public code */

void dead_camera_apply_patches()
{
	DETOUR_ATTACH(p_dead_camera_new, Memory::GetAddress<t_dead_camera_new>(0xCDF6D), dead_camera_new);
	DETOUR_ATTACH(p_dead_camera_update, Memory::GetAddress<t_dead_camera_update>(0xCDB2F), dead_camera_update);
}

void __cdecl dead_camera_new(s_dead_camera* camera, int32 user_index, datum target_object_index)
{
	uint32* seed = get_local_random_seed_address();

	camera->position = observer_get_camera(user_index)->position;
	camera->field_of_view = k_dead_camera_field_of_view_radians;

	camera->focus_distance = _real_random_range(seed, k_dead_camera_minimum_focus_distance, k_dead_camera_maximum_focus_distance);
	camera->facing.yaw = _real_random_range(seed, 0.f, DEGREES_TO_RADIANS(360.f));
	camera->facing.pitch = -_real_random_range(seed, k_dead_camera_minimum_pitch, k_dead_camera_maximum_pitch);
	
	camera->transition_timer = k_dead_camera_transition_time;

	if (target_object_index == NONE)
	{
		camera->switch_timer = game_is_multiplayer() ? k_dead_camera_switch_time_multiplayer : k_dead_camera_switch_time_campaign;
	}
	else
	{
		camera->switch_timer = FLT_MAX;
	}

	const datum player_index = player_index_from_user_index(user_index);
	
	camera->player_index = player_index;
	camera->view_player_index = player_index;
	camera->target_object_index = NONE;
	camera->death_transition_ticks = 0;
	camera->user_index = (int8)user_index;

	if (player_index != NONE)
	{
		const player_datum* player = player_get(player_index);
		if (player->unit_index != NONE)
		{
			camera->target_object_index = player->unit_index;
		}
		else if (player->dead_unit_index != NONE)
		{
			camera->target_object_index = player->dead_unit_index;
		}
		else
		{
			dead_camera_get_next_player_view(camera);
		}
	}
	return;
}

void __cdecl dead_camera_update(s_dead_camera* camera, s_director_update* director_update, s_observer_command* observer_command)
{
	bool switch_view = false;

	if (camera->target_object_index != NONE && object_try_and_get(camera->target_object_index) == NULL)
	{
		camera->target_object_index = NONE;
	}

	if (camera->target_object_index == NONE)
	{
		dead_camera_get_next_player_view(camera);

		if (camera->target_object_index == NONE)
		{
			if (game_is_multiplayer())
			{
				dead_camera_get_next_player_view(camera);
			}
			else
			{
				camera->target_object_index = dead_camera_get_campaign_view();
			}
		}
	}

	const int32 death_transition_ticks = game_seconds_to_ticks_round(k_dead_camera_death_transition_time);

	const object_datum* target_object = camera->target_object_index != NONE	? object_try_and_get(camera->target_object_index) : NULL;

	if (target_object != NULL)
	{
		real_matrix4x3 node_matrix;

		object_try_get_node_matrix_interpolated(camera->target_object_index, 0, &node_matrix);
		observer_command->position.position = node_matrix.position;

		if (target_object->object.object_identifier.get_type() == _object_type_biped)
		{
			const biped_datum* target_biped = (const biped_datum*)target_object;

			if (TEST_BIT((uint16)target_biped->biped.flags, _biped_flag_is_falling_to_its_death) || camera->death_transition_ticks != 0)
			{
				if (camera->death_transition_ticks < death_transition_ticks)
				{
					camera->death_transition_ticks++;
				}
			}
			if (camera->death_transition_ticks == death_transition_ticks)
			{
				camera->focus_position = observer_get_camera(camera->user_index)->position;
			}
		}
	}
	else
	{
		camera->death_transition_ticks = 0;
		observer_command->position.position = camera->position;
	}

	if (camera->death_transition_ticks == death_transition_ticks)
	{
		const real_vector3d up = observer_get_camera(camera->user_index)->up;

		observer_command->forward.i = observer_command->position.position.x - camera->focus_position.x;
		observer_command->forward.j = observer_command->position.position.y - camera->focus_position.y;
		observer_command->forward.k = observer_command->position.position.z - camera->focus_position.z;
		observer_command->focus_distance = normalize3d(&observer_command->forward);

		real_vector3d side;

		cross_product3d(&up, &observer_command->forward, &side);
		cross_product3d(&observer_command->forward, &side, &observer_command->up);
		normalize3d(&observer_command->up);

		observer_command->field_of_view = camera->field_of_view;
		observer_command->position.orientation = *global_zero_vector3d;
		observer_command->velocity = *global_zero_vector3d;
		observer_command->flags = 25;
		observer_command->timer = 0.f;
	}
	else
	{
		observer_command->focus_distance = camera->focus_distance;
		vector3d_from_euler_angles2d(&observer_command->forward, &camera->facing);
		generate_up_vector3d(&observer_command->forward, &observer_command->up);
		observer_command->field_of_view = camera->field_of_view;
		observer_command->position.orientation = *global_zero_vector3d;
		observer_command->velocity = *global_zero_vector3d;
		observer_command->flags = 1;
		observer_command->timer = MAX(camera->transition_timer, 0.f);
	}

	observer_command->observer_command_flags[0] = _observer_command_bit_3;
	observer_command->observer_command_values[0] = 0.f;
	if (camera->transition_timer == k_dead_camera_transition_time)
	{
		observer_command->focus_distance = k_dead_camera_transition_focus_distance;
		observer_command->observer_command_values[3] = 0.f;
		observer_command->observer_command_flags[3] = _observer_command_bit_3;
	}

	camera->transition_timer -= director_update->delta;
	camera->switch_timer = MAX(camera->switch_timer - director_update->delta, 0.f);

	if (camera->player_index != NONE)
	{
		const player_datum* player = player_get(camera->player_index);

		if ((int32)player->controller_index != NONE)
		{
			const s_game_input_state* input_state = &g_input_abstraction_globals->input_states[player->controller_index];

			if (input_state->m_down_frames[_button_jump] == 1)
			{
				const real32 maximum_switch_timer = game_is_multiplayer() ? k_dead_camera_switch_time_multiplayer : k_dead_camera_switch_time_campaign;

				if (maximum_switch_timer > camera->switch_timer + k_dead_camera_manual_switch_delay)
				{
					switch_view = true;
				}
			}
		}
	}

	if ((camera->switch_timer == 0.f && !game_time_get_paused()) || switch_view)
	{
		dead_camera_get_next_player_view(camera);
	}

	observer_validate_camera_command(observer_command);
	return;
}

/* private code */

static void __cdecl dead_camera_get_next_player_view(s_dead_camera* camera)
{
	INVOKE(0xCDA91, 0x0, dead_camera_get_next_player_view, camera);
	return;
}

static datum __cdecl dead_camera_get_campaign_view(void)
{
	return INVOKE(0xCDA36, 0x0, dead_camera_get_campaign_view);
}
