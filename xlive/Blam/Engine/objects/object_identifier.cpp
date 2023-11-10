#include "stdafx.h"
#include "object_identifier.h"
#include "object_globals.h"

void c_object_identifier::clear()
{
	this->m_object_type = (e_object_type)NONE;
	this->m_source = (e_object_source)NONE;
	this->m_origin_bsp_index = NONE;
	this->m_unique_id = NONE;
	return;
}

void c_object_identifier::clear_for_deletion()
{
	this->m_source = (e_object_source)NONE;
	this->m_origin_bsp_index = NONE;
	this->m_unique_id = NONE;
	return;
}

void c_object_identifier::create_dynamic(e_object_type type)
{
	s_object_globals* object_globals = object_globals_get();
	this->m_object_type = type;
	this->m_source = _object_source_dynamic;
	this->m_origin_bsp_index = NONE;
	this->m_unique_id = ++object_globals->unique_id;
	return;
}

int16 c_object_identifier::get_origin_bsp() const
{
	return this->m_origin_bsp_index;
}

e_object_source c_object_identifier::get_source() const
{
	return this->m_source;
}

e_object_type c_object_identifier::get_type() const
{
	return this->m_object_type;
}

int32 c_object_identifier::get_unique_id() const
{
	return this->m_unique_id;
}
