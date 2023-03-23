#include "stdafx.h"
#include "object_early_movers.h"
#include "objects.h"

#include "Blam/Cache/TagGroups/object_definition.hpp"
#include "H2MOD/Tags/TagInterface.h"

void object_early_mover_new(datum object_index)
{
    s_object_data_definition* object;

    object = object_get_fast_unsafe(object_index);
    if ((tags::get_tag_fast<s_object_group_definition>(object->tag_definition_index)->object_flags & s_object_group_definition::e_object_flags::early_mover) != 0
         && s_object_early_movers_globals::get()->object_index_count < 32)
    {
        object->physics_flags |= _object_is_early_mover_bit;
        s_object_early_movers_globals::get()->early_mover_objects[s_object_early_movers_globals::get()->object_index_count++] = object_index;
    }
}
