#include "stdafx.h"
#include "twizzler.h"

#ifdef PC5_DEF
PC5_DEF
#else
bool g_twizzler_status = true;
#endif

void twizzler_set_status(bool status)
{
	g_twizzler_status = status;
	TEST_N_DEF(PC5);
	return;
}
