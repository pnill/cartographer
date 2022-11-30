#pragma once
#include "Blam/Common/Common.h"

struct s_machine_identifier
{
	char gap[6];
}; 
CHECK_STRUCT_SIZE(s_machine_identifier, 0x6);
