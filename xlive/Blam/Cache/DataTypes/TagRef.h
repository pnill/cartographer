#pragma once
#include "..\..\stdafx.h"
#include "Blam\Enums\Tags\TagGroups.h"
#include "Blam\Cache\DataTypes\DatumIndex.h"

namespace Blam
{
	namespace Cache
	{
		namespace DataTypes
		{
			/*********************************************************************
			* Blam::Cache::DataTypes::tagRef
			* 8 BYTE Tag Structure for any Tag Reference Field
			**********************************************************************/
			struct tagRef
			{
				Blam::Enums::Tags::TagGroupTypes TagGroup;
				datum TagIndex;
			};
			static_assert(sizeof(tagRef) == 8, "Invalid Size for struct (tagRef)");
			/*********************************************************************
			* Blam::Cache::DataTypes::tagRefN
			* 4 BYTE Tag Structure for any Tag Reference Field (Lacks TagGroupType)
			**********************************************************************/
			struct tagRefN
			{
				datum TagIndex;
			};
			static_assert(sizeof(tagRefN) == 4, "Invalid Size for struct (tagRefN)");
		}
	}
}