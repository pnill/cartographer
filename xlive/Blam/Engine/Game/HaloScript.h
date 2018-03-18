#pragma once
#include "../../Enums/Game/HaloScriptTypes.h"

namespace Blam
{
	namespace EngineDefinitions
	{
		namespace Game
		{
			struct HaloScriptGlobal
			{
				hs_type type;
				byte padding[2];
				void *data;
			};

			typedef void **(__cdecl *hs_func_impl)(int hs_opcode, void *script_engine, char a3);

			struct HaloScriptCommand
			{
				hs_type type;
				WORD unk;
				hs_func_impl func;
				char *usage;
				WORD arg_count;
				hs_type arg_array[1]; // array size is dynamic, but always at least one
			};
		}
	}
}