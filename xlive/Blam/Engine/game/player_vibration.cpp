#include "stdafx.h"
#include "player_vibration.h"

#include "input/input_windows.h"
#include "interface/user_interface_controller.h"
#include "game/game.h"
#include "main/main_time.h"
#include "game/game_time.h"
#include "objects/damage_effects.h"
#include "simulation/simulation.h"

#include "H2MOD/Tags/TagInterface.h"

s_vibration_globals* vibration_globals_get(void);
s_vibration_state vibration_get_state_usercall(s_vibration_user_globals* user_globals);

s_vibration_state vibration_get_state(s_vibration_user_globals* user_globals);

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

real32 g_vibration_dt_accumulator = 0.f;
void __cdecl vibration_update(real32 dt)
{
    g_vibration_dt_accumulator += dt;
    real32 tick_length = 1.f / 30.f; //game_tick_length();    // ### TODO Use game_tick_length() here when we use 30 tick in multiplayer
    if (g_vibration_dt_accumulator >= tick_length)  // Only execute the rumble logic every tick
    {
        // update rumble with 33ms delta
        bool controller_rumble_enabled[4] = { false };
        if (!main_time_halted() && !simulation_starting_up() && !game_is_playback())
        {
            for (uint32 user_index = 0; user_index < k_number_of_users; user_index++)
            {
                s_vibration_user_globals* globals = vibration_get(user_index);
                s_vibration_state state = vibration_get_state(globals);
                datum player_index = player_index_from_user_index(user_index);

                dt = tick_length;       // Increment by the tick_length instead of dt for now to fix issues when playing above 30fps
                globals->duration[0] += dt;
                globals->duration[1] += dt;
                globals->duration[2] += dt;
                globals->duration[3] += dt;
                globals->duration[4] += dt;
                globals->duration[5] += dt;
                globals->duration[6] += dt;
                globals->duration[7] += dt;

                if (player_index != NONE)
                {
                    s_player* player = (s_player*)datum_get(s_player::get_data(), player_index);
                    if (player->controller_index != NONE && user_interface_controller_get_rumble_enabled(player->controller_index))
                    {
                        input_set_gamepad_rumbler_state(player->controller_index, state.left, state.right);

                        ASSERT(VALID_INDEX(player->controller_index, k_number_of_controllers));
                        controller_rumble_enabled[player->controller_index] = true;
                    }
                }
            }
        }

        for (uint32 i = 0; i < k_number_of_users; i++)
        {
            if (!controller_rumble_enabled[i])
            {
                input_set_gamepad_rumbler_state(i, 0, 0);
            }
        }

        g_vibration_dt_accumulator = 0.f;
    }

    return;
}


s_vibration_globals* vibration_globals_get(void)
{
    return *Memory::GetAddress<s_vibration_globals**>(0x4CA378, 0x4F3DC4);
}

s_vibration_state vibration_get_state_usercall(s_vibration_user_globals* user_globals) 
{
    s_vibration_state state;
    void* fn = (void*)Memory::GetAddress(0x90254);
    __asm {
        mov eax, user_globals
        call fn
        mov state, eax
    }
    return state;
}

s_vibration_state vibration_get_state(s_vibration_user_globals* user_globals)
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

    s_vibration_state state
    {
        (uint16)PIN(intensities[0], 0.0f, (real32)UINT16_MAX),  // Left
        (uint16)PIN(intensities[1], 0.0f, (real32)UINT16_MAX)   // Right
    };
    return state;
}
