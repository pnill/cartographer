#include "stdafx.h"
#include "user_interface.h"

float* get_ui_scale_factor()
{
	return Memory::GetAddress<float*>(0xA3E424);
}
