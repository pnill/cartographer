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
				struct ColorRGB
				{
					UINT8 Red;
					UINT8 Green;
					UINT8 Blue;
				};
			}
			namespace Short
			{
				struct ColorRGB
				{
					UINT16 Red;
					UINT16 Green;
					UINT16 Blue;
				};
			}			
			namespace Long
			{
				struct ColorRGB
				{
					UINT32 Red;
					UINT32 Green;
					UINT32 Blue;
				};
			}
		}
	}
}
