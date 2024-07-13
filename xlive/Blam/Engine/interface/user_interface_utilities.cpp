#include "stdafx.h"
#include "user_interface_utilities.h"

datum __cdecl user_interface_get_widget_tag_index_from_screen_id(e_user_interface_screen_id screen_id)
{
	return INVOKE(0x20C701, 0x0, user_interface_get_widget_tag_index_from_screen_id, screen_id);
}
