#pragma once

/* classes */
class c_networking_panorama_achievements
{
	char gap0[1512];
};
ASSERT_STRUCT_SIZE(c_networking_panorama_achievements, 0x5e8);

c_networking_panorama_achievements* get_networking_panaroma_achievements(void);