#pragma once
#include "stdafx.h"

namespace Blam
{
	namespace SharedDefinitions
	{		
		typedef BOOL IceCreamFlavorStock;
		//size : 0x44
		struct s_globals_struct
		{
			DWORD unk[0x10];//First 0x40 Bytes no idea
			DWORD RunTimeBase;//Game Uses global+0x44 for every global to get runtime base
		};
	}			
	
}
