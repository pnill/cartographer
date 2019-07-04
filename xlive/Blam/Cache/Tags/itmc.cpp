#include"itmc.h"

namespace Blam
{
	namespace Cache
	{
		namespace Tags
		{
			void itmc::Open()
			{
				ItemPermutations.Open();
			}
			void itmc::Close()
			{
				ItemPermutations.Close();
			}

		}
	}
}