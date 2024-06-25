#pragma once
#include "render_layers.h"

#include "memory/static_arrays.h"

/* classes */

class c_render_primitive
{
private:
	void* m_build_func;
	void* m_render_func;
	datum m_shader_index;
	datum m_particle_system_index;
	int32 field_10;
	bool m_field_14;
	int8 m_render_primitive_list_index;
	int8 m_pad[2];
};
ASSERT_STRUCT_SIZE(c_render_primitive, 24);

class c_render_primitive_list
{	
public:
	int32 m_max_primitive_count;
	int32 m_field_4;
	c_flags_no_init<e_render_layer, uint32, k_number_of_render_layers> m_render_layer_flags;
	int32 m_field_C;
	c_flags_no_init<e_render_layer, uint32, k_number_of_render_layers> m_render_layer_flags_backup;
	c_render_primitive* m_primitives;
};
ASSERT_STRUCT_SIZE(c_render_primitive_list, 24);

/* globals */

extern c_render_primitive_list g_render_primitives_by_primitive_list_type[3];

/* public code */

static c_render_primitive_list* render_primitive_get_by_primitive_list_type(uint8 primitive_list_type)
{
	ASSERT(VALID_INDEX(primitive_list_type, NUMBEROF(g_render_primitives_by_primitive_list_type)));
	return &Memory::GetAddress<c_render_primitive_list*>(0x4F4EC0)[primitive_list_type];
}
