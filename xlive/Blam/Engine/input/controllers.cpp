#include "stdafx.h"
#include "controllers.h"


s_controller_profile* controller_profile_get_global(void);
s_controller_button_state* controller_button_state_get_global(void);


s_controller_profile* controller_profile_get_global(void)
{
	return Memory::GetAddress<s_controller_profile*>(0x4A89B0);
}

s_controller_profile* controller_profile_get(e_controller_index index)
{
	return &controller_profile_get_global()[index];
}

s_controller_button_state* controller_button_state_get_global(void)
{
	return Memory::GetAddress<s_controller_button_state*>(0x47A5C8);
}

s_controller_button_state* controller_button_state_get(e_controller_index index)
{
	return &controller_button_state_get_global()[index];
}