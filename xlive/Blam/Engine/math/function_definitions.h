#pragma once
#include "tag_files/tag_block.h"

class c_function_definition
{
	tag_block<byte> function;
public:
	tag_block<byte> get_function();
};