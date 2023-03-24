#include "stdafx.h"
#include "animation_manager.h"
#include "Blam/Enums/HaloStrings.h"

c_animation_manager::c_animation_manager()
{
  c_animation_manager::initialize();
}

void c_animation_manager::initialize()
{
    this->channel1.initialize();
    this->channel2.initialize();
    this->channel3.initialize();
    this->animation_manager_flags = 0;
    this->unk6E = 0;
    this->animation_tag_index = -1;
    this->interpolator_control_0.disable();
    this->interpolator_control_1.disable();
    this->animation_string_ids[0] = -1;
    this->animation_string_ids[3] = -1;
    this->animation_string_ids[1] = -1;
    this->animation_string_ids[2] = -1;
    this->unk80 = 0;
}

bool c_animation_manager::reset_graph(datum jmad_tag_index, datum hlmt_tag_index, bool some_bool)
{
    typedef bool(__cdecl* c_animation_manager__validate_graph_t)(datum jmad_tag_datum, datum hlmt_tag_datum);
    auto c_animation_manager__validate_graph = Memory::GetAddress<c_animation_manager__validate_graph_t>(0xF325F);
    typedef bool(__thiscall* c_animation_manager__set_goal_t)(c_animation_manager* _this, string_id a2, string_id a3, string_id a4, string_id a5, unsigned int a6, int a7);
    auto c_animation_manager__set_goal = Memory::GetAddress<c_animation_manager__set_goal_t>(0xF5435);

    if (hlmt_tag_index != -1 && !c_animation_manager__validate_graph(jmad_tag_index, hlmt_tag_index))
        return false;
    if (some_bool)
        this->animation_manager_flags |= 2u;
    else
        this->animation_manager_flags &= ~2u;

    this->animation_tag_index = jmad_tag_index;
    this->animation_string_ids[0] = -1;
    this->animation_string_ids[3] = -1;
    this->animation_string_ids[1] = -1;
    this->animation_string_ids[2] = -1;

    this->channel3.create();
    this->channel2.create();
    this->channel1.create();

    c_animation_manager__set_goal(this, HS_DEFAULT, HS_DEFAULT, HS_DEFAULT, HS_DEFAULT, 791u, 63);
    this->interpolator_control_1.disable();
    this->interpolator_control_1.set_interpolation_type(1);
    this->interpolator_control_0.disable();
    this->unk6E = 0;
    this->unk80 = 0;
    return true;
}