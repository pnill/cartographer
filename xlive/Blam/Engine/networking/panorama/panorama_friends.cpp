#include "stdafx.h"
#include "panorama_friends.h"

c_networking_panorama_friends* get_networking_panorama_friends(void)
{
    return Memory::GetAddress<c_networking_panorama_friends*>(0x517920, 0x541DB8);
}

bool c_networking_panorama_friends::has_active_task()
{
    return m_current_task != nullptr;
}

void c_networking_panorama_friends::initialize_startup()
{
    INVOKE_TYPE(0x1B3C1A, 0x1AF1E3, void(__thiscall*)(c_networking_panorama_friends*), this);
}