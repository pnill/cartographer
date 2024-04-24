#include "stdafx.h"
#include "user_interface_networking.h"

#include "user_interface.h"

#include "Blam/Engine/Networking/Session/NetworkSession.h"

bool* byte_D6840E_get(void)
{
    return Memory::GetAddress<bool*>(0x46EE9F);
}

XSESSION_INFO* global_session_info_get(void)
{
    return Memory::GetAddress<XSESSION_INFO*>(0x46EEA4);
}

int32* dword_86EEE0_get(void)
{
    return Memory::GetAddress<int32*>(0x46EEE0);
}

bool* from_game_invite_global_get(void)
{
    return Memory::GetAddress<bool*>(0x46EEA0);
}

void user_interface_networking_set_globals(bool a1, XSESSION_INFO* session, int32 unused, bool from_game_invite)
{
    *byte_D6840E_get() = a1;
    memcpy(global_session_info_get(), session, sizeof(XSESSION_INFO));
    *dword_86EEE0_get() = unused;
    *from_game_invite_global_get() = from_game_invite;
}

void __cdecl sub_5AD643(void)
{
    INVOKE(0x1AD643, 0x0, sub_5AD643);
    return;
}

void __cdecl sub_40B8B9(void)
{
    INVOKE(0xB8B9, 0x0, sub_40B8B9);
    return;
}

void __cdecl user_interface_networking_open_join_screen(void)
{
    INVOKE(0x242E5B, 0x0, user_interface_networking_open_join_screen);
    return;
}

void __cdecl sub_5AD5AB(XSESSION_INFO* session, XNKEY* key, XNADDR* host_address, int32 local_player_count, s_player_identifier* user_identifiers, wchar_t** player_names, bool from_game_invite)
{
    INVOKE(0x1AD5AB, 0x0, sub_5AD5AB, session, key, host_address, local_player_count, user_identifiers, player_names, from_game_invite);
    return;
}

void user_interface_networking_join_game(XSESSION_INFO* session, int32 a2, bool from_game_invite)
{
    //INVOKE(0x2161E1, 0x1FD827, user_interface_networking_join_game, session_info, a2, from_game_invite);
    int32 local_player_count;
    wchar_t player_names[k_number_of_users][32];
    s_player_identifier user_identifiers[k_number_of_users];
    wchar_t* p_player_names[k_number_of_users];

    if (user_interface_local_player_count() <= 0)
    {
        user_interface_networking_set_globals(true, session, a2, from_game_invite);
        error_message_menu_open(1, 272, 4, 1 << a2, user_interface_controller_pick_profile_dialog_wrapper, 0);
    }
    else
    {
        sub_5AD643();
        local_player_count = 0;
        do
        {
            s_player_properties player_properties;
            if (network_session_interface_get_local_user_identifier(local_player_count, &user_identifiers[local_player_count])
                && network_session_interface_get_local_user_properties(local_player_count, NULL, &player_properties, NULL, NULL))
            {
                p_player_names[local_player_count] = player_names[local_player_count];
                wcsncpy_s(player_names[local_player_count], 32, player_properties.player_name, UINT_MAX);
            }
            ++local_player_count;
        } while (local_player_count < k_number_of_users);

        sub_40B8B9();
        sub_5AD5AB(session, &session->keyExchangeKey, &session->hostAddress, local_player_count, user_identifiers, p_player_names, from_game_invite);
        user_interface_networking_open_join_screen();

        return;
    }
}