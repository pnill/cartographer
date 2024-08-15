#include "stdafx.h"
#include "HaloScript.h"

namespace HaloScript
{
	typedef void(__cdecl object_destroy_t)(datum object_datum_index);
	object_destroy_t* p_object_destroy;
	void ObjectDestroy(datum object_datum_index)
	{
		p_object_destroy(object_datum_index);
	}

	void Initialize()
	{
		p_object_destroy = Memory::GetAddress<object_destroy_t*>(0xFDCFD, 0x124ED5);
	}
}