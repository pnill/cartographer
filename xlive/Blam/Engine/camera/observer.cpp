#include "stdafx.h"
#include "observer.h"

#include "director.h"

#include "Blam/Engine/cutscene/cinematics.h"
#include "Blam/Engine/effects/player_effects.h"
#include "Blam/Engine/game/game_time.h"
#include "Blam/Engine/interface/first_person_weapons.h"
#include "Blam/Engine/Networking/Session/NetworkSession.h"
#include "Blam/Engine/physics/collisions.h"
#include "Blam/Engine/render/render_visibility_collection.h"
#include "Blam/Engine/scenario/scenario.h"
#include "Util/Hooks/Hook.h"

real32 g_camera_speed = 1.0f;

s_observer* observer_user_globals_get(void);
real32* observer_get_speed_dt(void);
bool* observer_get_initial_update(void);
void __cdecl observer_validate_camera_command(s_observer_command* observer_command);
void __stdcall observer_pass_time(void);
void __stdcall observer_pass_time_usercall_to_stdcall(int32 user_index);
void __stdcall observer_update_command_usercall(int32 user_index);
void __cdecl observer_postcheck(int32 user_index);
void __cdecl observer_update(real32 dt);
void observer_update_internal(int32 user_index);

s_observer* observer_get_from_user(int32 user_index)
{
	return &observer_user_globals_get()[user_index];
}

void observer_apply_patches(void)
{
	PatchCall(Memory::GetAddress(0x39D5D, 0xC0EC), observer_update);
	PatchCall(Memory::GetAddress(0x97C93, 0xB6D47), observer_update);
	return;
}



s_observer* observer_user_globals_get(void)
{
	return Memory::GetAddress<s_observer*>(0x4C071C, 0x4CDE64);
}

real32* observer_get_speed_dt(void)
{
	return Memory::GetAddress<real32*>(0x4C0718, 0x4CDE60);
}

bool* observer_get_initial_update(void)
{
	return Memory::GetAddress<bool*>(0x4C147C, 0x4CEBC4);
}

void __cdecl observer_validate_camera_command(s_observer_command* observer_command)
{
	INVOKE(0x824F7, 0x4636A, observer_validate_camera_command, observer_command);
	return;
}

void __stdcall observer_pass_time(void)
{
	INVOKE(0x838A1, 0x47714, observer_pass_time);
	return;
}

void __stdcall observer_pass_time_usercall_to_stdcall(int32 user_index) {
	__asm {
		mov edi, user_index
		push edi				// user_index
		call observer_pass_time // __stdcall
	}
}

void __stdcall observer_update_command_usercall(int32 user_index) {
	
	typedef void(__stdcall* func_t)(void);
	func_t function = (func_t)Memory::GetAddress(0x82B7F, 0x0);
	__asm
	{
		mov ebx, function
		mov eax, user_index
		call ebx
	}
}

void __cdecl observer_postcheck(int32 user_index)
{
	INVOKE(0x8391F, 0x47792, observer_postcheck, user_index);
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
			observer->updated_for_frame = true;
			observer_update_command_usercall(user_index);
			if (*observer_speed_dt != 0.0f)
			{
				observer_pass_time_usercall_to_stdcall(user_index);
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
		}
	}

	*g_observer_intitial_update = false;
	return;
}

void observer_update_internal(int32 user_index)
{
	real_matrix4x3 camera_effect_matrix = global_identity4x3;
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
		normalize3d_with_default(&forward, &global_forward3d);
		normalize3d_with_default(&up, &global_forward3d);
		if (!valid_real_vector3d_axes2(&forward, &up))
		{
			forward = global_forward3d;
			up = global_up3d;
		}
	}

	if (!TEST_BIT(observer->pending_command.flags, 4))
	{
		s_location location;
		scenario_location_from_point(&location, &point);
		collision_result collision;
		collision.global_material_index = NONE;
		uint32 flags = (scenario_location_underwater(&location, &point, NULL) ? 0x808C0D : 0x808C0F);

		if (collision_test_line((e_collision_test_flags)flags, &observer->result.position, &point, NONE, NONE, &collision))
		{
			points_interpolate(&observer->result.position, &point, collision.t * 0.89999998, &point);
		}
	}

	observer->result.position = point;
	observer->result.forward = forward;
	observer->result.up = up;
	return;
}
