#pragma once
#include "..\stdafx.h"
namespace Blam
{
	namespace Maths
	{
		namespace Integer
		{
			namespace Byte
			{
				struct ColorARGB
				{
					UINT8 Alpha;
					UINT8 Red;
					UINT8 Green;
					UINT8 Blue;
				};
			}
			namespace Short
			{
				struct ColorARGB
				{
					UINT16 Alpha;
					UINT16 Red;
					UINT16 Green;
					UINT16 Blue;
				};
			}			
			namespace Long
			{
				struct ColorARGB
				{
					UINT32 Alpha;
					UINT32 Red;
					UINT32 Green;
					UINT32 Blue;
				};
			}
		}
	}
}
