#include "stdafx.h"
#include "observer.h"

#include "director.h"

#include "cutscene/cinematics.h"
#include "effects/player_effects.h"
#include "game/game.h"
#include "game/game_time.h"
#include "game/players.h"
#include "interface/first_person_weapons.h"
#include "math/matrix_math.h"
#include "physics/collisions.h"
#include "render/render_visibility_collection.h"
#include "scenario/scenario.h"
#include "shell/shell.h"


/* constants */

enum
{
	OBSERVER_SIGNATURE = 'rad!'
};

/* globals */

static real32 g_camera_speed = 1.0f;

/* prototypes */

static void observer_apply_interpolation_patches(void);

static s_observer* observer_user_globals_get(void);

static real32* observer_get_speed_dt(void);

static bool* observer_get_initial_update(void);

static void observer_pass_time_to_usercall(int32 user_index);

static void observer_update_command_to_usercall(int32 user_index);

static void __cdecl observer_postcheck(int32 user_index);

static void observer_update_internal(int32 user_index);

/* public code */

void observer_apply_patches(void)
{
	observer_apply_interpolation_patches();
	return;
}

s_observer* observer_get_from_user(int32 user_index)
{
	return &observer_user_globals_get()[user_index];
}

void __cdecl observer_game_tick(void)
{
	INVOKE(0x81872, 0x45640, observer_game_tick);
	return;
}

void __cdecl observer_update(real32 dt)
{
	bool* g_observer_intitial_update = observer_get_initial_update();
	real32* observer_speed_dt = observer_get_speed_dt();

	*observer_speed_dt = g_camera_speed * dt;
	
	for (uint32 user_index = 0; user_index < k_number_of_users; user_index++)
	{
		if (players_user_is_active(user_index))
		{
			s_observer* observer = observer_get_from_user(user_index);

			ASSERT(VALID_INDEX(user_index, k_number_of_users));
			ASSERT(observer->header_signature == OBSERVER_SIGNATURE && observer->trailer_signature == OBSERVER_SIGNATURE);
			ASSERT(!observer->updated_for_frame);

			observer->updated_for_frame = true;
			observer_update_command_to_usercall(user_index);
			if (*observer_speed_dt != 0.0f)
			{
				observer_pass_time_to_usercall(user_index);
			}

			s_location bsp_point;
			observer_postcheck(user_index);
			observer_update_internal(user_index);
			scenario_location_from_point(&bsp_point, &observer->result.position);
			if (!cinematic_in_progress())
			{
				if (*g_observer_intitial_update)
				{
					if (bsp_point.cluster_index != NONE)
					{
						render_visibility_predict_resources_for_pvs(bsp_point.cluster_index);
					}
				}
				else
				{
					predicted_resources_precache(bsp_point.cluster_index);
				}
			}
			
			ASSERT(observer->header_signature == OBSERVER_SIGNATURE && observer->trailer_signature == OBSERVER_SIGNATURE);
		}
	}

	*g_observer_intitial_update = false;
	return;
}

void __cdecl observer_validate_camera_command(s_observer_command* observer_command)
{
	INVOKE(0x824F7, 0x4636A, observer_validate_camera_command, observer_command);
	return;
}

real32 observer_suggested_field_of_view(void)
{
	return *Memory::GetAddress<real32*>(0x413780, 0x3B5300);
}

void observer_set_suggested_field_of_view(real32 fov)
{
	// Don't change the fov if it's 0 or greater than 110
	if (fov <= 0 || fov > 110) return;

	float final_fov_rad;

	if (game_is_multiplayer())
	{
		s_game_variant* variant = get_game_variant();
		if (variant && variant->cartographer_settings.flags.test(_cartographer_variant_force_default_fov))
		{
			final_fov_rad = DEGREES_TO_RADIANS(78.f);
		}
		else
		{
			final_fov_rad = DEGREES_TO_RADIANS(fov);
		}
	}
	else
	{
		final_fov_rad = DEGREES_TO_RADIANS(fov);
	}

	*Memory::GetAddress<float*>(0x413780, 0x3B5300) = final_fov_rad;
	return;
}

s_observer_result* __cdecl observer_get_camera(int32 user_index)
{
	return INVOKE(0x81EBA, 0x0, observer_get_camera, user_index);
}

s_observer_result* __cdecl observer_try_and_get_camera(int32 user_index)
{
	return INVOKE(0x818D6, 0x0, observer_try_and_get_camera, user_index);
}

/* private code */

static void observer_apply_interpolation_patches(void)
{
	if (!shell_is_dedicated_server())
	{
		PatchCall(Memory::GetAddress(0x39D5D, 0xC0EC), observer_update);
		PatchCall(Memory::GetAddress(0x97C93, 0xB6D47), observer_update);
	}
	return;
}

static s_observer* observer_user_globals_get(void)
{
	return Memory::GetAddress<s_observer*>(0x4C071C, 0x4CDE64);
}

static real32* observer_get_speed_dt(void)
{
	return Memory::GetAddress<real32*>(0x4C0718, 0x4CDE60);
}

static bool* observer_get_initial_update(void)
{
	return Memory::GetAddress<bool*>(0x4C147C, 0x4CEBC4);
}

static void observer_pass_time_to_usercall(int32 user_index)
{
	void* observer_pass_time_usercall = Memory::GetAddress<void*>(0x838A1);
	__asm
	{
		mov edi, user_index
		call observer_pass_time_usercall
	}
}

static void observer_update_command_to_usercall(int32 user_index)
{
	void* observer_update_command = Memory::GetAddress<void*>(0x82B7F);
	__asm 
	{
		mov eax, user_index
		call observer_update_command
	}
}

static void __cdecl observer_postcheck(int32 user_index)
{
	INVOKE(0x8391F, 0x47792, observer_postcheck, user_index);
	return;
}

static void observer_update_internal(int32 user_index)
{
	real_matrix4x3 camera_effect_matrix = *global_identity4x3;
	s_observer* observer = observer_get_from_user(user_index);
	if (user_index != NONE)
	{
		if (director_get_perspective(user_index) == _director_mode_game)
		{
			first_person_weapon_apply_camera_effect(user_index, &camera_effect_matrix);
		}
		if (!game_time_get_paused())
		{
			int32 perspective = director_get_perspective(user_index);
			if (perspective == _director_mode_game || perspective == _director_mode_editor)
			{
				player_effect_apply_camera_effect_matrix(user_index, &camera_effect_matrix);
			}
		}
	}

	real_point3d point = observer->result.position;
	real_vector3d forward = observer->result.forward;
	real_vector3d up = observer->result.up;
	real_matrix4x3 result_matrix;
	matrix4x3_from_point_and_vectors(&result_matrix, &point, &forward, &up);
	matrix4x3_multiply(&result_matrix, &camera_effect_matrix, &result_matrix);
	matrix4x3_to_point_and_vectors(&result_matrix, &point, &forward, &up);

	if (!valid_real_vector3d_axes2(&forward, &up))
	{
		normalize3d_with_default(&forward, global_forward3d);
		normalize3d_with_default(&up, global_forward3d);
		if (!valid_real_vector3d_axes2(&forward, &up))
		{
			error(_error_silent, "camera effect matrix was bad, cannot recover");
			forward = *global_forward3d;
			up = *global_up3d;
		}
	}

	assert_valid_real_vector3d_axes2(&forward, &up);

	if (!TEST_BIT(observer->pending_command.flags, 4))
	{
		s_location location;
		scenario_location_from_point(&location, &point);
		collision_result collision;
		collision.global_material_index = NONE;

		const e_collision_test_flags underwater_flags = (e_collision_test_flags)
		(
			FLAG(_collision_test_structure_bit) | FLAG(_collision_test_instanced_geometry_bit) | FLAG(_collision_test_objects_bit) |
			FLAG(_collision_test_bit_10) | FLAG(_collision_test_bit_11) | FLAG(_collision_test_bit_15) | FLAG(_collision_test_bit_23)
		);
		
		const e_collision_test_flags default_flags = (e_collision_test_flags)
		(
			FLAG(_collision_test_structure_bit) | FLAG(_collision_test_media_bit) | FLAG(_collision_test_instanced_geometry_bit) | FLAG(_collision_test_objects_bit) |
			FLAG(_collision_test_bit_10) | FLAG(_collision_test_bit_11) | FLAG(_collision_test_bit_15) | FLAG(_collision_test_bit_23)
		);

		const e_collision_test_flags flags = (scenario_location_underwater(&location, &point, NULL) ? underwater_flags : default_flags);
		if (collision_test_line(flags, &observer->result.position, &point, NONE, NONE, &collision))
		{
			points_interpolate(&observer->result.position, &point, collision.t * 0.9f, &point);
		}
	}

	observer->result.position = point;
	observer->result.forward = forward;
	observer->result.up = up;
	return;
}
