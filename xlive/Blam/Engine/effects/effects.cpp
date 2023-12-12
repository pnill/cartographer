#include "stdafx.h"
#include "effects.h"

datum __cdecl effect_new_from_object(
    datum effect_tag_index,
    s_damage_owner* damage_owner,
    datum object_index,
    real32 a4,
    real32 a5,
    real_rgb_color* color,
    const void* effect_vector_field)
{
    return INVOKE(0xAADCE, 0x9CE4E, effect_new_from_object, effect_tag_index, damage_owner, object_index, a4, a5, color, effect_vector_field);
}

s_data_array* get_effects_table()
{
    //TODO: Server offset
    return *Memory::GetAddress<s_data_array**>(0x4CE884, 0x0);
}

s_data_array* get_effects_location_table()
{
    //TODO: Server offset
    return *Memory::GetAddress<s_data_array**>(0x4CE880, 0);
}

effect_location_datum* __cdecl effect_location_get_next_valid_index(effect_datum* effect_datum, int32* out_index, int a3)
{
    //TODO: Server offset
    return INVOKE(0xA68DD, 0, effect_location_get_next_valid_index, effect_datum, out_index, a3);
}