#pragma once

/* classes */

class c_networking_panorama_favorites
{
	char gap0[2208];
};
ASSERT_STRUCT_SIZE(c_networking_panorama_favorites, 0x8a0);

c_networking_panorama_favorites* get_networking_panorama_favourites(void);