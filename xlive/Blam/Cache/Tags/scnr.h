#pragma once
#include"tag_base.h"

namespace Blam
{
	namespace Cache
	{
		namespace Tags
		{
			/*********************************************************************
			* name: scenario
			* group_tag : Scnr
			* header size : 992
			* *********************************************************************/
			struct scnr:tag_base 
			{
				//quite a lot of padding :P
				PAD(0x3D8);			
				struct SimulationDefinitionTable:tag_base
				{
					datum Tag;//0x0
				};
				TAG_BLOCK_SIZE_ASSERT(SimulationDefinitionTable, 0x4);
				Blam::Cache::DataTypes::Reflexive<SimulationDefinitionTable> SimulationDefinitionTable;//0x3D8

				void Open();
				void Close();
			};
			TAG_GROUP_SIZE_ASSERT(scnr, 0x3E0);
		}
	}
}
