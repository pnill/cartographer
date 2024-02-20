#pragma once
#include "tag_files/tag_groups.h"

/*********************************************************************
* Blam::Cache::DataTypes::tag_reference
* 8 BYTE Tag Structure for any Tag Reference Field
**********************************************************************/

struct tag_reference
{
	tag_group TagGroup;
	datum TagIndex;
	inline void operator=(const tag_reference &other)
	{
		this->TagGroup = other.TagGroup;
		this->TagIndex = other.TagIndex;
	}
};
CHECK_STRUCT_SIZE(tag_reference, 8);