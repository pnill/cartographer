#include"scnr.h"

namespace Blam
{
	namespace Cache
	{
		namespace Tags
		{
			void scnr::Open()
			{
				SimulationDefinitionTable.Open();
			}
			void scnr::Close()
			{
				SimulationDefinitionTable.Close();
			}
		}
	}
}