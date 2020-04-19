#pragma once
#include "..\Blam\Cache\DataTypes\DatumIndex.h"

namespace TagInterface
{
	using namespace std;	

	//* Main TagInterface Manager Class
	static class global_tags_interface
	{
		//Opens the tag in memory to be modified
		static void Open(__int32 type, void* tag_mem);
		//Closes the tag in memory
		static void Close(__int32 type, void* tag_mem);
	public:
		//function to retrieve interface to the tag via datum_index
		///required to dynamic_cast it to the required type
		static void* GetTagInterface(datum tag, int type);
		//Release function to called pre-map unloading
		//else MEMORY_LEAKS
		static void Release();
		//sets up call patch for interface unloading
		static void Init();
	}GlobalTagInterface;
}