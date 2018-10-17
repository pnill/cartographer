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
				struct Point2D
				{
					UINT8 X;
					UINT8 Y;
				};
			}
			namespace Short
			{
				struct Point2D
				{
					UINT16 X;
					UINT16 Y;
				};
			}			
			namespace Long
			{
				struct Point2D
				{
					UINT32 X;
					UINT32 Y;
				};
			}
		}
	}
}
