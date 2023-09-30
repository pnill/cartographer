#include "stdafx.h"
#include "object_identifier.h"

void c_object_identifier::clear()
{
	this->m_object_type = (e_object_type)NONE;
	this->m_source = (e_object_source)NONE;
	this->m_origin_bsp_index = NONE;
	this->m_unique_id = NONE;
}

void c_object_identifier::clear_for_deletion()
{
	this->m_source = (e_object_source)NONE;
	this->m_origin_bsp_index = NONE;
	this->m_unique_id = NONE;
}

short c_object_identifier::get_origin_bsp()
{
	return this->m_origin_bsp_index;
}

e_object_source c_object_identifier::get_source()
{
	return this->m_source;
}

e_object_type c_object_identifier::get_type()
{
	return this->m_object_type;
}

int c_object_identifier::get_unique_id()
{
	return this->m_unique_id;
}
