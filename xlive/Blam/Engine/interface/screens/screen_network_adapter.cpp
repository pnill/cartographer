#include "stdafx.h"
#include "screen_network_adapter.h"

void* c_screen_network_adapter_menu::load(s_screen_parameters* parameters)
{
	return INVOKE(0x21F5F8, 0x0, c_screen_network_adapter_menu::load, parameters);
}
