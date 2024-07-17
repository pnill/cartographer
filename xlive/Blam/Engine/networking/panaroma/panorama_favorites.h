#pragma once

/* classes */

class c_networking_panaroma_favorites
{
	char gap0[2208];
};
ASSERT_STRUCT_SIZE(c_networking_panaroma_favorites, 0x8a0);

c_networking_panaroma_favorites* get_networking_panaroma_favourites(void);