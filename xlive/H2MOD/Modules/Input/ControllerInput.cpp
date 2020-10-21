#include "ControllerInput.h"
#include "H2MOD.h"
#include "H2MOD/Modules/Config/Config.h"

void ControllerInput::SetSensitiviy(float value)
{
	if (value == 0)
		return;
	*h2mod->GetAddress<float*>(0x4A89B8) = 80.0f + 20.0f * value; //x-axis
	*h2mod->GetAddress<float*>(0x4A89BC) = 40.0f + 10.0f * value; //y-axis
}

void ControllerInput::Initialize()
{
	SetSensitiviy(H2Config_controller_sens);
}
