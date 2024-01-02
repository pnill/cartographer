#include "stdafx.h"
#include "input_windows.h"

rumble_state* controller_rumble_state_get(int32 controller_index)
{
	return Memory::GetAddress<rumble_state*>(0x47A704);
}

int32* hs_debug_simulate_gamepad_global_get(void)
{
	return Memory::GetAddress<int32*>(0x47A71C);
}

bool* input_suppress_global_get(void)
{
	return Memory::GetAddress<bool*>(0x479F52);
}
