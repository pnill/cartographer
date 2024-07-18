#pragma once

/* classes */
class c_networking_panaroma_user_history
{
	char gap0[7520];
};
ASSERT_STRUCT_SIZE(c_networking_panaroma_user_history, 0x1d60);

c_networking_panaroma_user_history* get_networking_panaroma_user_history(void);