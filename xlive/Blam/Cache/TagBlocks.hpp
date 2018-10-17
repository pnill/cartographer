#pragma once
#ifndef TAG_BLOCKS_H
#define TAG_BLOCKS_H
#include "Blam\Enums\Tags\TagGroups.h"
#include "Blam\Cache\DataTypes\Strings.h"

#define TAG_BLOCK_SIZE_ASSERT(tagblock,size)\
static_assert (sizeof(tagblock) == (size),"Invalid Size for TagBlock <" #tagblock ">")
namespace Blam
{
	namespace Cache
	{
		using Blam::Enums::Tags::TagGroupTypes;	

		//Base definition for a TagBlock
		template <const TagGroupTypes ParentTag> 
		struct TagBlock
		{
			static const TagGroupTypes BaseTag = ParentTag;
	
		};
	}
}
#endif
