#include "stdafx.h"
#include "director.h"

#include "editor_camera.h"
#include "cartographer/twizzler/twizzler.h"
#include "game/game.h"

#include "game/players.h"
#include "networking/logic/life_cycle_manager.h"
#include "networking/session/network_session.h"
#include "shell/shell.h"

/* public code */

s_camera_director* director_get(int32 user_index)
{
	return &Memory::GetAddress<s_camera_director*>(0x4A84A4)[user_index];
}

e_director_mode director_get_mode(void)
{
	return *Memory::GetAddress<e_director_mode*>(0x4A849C);
}

void director_set_mode(e_director_mode mode)
{
	e_director_mode* director_mode = Memory::GetAddress<e_director_mode*>(0x4A849C);
	if (*director_mode != mode)
	{
		*director_mode = mode;
		// request mode change next update
		*Memory::GetAddress<bool*>(0x4A84A0) = true;
	}
}

void __cdecl director_update(real32 dt)
{
	INVOKE(0x5A658, 0x48668, director_update, dt);
	return;
}

void __cdecl director_game_tick(void)
{
	INVOKE(0x59DF2, 0x47E1F, director_game_tick);
	return;
}

int32 __cdecl director_get_perspective(int32 user_index)
{
	return INVOKE(0x59E2B, 0x47E58, director_get_perspective, user_index);
}

void __cdecl director_debug_camera(bool enabled)
{
	if (shell_is_dedicated_server())
		return;

	if (game_is_multiplayer())
	{
		//c_network_session* session = NULL;
		//if (!network_life_cycle_in_squad_session(&session))
		//{
		//	return;
		//}

		/*if (!session->is_host())
		{
			return;
		}*/

		if (g_twizzler_status)
		{
			return;
		}

		//todo: add a better method for restricting this
	}

	if (enabled)
	{
		s_editor_camera_globals* globals = editor_camera_globals_get();

		const int32 user_index = players_first_active_user();
		const s_observer_result* current_camera = user_index != NONE ? observer_try_and_get_camera(user_index) : NULL;

		if (current_camera != NULL)
		{
			globals->saved_position = current_camera->position;
			globals->saved_facing.yaw = atan2f(current_camera->forward.j, current_camera->forward.i);
			globals->saved_facing.pitch = atan2f(current_camera->forward.k,
				sqrtf((current_camera->forward.i * current_camera->forward.i) + (current_camera->forward.j * current_camera->forward.j)));
			globals->state_valid = true;
		}

		globals->mode = _editor_camera_mode_flying;
		editor_camera_flying_reset();
		director_set_mode(_director_mode_editor);
	}
	else
	{
		director_set_mode(_director_mode_game);
	}
}

void director_inhibit_facing(int32 user_index)
{
	director_get(user_index)->inhibit_facing = true;
}

void director_inhibit_input(int32 user_index)
{
	director_get(user_index)->inhibit_input = true;
}

bool director_inhibited_facing(int32 user_index)
{
	return director_get(user_index)->inhibit_facing;
}

bool director_inhibited_input(int32 user_index)
{
	return director_get(user_index)->inhibit_input;
}
