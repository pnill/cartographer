#pragma once
#ifndef TAG_REF_H
#define TAG_REF_H
#include "Blam\Enums\Tags\TagGroups.h"
#include "Blam\Cache\DataTypes\DatumIndex.h"


/*********************************************************************
* Blam::Cache::DataTypes::tag_reference
* 8 BYTE Tag Structure for any Tag Reference Field
**********************************************************************/
struct tag_reference
{
	Blam::Enums::Tags::TagGroupTypes TagGroup;
	datum TagIndex;
};
CHECK_STRUCT_SIZE(tag_reference, 8);
#endif
