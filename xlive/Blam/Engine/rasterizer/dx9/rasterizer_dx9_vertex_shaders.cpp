#include "stdafx.h"
#include "rasterizer_dx9_vertex_shaders.h"

#include "cache/cache_files.h"

#include "rasterizer/dx9/vertex_shaders_dx9/preprocessed_hlsl_from_tool/compiled/_compiled_shader_table.h"
#include "rasterizer/rasterizer_globals.h"
#include "rasterizer/rasterizer_vertex_shader_definitions.h"

/* prototypes */

static void rasterizer_dx9_shaders_replace_classification_bytecode(s_vertex_shader_replacement_information const* type, datum tag_index, bool sm3_supported);

/* public code */

void rasterizer_dx9_vertex_shaders_replace_map_bytecode(
	datum vertex_shader_tag_index)
{
	/* DISABLED FOR NOW
	* 
	const bool sm3_supported = rasterizer_globals_get()->d3d9_sm3_supported;

	for (size_t i = 0; i < NUMBEROF(k_vertex_shader_replacement_information); ++i)
	{
		const s_vertex_shader_replacement_information* type = &k_vertex_shader_replacement_information[i];
		char name[256];
		csprintf(name, NUMBEROF(name), "rasterizer\\vertex_shaders_dx9\\%s", type->tag_name);
		const char* tag_name = tag_get_name(vertex_shader_tag_index);
		if (csstrncmp(tag_name, name, 256) == 0)
		{
			rasterizer_dx9_shaders_replace_classification_bytecode(type, vertex_shader_tag_index, sm3_supported);
			break;
		}
	}
	*/
	return;
}

/* private code */

static void rasterizer_dx9_shaders_replace_classification_bytecode(
	s_vertex_shader_replacement_information const* type,
	datum tag_index,
	bool sm3_supported)
{
	c_vertex_shader_definition* vertex_shader = vertex_shader_definition_get(tag_index);

	for (int8 classification_num = 0; classification_num < vertex_shader->m_geometry_classifications.count; ++classification_num)
	{
		c_vertex_shader_classification* classification = TAG_BLOCK_GET_ELEMENT(&vertex_shader->m_geometry_classifications, classification_num, c_vertex_shader_classification);
		s_cache_file_memory_globals* g_cache_file_memory_globals = cache_file_memory_globals_get();

		classification->compiled_shader.data = (int32)type->shader_bytecode[sm3_supported][classification_num] - g_cache_file_memory_globals->tag_cache_base_address;
		classification->compiled_shader.size = type->size[sm3_supported][classification_num];
	}

	return;
}
