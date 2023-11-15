#pragma once
#include "Blam/Math/real_matrix4x3.h"

#define MAXIMUM_NODES_PER_FIRST_PERSON_MODEL 64

struct s_first_person_model_data
{
	datum render_model_tag_index;
	datum object_index;
	uint32 flags;
	real_matrix4x3 nodes[MAXIMUM_NODES_PER_FIRST_PERSON_MODEL];
};

void toggle_first_person(bool state);
void first_person_weapons_apply_patches();