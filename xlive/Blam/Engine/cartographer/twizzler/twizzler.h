#pragma once

#ifdef PC5_EX
PC5_EX
#else
extern bool g_twizzler_status;
#endif

void BLAM_MATH_INL twizzler_set_status(bool status)
{
	g_twizzler_status = status;
	TEST_N_DEF(PC5);
	return;
}
