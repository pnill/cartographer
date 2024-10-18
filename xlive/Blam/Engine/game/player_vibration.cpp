#include "stdafx.h"
#include "player_vibration.h"

#include "input/input_windows.h"
#include "interface/user_interface_controller.h"
#include "game/game.h"
#include "main/main_time.h"
#include "objects/damage_effects.h"
#include "simulation/simulation.h"

/* globals */

real32 g_vibration_dt_accumulator = 0.f;

/* prototypes */

s_vibration_globals* vibration_globals_get(void);
XINPUT_VIBRATION vibration_get_state(s_vibration_user_globals* user_globals);

/* public code */

void __cdecl rumble_player_set_scripted_scale(real32 scale)
{
	INVOKE(0x9004F, 0x8EC60, rumble_player_set_scripted_scale, scale);
	return;
}

void __cdecl rumble_player_continuous(int32 user_index, real32 rumble_intensity_left, real32 rumble_intensity_right)
{
	INVOKE(0x90222, 0x8EE33, rumble_player_continuous, user_index, rumble_intensity_left, rumble_intensity_right);
	return;
}

s_vibration_user_globals* vibration_get(int32 user_index)
{
	s_vibration_globals* vibration_globals = vibration_globals_get();
 
	ASSERT(vibration_globals);
	ASSERT(VALID_INDEX(user_index, k_number_of_users));

	return &vibration_globals->user_data[user_index];
}

void player_vibration_apply_patches(void)
{
	PatchCall(Memory::GetAddress(0x39D69), vibration_update);
	return;
}

void __cdecl vibration_update(real32 dt)
{
	// ### TODO Use game_tick_length() here when we use 30 tick in multiplayer
	const real32 k_vibration_tick_length = 1.f / 30.f;

	g_vibration_dt_accumulator += dt;

	// Only execute the rumble logic every tick
	if (g_vibration_dt_accumulator >= k_vibration_tick_length)
	{
		g_vibration_dt_accumulator = 0.f;

		if (!main_time_halted() && !simulation_starting_up() && !game_is_playback())
		{
			XINPUT_VIBRATION controller_vibration_states[k_number_of_controllers] = { 0 };

			for (uint32 user_index = 0; user_index < k_number_of_users; ++user_index)
			{
				s_vibration_user_globals* globals = vibration_get(user_index);
				datum player_index = player_index_from_user_index(user_index);

				for (uint32 duration_index = 0; duration_index < k_count_of_effects_that_effect_vibration; ++duration_index)
				{
					// Increment by the tick_length instead of dt for now to fix issues when playing above 30fps
					globals->duration[duration_index] += k_vibration_tick_length;
				}

				if (player_index != NONE)
				{
					s_player* player = (s_player*)datum_get(s_player::get_data(), player_index);
					if (player->controller_index != NONE)
					{
						ASSERT(VALID_INDEX(player->controller_index, k_number_of_controllers));

						if (user_interface_controller_get_rumble_enabled(player->controller_index))
						{
							controller_vibration_states[player->controller_index] = vibration_get_state(globals);
						}
					}
				}
			}

			for (uint32 controller_index = 0; controller_index < NUMBEROF(controller_vibration_states); ++controller_index)
			{
				const XINPUT_VIBRATION state = controller_vibration_states[controller_index];

				input_set_gamepad_rumbler_state(controller_index, state.wLeftMotorSpeed, state.wRightMotorSpeed);
			}
		}
	}

	return;
}

/* private code */

s_vibration_globals* vibration_globals_get(void)
{
	return *Memory::GetAddress<s_vibration_globals**>(0x4CA378, 0x4F3DC4);
}

XINPUT_VIBRATION vibration_get_state(s_vibration_user_globals* user_globals)
{
	// Copy user values to calculate
	real32 intensities[2] = { user_globals->intensity[0],user_globals->intensity[1] };

	for (uint32 effect_index = 0; effect_index < k_count_of_effects_that_effect_vibration; effect_index++)
	{
		s_vibration_effect_globals* effect = &user_globals->effects[effect_index];
		if (effect->damage_effect_index != NONE && effect->response_index != NONE)
		{
			s_damage_effect_definition* damage_effect_definition = (s_damage_effect_definition*)tag_get_fast(effect->damage_effect_index);
			if (effect->response_index >= 0)
			{
				ASSERT(damage_effect_definition);
				if (effect->response_index < damage_effect_definition->player_responses.count)
				{
					s_damage_effect_player_response_definition* response = damage_effect_definition->player_responses[effect->response_index];
					real32 duration = user_globals->duration[effect_index];
					for (uint32 intensity_index = 0; intensity_index < 2; intensity_index++)
					{
						s_vibration_frequency_definition* frequency_definition = &response->vibration.frequency_vibration[intensity_index];
						if (frequency_definition->duration > duration)
						{
							real32 result = duration / frequency_definition->duration;
							result = PIN(result, 0.0f, 1.0f);
							result = frequency_definition->dirty_whore.evaluate(result, 1.0f);

							intensities[intensity_index] += (result * effect->field_8);
						}
					}
				}
			}
		}
	}

	const s_vibration_globals* vibration_globals = vibration_globals_get();
	real32 scripted_scale = vibration_globals->scripted_scale;
	
	if (scripted_scale != 0.0f)
	{
		intensities[0] = (vibration_globals->max_vibration_left * scripted_scale) + intensities[0];
		intensities[1] = (vibration_globals->max_vibration_right * scripted_scale) + intensities[1];
	}


	intensities[0] *= (real32)UINT16_MAX;
	intensities[1] *= (real32)UINT16_MAX;

	XINPUT_VIBRATION state
	{
		(uint16)PIN(intensities[0], 0.0f, (real32)UINT16_MAX),  // Left
		(uint16)PIN(intensities[1], 0.0f, (real32)UINT16_MAX)   // Right
	};
	return state;
}
