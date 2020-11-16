#pragma once
namespace ControllerInput
{
	char* __cdecl get_controller_input(__int16 index);
	void __cdecl procces_input();
	void ToggleModern();
	void SetSensitiviy(float value);
	bool HasInput();
	void SetDeadzones();
	void Initialize();
}