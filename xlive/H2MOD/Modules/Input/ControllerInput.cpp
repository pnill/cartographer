#include "ControllerInput.h"
#include "H2MOD.h"
#include "H2MOD/Modules/Config/Config.h"


typedef char*(__cdecl p_get_controller_input)(__int16 a1);
p_get_controller_input* c_get_controller_input;

char* ControllerInput::get_controller_input(short index)
{
	return c_get_controller_input(index);
}

void ControllerInput::SetSensitiviy(float value)
{
	if (value == 0)
		return;
	*h2mod->GetAddress<float*>(0x4A89B8) = 80.0f + 20.0f * value; //x-axis
	*h2mod->GetAddress<float*>(0x4A89BC) = 40.0f + 10.0f * value; //y-axis
}

void ControllerInput::Initialize()
{
	c_get_controller_input = h2mod->GetAddress<p_get_controller_input*>(0x2F433);
	SetSensitiviy(H2Config_controller_sens);
}
