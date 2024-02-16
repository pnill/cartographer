#include "stdafx.h"
#include "periodic_functions.h"

real32 __cdecl transition_function_evaluate(e_transition_function_type function_type, real32 function_value)
{
	return INVOKE(0x8F3F7, 0x74E6E, transition_function_evaluate, function_type, function_value);
}

real32 __cdecl periodic_function_evaluate(e_periodic_function_type function_type, real32 function_value)
{
	return INVOKE(0x8F2D4, 0x74D4B, periodic_function_evaluate, function_type, function_value);
}
