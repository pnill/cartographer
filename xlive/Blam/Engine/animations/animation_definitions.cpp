#include "stdafx.h"
#include "animation_definitions.h"

#include "H2MOD/Tags/TagInterface.h"

size_t c_animation_graph_resources::get_node_count() const
{ 
	return this->skeleton_nodes.size; 
}

bool c_model_animation::animation_is_world_relative() const
{
	return this->internal_flags & model_animation_flag_world_relative;
}

short c_model_animation::find_first_key_of_type(const e_frame_event_type event_type) const
{
	for (size_t i = 0; i < this->frame_events.size; ++i)
	{
		s_frame_event* frame_event = this->frame_events[i];
		if (frame_event->type == event_type)
		{
			return frame_event->frame;
		}
	}

	return DATUM_INDEX_NONE;
}

short c_model_animation::find_first_sound_event(s_sound_event* sound_event) const
{
	if (this->sound_events.size <= 0) { return 0; }

	*sound_event = *this->sound_events[0];
	return sound_event->frame;
}

short c_model_animation::find_next_key_of_type(const e_frame_event_type event_type, const int frame) const
{
	for (size_t i = 0; i < this->frame_events.size; ++i)
	{
		s_frame_event* frame_event = this->frame_events[i];
		if (frame_event->type == event_type && frame_event->frame > frame)
		{
			return frame_event->frame;
		}
	}
	return -1;
}

e_animation_type c_model_animation::get_animation_type() const
{
	return this->type;
}

float c_model_animation::get_authored_duration() const
{
	return this->frame_count * 0.0333333333f;
}


size_t c_model_animation::get_effect_events_size() const
{
	return this->effect_events.size;
}

short c_model_animation::get_frame_count() const
{
	return this->frame_count;
}

// Unsure what the original name of this is
double c_model_animation::get_frame_count_minus_epsilon() const
{
	return this->frame_count - k_real_math_epsilon;
}

size_t c_model_animation::get_frame_events_size() const
{
	return this->frame_events.size;
}

e_frame_info_type c_model_animation::get_frame_info_type() const
{
	return this->frame_info_type;
}

double c_model_animation::get_frame_position_from_playback_ratio(float playback_ratio) const
{

	float last_frame_index = this->get_last_frame_index() + k_real_math_epsilon;
	float frame_index_times_playback_ratio = last_frame_index * playback_ratio;

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

short c_model_animation::get_last_frame_index() const
{
	return this->frame_count - 1;
}

short c_model_animation::get_loop_frame_index() const
{
	return this->loop_frame_index;
}

byte c_model_animation::get_node_count() const
{
	return this->node_count;
}

int c_model_animation::get_node_list_checksum() const
{
	return this->node_list_checksum;
}

short c_model_animation::get_primary_key_frame_index() const
{
	return this->find_first_key_of_type(frame_event_type_primary_keyframe);
}

short c_model_animation::get_secondary_key_frame_index() const
{
	return this->find_first_key_of_type(frame_event_type_secondary_keyframe);
}

short c_model_animation::get_left_foot_frame_index() const
{
	return this->find_first_key_of_type(frame_event_type_left_foot);
}

short c_model_animation::get_right_foot_frame_index() const
{
	return this->find_first_key_of_type(frame_event_type_left_foot);
}

size_t c_model_animation::get_sound_events_size() const
{
	return this->sound_events.size;
}

short c_model_animation::get_sound_reference_index() const
{
	s_sound_event* sound_event = nullptr;
	this->find_first_sound_event(sound_event);
	if (sound_event)
	{
		return sound_event->sound;
	}

	return DATUM_INDEX_NONE;
}

string_id c_model_animation::get_string_id() const
{
	return this->name;
}


const c_model_animation_graph* c_model_animation_graph::get(const datum tag_datum_index) const
{
	return tags::get_tag_fast<const c_model_animation_graph>(tag_datum_index);
}

c_model_animation_graph* c_model_animation_graph::get_writable(const datum tag_datum_index) const
{
	return tags::get_tag_fast<c_model_animation_graph>(tag_datum_index);
}

short c_model_animation_graph::get_node_count() const
{
	return this->resources.get_node_count();
}
