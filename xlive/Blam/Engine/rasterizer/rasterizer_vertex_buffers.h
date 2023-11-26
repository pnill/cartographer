#pragma once

struct rasterizer_vertex_buffer
{
	int8 declaration;
	uint8 stride;
	uint16 count;
	int32 actual_vertex_offset_bytes;
	int32 default_vertex_offset_bytes;
	void* vertex_data;
	int32 field_10;
	void* hardware_format;
	uint32 field_18;
	uint32 field_1C;
};

CHECK_STRUCT_SIZE(rasterizer_vertex_buffer, 32);