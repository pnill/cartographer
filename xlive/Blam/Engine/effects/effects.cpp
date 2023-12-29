#include "stdafx.h"
#include "effects.h"

#include "Blam/Engine/interface/first_person_weapons.h"
#include "Blam/Engine/main/interpolator.h"
#include "Blam/Engine/objects/objects.h"
#include "Util/Hooks/Hook.h"

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
    return *Memory::GetAddress<s_data_array**>(0x4CE884, 0x4F5070);
}

s_data_array* get_effects_location_table()
{
    return *Memory::GetAddress<s_data_array**>(0x4CE880, 0x4F506C);
}

effect_location_datum* __cdecl effect_location_get_next_valid_index(effect_datum* effect_datum, int32* out_index, int16 a3)
{
    return INVOKE(0xA68DD, 0x9895D, effect_location_get_next_valid_index, effect_datum, out_index, a3);
}




void __cdecl effect_datum_get_node_matrix_relative_or_origin(int16 a1, effect_datum* effect, real_matrix4x3* out_mat, bool a4)
{
    int32 origin_user = effect->origin_local_user_index;
    datum origin_object_index = effect->multi_purpose_origin_index;

    if (!a4 || a1 == -1 || (a1 & 0x8000) == 0 || origin_user == -1)
    {
        if (a1 != -1)
        {
            if ((a1 & 0x8000) != 0)
            {
                if (origin_user != -1)
                {
                    first_person_weapon_get_worldspace_node_matrix(origin_user, origin_object_index, a1 & ~0x8000, out_mat);
                    return;
                }
            }
            else
            {
                int16 node_index = a1 & ~0x8000;
                if (!halo_interpolator_interpolate_object_node_matrix(origin_object_index, node_index, out_mat))
                {
                    csmemcpy(out_mat, object_get_node_matrix(origin_object_index, node_index), sizeof(real_matrix4x3));
                }
            }
        }
    }
    else
    {
        real_matrix4x3* mat = first_person_weapon_get_relative_node_matrix(origin_user, origin_object_index, a1 & ~0x8000);
        csmemcpy(out_mat, mat, sizeof(real_matrix4x3));
    }
}


__declspec(naked) void effect_datum_get_node_matrix_relative_or_origin_to_cdecl()
{
    __asm
    {
        push eax
        mov eax, [esp + 8h]
        push eax
        push ecx
        push edx
        mov eax, [esp + 0Ch]
        push eax
        call effect_datum_get_node_matrix_relative_or_origin
        add esp, 14h
        retn
    }
}


void effects_apply_patches()
{
    WriteJmpTo(Memory::GetAddress(0xA69A4), effect_datum_get_node_matrix_relative_or_origin_to_cdecl);
}