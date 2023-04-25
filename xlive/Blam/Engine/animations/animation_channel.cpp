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

void c_animation_id::set_index(short index)
{
	this->animation_index = index;
};

void c_animation_id::set_subgraph(short subgraph)
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

void c_animation_channel::reset()
{
	this->unk1C = 0.0;
	this->unk10 = 0;
	this->animation_state_flags = (e_animation_state_flag_bits)0;
	this->frame_event_types_1 = (e_animation_frame_event_types)0;
	this->unk18 = 1.0f;
	this->frame_event_types_2 = (e_animation_frame_event_types)0;
	this->frame_event_types_3 = (e_animation_frame_event_types)0;
	this->animation_string = HS_ID_NONE;
	this->unkC = -1;
	this->unkD = -1;
	this->unkE = -1;
}

void c_animation_channel::initialize()
{
	this->reset();
}
