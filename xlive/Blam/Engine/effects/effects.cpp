#include "stdafx.h"
#include "effects.h"

#include "interface/first_person_weapons.h"
#include "main/interpolator.h"
#include "objects/objects.h"

s_data_array* get_effects_table()
{
    return *Memory::GetAddress<s_data_array**>(0x4CE884, 0x4F5070);
}

s_data_array* get_effects_location_table()
{
    return *Memory::GetAddress<s_data_array**>(0x4CE880, 0x4F506C);
}

real_vector3d* effect_get_velocity(datum effect_index)
{
    effect_datum* effect = (effect_datum*)datum_get(get_effects_table(), effect_index);
    return &effect->velocity;
}

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

effect_location_datum* __cdecl effect_location_get_next_valid_index(effect_datum* effect_index, int32* out_index, int16 a3)
{
    return INVOKE(0xA68DD, 0x9895D, effect_location_get_next_valid_index, effect_index, out_index, a3);
}

void __cdecl effects_frame_advance(real32 dt)
{

}

void __cdecl effect_update(datum effect_index, real32 dt)
{
    void* fn = Memory::GetAddress<void*>(0xAA9FF, 0x0);

    __asm
    {
        mov eax, dt
        push eax
        mov eax, effect_index
        call fn
    }
}

void __cdecl effect_update_time(datum effect_datum, real32 dt)
{
    return INVOKE(0xAA7B4, 0x0, effect_update_time, effect_datum, dt);
}

void __cdecl effect_datum_get_node_matrix_relative_or_origin(int16 node, effect_datum* effect, real_matrix4x3* out_mat, bool a4)
{
    int32 origin_user = effect->first_person_user_index;
    datum origin_object_index = effect->object_index;

    bool special_node = (node & 0x8000) != 0;
    int16 node_index = node & ~0x8000;

    if (!a4 || node == -1 || !special_node || origin_user == -1)
    {
        if (node != -1)
        {
            if (special_node)
            {
                if (origin_user != NONE)
                {
                    //first_person_weapon_get_worldspace_node_matrix(origin_user, origin_object_index, node_index, out_mat);
                    first_person_weapon_get_worldspace_node_matrix_interpolated(origin_user, origin_object_index, node_index, out_mat);
                    return;
                }
            }
            else
            {
                if (!halo_interpolator_interpolate_object_node_matrix(origin_object_index, node_index, out_mat))
                {
                    csmemcpy(out_mat, object_get_node_matrix(origin_object_index, node_index), sizeof(real_matrix4x3));
                }
            }
        }
    }
    else
    {
        // real_matrix4x3* mat = first_person_weapon_get_relative_node_matrix(origin_user, origin_object_index, node_index);
        real_matrix4x3* mat = first_person_weapon_get_relative_node_matrix_interpolated(origin_user, origin_object_index, node_index);
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