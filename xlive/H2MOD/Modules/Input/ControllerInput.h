#pragma once
namespace ControllerInput
{
	struct CustomControllerLayout
	{
		WORD DPAD_UP		= XINPUT_GAMEPAD_DPAD_UP;
		WORD DPAD_DOWN		= XINPUT_GAMEPAD_DPAD_DOWN;
		WORD DPAD_LEFT		= XINPUT_GAMEPAD_DPAD_LEFT;
		WORD DPAD_RIGHT		= XINPUT_GAMEPAD_DPAD_RIGHT;
		WORD START			= XINPUT_GAMEPAD_START;
		WORD BACK			= XINPUT_GAMEPAD_BACK;
		WORD LEFT_THUMB		= XINPUT_GAMEPAD_LEFT_THUMB;
		WORD RIGHT_THUMB	= XINPUT_GAMEPAD_RIGHT_THUMB;
		WORD LEFT_SHOULDER	= XINPUT_GAMEPAD_LEFT_SHOULDER;
		WORD RIGHT_SHOULDER	= XINPUT_GAMEPAD_RIGHT_SHOULDER;
		WORD A				= XINPUT_GAMEPAD_A;
		WORD B				= XINPUT_GAMEPAD_B;
		WORD X				= XINPUT_GAMEPAD_X;
		WORD Y				= XINPUT_GAMEPAD_Y;
		void ToArray(WORD* out)
		{
			WORD temp[] = { DPAD_UP , DPAD_DOWN , DPAD_LEFT, DPAD_RIGHT, START, BACK, LEFT_THUMB, RIGHT_THUMB, LEFT_SHOULDER, RIGHT_SHOULDER, A, B ,X, Y };
			memcpy(out, temp, sizeof(temp));
		}
		std::string ToString()
		{
			std::string temp;
			temp += std::to_string(DPAD_UP) + "-";
			temp += std::to_string(DPAD_DOWN) + "-";
			temp += std::to_string(DPAD_LEFT) + "-";
			temp += std::to_string(DPAD_RIGHT) + "-";
			temp += std::to_string(START) + "-";
			temp += std::to_string(BACK) + "-";
			temp += std::to_string(LEFT_THUMB) + "-";
			temp += std::to_string(RIGHT_THUMB) + "-";
			temp += std::to_string(LEFT_SHOULDER) + "-";
			temp += std::to_string(RIGHT_SHOULDER) + "-";
			temp += std::to_string(A) + "-";
			temp += std::to_string(B) + "-";
			temp += std::to_string(X) + "-";
			temp += std::to_string(Y);
			return temp;
		}
		void FromString(std::string string)
		{
			std::stringstream ss(string);
			std::string item;
			std::vector<WORD> items;
			while(std::getline(ss, item, '-'))
			{
				items.push_back((WORD)std::stol(item));
			}
			DPAD_UP = items.at(0);
			DPAD_DOWN = items.at(1);
			DPAD_LEFT = items.at(2);
			DPAD_RIGHT = items.at(3);
			START = items.at(4);
			BACK = items.at(5);
			LEFT_THUMB = items.at(6);
			RIGHT_THUMB = items.at(7);
			LEFT_SHOULDER = items.at(8);
			RIGHT_SHOULDER = items.at(9);
			A = items.at(10);
			B = items.at(11);
			X = items.at(12);
			Y = items.at(13);
		}
	};

	struct alignas(4) controller_info
	{
		void **xinput_device_vtbl;
		DWORD dwUserIndex;
		DWORD error_level;
		XINPUT_STATE xinput_state;
	};


	unsigned char* __cdecl get_controller_input(__int16 index);
	void __cdecl procces_input();
	void ToggleModern();
	void SetSensitiviy(float value);
	bool HasInput();
	void SetDeadzones();
	void Initialize();
}