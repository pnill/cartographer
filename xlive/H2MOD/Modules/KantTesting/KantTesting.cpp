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
    int lkl = VK_END;

    XNKID temp_kid;
    XNKEY temp_key;
    XNADDR temp_addr;

    typedef bool(__cdecl t_set_xlive_join_game_parameters)(XNKID* kid, XNKEY* key, XNADDR* addr, void* session_info, int local_user_count, s_player_identifier* player_identifiers, wchar_t* player_names);
    t_set_xlive_join_game_parameters* p_set_xlive_join_game_parameters;

    bool __cdecl set_xlive_join_game_parameters(XNKID* kid, XNKEY* key, XNADDR* addr, void* session_info, int local_user_count, s_player_identifier* player_identifiers, wchar_t* player_names)
    {
        //store the current join game parameters to use in the example hotkey function
       /* temp_kid = *kid;
        temp_key = *key;
        temp_addr = *addr;*/
        return p_set_xlive_join_game_parameters(kid, key, addr, session_info, local_user_count, player_identifiers, player_names);
    }

    void test()
    {
        auto a = NetworkSession::GetActiveNetworkSession();
        auto host_xnaddr = a->p_network_observer->observer_channels[NetworkSession::GetPeerObserverChannel(a->session_host_peer_index)->observer_index].xnaddr;
        auto session_id = a->session_id;
        auto session_key = a->xnkey;
        temp_kid = session_id;
        temp_key = session_key;
        temp_addr = host_xnaddr;
        //LOG_INFO_GAME("{} {} {}", host_xnaddr., session_id, session_key);
    }

	void Initialize()
	{
		if (ENABLEKANTTEST) {
            p_set_xlive_join_game_parameters = Memory::GetAddress<t_set_xlive_join_game_parameters*>(0x1AD5E9);
            PatchCall(Memory::GetAddress(0x2161A5), set_xlive_join_game_parameters);
            KeyboardInput::RegisterHotkey(&klk, []()
            {
            	test();
                //force_join(temp_kid, temp_key, temp_addr, EXECUTABLE_TYPE, EXECUTABLE_VERSION, COMPATIBLE_VERSION);
            });
            KeyboardInput::RegisterHotkey(&lkl, []()
            {
                //test();
                game_direct_connect_to_session(temp_kid, temp_key, temp_addr, EXECUTABLE_TYPE, EXECUTABLE_VERSION, COMPATIBLE_VERSION);
            });
		}
	}
}
