#include "Blam/Maths/real_math.h"
namespace Blam
{
	namespace Engine
	{
		namespace Game 
		{
			struct __unaligned __declspec(align(1)) damage_data
			{
				DWORD damage_tag_index;
				DWORD flags;
				DWORD field_8;
				datum creator_datum;
				signed __int16 field_10;
				BYTE gap_12[2];
				signed int field_14;
				signed int field_18;
				int field_1C;
				BYTE gap_20[4];
				DWORD field_24;
				DWORD field_28;
				DWORD field_2C;
				DWORD field_30;
				DWORD field_34;
				DWORD field_38;
				real_point3d field_3C;
				BYTE gap48[12];
				real_point3d field_54;
				BYTE gap_60[24];
				float field_78;
				__int16 field_7C;
				signed __int16 field_7E;
				BYTE gap_80[4];
				char field_84;
			};
		}
	}
}