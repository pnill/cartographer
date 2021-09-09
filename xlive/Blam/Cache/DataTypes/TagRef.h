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
};
CHECK_STRUCT_SIZE(tag_reference, 8);