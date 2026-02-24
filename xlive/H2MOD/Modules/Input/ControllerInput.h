#pragma once
namespace ControllerInput
{
	struct CustomControllerLayout
	{
		WORD DPAD_UP;
		WORD DPAD_DOWN;
		WORD DPAD_LEFT;
		WORD DPAD_RIGHT;
		WORD START;
		WORD BACK;
		WORD LEFT_THUMB;
		WORD RIGHT_THUMB;
		WORD LEFT_SHOULDER;
		WORD RIGHT_SHOULDER;
		WORD A;
		WORD B;
		WORD X;
		WORD Y;
		
	public:
		CustomControllerLayout(void);
		void ToArray(WORD* out)
		{
			WORD temp[] = { DPAD_UP , DPAD_DOWN , DPAD_LEFT, DPAD_RIGHT, START, BACK, LEFT_THUMB, RIGHT_THUMB, LEFT_SHOULDER, RIGHT_SHOULDER, A, B ,X, Y };
			csmemcpy(out, temp, sizeof(temp));
		}
	};
}