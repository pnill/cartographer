#include "stdafx.h"
#include "online_task_xbox.h"
#include "tag_files/global_string_ids.h"

// stubbing these for now

string_id __cdecl online_task_get_header_text(datum task_datum)
{
	//return INVOKE(0x1AC4E6, 0x0, online_task_get_header_text,task_datum);
	return _string_id_invalid;
}

string_id __cdecl online_task_get_display_text(datum task_datum)
{
	//return INVOKE(0x1AC4E9, 0x0, online_task_get_display_text,task_datum);
	return _string_id_invalid;
}

int32 __cdecl online_task_continue(s_online_task_datum* task)
{
	//return INVOKE(0x1AC4EC, 0x0, ,online_task_continue,task);
	return K_ONLINE_TASK_STATUS_COMPLETED;
}

s_online_task_datum* __cdecl online_try_and_get_task(datum task_datum)
{
	//return INVOKE(0x1AC4E3, 0x0, online_try_and_get_task,task_datum);
	return nullptr;
}

void __cdecl online_task_close(datum task_datum)
{
	//return INVOKE(0x1AC4BE, 0x0, online_task_close, task_datum);
}
