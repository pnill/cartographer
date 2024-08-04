#include "stdafx.h"
#include "panorama_favorites.h"

c_networking_panorama_favorites* get_networking_panaroma_favourites(void)
{
    return Memory::GetAddress<c_networking_panorama_favorites*>(0x517970, 0x541E08);
}
