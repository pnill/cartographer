#pragma once
namespace ControllerInput
{
	
	
	enum XINPUT_BUTTONS : WORD
	{
		XINPUT_GAMEPAD_NONE = 0x0,
		XINPUT_GAMEPAD_DPAD_UP = 0x1,
		XINPUT_GAMEPAD_DPAD_DOWN = 0x2,
		XINPUT_GAMEPAD_DPAD_LEFT = 0x4,
		XINPUT_GAMEPAD_DPAD_RIGHT = 0x8,
		XINPUT_GAMEPAD_START = 0x10,
		XINPUT_GAMEPAD_BACK = 0x20,
		XINPUT_GAMEPAD_LEFT_THUMB = 0x40,
		XINPUT_GAMEPAD_RIGHT_THUMB = 0x80,
		XINPUT_GAMEPAD_LEFT_SHOULDER = 0x100,
		XINPUT_GAMEPAD_RIGHT_SHOULDER = 0x200,
		XINPUT_GAMEPAD_A = 0x1000,
		XINPUT_GAMEPAD_B = 0x2000,
		XINPUT_GAMEPAD_X = 0x4000,
		XINPUT_GAMEPAD_Y = 0x8000,
	};
	inline XINPUT_BUTTONS operator |(XINPUT_BUTTONS a, XINPUT_BUTTONS b)
	{
		return static_cast<XINPUT_BUTTONS>(static_cast<WORD>(a) | static_cast<WORD>(b));
	}
	inline XINPUT_BUTTONS operator &(XINPUT_BUTTONS a, XINPUT_BUTTONS b)
	{
		return static_cast<XINPUT_BUTTONS>(static_cast<WORD>(a) & static_cast<WORD>(b));
	}
	inline XINPUT_BUTTONS& operator |=(XINPUT_BUTTONS& a, XINPUT_BUTTONS b)
	{
		return a = a | b;
	}
	struct CustomControllerLayout
	{
		XINPUT_BUTTONS DPAD_UP			= XINPUT_GAMEPAD_DPAD_UP;
		XINPUT_BUTTONS DPAD_DOWN		= XINPUT_GAMEPAD_DPAD_DOWN;
		XINPUT_BUTTONS DPAD_LEFT		= XINPUT_GAMEPAD_DPAD_LEFT;
		XINPUT_BUTTONS DPAD_RIGHT		= XINPUT_GAMEPAD_DPAD_RIGHT;
		XINPUT_BUTTONS START			= XINPUT_GAMEPAD_START;
		XINPUT_BUTTONS BACK				= XINPUT_GAMEPAD_BACK;
		XINPUT_BUTTONS LEFT_THUMB		= XINPUT_GAMEPAD_LEFT_THUMB;
		XINPUT_BUTTONS RIGHT_THUMB		= XINPUT_GAMEPAD_RIGHT_THUMB;
		XINPUT_BUTTONS LEFT_SHOULDER	= XINPUT_GAMEPAD_LEFT_SHOULDER;
		XINPUT_BUTTONS RIGHT_SHOULDER	= XINPUT_GAMEPAD_RIGHT_SHOULDER;
		XINPUT_BUTTONS A				= XINPUT_GAMEPAD_A;
		XINPUT_BUTTONS B				= XINPUT_GAMEPAD_B;
		XINPUT_BUTTONS X				= XINPUT_GAMEPAD_X;
		XINPUT_BUTTONS Y				= XINPUT_GAMEPAD_Y;
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
			std::vector<XINPUT_BUTTONS> items;
			while(std::getline(ss, item, '-'))
			{
				items.push_back((XINPUT_BUTTONS)std::stoi(item));
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

	struct alignas(2) XINPUT_GAMEPAD
	{
		XINPUT_BUTTONS wButtons;
		BYTE bLeftTrigger;
		BYTE bRightTrigger;
		SHORT sThumbLX;
		SHORT sThumbLY;
		SHORT sThumbRX;
		SHORT sThumbRY;
	};

	struct alignas(4) XINPUT_STATE
	{
		DWORD dwPacketNumber;
		XINPUT_GAMEPAD Gamepad;
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