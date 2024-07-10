#pragma once
#include "tag_files/tag_block.h"

class c_function_definition
{
	tag_block<uint8> function;
public:
	tag_block<uint8> get_function(void);
	real32 evaluate(real32 min, real32 max);
};

