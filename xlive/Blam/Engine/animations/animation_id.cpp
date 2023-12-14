#include "stdafx.h"
#include "animation_id.h"

c_animation_id::c_animation_id(void)
{
	this->clear();
	return;
};

void c_animation_id::clear(void)
{
	this->graph_index = NONE;
	this->animation_index = NONE;
	return;
};

int16 c_animation_id::index(void) const
{
	return this->animation_index;
};

void c_animation_id::set_index(int16 index)
{
	this->animation_index = index;
	return;
};

void c_animation_id::set_subgraph(int16 subgraph)
{
	this->graph_index = subgraph;
	return;
};

int16 c_animation_id::subgraph(void) const
{
	return this->graph_index;
};

bool c_animation_id::valid(void) const
{
	return this->animation_index != NONE;
};

bool c_animation_id::operator==(c_animation_id* animation_id)
{
	return this->graph_index == animation_id->graph_index && this->animation_index == animation_id->animation_index;
}
