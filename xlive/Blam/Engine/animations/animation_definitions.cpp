#include "stdafx.h"
#include "animation_definitions.h"

#include "H2MOD/Tags/TagInterface.h"

const uint32* c_model_animation_runtime_data::get_left_arm_nodes(void) const
{
	return this->left_arm_nodes;
}

const uint32* c_model_animation_runtime_data::get_right_arm_nodes(void) const
{
	return this->right_arm_nodes;
}

s_animation_inheritence* c_model_animation_runtime_data::get_animation_inhertence(uint8 index) const
{
	return this->inheritence_list[index];
}

s_weapon_class_listing* c_model_animation_runtime_data::get_weapon_list(uint8 index) const
{
	return this->weapon_list[index];
}

const c_model_animation* c_animation_graph_resources::get_animation(c_animation_id animation_id) const
{
	return this->animations[animation_id.index()];
}

size_t c_animation_graph_resources::get_animation_count(void) const
{
	return this->animations.size;
}

e_animation_graph_resources_flags c_animation_graph_resources::get_animation_graph_resources_flags(void) const
{
	return this->animation_graph_resources_flags;
}

const s_animation_blend_screen* c_animation_graph_resources::get_blend_screen(uint32 index) const
{
	return this->blend_screens[index];
}

size_t c_animation_graph_resources::get_blend_screen_count(void) const
{
	return this->blend_screens.size;
}

int16 c_animation_graph_resources::get_codec_pack(void) const
{
	return this->animation_codec_pack;
}

const animation_graph_effect_reference* c_animation_graph_resources::get_effect_reference(uint8 node_index) const
{
	return this->effect_references[node_index];
}

size_t c_animation_graph_resources::get_effect_reference_count(void) const
{
	return this->effect_references.size;
}


e_inheritance_flags c_animation_graph_resources::get_inheritance_flags(void) const
{
	return this->inheritance_flags;
}

const animation_graph_node* c_animation_graph_resources::get_node(uint8 node_index) const
{
	return this->skeleton_nodes[node_index];
}

int16 c_animation_graph_resources::get_node_count(void) const
{ 
	return (int16)this->skeleton_nodes.size; 
}

const animation_graph_sound_reference* c_animation_graph_resources::get_sound_reference(uint8 node_index) const
{
	return this->sound_references[node_index];
}

size_t c_animation_graph_resources::get_sound_reference_count(void) const
{
	return this->sound_references.size;
}

tag_reference c_animation_graph_resources::get_parent_graph(void) const
{
	return this->parent_animation_graph;
}

bool c_model_animation::animation_is_world_relative(void) const
{
	return TEST_FLAG(this->internal_flags, _model_animation_flag_world_relative);
}

int16 c_model_animation::find_first_key_of_type(const e_frame_event_type event_type) const
{
	int16 result = NONE;
	for (int32 i = 0; i < this->frame_events.size; ++i)
	{
		s_frame_event* frame_event = this->frame_events[i];
		if (frame_event->type == event_type)
		{
			result = frame_event->frame;
			break;
		}
	}

	return result;
}

int16 c_model_animation::find_first_sound_event(s_sound_event* sound_event) const
{
	int16 frame = 0;
	if (this->sound_events.size > 0) 
	{
		s_sound_event* event = this->sound_events[0];
		frame = event->frame;
		
		// Populate sound event pointer if not null
		if (sound_event)
		{
			sound_event = event;
		}
	}

	return frame;
}

int16 c_model_animation::find_next_key_of_type(e_frame_event_type event_type, int32 frame) const
{
	for (uint32 i = 0; i < this->frame_events.size; ++i)
	{
		s_frame_event* frame_event = this->frame_events[i];
		if (frame_event->type == event_type && frame_event->frame > frame)
		{
			return frame_event->frame;
		}
	}
	return NONE;
}

e_animation_type c_model_animation::get_animation_type(void) const
{
	return this->type;
}

real32 c_model_animation::get_authored_duration(void) const
{
	return this->frame_count * (1.0f / 30.0f);
}


size_t c_model_animation::get_effect_events_size(void) const
{
	return this->effect_events.size;
}

int16 c_model_animation::get_frame_count(void) const
{
	return this->frame_count;
}

// Unsure what the original name of this is
double c_model_animation::get_frame_count_minus_epsilon(void) const
{
	return this->frame_count - k_real_math_epsilon;
}

size_t c_model_animation::get_frame_events_size(void) const
{
	return this->frame_events.size;
}

e_frame_info_type c_model_animation::get_frame_info_type(void) const
{
	return this->frame_info_type;
}

real64 c_model_animation::get_frame_position_from_playback_ratio(real32 playback_ratio) const
{

	real32 last_frame_index = this->get_last_frame_index() + k_real_math_epsilon;
	real32 frame_index_times_playback_ratio = last_frame_index * playback_ratio;

	if ((last_frame_index * playback_ratio) < 0.0f)
	{
		return 0.0f;
	}
	
	if (frame_index_times_playback_ratio <= last_frame_index)
	{
		return frame_index_times_playback_ratio;
	}

	return last_frame_index;
}

int16 c_model_animation::get_last_frame_index(void) const
{
	return this->frame_count - 1;
}

int16 c_model_animation::get_loop_frame_index(void) const
{
	return this->loop_frame_index;
}

uint8 c_model_animation::get_node_count(void) const
{
	return this->node_count;
}

int32 c_model_animation::get_node_list_checksum(void) const
{
	return this->node_list_checksum;
}

e_playback_flags c_model_animation::get_playback_flags(void) const
{
	return this->playback_flags;
}

int16 c_model_animation::get_primary_key_frame_index(void) const
{
	return this->find_first_key_of_type(_frame_event_type_primary_keyframe);
}

int16 c_model_animation::get_secondary_key_frame_index(void) const
{
	return this->find_first_key_of_type(_frame_event_type_secondary_keyframe);
}

int16 c_model_animation::get_left_foot_frame_index(void) const
{
	return this->find_first_key_of_type(_frame_event_type_left_foot);
}

int16 c_model_animation::get_right_foot_frame_index(void) const
{
	return this->find_first_key_of_type(_frame_event_type_right_foot);
}

size_t c_model_animation::get_sound_events_size(void) const
{
	return this->sound_events.size;
}

int16 c_model_animation::get_sound_reference_index(void) const
{
	s_sound_event* sound_event = NULL;
	this->find_first_sound_event(sound_event);
	return (sound_event ? sound_event->sound : NONE);
}

string_id c_model_animation::get_string_id(void) const
{
	return this->name;
}

int32 c_model_animation_graph::find_node(string_id string) const
{
	int16 node_count = this->get_node_count();
	for (int16 i = 0; i < node_count; ++i)
	{
		if (string == this->get_node(i)->name)
		{
			return i;
		}
	}

	return NONE;
}

int32 c_model_animation_graph::find_node_with_flags(e_node_model_flags flags) const
{
	int16 node_count = this->get_node_count();
	for (int32 i = 0; i < node_count; ++i)
	{
		if ((this->get_node(i)->model_flags & flags) == flags)
		{
			return i;
		}
	}

	return NONE;
}

const animation_graph_node* c_model_animation_graph::get_node(uint8 node_index) const
{
	return this->resources.get_node(node_index);
}

c_model_animation_graph* c_model_animation_graph::get_writable(datum tag_index) const
{
	return (c_model_animation_graph*)tag_get_fast(tag_index);
}

int16 c_model_animation_graph::get_node_count(void) const
{
	return this->resources.get_node_count();
}
