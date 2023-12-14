#include "stdafx.h"
#include "ik_point_iterator.h"

ik_point_iterator::ik_point_iterator(void)
{
	this->clear();
	return;
}

void ik_point_iterator::clear(void)
{
	this->marker = NONE;
	this->attach_to_marker = NONE;
	this->weapon_ik_index = NONE;
	this->field_A = 0;
	this->field_C = NONE;
	this->field_10 = NONE;
	return;
}
