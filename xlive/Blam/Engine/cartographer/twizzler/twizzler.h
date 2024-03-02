#pragma once

#ifdef PC5_EX
PC5_EX
#else
extern bool g_twizzler_status;
#endif

void twizzler_set_status(bool status);
