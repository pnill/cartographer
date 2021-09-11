#include "GameStateObjects.h"

s_datum_array* s_game_state_objects::getArray()
{
    //Grabs the pointer from the engine to the s_datum_array of the objects table
    return *Memory::GetAddress<s_datum_array**>(0x4E461C, 0x50C8EC);
}

bool s_game_state_objects::indexValid(datum objectDatum)
{
    //TODO
    if (objectDatum.ToAbsoluteIndex() >= 0 && objectDatum.ToAbsoluteIndex() < getArray()->datum_max_elements)
        return true;
    return false;
}

//TODO: Add template parameter to cast return to given type biped, weapon, etc.
void* s_game_state_objects::getObject(datum objectDatum)
{
    if (!indexValid(objectDatum))
        return nullptr;
    return (void*)&getArray()->datum[objectDatum.ToAbsoluteIndex() * getArray()->datum_element_size];
}

void* s_game_state_objects::getObject(short objectIndex)
{
	return (void*)&getArray()->datum[objectIndex * getArray()->datum_element_size + 8];
}
