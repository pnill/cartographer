#include "stdafx.h"
#include "user_interface_controller.h"

bool __cdecl user_interface_controller_get_rumble_enabled(int32 gamepad_index)
{
    return INVOKE(0x207600, 0x0, user_interface_controller_get_rumble_enabled, gamepad_index);
}
