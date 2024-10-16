#pragma once
#include "tag_files/data_reference.h"
#include "tag_files/tag_block.h"

/* enums */

enum e_vertex_shader_platform : int16
{
	_vertex_shader_platform_pc,
	_vertex_shader_platform_xbox
};

/* structures */

class c_vertex_shader_classification
{
private:
	IDirect3DVertexShader9* runtime_pointer;

	/****************************************
	* definition_name: compiled_shader_block
	* flags: 0
	* alignment_bit: 0
	****************************************/
	// DataSize(8192)
	data_reference compiled_shader;

	/****************************************
	* definition_name: code_block
	* flags: 6
	* alignment_bit: 0
	****************************************/
	// DataSize(65535)
	data_reference code;
};
ASSERT_STRUCT_SIZE(c_vertex_shader_classification, 20);

class c_vertex_shader_definition
{
	static const uint32 k_vertex_shader_classification_count = 12;

private:
	e_vertex_shader_platform m_platform;
	int16 pad;
	tag_block<c_vertex_shader_classification> m_geometry_classifications;
	int32 m_output_swizzles;
};
ASSERT_STRUCT_SIZE(c_vertex_shader_definition, 16);
