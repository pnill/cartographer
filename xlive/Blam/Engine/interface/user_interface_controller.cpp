#include "stdafx.h"
#include "user_interface_controller.h"

bool __cdecl user_interface_controller_get_rumble_enabled(e_controller_index gamepad_index)
{
    return INVOKE(0x207600, 0x0, user_interface_controller_get_rumble_enabled, gamepad_index);
}

void __cdecl user_interface_controller_update_user_session_data(e_controller_index gamepad_index)
{
    return INVOKE(0x206A97, 0x0, user_interface_controller_update_user_session_data, gamepad_index);
}

void __cdecl user_interface_controller_set_desired_team_index(e_controller_index gamepad_index, e_game_team team)
{
    return INVOKE(0x2068F2, 0x0, user_interface_controller_set_desired_team_index, gamepad_index, team);
}
