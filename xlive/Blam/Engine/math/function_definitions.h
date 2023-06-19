#pragma once
#include "Blam/Cache/DataTypes/TagBlock.h"

class c_function_definition
{
	tag_block<byte> function;
public:
	tag_block<byte> get_function();
};