#include "stdafx.h"
#include "render_primitive.h"

/* globals */

c_render_primitive_list g_render_primitives_by_primitive_list_type[3];	// Only used for a assert for now (DO NOT USE this until we "own" this global)

/* public code */

bool c_render_primitive_list::is_layer_different(e_render_layer layer_index) const
{
	ASSERT(VALID_INDEX(layer_index, k_number_of_render_layers));
	return !this->m_render_layer_flags.test(layer_index);
}

void __cdecl create_visible_render_primitives(int32 hologram_flag)
{
	INVOKE(0x19BDBC, 0x0, create_visible_render_primitives, hologram_flag);
	return;
}
