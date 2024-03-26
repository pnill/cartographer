#pragma once
#include "input/controllers.h"
#include "objects/objects.h"

bool __cdecl user_interface_controller_get_rumble_enabled(e_controller_index gamepad_index);

void __cdecl user_interface_controller_update_user_session_data(e_controller_index gamepad_index);

void __cdecl user_interface_controller_set_desired_team_index(e_controller_index gamepad_index, e_game_team team);
