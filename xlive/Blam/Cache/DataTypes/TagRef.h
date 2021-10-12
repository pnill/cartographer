#pragma once

#include "BlamTag.h"

/*********************************************************************
* Blam::Cache::DataTypes::tag_reference
* 8 BYTE Tag Structure for any Tag Reference Field
**********************************************************************/

struct tag_reference
{
	blam_tag TagGroup;
	datum TagIndex;
	inline void operator=(const tag_reference &other)
	{
		this->TagGroup = other.TagGroup;
		this->TagIndex = other.TagIndex;
	}
};
CHECK_STRUCT_SIZE(tag_reference, 8);