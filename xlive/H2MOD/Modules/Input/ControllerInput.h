#pragma once
namespace ControllerInput
{
	char* __cdecl get_controller_input(__int16 index);
	void SetSensitiviy(float value);
	void Initialize();
}