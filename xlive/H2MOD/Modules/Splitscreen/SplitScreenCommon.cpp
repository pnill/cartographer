#include "H2MOD.h"
bool isSplitScreen()
{
	return *(int*)h2mod->GetAddress(0x4E6974) > 1;
}