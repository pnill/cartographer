#pragma once

template <typename T>
class hkArray
{
public:

	// Public so that the serialization can access it.
	enum
	{
		CAPACITY_MASK = int(0x3FFFFFFF),
		FLAG_MASK = int(0xC0000000),
		DONT_DEALLOCATE_FLAG = int(0x80000000), // Indicates that the storage is not the array's to delete
		LOCKED_FLAG = int(0x40000000),  // Indicates that the array will never have its dtor called (read in from packfile for instance)
		FORCE_SIGNED = -1
	};
	const unsigned int** m_data;
	int m_size;
	int m_capacityAndDeallocateFlag; // highest 2 bits indicate any special considerations about the allocation for the array
};

template <typename T, unsigned N>
class hkInplaceArray : public hkArray<T>
{
	public:
		T m_storage[N];
};