#pragma once

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
			* Structs using Reflexive type declaration should at least have an Open() and Close() members
			**********************************************************************/
			template<typename T>
			struct Reflexive
			{
				//* Returns Number of Block Field Elements
				uint32_t GetElementCount();
				//* Returns Block Field Offset
				uint32_t GetFieldOffset();
				//* Returns Tag Block Field Size
				size_t GetFieldSize();
				//* Returns Total Tag Block Field Size of all Elements
				size_t GetTotalSize();
				//* Returns Tag Block Field Elements Actual Pointer(List)
				void* GetTagBlockElements();
				//* Block access via index
				inline T* operator[](int index);
				//* Opens the tag_block for Block addition and Deletion
				//* Only to be called once else memory_leaks
				void Open();
				//* Releases the memory allocated for editing purposes
				//* Shouldnt be called unless opened else heap_corruption
				void Close();
				//Adds an element at the end of the Container(does a memory copy)
				void PushBack(T*);
				//Removes the element at the specifed index
				void RemoveAt(int);
			private:
				//* Number of Block Field Elements
				uint32_t TagBlockCount;
				//* Block Field Offset
				uint32_t TagBlockOffset;
			};
		}
	}
}