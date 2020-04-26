#include "Reflexive.h"

#include "H2MOD.h"

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
size_t Blam::Cache::DataTypes::Reflexive<T>::GetFieldSize()
{
	return sizeof(T);
}

template<typename T>
size_t Blam::Cache::DataTypes::Reflexive<T>::GetTotalSize()
{
	return this->GetFieldSize() * TagBlockCount;
}

template<typename T>
void* Blam::Cache::DataTypes::Reflexive<T>::GetTagBlockElements()
{
	int MemPtr = *h2mod->GetAddress<DWORD*>(0x47CD54, 0x4A29BC) + TagBlockOffset;	
	return (void*)MemPtr;
}

template<typename T>
T* Blam::Cache::DataTypes::Reflexive<T>::operator[](int index)
{
	if ((index < this->TagBlockCount) && (index >= 0))
		return ((T*)GetTagBlockElements() + index);
	else throw new std::exception("Invalid index");
}

template<typename T>
uint32_t Blam::Cache::DataTypes::Reflexive<T>::GetFieldOffset()
{ 
	return this->TagBlockOffset; 
}

template<typename T>
uint32_t Blam::Cache::DataTypes::Reflexive<T>::GetElementCount()
{ 
	return this->TagBlockCount; 
}