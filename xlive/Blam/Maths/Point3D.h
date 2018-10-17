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
				struct Point3D
				{
					UINT8 X;
					UINT8 Y;
					UINT8 Z;
				};
			}
			namespace Short
			{
				struct Point3D
				{
					UINT16 X;
					UINT16 Y;
					UINT16 Z;
				};
			}
			namespace Long
			{
				struct Point3D
				{
					UINT32 X;
					UINT32 Y;
					UINT32 Z;
				};
			}
		}
	}
}
