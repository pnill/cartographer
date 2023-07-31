#include "stdafx.h"
#include "object_identifier.h"

void c_object_identifier::clear()
{
	this->object_type = object_type_none;
	this->source = object_source_none;
	this->origin_bsp_index = -1;
	this->unique_id = -1;
}

void c_object_identifier::clear_for_deletion()
{
	this->source = object_source_none;
	this->origin_bsp_index = -1;
	this->unique_id = -1;
}

short c_object_identifier::get_origin_bsp()
{
	return this->origin_bsp_index;
}

e_object_source c_object_identifier::get_source()
{
	return this->source;
}

e_object_type c_object_identifier::get_type()
{
	return this->object_type;
}

int c_object_identifier::get_unique_id()
{
	return this->unique_id;
}
