#pragma once
#include <Windows.h>
namespace Blam
{
	namespace Cache
	{
		namespace DataTypes
		{
			/*********************************************************************
			* Blam::Cache::DataTypes::Reflexive
			* 8 BYTE Tag Structure for any Tag Block Field
			* UINT32 TagBlockCount;
			* UINT32 TagBlockOffset;
			**********************************************************************/
			template<typename T>
			struct Reflexive
			{
				//* Default Constructor
				Reflexive()
				{
					this->TagBlockCount = 0;
					this->TagBlockOffset = 0;
				}
				//* Initialize a Tag Block Field With Offset Relative to TagMemBase
				Reflexive(INT32 BlockCount)
				{
					this->TagBlockCount = BlockCount;
					int SharedMapBase = 0;
					this->TagBlockOffset = (int)this - SharedMapBase;
				}
				//* Initialize a Tag Block Field With Count Offset
				Reflexive(INT32 BlockCount, INT32 BlockOffset)
				{
					this->TagBlockCount = BlockCount;
					this->TagBlockOffset = BlockOffset;
				}
				//* Returns Number of Block Field Elements
				UINT32 GetElementCount();
				//* Returns Block Field Offset
				UINT32 GetFieldOffset();
				//* Returns Tag Block Field Size
				std::size_t GetFieldSize();
				//* Returns Total Tag Block Field Size of all Elements
				std::size_t GetTotalSize();
				//* Returns Tag Block Field Elements Pointer(List)
				T** GetTagBlockElements();
			private:
				//* Number of Block Field Elements
				UINT32 TagBlockCount;
				//* Block Field Offset
				UINT32 TagBlockOffset;

			};
		}
	}
}

template<typename T>
inline std::size_t Blam::Cache::DataTypes::Reflexive<T>::GetFieldSize()
{
	return sizeof(T);
}
template<typename T>
inline std::size_t Blam::Cache::DataTypes::Reflexive<T>::GetTotalSize()
{
	return this->GetFieldSize() * TagBlockCount;
}
template<typename T>
T**  Blam::Cache::DataTypes::Reflexive<T>::GetTagBlockElements()
{	
	int SharedMapBase = 0;//Need to update it later
	int MemPtr = SharedMapBase + TagBlockOffset;
	T** TagFields = MemPtr;
	return TagFields;
}
template<typename T>
inline UINT32 Blam::Cache::DataTypes::Reflexive<T>::GetFieldOffset() { return this->TagBlockOffset; }
template<typename T>
inline UINT32 Blam::Cache::DataTypes::Reflexive<T>::GetElementCount() { return this->TagBlockCount; }