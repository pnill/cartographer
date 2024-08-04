#include "stdafx.h"
#include "panorama_achievements.h"

c_networking_panorama_achievements* get_networking_panorama_achievements(void)
{
	return Memory::GetAddress<c_networking_panorama_achievements*>(0x519F70, 0x544408);

}
