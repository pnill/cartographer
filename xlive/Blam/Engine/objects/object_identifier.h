#pragma once
#include "object_type_list.h"

enum e_object_source : int8
{
	_object_source_structure = 0,
	_object_source_editor = 1,
	_object_source_dynamic = 2,
	_object_source_legacy = 3,
};

class c_object_identifier
{
private:
	int32 m_unique_id;
	int16 m_origin_bsp_index;
	e_object_type m_object_type;
	e_object_source m_source;

public:
	c_object_identifier() = default;
	~c_object_identifier() = default;
	void clear();
	void clear_for_deletion();

	short get_origin_bsp();
	e_object_source get_source();
	e_object_type get_type();
	int get_unique_id();
};
CHECK_STRUCT_SIZE(c_object_identifier, 8);