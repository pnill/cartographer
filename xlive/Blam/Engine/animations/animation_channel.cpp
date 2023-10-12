#include "stdafx.h"
#include "animation_channel.h"

#include "Blam/Engine/tag_files/global_string_ids.h"

c_animation_id::c_animation_id() 
{ 
	this->clear(); 
};

void c_animation_id::clear()
{
	this->graph_index = -1;
	this->animation_index = -1;
};

short c_animation_id::index() 
{ 
	return this->animation_index; 
};

void c_animation_id::set_index(int16 index)
{
	this->animation_index = index;
};

void c_animation_id::set_subgraph(int16 subgraph)
{
	this->graph_index = subgraph;
};

short c_animation_id::subgraph()
{
	return this->graph_index;
};

bool c_animation_id::operator==(c_animation_id* animation_id)
{
	return this->graph_index== animation_id->graph_index && this->animation_index == animation_id->animation_index;
}

c_animation_channel::c_animation_channel()
{
	this->reset();
};

c_animation_channel::~c_animation_channel()
{
	this->unk10 = 0;
}

void c_animation_channel::initialize()
{
	this->reset();
}

void c_animation_channel::reset()
{
	this->authored_time = 0.0;
	this->unk10 = 0;
	this->animation_state_flags = animation_state_none;
	this->frame_event_types_1 = animation_frame_event_type_none;
	this->unk18 = 1.0f;
	this->frame_event_types_2 = animation_frame_event_type_none;
	this->frame_event_types_3 = animation_frame_event_type_none;
	this->animation_string = HS_ID_NONE;
	this->animation_mode = -1;
	this->unkD = -1;
	this->unkE = -1;
}
