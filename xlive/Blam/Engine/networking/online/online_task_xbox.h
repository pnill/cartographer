#pragma once
#include "tag_files/string_id.h"


/* macro defines */

#define K_ONLINE_TASK_STATUS_COMPLETED 0x80004005

/* forward declarations */
struct s_online_task_datum;

string_id __cdecl online_task_get_header_text(datum task_datum);
string_id __cdecl online_task_get_display_text(datum task_datum);
int32 __cdecl online_task_continue(s_online_task_datum* task);
s_online_task_datum* __cdecl online_try_and_get_task(datum task_datum);
void __cdecl online_task_close(datum task_datum);
