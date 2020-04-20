#pragma once

#include"..\DataTypes\DataTypes.h"
#include"..\TagGroups.hpp"
#include"..\Blam\Maths\Maths.h"

namespace Blam
{
	namespace Cache
	{
		namespace Tags
		{
			
			/*********************************************************************
			* Defines some functions to manage TagBlock Memory
			* Inherited by other TagBlocks
			**********************************************************************/
			struct tag_base
			{
			public:
				//does nothing
				void Open();
				void Close();
			};
			
		}
	}
}