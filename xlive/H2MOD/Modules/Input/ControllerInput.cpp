#include "ControllerInput.h"
#include "H2MOD.h"
#include "H2MOD/Modules/Config/Config.h"
#include "Util/Hooks/Hook.h"
#include "H2MOD/Modules/Utils/Utils.h"
#include "H2MOD/Modules/Startup/Startup.h"

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


		typedef char(p_game_minimized)();
		p_game_minimized* game_is_minimized;

		typedef int(__cdecl p_system_milliseconds)();
		p_system_milliseconds* system_milliseconds;

		typedef char(__cdecl p_sub_B31EA2)(int local_player_index, byte* keybind_base, int a4, float* a5, float* a6, byte* a7);
		p_sub_B31EA2* sub_B31EA2;

		byte* default_profile;
		byte* unk_input_struct;
	}
	void __cdecl sub_B31BF4(int user_index, unsigned int a2)
	{
		memcpy((void *)a2, &default_profile + user_index, 0x1680u);
	}
	//Hijacking the call to game_is_minimized to apply radial deadzone calculations instead of rewritting the main function
	//File offset: 0x2EB1B
	char h_game_is_minimized()
	{
		//Just pulling the first controller index, if multiple controller support is added find a way to link
		//the controller index used in ControllerInput::update_controller_input.
		short* controller_input = (short*)ControllerInput::get_controller_input(0);

		//Radial deadzone is being calculated using the Pythagorean Theorem, if the point is outside of the given
		//Radius it is accepted as valid input otherwise it is rejected.
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

	//TODO: Refactor this to a General Input namespace, it's called whether or not you are using a controller.
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
		sub_B31EA2(ControllerIndex, default_profile, (int)InputMap, RightStickThrottle, LeftStickThrottle, unk_input_struct);
	}

	typedef char*(__cdecl p_get_controller_input)(__int16 a1);
	p_get_controller_input* c_get_controller_input;

	char* ControllerInput::get_controller_input(short index)
	{
		return c_get_controller_input(index);
	}
	void ControllerInput::ToggleModern()
	{
		if(H2Config_controller_modern)
		{
			PatchCall(h2mod->GetAddress(0x39B82), procces_input);
		}
		else
		{
			PatchCall(h2mod->GetAddress(0x39B82), h2mod->GetAddress(0x628A8));
		}
	}
	void ControllerInput::SetSensitiviy(float value)
	{
		if (value == 0)
			return;
		*h2mod->GetAddress<float*>(0x4A89B8) = 80.0f + 20.0f * value; //x-axis
		*h2mod->GetAddress<float*>(0x4A89BC) = 40.0f + 10.0f * value; //y-axis
	}
	void ControllerInput::SetDeadzones()
	{
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
		axialDeadzoneX = h2mod->GetAddress<short*>(0x4AA02C);
		axialDeadzoneY = h2mod->GetAddress<short*>(0x4AA02E);

		c_get_controller_input = h2mod->GetAddress<p_get_controller_input*>(0x2F433);

		default_profile = h2mod->GetAddress<byte*>(0x4a89b0);
		unk_input_struct = h2mod->GetAddress<byte*>(0x4AE578);
		sub_B31EA2 = h2mod->GetAddress< p_sub_B31EA2*>(0x61EA2);
		

		//PatchCall(h2mod->GetAddress(0x2e95d), sub_B31BF4);



		byte_F4A5C8 = h2mod->GetAddress<byte*>(0x47A5C8);
		InputDevicesArray = h2mod->GetAddress<int*>(0x479F00);
		main_controller_index = h2mod->GetAddress<int*>(0x47A714);
		byte_F4A5CE = h2mod->GetAddress<byte*>(0x47A5CE);
		button_state_array = h2mod->GetAddress<byte*>(0x47A5CC);
		byte_F4A5D0 = h2mod->GetAddress<byte*>(0x47A5D0);
		word_F4A5E0 = h2mod->GetAddress<WORD*>(0x47A5E0);
		xinput_button_bitmask = h2mod->GetAddress<int*>(0x39DEE0);
		unk_F4A5D2 = h2mod->GetAddress<byte*>(0x47A5D2);
		word_F4A5E4 = h2mod->GetAddress<short*>(0x47A5E4);
		sThumbLX = h2mod->GetAddress<short*>(0x47A600);
		sThumbLY = h2mod->GetAddress<short*>(0x47A602);
		sThumbRX = h2mod->GetAddress<short*>(0x47A604);
		sThumbRY = h2mod->GetAddress<short*>(0x47A606);
		dword_F4A724 = h2mod->GetAddress<int*>(0x47A724);
		dword_F4A720 = h2mod->GetAddress<int*>(0x47A720);
		game_is_minimized = h2mod->GetAddress<p_game_minimized*>(0x28729);
		system_milliseconds = h2mod->GetAddress<p_system_milliseconds*>(0x37E51);
		//PatchCall(h2mod->GetAddress(0x2FBD2), update_controller_input);
		PatchCall(h2mod->GetAddress(0x2eb1b), h_game_is_minimized);
		//Codecave(h2mod->GetAddress(0x2e975), apply_dead_zones, 166);
		SetSensitiviy(H2Config_controller_sens);
		ToggleModern();
		SetDeadzones();
	}
}