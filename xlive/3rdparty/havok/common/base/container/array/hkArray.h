#pragma once
#include "havok/common/base/types/hkBaseTypes.h"

template <typename T>
class hkArray
{
public:

	// Public so that the serialization can access it.
	enum
	{
		CAPACITY_MASK = hkInt32(0x3FFFFFFF),
		FLAG_MASK = hkInt32(0xC0000000),
		DONT_DEALLOCATE_FLAG = hkInt32(0x80000000), // Indicates that the storage is not the array's to delete
		LOCKED_FLAG = hkInt32(0x40000000),  // Indicates that the array will never have its dtor called (read in from packfile for instance)
		FORCE_SIGNED = -1
	};
	const unsigned hkInt32** m_data;
	hkInt32 m_size;
	hkInt32 m_capacityAndDeallocateFlag; // highest 2 bits indicate any special considerations about the allocation for the array
};

template <typename T, unsigned N>
class hkInplaceArray : public hkArray<T>
{
	public:
		T m_storage[N];
};