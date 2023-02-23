#include "stdafx.h"
#include "object_globals.h"

s_object_globals* s_object_globals::get()
{
	return *Memory::GetAddress<s_object_globals**>(0x4E4618);
}

bool s_object_globals::object_is_connected_to_map(datum header_datum)
{
	typedef bool(__cdecl* object_is_connected_to_map_t)(datum header_datum);
	auto p_object_is_connected_to_map = Memory::GetAddress<object_is_connected_to_map_t>(0x132922);
	return p_object_is_connected_to_map(header_datum);
}

void s_object_globals::object_connect_lights_recursive(datum header_datum, char a2, char a3, char a4, char a5)
{
	typedef void(__cdecl* object_connect_lights_recursive_t)(int a1, char a2, char a3, char a4, char a5);
	auto p_object_connect_lights_recursive = Memory::GetAddress<object_connect_lights_recursive_t>(0x134A78, 0x123948);
	p_object_connect_lights_recursive(header_datum, a2, a3, a4, a5);
}

void s_object_globals::object_update_collision_culling(datum header_datum)
{
	typedef void(__cdecl* object_update_collision_culling_t)(datum header_datum);
	auto p_object_update_collision_culling = Memory::GetAddress<object_update_collision_culling_t>(0x1324E9);
	p_object_update_collision_culling(header_datum);
}

void s_object_globals::object_wake(datum object_datum)
{
	typedef void(__cdecl* object_wake_t)(datum object_datum);
	auto p_object_wake = Memory::GetAddress<object_wake_t>(0x12FA1E);
	p_object_wake(object_datum);
}

bool s_object_globals::objects_can_connect_to_map()
{
	return get() && get()->initialized;
}
