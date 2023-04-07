#include "stdafx.h"

#include "ControllerInput.h"
#include "Blam/Engine/game/game_globals.h"
#include "Blam/Engine/Networking/logic/life_cycle_manager.h"
#include "H2MOD\Modules\Shell\Config.h"
#include "Util\Hooks\Hook.h"

namespace ControllerInput
{
	namespace {
		byte* byte_F4A5C8;
		int* InputDevicesArray;
		int* main_controller_index;
		byte* byte_F4A5CE;
		byte* button_state_array;
		byte* byte_F4A5D0;
		WORD* word_F4A5E0;
		int* xinput_button_bitmask;
		byte* unk_F4A5D2;
		short* word_F4A5E4;
		short* sThumbLX;
		short* sThumbLY;
		short* sThumbRX;
		short* sThumbRY;
		int* dword_F4A724;
		int* dword_F4A720;

		short baseDeadzone = 8689;
		short* axialDeadzoneX;
		short* axialDeadzoneY;
		short radialDeadzone;

		typedef int(__cdecl system_milliseconds_t)();
		system_milliseconds_t* p_system_milliseconds;

		typedef char(__cdecl* sub_B31EA2_t)(int local_player_index, byte* keybind_base, int a4, float* a5, float* a6, byte* a7);
		sub_B31EA2_t p_sub_B31EA2;

		byte* default_profile;
		byte* unk_input_struct;
	}


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
				if (get_game_life_cycle() == _life_cycle_in_game || get_current_engine_type() == _single_player)
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

	// Hijacking the call to game_is_minimized to apply radial deadzone calculations instead of rewritting the main function
	// File offset: 0x2EB1B
	char h_game_is_minimized()
	{
		// Just pulling the first controller index, if multiple controller support is added find a way to link
		// the controller index used in ControllerInput::update_controller_input.
		short* controller_input = (short*)ControllerInput::get_controller_input(0);

		// Radial deadzone is being calculated using the Pythagorean Theorem, if the point is outside of the given
		// Radius it is accepted as valid input otherwise it is rejected.
		byte result = 0;
		
		unsigned int ar = pow(radialDeadzone, 2);
		int lx = (int)controller_input[26];
		int ly = (int)controller_input[27];
		int rx = (int)controller_input[28];
		int ry = (int)controller_input[29];

		unsigned int alx = pow(lx, 2);
		unsigned int aly = pow(ly, 2);
		unsigned int arx = pow(rx, 2);
		unsigned int ary = pow(ry, 2);

		unsigned int lh = alx + aly;
		unsigned int rh = arx + ary;

		if (lh <= ar)
		{
			*sThumbLX = 0;
			*sThumbLY = 0;
		}
		else
			result++;
		if(rh <= ar)
		{
			*sThumbRX = 0;
			*sThumbRY = 0;
		}
		else
			result++;

		if (result >= 1)
			return 0;
		else
			return 1;
	}


	static float LeftStickThrottle[2]; // [rsp+B8h] [rbp+67h]
	static float RightStickThrottle[2]; // [rsp+C0h] [rbp+6Fh]
	static short *InputMap; // rax

	// TODO: Refactor this to a General Input namespace, it's called whether or not you are using a controller.
	void __cdecl ControllerInput::procces_input()
	{
		unsigned int ControllerIndex = 0; // edi
		float LeftStickX; // xmm0_4
		float LeftStickY; // xmm0_4
		float RightStickX; // xmm0_4
		float RightStickY; // xmm0_4
		signed __int64 result; // rax

		RightStickThrottle[0] = 0;
		RightStickThrottle[1] = 0;
		LeftStickThrottle[0] = 0;
		LeftStickThrottle[1] = 0;
		InputMap = (short*)ControllerInput::get_controller_input(ControllerIndex);
		if (InputMap)
		{
			LeftStickX = InputMap[26] * 0.000030518509;
			if (LeftStickX >= -1.0)
			{
				if (LeftStickX <= 1.0)
					RightStickThrottle[0] = InputMap[26] * 0.000030518509;
				else
					RightStickThrottle[0] = 1.0f;
			}
			else
			{
				RightStickThrottle[0] = -1.0f;
			}
			LeftStickY = InputMap[27] * 0.000030518509;
			if (LeftStickY >= -1.0)
			{
				if (LeftStickY <= 1.0)
					RightStickThrottle[1] = InputMap[27] * 0.000030518509;
				else
					RightStickThrottle[1] = 1.0f;
			}
			else
			{
				RightStickThrottle[1] = -1.0f;
			}
			RightStickX = InputMap[28] * 0.000030518509;
			if (RightStickX >= -1.0)
			{
				if (RightStickX <= 1.0)
					LeftStickThrottle[0] = InputMap[28] * 0.000030518509;
				else
					LeftStickThrottle[0] = 1.0f;
			}
			else
			{
				LeftStickThrottle[0] = -1.0f;
			}
			RightStickY = InputMap[29] * 0.000030518509;
			if (RightStickY >= -1.0)
			{
				if (RightStickY <= 1.0)
					LeftStickThrottle[1] = InputMap[29] * 0.000030518509;
				else
					LeftStickThrottle[1] = 1.0f;
			}
			else
			{
				LeftStickThrottle[1] = -1.0f;
			}
		}
		p_sub_B31EA2(ControllerIndex, default_profile, (int)InputMap, RightStickThrottle, LeftStickThrottle, unk_input_struct);
	}

	typedef unsigned char*(__cdecl* get_controller_input_t)(__int16 a1);
	get_controller_input_t p_get_controller_input;

	unsigned char* ControllerInput::get_controller_input(__int16 index)
	{
		return p_get_controller_input(index);
	}

	void ControllerInput::ToggleModern()
	{
		if (Memory::IsDedicatedServer()) return;
		if(H2Config_controller_modern)
		{
			PatchCall(Memory::GetAddress(0x39B82), procces_input);
		}
		else
		{
			PatchCall(Memory::GetAddress(0x39B82), Memory::GetAddress(0x628A8));
		}
	}

	void ControllerInput::SetSensitiviy(float value)
	{
		if (Memory::IsDedicatedServer()) return;
		if (value == 0.0f) return;

		value = blam_max(value - 1.0f, 0.0f);

		*Memory::GetAddress<float*>(0x4A89B8) = 80.0f + 20.0f * value; //x-axis
		*Memory::GetAddress<float*>(0x4A89BC) = 40.0f + 10.0f * value; //y-axis
	}

	void ControllerInput::SetDeadzones()
	{
		if (Memory::IsDedicatedServer()) return;
		if (H2Config_Controller_Deadzone == Axial || H2Config_Controller_Deadzone == Both) {
			*axialDeadzoneX = (short)((float)MAXSHORT * (H2Config_Deadzone_A_X / 100));
			*axialDeadzoneY = (short)((float)MAXSHORT * (H2Config_Deadzone_A_Y / 100));
			//*axialDeadzoneX = baseDeadzone * H2Config_Deadzone_A_X;
			//*axialDeadzoneY = baseDeadzone * H2Config_Deadzone_A_Y;
		}
		else
		{
			*axialDeadzoneX = 0;
			*axialDeadzoneY = 0;
		}
		if (H2Config_Controller_Deadzone == Radial || H2Config_Controller_Deadzone == Both) {
			radialDeadzone = (short)((float)MAXSHORT * (H2Config_Deadzone_Radial / 100));
		} 
		else
		{
			radialDeadzone = 0;
		}
	}

	bool ControllerInput::HasInput()
	{
		char* cInput = (char*)ControllerInput::get_controller_input(0);
		bool buttons = false;
		for (auto i = 0; i < 42; i++)
			if (cInput[i] != 0 && cInput[i] != MAXBYTE && cInput[i] != 0x40)
				buttons = true;
		short* scInput = (short*)cInput;

		int thumbStickL = 0;
		int thumbStickR = 0;
		bool radialL = false;
		bool radialR = false;
		if (H2Config_Controller_Deadzone == Axial || H2Config_Controller_Deadzone == Both) {
			if (abs(scInput[26]) >= ((float)MAXSHORT * (H2Config_Deadzone_A_X / 100)))
				thumbStickL++;
			if (abs(scInput[27]) >= ((float)MAXSHORT * (H2Config_Deadzone_A_Y / 100)))
				thumbStickL++;
			if (abs(scInput[28]) >= ((float)MAXSHORT * (H2Config_Deadzone_A_X / 100)))
				thumbStickR++;
			if (abs(scInput[29]) >= ((float)MAXSHORT * (H2Config_Deadzone_A_Y / 100)))
				thumbStickR++;
		}
		unsigned int ar = pow((short)((float)MAXSHORT * (H2Config_Deadzone_Radial / 100)), 2);
		unsigned int alx = pow(scInput[26], 2);
		unsigned int aly = pow(scInput[27], 2);
		unsigned int arx = pow(scInput[28], 2);
		unsigned int ary = pow(scInput[29], 2);
		unsigned int lh = alx + aly;
		unsigned int rh = arx + ary;
		if (rh >= ar)
			radialR = true;
		if (lh >= ar)
			radialL = true;
		return radialR || radialL || thumbStickL > 1 || thumbStickR > 1 || buttons;
	}

	void ControllerInput::Initialize()
	{
		if (Memory::IsDedicatedServer()) return;
		axialDeadzoneX = Memory::GetAddress<short*>(0x4AA02C);
		axialDeadzoneY = Memory::GetAddress<short*>(0x4AA02E);

		p_get_controller_input = Memory::GetAddress<get_controller_input_t>(0x2F433);

		default_profile = Memory::GetAddress<byte*>(0x4a89b0);
		unk_input_struct = Memory::GetAddress<byte*>(0x4AE578);
		p_sub_B31EA2 = Memory::GetAddress<sub_B31EA2_t>(0x61EA2);
		

		//PatchCall(Memory::GetAddress(0x2e95d), sub_B31BF4);



		byte_F4A5C8 = Memory::GetAddress<byte*>(0x47A5C8);
		InputDevicesArray = Memory::GetAddress<int*>(0x479F00);
		main_controller_index = Memory::GetAddress<int*>(0x47A714);
		byte_F4A5CE = Memory::GetAddress<byte*>(0x47A5CE);
		button_state_array = Memory::GetAddress<byte*>(0x47A5CC);
		byte_F4A5D0 = Memory::GetAddress<byte*>(0x47A5D0);
		word_F4A5E0 = Memory::GetAddress<WORD*>(0x47A5E0);
		xinput_button_bitmask = Memory::GetAddress<int*>(0x39DEE0);
		unk_F4A5D2 = Memory::GetAddress<byte*>(0x47A5D2);
		word_F4A5E4 = Memory::GetAddress<short*>(0x47A5E4);
		sThumbLX = Memory::GetAddress<short*>(0x47A600);
		sThumbLY = Memory::GetAddress<short*>(0x47A602);
		sThumbRX = Memory::GetAddress<short*>(0x47A604);
		sThumbRY = Memory::GetAddress<short*>(0x47A606);
		dword_F4A724 = Memory::GetAddress<int*>(0x47A724);
		dword_F4A720 = Memory::GetAddress<int*>(0x47A720);
		p_system_milliseconds = Memory::GetAddress<system_milliseconds_t*>(0x37E51);
		//PatchCall(Memory::GetAddress(0x2FBD2), update_controller_input);
		PatchCall(Memory::GetAddress(0x2eb1b), h_game_is_minimized);
		PatchCall(Memory::GetAddress(0x2FA58), update_xinput_devices);
		//Codecave(Memory::GetAddress(0x2e975), apply_dead_zones, 166);
		SetSensitiviy(H2Config_controller_sens);
		ToggleModern();
		SetDeadzones();
	}
}