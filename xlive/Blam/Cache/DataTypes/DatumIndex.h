#pragma once
#include <Windows.h>
#include <codecvt>
namespace Blam
{
	namespace Cache
	{
		/*********************************************************************
		* Blam::Cache::DatumIndex
		* 4 BYTE Tag Structure for any TagIndices of Halo 2 CacheFiles
		**********************************************************************/
		struct DatumIndex
		{
			//Default Constructor
			DatumIndex()
			{
				this->Index = 0xFFFFu;
				this->Salt = 0xFFFFu;
			}
			//Copy Constructor
			DatumIndex(INT32 Identifier)
			{
				this->data = Identifier;
			}

			constexpr DatumIndex(short _index, short _salt) :
				Index(_index),
				Salt(_salt)
			{
			}

			static const INT32 Null = 0xFFFFFFFF;

			//Returns true is DatumIndex is not Invalid
			inline bool IsNull()
			{
				return this->Salt = 0xFFFF && this->Index == 0xFFFF;
			}

			inline unsigned long ToInt()
			{
				return data;
			}

			inline signed short ToAbsoluteIndex() { return Index; };

			inline bool operator == (const DatumIndex &other)
			{
				return this->data == other.data;
			}
			inline bool operator != (const DatumIndex &other)
			{
				return this->data != other.data;
			}

			union {
				struct {
					signed short Index;
					signed short Salt;
				};
				unsigned long data;
			};
		};
		static_assert(sizeof(DatumIndex) == 4, "Invalid Size for struct (DatumIndex)");
	}
}
			