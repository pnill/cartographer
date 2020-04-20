#pragma once
#ifndef TAG_GROUPS_H
#define TAG_GROUPS_H
#include "Blam\Enums\Tags\TagGroups.h"
#include "Blam\Cache\DataTypes\Strings.h"


//Base definition for a TagGroup
template <const LONG GroupTag>
struct TagGroup
{
	//TagGroup Shorthand Class Code
	static const LONG GroupTagValue = GroupTag;

	//TagGroup Full Name
	static std::string LongName; //Need a quite way to define for individual tags

};


#endif