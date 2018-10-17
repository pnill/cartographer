#pragma once
#pragma pack(push,1)
#ifndef NHDT_H
#define NHDT_H
#include "Blam\Cache\DataTypes\DataTypes.h"
#include "Blam\Cache\TagBlocks\Interface\nhdt.hpp"
#include "Blam\Cache\TagGroups.hpp"
using namespace Blam::Cache;
namespace Blam
{
	namespace Cache
	{
		namespace TagGroups
		{
			namespace Interface
			{
				/*********************************************************************
				* name: new_hud_definition
				* group_tag: nhdt
				* header size: 40
				**********************************************************************/
				struct nhdt :TagGroup<'nhdt'>
				{					
					DataTypes::tagRef dONOTUSE;
					DataTypes::Reflexive<TagBlocks::Interface::bitmap_widgets> BitmapWidgets;
					DataTypes::Reflexive<TagBlocks::Interface::numerical_Elements> NumericalElements;
					INT16 LowClipCutoff;
					INT16 LowAmmoCutoff;
					float AgeCutoff;
					PAD(8)// unknown98;
				};
				TAG_GROUP_SIZE_ASSERT(nhdt, 40);
			}
		}
	}
}
#endif
#pragma pack(pop)