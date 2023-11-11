#include "stdafx.h"
#include "animation_manager.h"

#include "Blam/Engine/tag_files/global_string_ids.h"

c_animation_manager::c_animation_manager()
{
    this->initialize();
}

void c_animation_manager::initialize()
{
    this->channel1.initialize();
    this->channel2.initialize();
    this->channel3.initialize();
    this->animation_manager_flags = 0;
    this->unk6E = 0;
    this->jmad_tag_datum = NONE;
    this->interpolator_control_0.disable();
    this->interpolator_control_1.disable();
    this->animation_string_ids[0] = HS_ID_NONE;
    this->animation_string_ids[1] = HS_ID_NONE;
    this->animation_string_ids[2] = HS_ID_NONE;
    this->animation_string_ids[3] = HS_ID_NONE;
    this->unk80 = 0;
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

bool c_animation_manager::validate_graph(datum jmad_tag_index, datum hlmt_tag_index)
{
    typedef bool(__thiscall* c_animation_manager__validate_graph_t)(c_animation_manager*, datum, datum);
    auto c_animation_manager__validate_graph = Memory::GetAddress<c_animation_manager__validate_graph_t>(0xF325F, 0xF6CA5);
    return c_animation_manager__validate_graph(this, jmad_tag_index, hlmt_tag_index);
}
