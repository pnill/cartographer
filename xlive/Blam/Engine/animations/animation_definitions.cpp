#include "stdafx.h"
#include "animation_definitions.h"

#include "H2MOD/Tags/TagInterface.h"

DWORD c_animation_graph_resources::get_node_count() 
{ 
	return this->skeleton_nodes.size; 
};

const c_model_animation_graph* c_model_animation_graph::get(datum tag_datum_index)
{
	return tags::get_tag_fast<const c_model_animation_graph>(tag_datum_index);
};

c_model_animation_graph* c_model_animation_graph::get_writable(datum tag_datum_index)
{
	return tags::get_tag_fast<c_model_animation_graph>(tag_datum_index);
};

short c_model_animation_graph::get_node_count()
{
    return this->resources.get_node_count();
}
