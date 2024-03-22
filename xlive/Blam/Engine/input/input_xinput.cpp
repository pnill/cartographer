#include "stdafx.h"
#include "input_xinput.h"

#include "Blam/Engine/game/game.h"
#include "Blam/Engine/game/game_time.h"

bool g_input_feedback_suppress = false;
XINPUT_VIBRATION g_xinput_vibration{};
xinput_device** g_xinput_devices;
uint32* g_main_controller_index;

void input_xinput_clear_rumble_state(void)
{
    // Originally 20
    // We set to 4 and check if if the controller is plugged in
    XINPUT_VIBRATION vibration{};

    for (uint32 i = 0; i < 4; i++)
    {
        xinput_device* device = g_xinput_devices[i];
        if (device && controller_button_state_get((e_controller_index)i)->plugged_in)
        {
            device->set_state(&vibration);
        }
    }
    return;
}

void input_xinput_update_rumble_state(void)
{
    bool suppress_rumble = g_input_feedback_suppress || *input_suppress_global_get();
    if (!game_in_progress() || game_time_get_paused())
    {
        suppress_rumble = true;
    }

    if (controller_profile_get(_controller_index_0)->field_1658 != 1)
    {
        suppress_rumble = true;
    }

    if (controller_button_state_get(_controller_index_0)->plugged_in)
    {
        g_xinput_vibration.wLeftMotorSpeed = (suppress_rumble ? 0 : g_vibration_state[_controller_index_0].left);
        g_xinput_vibration.wRightMotorSpeed = (suppress_rumble ? 0 : g_vibration_state[_controller_index_0].right);
        xinput_device* device = g_xinput_devices[*g_main_controller_index];
        
        if (device)
        {
            device->set_state(&g_xinput_vibration);
        }
    }

    return;
}


void xinput_apply_patches(void)
{
    g_xinput_devices = Memory::GetAddress<xinput_device**>(0x479F00);
    g_main_controller_index = Memory::GetAddress<uint32*>(0x47A714);

    PatchCall(Memory::GetAddress(0x2FBDA), input_xinput_update_rumble_state);
    PatchCall(Memory::GetAddress(0x2FC34), input_xinput_clear_rumble_state);
    return;
}