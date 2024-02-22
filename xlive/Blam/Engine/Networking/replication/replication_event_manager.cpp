#include "stdafx.h"
#include "replication_event_manager.h"

void c_replication_event_manager::reset(void)
{
	typedef void(__thiscall* reset_t)(c_replication_event_manager*);
	INVOKE_TYPE(0x1D676D, 0x1D9A8E, reset_t, this);
	return;
}
