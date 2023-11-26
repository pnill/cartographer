#include "stdafx.h"
#include "KantTesting.h"

#include "Blam/Engine/game/game.h"
#include "Blam/Engine/Networking/logic/life_cycle_manager.h"
#include "H2MOD/Modules/Input/KeyboardInput.h"
#include "H2MOD/Modules/OnScreenDebug/OnscreenDebug.h"
#include "Util/Hooks/Hook.h"

namespace KantTesting
{
	void MapLoad()
	{
	}

    int klk = VK_DELETE;

    XNKID temp_kid;
    XNKEY temp_key;
    XNADDR temp_addr;
    void force_join(XNKID kid, XNKEY key, XNADDR addr, int8 exe_type, int32 exe_version, int32 comp_version)
    {
        auto handler = (c_game_life_cycle_handler_joining*)c_game_life_cycle_manager::get()->life_cycle_handlers[e_game_life_cycle::_life_cycle_joining];
        handler->joining_xnkid = kid;
        handler->joining_xnkey = key;
        handler->joining_xnaddr = addr;
        if (exe_type != EXECUTABLE_TYPE || exe_version != EXECUTABLE_VERSION || comp_version != COMPATIBLE_VERSION)
        {
            handler->join_attempt_result_code = 9;
        }
        else
        {
            c_game_life_cycle_handler_joining::check_joining_capability();
            wchar_t local_usernames[4][16];
            s_player_identifier local_identifiers[4];
            int valid_local_player_count = 0;
            for (auto i = 0; i < 4; i++)
            {
                s_player_identifier temp_identifier;
                s_player_properties temp_properties;
                if (network_session_interface_get_local_user_identifier(i, &temp_identifier) || get_local_user_properties(i, 0, &temp_properties, 0, 0))
                {
                    memcpy(local_usernames[valid_local_player_count], temp_properties.player_name, 16);
                    local_identifiers[valid_local_player_count].unk1 = temp_identifier.unk1;
                    local_identifiers[valid_local_player_count].unk2 = temp_identifier.unk2;
                    valid_local_player_count++;
                }
            }
            reset_global_player_counts();
            network_session_reset_something(2, 1);
            memset(&handler->player_identifiers, 0, sizeof(handler->player_identifiers));
            memcpy(&handler->player_identifiers, local_identifiers, sizeof(s_player_identifier) * valid_local_player_count);
            memcpy(&handler->player_names, local_usernames, sizeof(wchar_t) * 16 * valid_local_player_count);
            handler->field_11 = 0; //Always 0 in the original function
            handler->field_12 = 0; //Always 0 in the original function
            handler->field_14 = 1;
            handler->joining_user_count = valid_local_player_count;
            handler->field_54 = 2; //Always 2 in original function
            handler->field_10 = true; //Always 1 in original function

            handler->join_attempt_result_code = 0; //Force valid result code, leave the denying the connection up to the host.
        }
        c_game_life_cycle_manager::get()->request_state_change(_life_cycle_joining, 0, 0);
        game_shell_set_in_progress();
    }

    typedef bool(__cdecl t_set_xlive_join_game_parameters)(XNKID* kid, XNKEY* key, XNADDR* addr, void* session_info, int local_user_count, s_player_identifier* player_identifiers, wchar_t* player_names);
    t_set_xlive_join_game_parameters* p_set_xlive_join_game_parameters;

    bool __cdecl set_xlive_join_game_parameters(XNKID* kid, XNKEY* key, XNADDR* addr, void* session_info, int local_user_count, s_player_identifier* player_identifiers, wchar_t* player_names)
    {
        //store the current join game parameters to use in the example hotkey function
        temp_kid = *kid;
        temp_key = *key;
        temp_addr = *addr;
        return p_set_xlive_join_game_parameters(kid, key, addr, session_info, local_user_count, player_identifiers, player_names);
    }

	void Initialize()
	{
		if (ENABLEKANTTEST) {
            p_set_xlive_join_game_parameters = Memory::GetAddress<t_set_xlive_join_game_parameters*>(0x1AD5E9);
            PatchCall(Memory::GetAddress(0x2161A5), set_xlive_join_game_parameters);
            KeyboardInput::RegisterHotkey(&klk, []()
            {
                force_join(temp_kid, temp_key, temp_addr, EXECUTABLE_TYPE, EXECUTABLE_VERSION, COMPATIBLE_VERSION);
            });
		}
	}
}
