#include "stdafx.h"
#include "FirstPersonInterpolate.h"
#include "Interpolation.h"

#if GAME_FRAME_INTERPOLATOR_ENABLED
#include "Blam/Engine/game/game_time.h"
#include "Blam/Engine/Networking/Session/NetworkSession.h"
#include "Blam/Engine/game/players.h"
#include "Blam/Engine/math/BlamMath.h"
#include "Util/Hooks/Hook.h"

namespace FirstPersonInterpolate
{
	namespace
	{
		// static namespace members
		struct s_camera_state
		{
			bool valid;
			datum unit_idx = DATUM_INDEX_NONE;
			datum player_idx = DATUM_INDEX_NONE;
			real_vector3d previous_position;
		} local_players_states[k_number_of_users];

		void unit_get_camera_position(datum unit_idx, real_point3d* out_position)
		{
			typedef void(__cdecl* unit_get_camera_position_t)(datum unit_idx, real_point3d* out_position);
			auto p_unit_get_camera_position = Memory::GetAddress<unit_get_camera_position_t>(0x13D2EC);

			p_unit_get_camera_position(unit_idx, out_position);
		}

		void ResetPlayer(int local_player_idx)
		{
			s_camera_state* camera_state = &local_players_states[local_player_idx];
			if (!camera_state->valid)
				return;
			camera_state->valid = false;
			camera_state->unit_idx = DATUM_INDEX_NONE;
			camera_state->player_idx = DATUM_INDEX_NONE;
			memset(&camera_state->previous_position, 0, sizeof(camera_state->previous_position));
		}
	}

	void PreGameTickUpdateLocalPlayers()
	{
		if (!Interpolation::ShouldInterpolate())
		{
			Reset(); // just reset
			return;
		}

		for (int i = 0; i < k_number_of_users; i++)
		{
			datum player_datum, player_unit_datum;
			s_camera_state* camera_state = &local_players_states[i];

			// check if we actually executed any ticks
			if (!(time_globals::get_game_time() > 0))
			{
				Reset();
				break;
			}

			if (!players_user_is_active(i))
			{
				ResetPlayer(i);
				continue;
			}

			player_datum = player_index_from_user_index(i);
			player_unit_datum = s_player::get_unit_index(player_datum);

			if (DATUM_IS_NONE(player_unit_datum))
			{
				ResetPlayer(i);
				continue;
			}

			// if we got this far, the local player/user must control an unit
			// pretick, update previous camera position with what's currently in the game's state before updating it (i.e executing a tick)
			// TODO FP weapon interpolation
			unit_get_camera_position(player_unit_datum, &camera_state->previous_position);
			camera_state->unit_idx = player_unit_datum;
			camera_state->player_idx = player_datum;
			camera_state->valid = true;
		}
	}

	void PostGameTickUpdateLocalPlayers()
	{
		if (!Interpolation::ShouldInterpolate())
			return;

		// basic checks, verify if player died, if unit has changed etc. after a game tick update
		for (int i = 0; i < k_number_of_users; i++)
		{
			datum player_datum, player_unit_datum;
			s_camera_state* camera_state = &local_players_states[i];

			if (!camera_state->valid)
				continue;
			
			if (players_user_is_active(i))
			{
				player_datum = player_index_from_user_index(i);
				player_unit_datum = s_player::get_unit_index(player_datum);

				if (camera_state->unit_idx == player_unit_datum)
				{
					continue;
				}
			}

			ResetPlayer(i);
		}
	}

	void UnitGetInterpolatedCameraPosition(datum unit_idx, real_vector3d* out_cam_position)
	{
		if (!Interpolation::ShouldInterpolate())
		{
			unit_get_camera_position(unit_idx, out_cam_position);
			return;
		}

		if (DATUM_IS_NONE(unit_idx))
			return;

		for (int i = 0; i < k_number_of_users; i++)
		{
			datum player_datum, player_unit_datum;
			s_camera_state* camera_state = &local_players_states[i];
			if (!players_user_is_active(i))
				continue;

			player_datum = player_index_from_user_index(i);
			player_unit_datum = s_player::get_unit_index(player_datum);

			if (DATUM_IS_NONE(player_unit_datum))
				continue;

			if (unit_idx != player_unit_datum)
				continue;

			if (camera_state->valid 
				&& camera_state->unit_idx == unit_idx)
			{
				real_point3d target_camera_pos;
				unit_get_camera_position(unit_idx, &target_camera_pos);
				points_interpolate(&camera_state->previous_position, &target_camera_pos, Interpolation::GetInterpolateTime(), out_cam_position);
				return;
			}
		}

		// if the interpolation data was invalid, just output what's in the game state at that time
		unit_get_camera_position(unit_idx, out_cam_position);
		return;
	}

	void Reset()
	{
		for (int i = 0; i < k_number_of_users; i++)
		{
			ResetPlayer(i);
		}
	}

	void __cdecl unit_get_camera_position_hook(datum unit_idx, real_point3d* out_position)
	{
		UnitGetInterpolatedCameraPosition(unit_idx, out_position);
	}

	void ApplyPatches()
	{
		// camera interpolation
		PatchCall(Memory::GetAddress(0xCD64F), unit_get_camera_position_hook);

		// first person weapons interpolation
	}
}

#endif