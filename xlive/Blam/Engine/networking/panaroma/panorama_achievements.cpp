#include "stdafx.h"
#include "panorama_achievements.h"

c_networking_panaroma_achievements* get_networking_panaroma_achievements(void)
{
	return Memory::GetAddress<c_networking_panaroma_achievements*>(0x519F70, 0x544408);

}
