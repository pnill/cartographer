#pragma once
#include "Blam\Enums\Tags\TagGroups.h"

//Base definition for a TagGroup
template <const LONG GroupTag>
struct TagGroup
{
	//TagGroup Shorthand Class Code
	static const LONG GroupTagValue = GroupTag;

	//TagGroup Full Name
	static std::string LongName; //Need a quite way to define for individual tags
};
