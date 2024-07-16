#include "stdafx.h"
#include "replication_entity_manager_view.h"


void c_replication_entity_manager_view::reset(void)
{
	typedef void(__thiscall* reset_t)(c_replication_entity_manager_view*);
	INVOKE_TYPE(0x1D1718, 0x1D69A4, reset_t, this);
	return;
}