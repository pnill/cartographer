#include "stdafx.h"
#include "animation_channel.h"

#include "Blam/Engine/tag_files/global_string_ids.h"

void c_animation_channel::initialize()
{
	this->reset();
	return;
}

c_animation_channel::c_animation_channel()
{
	this->initialize();
	return;
};

c_animation_channel::~c_animation_channel()
{
	this->animation_ids[0].clear();
	this->animation_ids[1].clear();
	this->animation_string = NONE;
	this->field_10 = 0;
	return;
}

void c_animation_channel::apply_node_orientations(real32 a2, real32 a3, int32 orientation_count, real_orientation* orientation_list, void* func, datum object_index)
{
	this->apply_weighted_node_orientations(a2, 1.0f, a3, orientation_count, orientation_list, func, object_index);
	return;
}

void c_animation_channel::apply_weighted_blend_screen_node_orientations(real32 yaw,
	real32 pitch,
	real32 ratio,
	const c_static_flags<255>* flags,
	int32 orientation_count,
	real_orientation* orientation_list,
	void* func,
	datum object_index)
{
	typedef void(__thiscall* apply_weighted_blend_screen_node_orientations_t)(c_animation_channel*, real32, real32, real32, const c_static_flags<255>*, int32, real_orientation*, void*, datum);
	apply_weighted_blend_screen_node_orientations_t function = Memory::GetAddress<apply_weighted_blend_screen_node_orientations_t>(0x112C8B, 0x1041AB);
	function(this, yaw, pitch, ratio, flags, orientation_count, orientation_list, func, object_index);
	return;
}

void c_animation_channel::apply_weighted_node_orientations(real32 a2, real32 ratio, real32 a4, int32 orientation_count, real_orientation* orientation_list, void* func, datum object_index)
{
	typedef void(__thiscall* apply_weighted_node_orientations_t)(c_animation_channel*, real32, real32, real32, int32, real_orientation*, void*, datum);
	apply_weighted_node_orientations_t function = Memory::GetAddress<apply_weighted_node_orientations_t>(0x112B58, 0x104078);
	function(this, a2, ratio, a4, orientation_count, orientation_list, func, object_index);
	return;
}

const c_model_animation* c_animation_channel::get_state_animation(void)
{
	typedef c_model_animation* (__thiscall* get_state_animation_t)(c_animation_channel*);
	get_state_animation_t get_state_animation = Memory::GetAddress<get_state_animation_t>(0x112749, 0x103C69);
	return get_state_animation(this);
}

void c_animation_channel::reset(void)
{
	this->animation_ids[0].clear();
	this->animation_ids[1].clear();
	this->authored_time = 0.0f;
	this->field_10 - 0;
	this->animation_state_flags = animation_state_none;
	this->frame_event_types[0] = animation_frame_event_type_none;
	this->field_18 = 1.0f;
	this->frame_event_types[1] = animation_frame_event_type_none;
	this->frame_event_types[2] = animation_frame_event_type_none;
	this->animation_string = HS_ID_NONE;
	this->field_E = -1;
	this->animation_mode = -1;
	this->field_D = -1;
	return;
}

void c_animation_channel::set_frame_position(real32 position)
{
	typedef void(__thiscall* set_frame_position_t)(c_animation_channel*, real32);
	set_frame_position_t set_frame_position = Memory::GetAddress<set_frame_position_t>(0x112F0A, 0x10442A);
	set_frame_position(this, position);
	return;
}

bool c_animation_channel::valid(void) const
{
	return this->animation_ids[0].valid() && this->animation_ids[1].valid();
}
