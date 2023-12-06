#include "stdafx.h"
#include "weapon_definitions.h"

#include "H2MOD/Tags/TagInterface.h"
#include "Util/Hooks/Hook.h"

weapon_first_person_interface_definition* first_person_interface_definition_get(const _weapon_definition* definition, e_character_type character_type)
{
    weapon_first_person_interface_definition* result = NULL;
    int32 index = 0;
    switch (character_type)
    {
    case _character_type_masterchief:
    case _character_type_spartan:
        index = 0;
        break;
    case _character_type_dervish:
    case _character_type_elite:
    case _character_type_flood:
        index = 1;
        break;
    default:
        break;
	}
    int32 size = definition->player_interface.first_person.size;
    if (index >= size && size > 0)
    {
        if (definition->player_interface.first_person.data != NONE)
        {
            result = definition->player_interface.first_person[0];
        }
    }
    else
    {
        if (definition->player_interface.first_person.data != NONE)
        {
            result = definition->player_interface.first_person[index];
        }
    }
    return result;
}

void weapon_definitions_apply_patches(void)
{
    DetourFunc(Memory::GetAddress<uint8*>(0x190164, 0x17CE56), (uint8*)first_person_interface_definition_get, 11);
    return;
}