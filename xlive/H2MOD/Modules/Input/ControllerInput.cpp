#include "stdafx.h"
#include "ControllerInput.h"

#include "game/game.h"
#include "math/math.h"
#include "Networking/logic/life_cycle_manager.h"
#include "H2MOD/Modules/Shell/Config.h"


namespace ControllerInput
{

	//Main function that iterates through all available input devices and calls update_xinput_state from the items vtable
	void __cdecl update_xinput_devices()
	{
		WORD controller_button = 0;
		auto InputDevices = Memory::GetAddress<controller_info**>(0x479F00);
		//Game checks for only a max of 20 inputs, if someone ever exceeds this.. I'll be impressed
		for(auto i = 0; i < 20; i++)
		{
			auto InputDevice = InputDevices[i];
			if (InputDevice->error_level == 0)
			{
				(*reinterpret_cast<void(__thiscall *)(controller_info*)>(InputDevice->xinput_device_vtbl[2]))(InputDevice);
				if (get_game_life_cycle() == _life_cycle_in_game || game_mode_get() == _game_mode_campaign)
				{
					if (InputDevice->xinput_state.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_UP)
						controller_button |= H2Config_CustomLayout.DPAD_UP;
					if (InputDevice->xinput_state.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_DOWN)
						controller_button |= H2Config_CustomLayout.DPAD_DOWN;
					if (InputDevice->xinput_state.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_LEFT)
						controller_button |= H2Config_CustomLayout.DPAD_LEFT;
					if (InputDevice->xinput_state.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_RIGHT)
						controller_button |= H2Config_CustomLayout.DPAD_RIGHT;
					if (InputDevice->xinput_state.Gamepad.wButtons & XINPUT_GAMEPAD_START)
						controller_button |= H2Config_CustomLayout.START;
					if (InputDevice->xinput_state.Gamepad.wButtons & XINPUT_GAMEPAD_BACK)
						controller_button |= H2Config_CustomLayout.BACK;
					if (InputDevice->xinput_state.Gamepad.wButtons & XINPUT_GAMEPAD_LEFT_THUMB)
						controller_button |= H2Config_CustomLayout.LEFT_THUMB;
					if (InputDevice->xinput_state.Gamepad.wButtons & XINPUT_GAMEPAD_RIGHT_THUMB)
						controller_button |= H2Config_CustomLayout.RIGHT_THUMB;
					if (InputDevice->xinput_state.Gamepad.wButtons & XINPUT_GAMEPAD_LEFT_SHOULDER)
						controller_button |= H2Config_CustomLayout.LEFT_SHOULDER;
					if (InputDevice->xinput_state.Gamepad.wButtons & XINPUT_GAMEPAD_RIGHT_SHOULDER)
						controller_button |= H2Config_CustomLayout.RIGHT_SHOULDER;
					if (InputDevice->xinput_state.Gamepad.wButtons & XINPUT_GAMEPAD_A)
						controller_button |= H2Config_CustomLayout.A;
					if (InputDevice->xinput_state.Gamepad.wButtons & XINPUT_GAMEPAD_B)
						controller_button |= H2Config_CustomLayout.B;
					if (InputDevice->xinput_state.Gamepad.wButtons & XINPUT_GAMEPAD_X)
						controller_button |= H2Config_CustomLayout.X;
					if (InputDevice->xinput_state.Gamepad.wButtons & XINPUT_GAMEPAD_Y)
						controller_button |= H2Config_CustomLayout.Y;

					InputDevice->xinput_state.Gamepad.wButtons = controller_button;
				}
			}
		}
	}


	void ControllerInput::Initialize()
	{
		if (Memory::IsDedicatedServer()) return;
		
		PatchCall(Memory::GetAddress(0x2FA58), update_xinput_devices);
	}
}