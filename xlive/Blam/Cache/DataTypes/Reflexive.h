#pragma once

#include "..\H2MOD.h"

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
				std::size_t GetFieldSize();
				//* Returns Total Tag Block Field Size of all Elements
				std::size_t GetTotalSize();
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
template<typename T>
void Blam::Cache::DataTypes::Reflexive<T>::Open()
{
	if (this->TagBlockCount == 0)
		return;
	//allocate new memory outside of the memory allocated via game for tags
	char* tptr = new char[this->GetTotalSize()];
	memcpy(tptr, this->GetTagBlockElements(), this->GetTotalSize());

	this->TagBlockOffset = (DWORD)tptr - *h2mod->GetAddress<DWORD*>(0x47CD54, 0x4A29BC);

	//Do same for the child Blocks
	T* iterator = (T*)(this->GetTagBlockElements());
	for (uint32_t i = 0; i < this->GetElementCount(); i++)
	{
		iterator->Open();
		i++;
	}
}
template<typename T>
void Blam::Cache::DataTypes::Reflexive<T>::Close()
{
	if (this->TagBlockCount == 0)
		return;

	//first close child Blocks
	T* iterator = (T*)(this->GetTagBlockElements());
	for (uint32_t i = 0; i < this->GetElementCount(); i++)
	{
		iterator->Close();
		i++;
	}
	//first deallocate self
	delete[] this->GetTagBlockElements();

	//then nullify the count and the offset(should modify them at the end as Reflexive field make use of it)
	this->TagBlockCount = 0x0;
	this->TagBlockOffset = -1;
}
template<typename T>
void Blam::Cache::DataTypes::Reflexive<T>::PushBack(T* arg)
{
	//allocate new
	char* tptr = new char[this->GetTotalSize() + GetFieldSize()];

	if (this->TagBlockCount)
	{
		//copy old
		memcpy(tptr, this->GetTagBlockElements(), this->GetTotalSize());
		//clean up old
		delete[] this->GetTagBlockElements();
	}
	//copy new
	memcpy(tptr + this->GetTotalSize(), arg, GetFieldSize());

	this->TagBlockCount++;
	this->TagBlockOffset = (DWORD)tptr - *h2mod->GetAddress<DWORD*>(0x47CD54, 0x4A29BC);
}
template<typename T>
void Blam::Cache::DataTypes::Reflexive<T>::RemoveAt(int index)
{
	if (!this->TagBlockCount)
		throw new std::exception("Trying to delete element in ZERO size array");
	if (index < 0 && index >= this->TagBlockCount)
		throw new std::exception("Invalid index");

	//alloc new dest(0 element size protection)
	char* tptr;
	if (this->TagBlockCount - 1)
		tptr = new char[GetFieldSize()*(TagBlockCount - 1)];
	//src data
	char* optr = (char*)this->GetTagBlockElements();
	//iterate and add reqired
	int j = 0;///to use as an index index for dest
	for (uint32_t i = 0; i < TagBlockCount; i++)
	{
		if (i != index)
		{
			//addd it
			memcpy(tptr + this->GetFieldSize()*j, optr + this->GetFieldSize()*i, this->GetFieldSize());
			j++;
		}
		else
		{
			//else free its child blocks of the memory
			T* temp = (T*)(optr + this->GetFieldSize()*i);
			temp->Close();
		}
	}
	delete[] optr;

	this->TagBlockCount--;
	//to save from 0 size
	if (this->TagBlockCount)
		this->TagBlockOffset = (DWORD)tptr - *h2mod->GetAddress<DWORD*>(0x47CD54, 0x4A29BC);
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
void*  Blam::Cache::DataTypes::Reflexive<T>::GetTagBlockElements()
{		
	int MemPtr = *h2mod->GetAddress<DWORD*>(0x47CD54, 0x4A29BC) + TagBlockOffset;	
	return (void*)MemPtr;
}
template<typename T>
inline T* Blam::Cache::DataTypes::Reflexive<T>::operator[](int index)
{
	if ((index < this->TagBlockCount) && (index >= 0))
		return ((T*)GetTagBlockElements() + index);
	else throw new std::exception("Invalid index");
}
template<typename T>
inline UINT32 Blam::Cache::DataTypes::Reflexive<T>::GetFieldOffset() { return this->TagBlockOffset; }
template<typename T>
inline UINT32 Blam::Cache::DataTypes::Reflexive<T>::GetElementCount() { return this->TagBlockCount; }