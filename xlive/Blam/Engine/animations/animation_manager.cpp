#include "stdafx.h"
#include "animation_manager.h"

#include "tag_files/global_string_ids.h"

c_animation_manager::c_animation_manager()
{
    this->initialize();
}

void c_animation_manager::initialize()
{
    for (int32 i = 0; i < NUMBEROF(channels); i++)
    {
        channels[i].reset();
    }

    this->animation_manager_flags = 0;
    this->unk6E = 0;
    this->jmad_tag_datum = NONE;
    this->interpolator_controls[0].disable();
    this->interpolator_controls[1].disable();
    this->animation_string_ids[0] = _string_id_invalid;
    this->animation_string_ids[1] = _string_id_invalid;
    this->animation_string_ids[2] = _string_id_invalid;
    this->animation_string_ids[3] = _string_id_invalid;
    this->field_80 = 0;
}

bool c_animation_manager::find_next_weapon_ik_point(ik_point_iterator* iterator)
{
    typedef bool(__thiscall* find_next_weapon_ik_point_t)(c_animation_manager*, ik_point_iterator*);
    find_next_weapon_ik_point_t find_next_weapon_ik_point = Memory::GetAddress<find_next_weapon_ik_point_t>(0xF41CC, 0xF5A46);
    return find_next_weapon_ik_point(this, iterator);
}

void c_animation_manager::interpolate_node_orientations(int16 orientation_count, const c_static_flags<255>* flags, real_orientation* orientation_in, real_orientation* orientation_out)
{
    typedef void(__thiscall* interpolate_node_orientations_t)(c_animation_manager*, int16, const c_static_flags<255>*, real_orientation*, real_orientation*);
    interpolate_node_orientations_t get_node_orientations = Memory::GetAddress<interpolate_node_orientations_t>(0xF35D5, 0xF4E4F);
    get_node_orientations(this, orientation_count, flags, orientation_in, orientation_out);
    return;
}

const c_model_animation_graph* c_animation_manager::get_graph(void) const
{
    return c_model_animation_graph::get(this->jmad_tag_datum);
}

int16 c_animation_manager::get_node_count(void) const
{
    const c_model_animation_graph* graph = this->get_graph();
    return graph->get_node_count();
}

void c_animation_manager::get_node_orientations(real32 a2, real32 a3, const c_static_flags<255>* flags, int32 node_orientations_count, real_orientation* orientation_list, void* func, datum object_index)
{
    typedef void(__thiscall* get_node_orientations_t)(c_animation_manager*, real32, real32, const c_static_flags<255>*, int32, real_orientation*, void*, datum);
    get_node_orientations_t get_node_orientations = Memory::GetAddress<get_node_orientations_t>(0xF5801, 0xF7071);
    get_node_orientations(this, a2, a3, flags, node_orientations_count, orientation_list, func, object_index);
    return;
}

string_id c_animation_manager::get_state_name(void) const
{
    return this->animation_string_ids[3];
}

void c_animation_manager::node_matrices_from_orientations(real_matrix4x3* node_matrices, const real_orientation* orientation, const real_matrix4x3* base_matrix)
{
    typedef void(__thiscall* node_matrices_from_orientations_t)(c_animation_manager*, real_matrix4x3*, const real_orientation*, const real_matrix4x3*);
    node_matrices_from_orientations_t func = Memory::GetAddress<node_matrices_from_orientations_t>(0xF422E, 0xF5AA8);
    func(this, node_matrices, orientation, base_matrix);
    return;
}

void c_animation_manager::node_matrices_from_orientations_with_gun_hand_swap(real_matrix4x3* node_matrices,
    real_orientation* orientation_list,
    real_matrix4x3* matrix,
    int16 child_node_index,
    int16 alternate_parent_node_index)
{
    typedef void(__thiscall* node_matrices_from_orientations_with_gun_hand_swap_t)(c_animation_manager*, real_matrix4x3*, real_orientation*, real_matrix4x3*, int16, int16);
    node_matrices_from_orientations_with_gun_hand_swap_t func = Memory::GetAddress<node_matrices_from_orientations_with_gun_hand_swap_t>(0xF4241, 0xF5ABB);
    func(this, node_matrices, orientation_list, matrix, child_node_index, alternate_parent_node_index);
    return;
}

bool c_animation_manager::reset_graph(datum jmad_tag_index, datum hlmt_tag_index, bool some_bool)
{
    typedef bool(__thiscall* c_animation_manager__reset_graph_t)(c_animation_manager*, datum, datum, bool);
    auto c_animation_manager__reset_graph = Memory::GetAddress<c_animation_manager__reset_graph_t>(0xF59AD, 0xF721D);
    return c_animation_manager__reset_graph(this, jmad_tag_index, hlmt_tag_index, some_bool);
}

bool c_animation_manager::set_goal(string_id a2, string_id a3, string_id a4, string_id a5, uint32 a6, int32 a7)
{
    typedef bool(__thiscall* c_animation_manager__set_goal_t)(c_animation_manager*, string_id, string_id, string_id, string_id, uint32, int32);
    auto c_animation_manager__set_goal = Memory::GetAddress<c_animation_manager__set_goal_t>(0xF5435, 0xF6CA5);
    return c_animation_manager__set_goal(this, a2, a3, a4, a5, a6, a7);
}

bool c_animation_manager::setup_animation_channel_by_index(c_animation_channel* animation_channel, c_animation_id animation_id, uint32 a4)
{
    typedef bool(__thiscall* c_animation_manager__set_goal_t)(c_animation_manager*, c_animation_channel*, c_animation_id, uint32);
    auto c_animation_manager__set_goal = Memory::GetAddress<c_animation_manager__set_goal_t>(0xF3CB0, 0xF552A);
    return c_animation_manager__set_goal(this, animation_channel, animation_id, a4);
}

bool c_animation_manager::valid(void) const
{
    return this->animation_string_ids[1] != NONE && this->channels[0].valid();
}

bool c_animation_manager::validate_graph(datum jmad_tag_index, datum hlmt_tag_index)
{
    typedef bool(__thiscall* validate_graph_t)(c_animation_manager*, datum, datum);
    validate_graph_t validate_graph = Memory::GetAddress<validate_graph_t>(0xF325F, 0xF4AD9);
    return validate_graph(this, jmad_tag_index, hlmt_tag_index);
}
