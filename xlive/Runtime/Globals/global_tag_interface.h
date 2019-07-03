#pragma once
#include "..\Blam\Engine\Tags\TagInstance.h"
#include<map>

namespace Runtime
{
	namespace Globals
	{
		using namespace std;

		//* Main TagInterface Manager Class
		static class global_tag_interface
		{
			//Opens the tag in memory to be modified
			static void Open(__int32 type, void* tag_mem);
			//Closes the tag in memory
			static void Close(__int32 type, void* tag_mem);
		public:
			//function to retrieve interface to the tag via datum_index
			///required to dynamic_cast it to the required type
			static void* GetTagInterface(DatumIndex arg0,int type);
			//function to retrieve interface to the tag via datum_index			
			template<typename TagDefinition>
			inline TagDefinition* GetTagInterface(DatumIndex arg0,int type);
			//Release function to called pre-map unloading
			//else MEMORY_LEAKS
			static void Release();
			//sets up call patch for interface unloading
			static void Init();
		}GlobalTagInterface;
	}
}