#pragma once
#include "stdafx.h"

namespace Blam
{
	namespace SharedDefinitions
	{
		//Use this struct for Map Datum Indices
		struct DatumIndex
		{
			INT16 Index;
			INT16 Salt;
		};
		//Use this struct for Ingame generated Datum Indices
		struct c_object_index
		{
			INT16 Index;
			INT16 Salt;
		};
	}			
	
}
