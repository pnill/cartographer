#pragma once
#include "Blam/Engine/objects/objects.h"
#include "Blam/Engine/objects/object_placement.h"

class c_object_identifier
{
private:
	int unique_id;
	short origin_bsp_index;
	e_object_type object_type;
	e_object_source source;
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