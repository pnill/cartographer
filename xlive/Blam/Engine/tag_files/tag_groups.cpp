#include "stdafx.h"

#include "tag_groups.h"

datum s_tag_data_iterator::get_next_datum()
{
	typedef datum(__cdecl t_get_next_datum)(s_tag_data_iterator* it);
	auto p_get_next_datum = Memory::GetAddress<t_get_next_datum*>(0x3172C);
	return  p_get_next_datum(this);
}
