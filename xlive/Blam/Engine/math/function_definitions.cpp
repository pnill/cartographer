#include "stdafx.h"
#include "function_definitions.h"

tag_block<byte> c_function_definition::get_function()
{
	return function;
}

real32 c_function_definition::evaluate(real32 v_min, real32 v_max)
{
	typedef real32(__thiscall* evaluate_t)(c_function_definition*, real32, real32);
	return INVOKE_BY_TYPE(0xE79DA, 0xD60BA, evaluate_t, this, v_min, v_max);
}
