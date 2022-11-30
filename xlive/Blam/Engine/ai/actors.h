#pragma once
#include "Blam/Cache/DataTypes/BlamDataTypes.h"
#include "Blam/Engine/memory/data.h"

/*
   TODO: Reverse engineer more of the actor struct and fill this data in appropriately.
   For now, we only know where the character datum which was used to create the actor is.
*/

struct s_actor {
	BYTE pad[0x54];
	datum character_datum;
	BYTE pad2[0x840];
};
CHECK_STRUCT_SIZE(s_actor, 0x898);

namespace actors
{
	s_data_array* get_actor_table();
}
