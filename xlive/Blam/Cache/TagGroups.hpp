#pragma once
#ifndef TAG_GROUPS_H
#define TAG_GROUPS_H
#include "Blam\Enums\Tags\TagGroups.h"
#include "Blam\Cache\DataTypes\Strings.h"

#define TAG_GROUP_SIZE_ASSERT(tagGroup,size)\
static_assert (sizeof(tagGroup) == (size),"Invalid Size for TagGroup <" #tagGroup">");
namespace Blam
{
	namespace Cache
	{		
		//Base definition for a TagGroup
		template <const LONG GroupTag>
		struct TagGroup
		{
			//TagGroup Shorthand Class Code
			static const LONG GroupTagValue = GroupTag;	

			//TagGroup Full Name
			static std::string LongName; //Need a quite way to define for individual tags
			
		};
		
			
		
	}
}

#endif